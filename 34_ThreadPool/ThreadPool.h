/**
 * ============================================================================
 * File: ThreadPool.h
 * 
 * --- DESIGN OVERVIEW:
 * This header provides a robust synchronization infrastructure for concurrent
 * execution. It is designed to be task-agnostic, allowing the core engine to
 * remain stable regardless of changes in business logic.
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
 * barrier mechanism. Synchronization is strictly handled within mutex-protected
 * scopes to prevent 'lost wake-ups' and ensure that the main thread always
 * receives the completion signal reliably.
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
   size_t                  max_queue_size_;
   bool                    closed_{false};

public:
   explicit SafeQueue(size_t max_queue_size) : max_queue_size_{max_queue_size} { }

   ~SafeQueue() 
   {
      close();
   }

   void push(T item)
   {
      std::unique_lock lock{mutex_};
      cond_var_push_.wait(lock, [this]() {
         return queue_.size() < max_queue_size_ || closed_;
      });

      if(closed_) return;

      queue_.push(std::move(item));
      cond_var_pop_.notify_one();
   }

   bool pop(T& item)
   {
      std::unique_lock lock{mutex_};
      cond_var_pop_.wait(lock, [this]() {
         return !queue_.empty() || closed_;
      });

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

   std::vector<std::jthread> workers_;
   SafeQueue<Task>           task_queue_;
   std::atomic<int>          tasks_in_queue_{0};
   std::mutex                mutex_;
   std::condition_variable   cond_var_pool;

public:
   explicit ThreadPool(size_t num_workers, size_t max_queue_size)
      : task_queue_{max_queue_size}
   {
      for(size_t i = 0; i < num_workers; ++i)
         workers_.emplace_back([this](std::stop_token stop_token){
            worker_loop(stop_token);
         });
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

   void wait_until_all_tasks_are_processed()
   {
      std::unique_lock lock{mutex_};
      cond_var_pool.wait(lock, [this]() {
         return tasks_in_queue_ == 0;
      });
   }

   void worker_loop(std::stop_token stop_token)
   {
      Task task;
      while(!stop_token.stop_requested() && task_queue_.pop(task))
      {
         task();
         
         std::lock_guard lock{mutex_};
         if(--tasks_in_queue_ == 0)
            // Notify the main thread that is waiting for task completion
            cond_var_pool.notify_one();
      }
   }
};

#endif

//================================================================================ END
