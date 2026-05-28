/**
 * ============================================================================
 * File: Polymorphic_bottleneck.cpp
 * Author: Mario Galindo Queralt, Ph.D.
 *
 * --- DESIGN OVERVIEW:
 * This program serves as a performance baseline analysis to evaluate the real
 * computational cost of Dynamic Dispatch (Virtual Functions) under different
 * memory organization layouts. It contrasts classical Object-Oriented (OOP)
 * polymorphism against Data-Oriented Design (DOD) principles.
 *
 * --- DYNAMIC DISPATCH & HARDWARE COOPERATION:
 * Classical C++ polymorphism relies on runtime vtable lookups. However, modern
 * CPU architectures mitigate this overhead via the Branch Target Buffer (BTB).
 * This program isolates two distinct scenarios to prove a fundamental rule:
 * 1. Predictable Execution: When a virtual function is invoked sequentially 
 *    on a stable memory address, the hardware branch predictor caches the 
 *    destination, yielding near zero-overhead performance.
 * 2. Chaotic Execution: When pointers to heterogeneous types are interleaved 
 *    randomly, the hardware predictor falls into a permanent state of 
 *    misprediction, forcing massive CPU pipeline flushes. Additionally, 
 *    shuffled pointer access triggers frequent 'Cache Misses'.
 *
 * --- COMPILER TIMING SAFEGUARDS:
 * High-level optimization flags (-O3) tend to pre-calculate or eliminate 
 * loops that accumulate constant values at compile-time. To ensure honest 
 * hardware profiling without assembly-level trickery, this implementation:
 * 1. Employs 'volatile' type qualifiers on accumulators to enforce strict 
 *    read/write operations on memory per iteration.
 * 2. All functions return the same constant (7) to ensure the ALU performs 
 *    the exact same mathematical workload in every test case.
 * ============================================================================
 */

#include <iostream>
#include <chrono>
#include <vector>
#include <random>
#include <algorithm>

//--------------------------------------------------------- Test Hierarchy:
class Base
{
public:
   int concrete() const { return 7; }
   virtual int virt() const = 0;
   virtual ~Base() = default;
};

class DerivedA : public Base
{
public:
   int virt() const override { return 7; }
};

class DerivedB : public Base
{
public:
   int virt() const override { return 7; }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   // WORKLOAD BOUNDS: 1,000,000,000 operations total per test.
   // num_iterations = num_objects * num_passes
   const size_t num_iterations = 1000000000;
   const size_t num_objects    = 10000;
   const size_t num_passes     = 100000;
   const size_t num_warm_cpu   = 10;

   std::cout << "=== POLYMORPHIC BOTTLENECK ANALYSIS ===\n" << std::endl;

   // Wakeup the cpu for a fine time measurement
   for(size_t iWarm = 0; iWarm < num_warm_cpu; iWarm++)
   {
      DerivedA* derived = new DerivedA();
      Base* b = derived;

      volatile size_t total_concrete = 0;
      for(size_t i = 0; i < num_iterations; i++)
         total_concrete += b->concrete();

      delete derived;
   }

   //--------------------------------------------------------------------------
   // TEST 1: PREDICTABLE MEMORY ACCESS (Single Object Loop)
   // Hardware can predict the virtual call destination perfectly.
   //--------------------------------------------------------------------------
   std::cout << "--- [TEST 1] PREDICTABLE ACCESS (Single Object) ---" << std::endl;
   {
      DerivedA* derived = new DerivedA();
      Base* b = derived;

      // Measure Concrete Call
      volatile size_t total_concrete = 0;
      auto start = std::chrono::steady_clock::now();
      for(size_t i = 0; i < num_iterations; i++)
         total_concrete += b->concrete();
      auto end = std::chrono::steady_clock::now();
      auto t_concrete = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

      // Measure Virtual Call
      volatile size_t total_virt = 0;
      start = std::chrono::steady_clock::now();
      for(size_t i = 0; i < num_iterations; i++)
         total_virt += b->virt();
      end = std::chrono::steady_clock::now();
      auto t_virtual = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

      std::cout << " Concrete function: " << t_concrete << " ms" << std::endl;
      std::cout << " Virtual function : " << t_virtual << " ms" << std::endl;
      std::cout << " Delta Overhead   : " << (t_virtual - t_concrete)
                << " ms (Minimal)\n" << std::endl;

      delete derived;
   }

   //--------------------------------------------------------------------------
   // TEST 2: CHAOTIC MEMORY ACCESS (Shuffled Vector Layout)
   // Forces Branch Mispredictions and Cache Misses.
   //--------------------------------------------------------------------------
   std::cout << "--- [TEST 2] CHAOTIC ACCESS (Shuffled Layout) ---" << std::endl;
   {
      std::vector<Base*> chaotic_objects;
      chaotic_objects.reserve(num_objects);

      for(size_t i = 0; i < num_objects / 2; ++i)
      {
         chaotic_objects.push_back(new DerivedA());
         chaotic_objects.push_back(new DerivedB());
      }

      // Randomize the order to destroy hardware predictability
      std::random_device rd;
      std::mt19937 g(rd());
      std::shuffle(chaotic_objects.begin(), chaotic_objects.end(), g);

      // Measure Concrete Call (Sequential pointer access, but same method)
      volatile size_t total_concrete = 0;
      auto start = std::chrono::steady_clock::now();
      for(size_t pass = 0; pass < num_passes; ++pass)
         for(size_t i = 0; i < num_objects; ++i)
            total_concrete += chaotic_objects[i]->concrete();
      auto end = std::chrono::steady_clock::now();
      auto t_concrete = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

      // Measure Virtual Call (Sequential pointer access, but chaotic vtable jumps)
      volatile size_t total_virt = 0;
      start = std::chrono::steady_clock::now();
      for(size_t pass = 0; pass < num_passes; ++pass)
         for(size_t i = 0; i < num_objects; ++i)
            total_virt += chaotic_objects[i]->virt();
      end = std::chrono::steady_clock::now();
      auto t_virtual = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

      std::cout << " Concrete function: " << t_concrete << " ms" << std::endl;
      std::cout << " Virtual function : " << t_virtual << " ms" << std::endl;
      std::cout << " Delta Overhead   : " << (t_virtual - t_concrete)
                << " ms (MASSIVE PENALTY!)\n" << std::endl;

      for(auto obj : chaotic_objects) delete obj;
   }

   std::cout << "=== ANALYSIS COMPLETED ===" << std::endl;
}

//============================================================================== END
