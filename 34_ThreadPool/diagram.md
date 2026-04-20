# Thread Pool / Asynchronous Pipeline Diagram

```mermaid
classDiagram
   class SafeQueue_~Task~ {
      -queue~Task~ queue_
      -mutex mutex_
      -condition_variable cond_
      +push(Task)
      +pop(Task&) bool
      +close()
   }

   class SafeQueue~Result~ {
      -queue~Result~ queue_
      -mutex mutex_
      -condition_variable cond_
      +push(Result)
      +pop(Result&) bool
      +close()
   }

   class ThreadPool {
      -SafeQueue~Task~ task_queue_
      -vector~jthread~ workers_
      -atomic~int~ tasks_in_flight_
      +submit(Task)
      +wait_until_empty()
   }

   class Worker {
      <<std::jthread>>
      +worker_loop()
   }

   class Result {
   }

   class Task {
   }

   class Client {
      +main()
   }

   class Producer {
      <<std::jthread>>
   }

   class Reporter {
      <<std::jthread>>
   }

   class ComputationLogic {
      +my_sqrt()
      +my_trig()
   }

   %% The Pool owns its internal components
   ThreadPool *-- SafeQueue_~Task~ : task_queue_
   ThreadPool *-- "n" Worker

   %% Tasks in SafeQueue~Task~
   SafeQueue_~Task~ *-- "n" Task : queue_

   %% Results in SafeQueue~Result~
   SafeQueue~Result~ *-- "n" Result : queue_

   %% The Client (Main) creates the high-level infrastructure
   Client --> ThreadPool
   Client --> Producer
   Client --> Reporter
   Client *-- SafeQueue~Result~

   %% Operational dependencies
   Producer ..> ThreadPool : submits tasks
   Worker ..> SafeQueue_~Task~ : pops tasks
   Worker ..> ComputationLogic : executes math
   Worker ..> SafeQueue~Result~ : pushes results
   Reporter ..> SafeQueue~Result~ : pops results to report
```

### Design Note:
This diagram illustrates the complete asynchronous flow. The 'ThreadPool'
provides the stable execution environment ('Worker Pool'). The 'Client'
orchestrates the lifecycle by creating 'Producer' threads to feed the pool and
'Reporter' threads to drain the results. The 'SafeQueue' acts as the
synchronized bridge between all participants, ensuring thread-safety and
providing backpressure when limits are reached.
