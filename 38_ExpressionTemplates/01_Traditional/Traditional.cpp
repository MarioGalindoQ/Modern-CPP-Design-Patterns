/**
 * ============================================================================
 * File: Traditional.cpp
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the traditional approach to operator overloading 
 * for mathematical vectors, implemented using modern C++ standards. It 
 * represents a robust, professional-grade implementation that fully utilizes 
 * Move Semantics to optimize memory management.
 * 
 * --- ARCHITECTURAL ANALYSIS:
 * Although this implementation avoids unnecessary deep copies by recycling 
 * temporary buffers (Move Semantics), it remains fundamentally limited by the 
 * "Memory Wall". In a complex expression like R = A + 2.0*B + 3.0*C:
 * 1. Sequential Execution: Each operator (+, *) triggers its own independent 
 *    loop, forcing the CPU to traverse the data multiple times.
 * 2. Memory Pressure: Intermediate temporary results still require full-sized 
 *    buffers (~8.94 GiB each). At the peak of evaluation, the total memory 
 *    requirement exceeds physical RAM, triggering OS Swapping and increasing 
 *    latency.
 * 
 * --- PERFORMANCE NOTES:
 * This baseline profiles the maximum throughput achievable through standard 
 * polymorphism and buffer recycling before introducing Loop Fusion.
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <utility>

// CALIBRATION CONSTANTS:
// 1.2 billion elements require ~8.94 GB of RAM per vector.
const size_t VECTOR_SIZE = 1'200'000'000; 

// Calibrate WARMUP_ITERATIONS to achieve approximately 5 seconds of 
// execution time to stabilize CPU frequency and cache lines.
const size_t WARMUP_ITERATIONS = 3'000'000'000;

//--------------------------------------------------------- Traditional Vector:
class Vector
{
private:
   std::vector<double> data_;

public:
   // Constructors
   explicit Vector(size_t size) : data_(size) { }
   Vector(size_t size, double value) : data_(size, value) { }

   // Rule of Five (Standard Modern C++ requirement)
   Vector(const Vector&) = default;
   Vector& operator=(const Vector&) = default;
   Vector(Vector&&) noexcept = default;
   Vector& operator=(Vector&&) noexcept = default;
   ~Vector() = default;

   size_t size() const { return data_.size(); }

   double operator[](size_t i) const { return data_[i]; }
   double& operator[](size_t i) { return data_[i]; }

   // --- Addition Operators (Overloaded to handle all Value Categories) ---

   // Lvalue + Lvalue
   friend Vector operator+(const Vector& lhs, const Vector& rhs)
   {
      Vector result(lhs.size());
      for (size_t i = 0; i < lhs.size(); ++i) result[i] = lhs[i] + rhs[i];
      return result;
   }

   // Rvalue + Lvalue (Recycle LHS)
   friend Vector operator+(Vector&& lhs, const Vector& rhs)
   {
      for (size_t i = 0; i < lhs.size(); ++i) lhs.data_[i] += rhs.data_[i];
      return std::move(lhs);
   }

   // Lvalue + Rvalue (Recycle RHS)
   friend Vector operator+(const Vector& lhs, Vector&& rhs)
   {
      for (size_t i = 0; i < rhs.size(); ++i) rhs.data_[i] += lhs.data_[i];
      return std::move(rhs);
   }

   // Rvalue + Rvalue (Recycle LHS - Resolves Ambiguity)
   friend Vector operator+(Vector&& lhs, Vector&& rhs)
   {
      for (size_t i = 0; i < lhs.size(); ++i) lhs.data_[i] += rhs.data_[i];
      return std::move(lhs);
   }

   // --- Multiplication Operators ---

   // Scalar * Lvalue
   friend Vector operator*(double scalar, const Vector& vec)
   {
      Vector result(vec.size());
      for (size_t i = 0; i < vec.size(); ++i) result[i] = scalar * vec[i];
      return result;
   }

   // Scalar * Rvalue (Recycle buffer)
   friend Vector operator*(double scalar, Vector&& vec)
   {
      for (size_t i = 0; i < vec.size(); ++i) vec.data_[i] *= scalar;
      return std::move(vec);
   }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== EXPRESSION TEMPLATES: TRADITIONAL BASELINE ===\n" << std::endl;

   // 1. DATA INITIALIZATION
   std::cout << " [1/4] Initializing vectors of size: " << VECTOR_SIZE << "...\n";
   Vector A(VECTOR_SIZE, 1.0);
   Vector B(VECTOR_SIZE, 2.0);
   Vector C(VECTOR_SIZE, 3.0);
   Vector R(VECTOR_SIZE);

   // 2. CPU WARM-UP PHASE
   std::cout << " [2/4] Warming up CPU (Target: ~5 seconds)..." << std::endl;
   volatile double warm = 0.0;
   for (size_t i = 0; i < WARMUP_ITERATIONS; ++i) warm += std::sqrt(static_cast<double>(i));

   // 3. BENCHMARK MEASUREMENT
   std::cout << " [3/4] Executing: R = A + 2.0 * B + 3.0 * C ..." << std::endl;
   auto start = std::chrono::high_resolution_clock::now();

   // This operation triggers the 'Memory Wall' bottleneck.
   // Peak memory usage will reach its maximum during the evaluation of sub-expressions.
   R = A + 2.0 * B + 3.0 * C;

   auto end = std::chrono::high_resolution_clock::now();
   std::chrono::duration<double> elapsed = end - start;

   // 4. RESULTS REPORT AND VERIFICATION
   std::cout << " [4/4] Verification - R[0]: " << R[0] << " (Expected: 14)\n";
   std::cout << "\n Elapsed time: " << elapsed.count() << " seconds.\n";

   std::cout << "\n=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
