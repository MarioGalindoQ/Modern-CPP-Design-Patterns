/**
 * ============================================================================
 * File: ThreadPool_Example.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates an advanced asynchronous pipeline using a 
 * task-agnostic ThreadPool. It simulates a two-stage scientific workflow 
 * where workers process complex data structures and return results along 
 * with the original inputs for full traceability.
 * 
 * --- BATCH PROCESSING LADDER:
 * 1. Batch 1: Single-input/Single-output (Square Roots).
 * 2. Batch 2: Multi-input/Multi-output (Sines and Cosines from dual inputs).
 * 
 * --- ARCHITECTURAL SEPARATION:
 * - Computation Logic: Pure functions (my_sqrt, my_trig) that encapsulate 
 *   the heavy mathematical work, independent of the threading model.
 * - Dispatch Logic: Flexible lambdas that act as "glue", bridging the 
 *   ThreadPool with SafeQueues to coordinate inputs and results.
 * - Reporting Logic: Ephemeral reporter threads created per batch to handle 
 *   specific I/O requirements and persistence to disk.
 * 
 * --- CONCURRENCY TOPOLOGY & SYNCHRONIZATION:
 * - Producer Thread: Generates problems and submits them to the pool.
 * - Worker Threads (10): Persistent threads that consume and execute tasks.
 * - Reporter Thread: Consumes results and persists them to disk.
 * - Determinism: We use explicit producer joining and the 'wait_until_empty()' 
 *   barrier to ensure all tasks are submitted and processed before the 
 *   reporter shuts down, guaranteeing data integrity across transitions.
 * ============================================================================
 */

#include "ThreadPool.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <random>
#include <chrono>

//--------------------------------------------------------- Functional Structs:
// Results returned by the heavy computation functions
struct SqrtOutput { double val; };
struct TrigOutput { double sinVal; double cosVal; };

//--------------------------------------------------------- Queue Structs:
// Packets that travel through the result queues
struct ResultRoots
{
   int id;
   double input;
   double value;
   bool is_sentinel{false};
};

struct ResultTrig
{
   int id;
   double input1;
   double input2;
   double sinVal;
   double cosVal;
   bool is_sentinel{false};
};

//--------------------------------------------------------- Heavy Computations:
void simulate_work()
{
   static std::random_device rd;
   static std::mt19937 gen(rd());
   std::uniform_int_distribution<> dis(10, 50);
   std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
}

SqrtOutput my_sqrt(double n)
{
   simulate_work(); // Simulate complex root finding
   return { std::sqrt(n) };
}

TrigOutput my_trig(double n1, double n2)
{
   simulate_work(); // Simulate complex trigonometric analysis
   return { std::sin(n1), std::cos(n2) };
}

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== THREAD POOL: ASYNCHRONOUS PIPELINE SIMULATION ===\n" << std::endl;

   ThreadPool pool(10, 20);

   // --- PHASE 1: Square Roots ---
   {
      std::cout << "--- Starting Batch 1: Square Roots ---\n";
      SafeQueue<ResultRoots> roots_results{100};

      std::jthread reporter([&roots_results]() {
         std::ofstream file("batch1_roots.txt");
         ResultRoots r;
         while (roots_results.pop(r))
         {
            if (r.is_sentinel) break;
            file << "Job ID " << r.id << ": sqrt(" << r.input << ") = " << r.value << "\n";
         }
         std::cout << " [Reporter 1] All roots saved. Closing file.\n";
      });

      std::jthread producer([&pool, &roots_results]() {
         for (int i = 1; i <= 100; ++i)
         {
            double val = static_cast<double>(i);
            pool.submit([&roots_results, i, val]() {
               // The lambda only coordinates the call to the heavy function
               SqrtOutput res = my_sqrt(val);
               roots_results.push({i, val, res.val, false});
            });
         }
      });

      producer.join(); 
      pool.wait_until_empty();
      roots_results.push({0, 0.0, 0.0, true}); 
      std::cout << " [System] Batch 1 completed.\n\n";
   }

   // --- PHASE 2: Sine & Cosine ---
   {
      std::cout << "--- Starting Batch 2: Sine & Cosine ---\n";
      SafeQueue<ResultTrig> trig_results{50};

      std::jthread reporter([&trig_results]() {
         std::ofstream fileSin("batch2_sines.txt");
         std::ofstream fileCos("batch2_cosines.txt");
         ResultTrig r;
         while (trig_results.pop(r))
         {
            if (r.is_sentinel) break;
            fileSin << "Job ID " << r.id << ": sin(" << r.input1 << ") = " << r.sinVal << "\n";
            fileCos << "Job ID " << r.id << ": cos(" << r.input2 << ") = " << r.cosVal << "\n";
         }
         std::cout << " [Reporter 2] Trig results saved. Closing files.\n";
      });

      std::jthread producer([&pool, &trig_results]() {
         for (int i = 1; i <= 50; ++i)
         {
            double val1 = static_cast<double>(i) * 0.1;
            double val2 = static_cast<double>(i) * 0.2;
            pool.submit([&trig_results, i, val1, val2]() {
               // Calculating sin of first and cos of second
               TrigOutput res = my_trig(val1, val2);
               trig_results.push({i, val1, val2, res.sinVal, res.cosVal, false});
            });
         }
      });

      producer.join();
      pool.wait_until_empty();
      trig_results.push({0, 0.0, 0.0, 0.0, 0.0, true}); 
      std::cout << " [System] Batch 2 completed.\n\n";
   }

   std::cout << "=== SIMULATION COMPLETED. SHUTTING DOWN POOL ===\n";
}

//================================================================================ END
