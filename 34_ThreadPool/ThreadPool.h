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
 * The pool uses an atomic 'tasks_in_flight' counter and a condition-based 
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
   std::queue<T> queue_;
   std::mutex mutex_;
   std::condition_variable cond_push_;
   std::condition_variable cond_pop_;
   size_t max_size_;
   bool closed_{false};

public:
   explicit SafeQueue(size_t max_size) : max_size_{max_size} { }

   void push(T item)
   {
      std::unique_lock lock{mutex_};
      cond_push_.wait(lock, [this]() { return queue_.size() < max_size_ || closed_; });
      
      if (closed_) return;

      queue_.push(std::move(item));
      cond_pop_.notify_one();
   }

   bool pop(T& item)
   {
      std::unique_lock lock{mutex_};
      cond_pop_.wait(lock, [this]() { return !queue_.empty() || closed_; });

      if (queue_.empty() && closed_) return false;

      item = std::move(queue_.front());
      queue_.pop();
      cond_push_.notify_one();
      return true;
   }

   void close()
   {
      std::lock_guard lock{mutex_};
      closed_ = true;
      cond_push_.notify_all();
      cond_pop_.notify_all();
   }
};

//--------------------------------------------------------- ThreadPool:
class ThreadPool
{
private:
   using Task = std::function<void()>;

   SafeQueue<Task> work_queue_;
   std::vector<std::jthread> workers_;
   std::atomic<int> tasks_in_flight_{0};
   std::mutex barrier_mutex_;
   std::condition_variable barrier_cv_;

public:
   explicit ThreadPool(size_t num_threads, size_t queue_size) 
      : work_queue_{queue_size}
   {
      for (size_t i = 0; i < num_threads; ++i)
         workers_.emplace_back([this](std::stop_token st) { worker_loop(st); });
   }

   ~ThreadPool() { work_queue_.close(); }

   void submit(Task task)
   {
      tasks_in_flight_++;
      work_queue_.push(std::move(task));
   }

   void wait_until_empty()
   {
      std::unique_lock lock{barrier_mutex_};
      barrier_cv_.wait(lock, [this]() { return tasks_in_flight_ == 0; });
   }

   void worker_loop(std::stop_token st)
   {
      Task task;
      while (!st.stop_requested() && work_queue_.pop(task))
      {
         task();
         
         // Decrement and notify must be synchronized with the barrier mutex
         std::lock_guard lock{barrier_mutex_};
         if (--tasks_in_flight_ == 0) barrier_cv_.notify_all();
      }
   }
};

#endif
//================================================================================ END
