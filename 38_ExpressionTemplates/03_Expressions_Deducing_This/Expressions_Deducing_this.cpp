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

// CALIBRATION CONSTANTS (Identical to Traditional.cpp for a fair benchmark)
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
   //double& operator[](size_t i) { return data_[i]; }

   // Lazy Evaluation Assignment Operator
   // This is where the "Loop Fusion" occurs.
   // It accepts any expression node and evaluates it in a single pass.
   template <class Expression>
   Vector& operator=(const Expression& expr)
   {
      for(size_t i = 0; i < expr.size(); ++i)
         data_[i] = expr[i]; // data_[i] = 2.0 * ( A[i] + (3.0 * B[i]) + (4.0 * C[i]) )
      return *this;
   }
};

//--------------------------------------------------------- 3. VecSum Node:
// Represents a delayed sum of two independent expressions.
template <class LHS_Expr, class RHS_Expr>
class VecSum : public VecExpression
{
private:
   const LHS_Expr& lhs_; // Reference to the left-hand side expression operand
   const RHS_Expr& rhs_; // Reference to the right-hand side expression operand

public:
   VecSum(const LHS_Expr& lhs, const RHS_Expr& rhs) : lhs_(lhs), rhs_(rhs) { }

   size_t size() const { return lhs_.size(); }

   // Inline element access: propagates index requests down the expression tree
   double operator[](size_t i) const { return lhs_[i] + rhs_[i]; }
};

//--------------------------------------------------------- 4. VecScale Node:
// Represents a delayed multiplication of a scalar value and an expression.
template <class Expression>
class VecScale : public VecExpression
{
private:
   double            factor_; // The scaling factor
   const Expression& expr_;   // Reference to the expression being scaled

public:
   VecScale(double factor, const Expression& expr) : factor_(factor), expr_(expr) { }

   size_t size() const { return expr_.size(); }

   // Inline element access: performs scalar multiplication on the fly
   double operator[](size_t i) const { return factor_ * expr_[i]; }
};

//--------------------------------------------------------- 5. Operator Overloads:
// These operators DO NOT execute any loops or allocate heap memory.
// They simply deduce and assemble the Abstract Syntax Tree (AST) at compile-time.

// Non-member operator+ for two arbitrary expressions
template <class LHS_Expr, class RHS_Expr>
auto operator+(const LHS_Expr& lhs, const RHS_Expr& rhs)
{
   return VecSum<LHS_Expr, RHS_Expr>(lhs, rhs);
}

// Non-member operator* for a scalar factor multiplying an arbitrary expression
template <class Expression>
auto operator*(double factor, const Expression& expr)
{
   return VecScale<Expression>(factor, expr);
}

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== EXPRESSION TEMPLATES: DEDUCING THIS (C++23) ===\n" << std::endl;

   // 1. DATA INITIALIZATION (Identical sequence to traditional version)
   std::cout << " [1/4] Initializing vectors of size: " << VECTOR_SIZE << "...\n";
   Vector A(VECTOR_SIZE, 1.0);
   Vector B(VECTOR_SIZE, 2.0);
   Vector C(VECTOR_SIZE, 3.0);
   Vector R(VECTOR_SIZE);

   // 2. CPU WARM-UP PHASE
   std::cout << " [2/4] Warming up CPU (Target: ~5 seconds)..." << std::endl;
   volatile double warm = 0.0;
   for(size_t i = 0; i < WARMUP_ITERATIONS; ++i)
      warm += std::sqrt(static_cast<double>(i));

   // 3. BENCHMARK MEASUREMENT
   std::cout << " [3/4] Executing: R = 2.0 * ( A + 3.0 * B + 4.0 * C ) ..." << std::endl;
   auto start = std::chrono::high_resolution_clock::now();

   /**
    * THE EXPRESSION TEMPLATE MAGIC:
    *
    * Unlike the traditional approach, no temporary vectors are created.
    * The compiler builds a static AST and executes a single loop.
    *
    * Visually, the compiled static AST looks like this:
    *
    *                VecScale(2*(A + 3*B + 4*C))
    *                   |
    *                VecSum(A + 3*B + 4*C)
    *               /       \
    *         VecSum(A+3*B)  VecScale(4*C)
    *        /      \
    *    Vector(A)   VecScale(3*B)
    *
    * When assigned to R, the operator= triggers a single, fused, highly
    * optimized loop: R[i] = 2.0 * ( A[i] + (3.0 * B[i]) + (4.0 * C[i]) )
    *
    * Zero temporary vectors are allocated on the heap during evaluation.
    */
   R = 2.0 * ( A + 3.0 * B + 4.0 * C );

   auto end = std::chrono::high_resolution_clock::now();
   std::chrono::duration<double> elapsed = end - start;

   // 4. RESULTS REPORT AND VERIFICATION
   std::cout << " [4/4] Verification - R[0]: " << R[0] << " (Expected: 38)\n";
   std::cout << "\n Elapsed time: " << elapsed.count() << " seconds.\n";

   std::cout << "\n=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
