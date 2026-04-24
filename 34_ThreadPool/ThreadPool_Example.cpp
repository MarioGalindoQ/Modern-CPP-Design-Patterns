/**
 * ============================================================================
 * File: ThreadPool_Example.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates an advanced asynchronous pipeline using a
 * task-agnostic ThreadPool. It simulates a two-stage scientific workflow where
 * workers process complex data structures and return results along with the
 * original inputs for full traceability.
 * 
 * --- BATCH PROCESSING LADDER:
 * 1. Batch 1: Single-input/Single-output (Square Roots).
 * 2. Batch 2: Multi-input/Multi-output (Sines and Cosines from dual inputs).
 * 
 * --- ARCHITECTURAL SEPARATION:
 * - Computation Logic: Pure functions (my_sqrt, my_trig) that encapsulate the
 *   heavy mathematical work, independent of the threading model.
 * - Dispatch Logic: Flexible lambdas that act as "glue", bridging the
 *   ThreadPool with SafeQueues to coordinate inputs and results.
 * - Reporting Logic: Ephemeral reporter threads created per batch to handle
 *   specific I/O requirements and persistence to disk.
 * 
 * --- CONCURRENCY TOPOLOGY & SYNCHRONIZATION:
 * - Producer Thread: Generates problems and submits them to the pool.
 * - Worker Threads (15): Persistent threads that consume and execute tasks.
 * - Reporter Thread: Consumes results and persists them to disk.
 * - Determinism: We use explicit producer joining to ensure all tasks are
 *   submitted and the 'wait_until_all_tasks_are_processed()' barrier to ensure
 *   all tasks are processed before the reporter shuts down, guaranteeing data
 *   integrity across transitions.
 *   ============================================================================
 */

#include "ThreadPool.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <random>
#include <chrono>
#include <thread>

//--------------------------------------------------------- Functional Structs:
// Results returned by the heavy computation functions
struct SqrtOutput
{
   double val;
};

struct TrigOutput
{
   double sinVal;
   double cosVal;
};

//--------------------------------------------------------- Queue Structs:
// Packets that travel through the result queues
struct ResultRoots
{
   int    id;
   double input;
   double value;
};

struct ResultTrig
{
   int    id;
   double input1;
   double input2;
   double sinVal;
   double cosVal;
};

//--------------------------------------------------------- Heavy Computations:
void simulate_work(int min, int max)
{
   static std::random_device rd;
   static std::mt19937 gen(rd());
   std::uniform_int_distribution<> dis(min, max);
   std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
}

SqrtOutput my_sqrt(double n)
{
   simulate_work(50, 350); // Simulate complex root finding
   return { std::sqrt(n) };
}

TrigOutput my_trig(double n1, double n2)
{
   simulate_work(90, 450); // Simulate complex trigonometric analysis
   return { std::sin(n1), std::cos(n2) };
}

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== THREAD POOL: ASYNCHRONOUS PIPELINE SIMULATION ===\n" << std::endl;

   // Create a pool of working threads
   ThreadPool pool(15, 20); // num_workers = 15, task_max_queue_size = 20

   // ---------------------------------------------------------------- PHASE 1: Square Roots:
   {
      std::cout << "--- Starting Batch 1: Square Roots ---\n";

      // Create a queue to receive the results.
      SafeQueue<ResultRoots> root_results_queue{10}; // max_queue_size = 10

      // Create a thread to get the results and use them as appropriate.
      std::jthread reporter([&root_results_queue]()
      {
         std::ofstream file("batch1_roots.txt");
         ResultRoots result_root;
         // Get results
         while(root_results_queue.pop(result_root))
         {
            // Use them as appropriate
            file << "Job ID "
                 << result_root.id
                 << ": sqrt("
                 << result_root.input
                 << ") = "
                 << result_root.value
                 << "\n";
         }
         std::cout << " [Reporter 1] All roots saved. Closing file.\n";
      });

      // Create a thread to produce the data and send it for processing
      std::jthread producer([&pool, &root_results_queue]()
      {
         for(int i = 1; i <= 200; ++i)
         {
            // Prepare input data
            simulate_work(0, 10);
            double val = static_cast<double>(i);

            // Send a function with the data to be processed
            pool.submit([&root_results_queue, i, val]()
            {
               // This is the work that the worker will perform
               SqrtOutput res = my_sqrt(val);

               // This is how the worker will return the results
               root_results_queue.push({
                     .id=i,
                     .input=val,
                     .value=res.val
                  });
            });
         }
         std::cout << " [Producer 1] All jobs submitted.\n";
      });

      // Wait for producer thread to end producing all tasks
      producer.join();

      // Wait until all tasks have been processed by the worker threads
      pool.wait_until_all_tasks_are_processed();

      // Close the results queue, thus stopping the reporter
      root_results_queue.close();

      simulate_work(5, 5);

      std::cout << " [System] Batch 1 completed.\n";
   }

   // --------------------------------------------------------------- PHASE 2: Sine & Cosine:
   {
      std::cout << "\n--- Starting Batch 2: Sine & Cosine ---\n";

      // Create a queue to receive the results.
      SafeQueue<ResultTrig> trig_results_queue{10}; // queue_size = 10

      // Create a thread to get the results and use them as appropriate.
      std::jthread reporter([&trig_results_queue]()
      {
         std::ofstream fileSin("batch2_sines.txt");
         std::ofstream fileCos("batch2_cosines.txt");
         ResultTrig result_trig;
         // Get results
         while(trig_results_queue.pop(result_trig))
         {
            // Use them as appropriate
            fileSin << "Job ID "
                    << result_trig.id
                    << ": sin("
                    << result_trig.input1
                    << ") = "
                    << result_trig.sinVal
                    << "\n";
            fileCos << "Job ID "
                    << result_trig.id
                    << ": cos("
                    << result_trig.input2
                    << ") = "
                    << result_trig.cosVal
                    << "\n";
         }
         std::cout << " [Reporter 2] Trig results saved. Closing files.\n";
      });

      // Create a thread to produce the data and send it for processing
      std::jthread producer([&pool, &trig_results_queue]()
      {
         for(int i = 1; i <= 100; ++i)
         {
            // Prepare input data
            simulate_work(0, 15);
            double val1 = static_cast<double>(i) * 0.1;
            double val2 = static_cast<double>(i) * 0.2;

            // Send a function with the data to be processed
            pool.submit([&trig_results_queue, i, val1, val2]()
            {
               // This is the work that the worker will perform
               TrigOutput res = my_trig(val1, val2);

               // This is how the worker will return the results
               trig_results_queue.push({
                     .id = i,
                     .input1 = val1,
                     .input2 = val2,
                     .sinVal = res.sinVal,
                     .cosVal = res.cosVal
                  });
            });
         }
         std::cout << " [Producer 2] All jobs submitted.\n";
      });

      // Wait for producer thread to end producing all tasks
      producer.join();

      // Wait until all tasks have been processed by the worker threads
      pool.wait_until_all_tasks_are_processed();

      // Close the results queue, thus stopping the reporter
      trig_results_queue.close();

      simulate_work(5, 5);

      std::cout << " [System] Batch 2 completed.\n";
   }

   std::cout << "\n=== SIMULATION COMPLETED. SHUTTING DOWN POOL ===\n";
}

//================================================================================ END
