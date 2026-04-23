/**
 * ============================================================================
 * File: ThreadPool.h
 * 
 * --- DESIGN OVERVIEW:
 * This header provides a robust synchronization infrastructure for concurrent 
 * execution. It is designed to be task-agnostic, allowing the core engine 
 * to remain stable regardless of changes in business logic.
 * 
 * --- KEY COMPONENTS:
 * 1. SafeQueue<T>: A monitor-based thread-safe queue with fixed capacity. 
 *    It implements 'Backpressure' to prevent memory exhaustion by blocking 
 *    the producer when the queue is full.
 * 2. ThreadPool: A manager for a fixed set of persistent worker threads 
 *    (std::jthread). It decouples task submission from execution.
 * 
 * --- SYNCHRONIZATION & DETERMINISM:
 * The pool uses an atomic 'tasks_in_queue' counter and a condition-based 
 * barrier. Synchronization is strictly handled within mutex-protected 
 * scopes to prevent 'lost wake-ups' and ensure that the main thread 
 * always receives the completion signal reliably.
 * ============================================================================
 */

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

//--------------------------------------------------------- SafeQueue (Monitor):
template <class T>
class SafeQueue
{
private:
   std::queue<T>           queue_;
   std::mutex              mutex_;
   std::condition_variable cond_var_push_;
   std::condition_variable cond_var_pop_;
   size_t                  max_size_;
   bool                    closed_{false};

public:
   explicit SafeQueue(size_t max_size) : max_size_{max_size} { }

   ~SafeQueue() 
   { 
      close(); 
   }

   void push(T item)
   {
      std::unique_lock lock{mutex_};
      cond_var_push_.wait(lock, [this]() { return queue_.size() < max_size_ || closed_; });
      
      if(closed_) return;

      queue_.push(std::move(item));
      cond_var_pop_.notify_one();
   }

   bool pop(T& item)
   {
      std::unique_lock lock{mutex_};
      cond_var_pop_.wait(lock, [this]() { return !queue_.empty() || closed_; });

      if(closed_) return false;

      item = std::move(queue_.front());
      queue_.pop();
      cond_var_push_.notify_one();
      return true;
   }

   void close()
   {
      std::lock_guard lock{mutex_};
      closed_ = true;
      cond_var_pop_.notify_all();
      cond_var_push_.notify_all();
   }
};

//--------------------------------------------------------- ThreadPool:
class ThreadPool
{
private:
   using Task = std::function<void()>;

   SafeQueue<Task>           task_queue_;
   std::vector<std::jthread> workers_;
   std::atomic<int>          tasks_in_queue_{0};
   std::mutex                mutex_;
   std::condition_variable   cond_var_pool;

public:
   explicit ThreadPool(size_t num_threads, size_t queue_size) : task_queue_{queue_size}
   {
      for(size_t i = 0; i < num_threads; ++i)
         workers_.emplace_back([this](std::stop_token st) { worker_loop(st); });
   }

   ~ThreadPool()
   { 
      task_queue_.close();
   }

   void submit(Task task)
   {
      {
         std::lock_guard lock{mutex_};
         tasks_in_queue_++;
      }
      task_queue_.push(std::move(task));
   }

   void wait_until_empty()
   {
      std::unique_lock lock{mutex_};
      cond_var_pool.wait(lock, [this]() { return tasks_in_queue_ == 0; });
   }

   void worker_loop(std::stop_token st)
   {
      Task task;
      while(!st.stop_requested() && task_queue_.pop(task))
      {
         task();
         
         std::lock_guard lock{mutex_};
         if(--tasks_in_queue_ == 0)
            // Notify to the main that is waiting_until_empty
            cond_var_pool.notify_one();
      }
   }
};

#endif

//================================================================================ END
