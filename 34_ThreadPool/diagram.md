# Thread Pool / Asynchronous Pipeline Diagram

```mermaid
classDiagram
   class SafeQueue_~Task~ {
      -queue~T~ queue_
      -mutex mutex_
      -condition_variable cond_
      +push(T)
      +pop(T&) bool
      +close()
   }

   class SafeQueue~Result~ {
      -queue~T~ queue_
      -mutex mutex_
      -condition_variable cond_
      +push(T)
      +pop(T&) bool
      +close()
   }

   class ThreadPool {
      -SafeQueue~Task~ work_queue_
      -vector~jthread~ workers_
      -atomic~int~ tasks_in_flight_
      +submit(Task)
      +wait_until_empty()
   }

   class Worker {
      <<std::jthread>>
      +worker_loop()
   }

   class Client_Main {
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
   ThreadPool *-- SafeQueue~Task~
   ThreadPool *-- "n" Worker

   %% The Client (Main) creates the high-level infrastructure
   Client_Main --> ThreadPool
   Client_Main --> Producer
   Client_Main --> Reporter
   Client_Main *-- "n" SafeQueue~Result~

   %% Operational dependencies
   Producer ..> ThreadPool : submits tasks
   Worker ..> SafeQueue~T~ : pops work
   Worker ..> ComputationLogic : executes math
   Worker ..> SafeQueue~T~ : pushes results
   Reporter ..> SafeQueue~T~ : pops results to save
```

### Design Note:
This diagram illustrates the complete asynchronous flow. The 'ThreadPool'
provides the stable execution environment ('Worker Pool'). The 'Client_Main'
orchestrates the lifecycle by creating 'Producer' threads to feed the pool and
'Reporter' threads to drain the results. The 'SafeQueue' acts as the
synchronized bridge between all participants, ensuring thread-safety and
providing backpressure when limits are reached.
