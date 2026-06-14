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
 * This program contrasts virtual dispatch against Data-Oriented Design (DOD)
 * principles to prove a fundamental rule:
 * 1. Predictable Execution: When a virtual function is invoked sequentially
 *    with a predictable type pattern, the hardware branch predictor caches the
 *    destination perfectly. This avoids branch misprediction stalls, isolating
 *    the unavoidable baseline overhead of vtable indirection (pointer chasing).
 * 2. Chaotic Execution: When pointers to heterogeneous types are interleaved
 *    randomly, the hardware predictor falls into a permanent state of
 *    misprediction, forcing massive CPU pipeline flushes. Additionally,
 *    shuffled pointer access triggers frequent 'Cache Misses'.
 * 3. Concrete (DOD) Execution: When invoking non-virtual (concrete) methods on
 *    homogeneously grouped objects, we bypass both indirection and branch
 *    prediction entirely. This enables aggressive compiler optimizations (like
 *    inlining) and maximizes instruction cache locality, delivering the highest
 *    possible hardware execution throughput.
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
#include <cmath>

//-------------------------------------------- CPU Warmup via FPU workload:
// Performs floating-point square root operations to force the CPU 
// out of low-power idle states (C-states) into its peak turbo frequency.
void warm_up_cpu(size_t iterations)
{
   std::cout << "--- Warming up the CPU ---\n" << std::endl;
   volatile double accumulator = 0.0;
   for (size_t i = 0; i < iterations; ++i)
   {
      accumulator += std::sqrt(static_cast<double>(i));
   }
}

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
   const size_t num_objects    = 10'000;
   const size_t num_passes     = 100'000;
   const size_t num_warm_cpu   = 1'000'000'000;

   std::cout << "=== POLYMORPHIC BOTTLENECK ANALYSIS ===\n" << std::endl;

   //--------------------------------------------------------------------------
   // TEST 1: PREDICTABLE MEMORY ACCESS (Orderly Vector Layout)
   // Hardware can predict the virtual call destination perfectly.
   //--------------------------------------------------------------------------
   std::cout << "--- [TEST 1] PREDICTABLE ACCESS (Orderly Object) ---" << std::endl;
   {
      std::vector<Base*> predictable_objects;
      predictable_objects.reserve(num_objects);

      for(size_t i = 0; i < num_objects; ++i)
         predictable_objects.push_back(new DerivedA());

      warm_up_cpu(num_warm_cpu);

      // Measure Concrete Call (Sequential pointer access, but same method)
      volatile size_t total_concrete = 0;
      auto start = std::chrono::steady_clock::now();
      for(size_t pass = 0; pass < num_passes; ++pass)
         for(size_t i = 0; i < num_objects; ++i)
            total_concrete += predictable_objects[i]->concrete();
      auto end = std::chrono::steady_clock::now();
      auto t_concrete = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

      // Measure Virtual Call (Sequential pointer access with predictable vtable jumps)
      volatile size_t total_virt = 0;
      start = std::chrono::steady_clock::now();
      for(size_t pass = 0; pass < num_passes; ++pass)
         for(size_t i = 0; i < num_objects; ++i)
            total_virt += predictable_objects[i]->virt();
      end = std::chrono::steady_clock::now();
      auto t_virtual = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

      std::cout << " Concrete function: " << t_concrete << " ms" << std::endl;
      std::cout << " Virtual function : " << t_virtual << " ms" << std::endl;
      std::cout << " Delta Overhead   : " << (t_virtual - t_concrete)
                << " ms (Predictable Virtual Overhead!)\n" << std::endl;

      for(auto obj : predictable_objects) delete obj;
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

      warm_up_cpu(num_warm_cpu);

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
