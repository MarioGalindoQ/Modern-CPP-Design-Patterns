/**
 * ============================================================================
 * File: Expressions_Deducing_this.cpp
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This program implements the "Expression Templates" pattern using the 
 * modern C++23 "Deducing This" feature (Explicit Object Parameters). It 
 * represents the state-of-the-art evolution of high-performance mathematical 
 * DSLs (Domain Specific Languages) in C++.
 * 
 * --- THE UNIFIED INTERFACE ADVANTAGE:
 * Unlike more complex implementations where operators (+, *) must be 
 * redefined inside every node type (Vector, VecSum, VecScale), we introduce 
 * a non-template base class: 'VecExpression'. 
 * 
 * By having all expression nodes inherit from this single interface:
 * 1. Code Reusability (DRY): Mathematical operators are defined once as 
 *    global templates. Because they target 'VecExpression', they 
 *    automatically work for any current or future node in the tree.
 * 2. Static Contract: It enforces a uniform interface (size and operator[]) 
 *    across the entire hierarchy without the overhead of virtual functions.
 * 
 * --- DEDUCING THIS (C++23) vs. CRTP:
 * This version eliminates the "Curiously Recurring" template syntax. By using 
 * 'this auto&& self' in the base class methods, the compiler automatically 
 * deduces the concrete derived type (Vector, VecSum, etc.) at the call site. 
 * This removes the need for 'static_cast' and makes the code significantly 
 * more readable while maintaining identical zero-overhead performance.
 * 
 * --- PERFORMANCE & LOOP FUSION:
 * The traditional "Naive" approach creates costly temporary vectors for 
 * every sub-expression. This implementation:
 * 1. Eliminates Temporaries: Operations return lightweight proxies that 
 *    act as an Abstract Syntax Tree (AST).
 * 2. Single-Pass Execution: The entire calculation is collapsed into one 
 *    continuous loop inside the assignment operator, maximizing CPU cache 
 *    locality and overcoming the "Memory Wall" bottleneck.
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <cassert>

// CALIBRATION CONSTANTS (Identical to previous versions)
const size_t VECTOR_SIZE       = 1'200'000'000;
const size_t WARMUP_ITERATIONS = 3'000'000'000;

//--------------------------------------------------------- 1. Base Class:
// C++23 Base: Uses Explicit Object Parameters to deduce the derived type.
class VecExpression
{
public:
   // 'this auto&& self' deduces if we are a Vector, a VecSum, or a VecScale.
   auto size(this auto&& self) { return self.size(); }
   auto operator[](this auto&& self, size_t i) { return self[i]; }
};

//--------------------------------------------------------- 2. Vector Container:
class Vector : public VecExpression
{
private:
   std::vector<double> data_;

public:
   // Constructors
   explicit Vector(size_t size) : data_(size) { }
   Vector(size_t size, double value) : data_(size, value) { }

   size_t size() const { return data_.size(); }
   double operator[](size_t i) const { return data_[i]; }
   double& operator[](size_t i) { return data_[i]; }

   // Lazy Evaluation Assignment Operator
   // This is where the "Loop Fusion" occurs. 
   // It accepts any expression node and evaluates it in a single pass.
   template <class Expression>
   Vector& operator=(const Expression& expr)
   {
      assert(size() == expr.size());
      for (size_t i = 0; i < data_.size(); ++i)
         data_[i] = expr[i]; 
      return *this;
   }
};

//--------------------------------------------------------- 3. VecSum Node:
template <class LHS_Expr, class RHS_Expr>
class VecSum : public VecExpression
{
private:
   const LHS_Expr& l_;
   const RHS_Expr& r_;

public:
   VecSum(const LHS_Expr& lhs, const RHS_Expr& rhs) : l_(lhs), r_(rhs) { }

   size_t size() const { return l_.size(); }
   double operator[](size_t i) const { return l_[i] + r_[i]; }
};

//--------------------------------------------------------- 4. VecScale Node:
template <class RHS_Expr>
class VecScale : public VecExpression
{
private:
   double          scalar_;
   const RHS_Expr& v_;

public:
   VecScale(double s, const RHS_Expr& vec) : scalar_(s), v_(vec) { }

   size_t size() const { return v_.size(); }
   double operator[](size_t i) const { return scalar_ * v_[i]; }
};

//--------------------------------------------------------- 5. Operator Overloads:
// These functions build the Abstract Syntax Tree (AST) at compile-time.

template <class LHS_Expr, class RHS_Expr>
auto operator+(const LHS_Expr& a, const RHS_Expr& b)
{
   return VecSum<LHS_Expr, RHS_Expr>(a, b);
}

template <class Expression>
auto operator*(double scalar, const Expression& vec)
{
   return VecScale<Expression>(scalar, vec);
}

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== EXPRESSION TEMPLATES: DEDUCING THIS (C++23) ===\n" << std::endl;

   // 1. DATA INITIALIZATION
   std::cout << " [1/4] Initializing vectors of size: " << VECTOR_SIZE << "...\n";
   Vector A(VECTOR_SIZE, 1.0);
   Vector B(VECTOR_SIZE, 2.0);
   Vector C(VECTOR_SIZE, 3.0);
   Vector R(VECTOR_SIZE);

   // 2. CPU WARM-UP PHASE
   std::cout << " [2/4] Warming up CPU (Target: ~5 seconds)..." << std::endl;
   volatile double warm = 0.0;
   for (size_t i = 0; i < WARMUP_ITERATIONS; ++i)
      warm += std::sqrt(static_cast<double>(i));

   // 3. BENCHMARK MEASUREMENT
   std::cout << " [3/4] Executing: R = A + 2.0 * B + 3.0 * C ..." << std::endl;
   auto start = std::chrono::high_resolution_clock::now();

   /**
    * THE MAGIC OF FUSED EVALUATION:
    * 
    * Unlike the traditional approach, no temporary vectors are created.
    * The compiler builds a static AST and executes a single loop.
    * 
    * Visually, the tree being evaluated looks like this:
    * 
    *                VecSum (Outer Addition)
    *               /       \
    *         VecSum         VecScale (3.0 * C)
    *        /      \
    *    Vector     VecScale (2.0 * B)
    *     (A)
    *
    * When assigned to R, the operator= triggers a single, fused, highly 
    * optimized loop: R[i] = A[i] + (2.0 * B[i]) + (3.0 * C[i])
    *
    * Zero temporary vectors are allocated on the heap during evaluation.
    */
   R = A + 2.0 * B + 3.0 * C;

   auto end = std::chrono::high_resolution_clock::now();
   std::chrono::duration<double> elapsed = end - start;

   // 4. RESULTS REPORT AND VERIFICATION
   std::cout << " [4/4] Verification - R[0]: " << R[0] << " (Expected: 14)\n";
   std::cout << "\n Elapsed time: " << elapsed.count() << " seconds.\n";

   std::cout << "\n=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
