/**
 * ============================================================================
 * File: Expressions_CRTP.cpp
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the "Expression Templates" pattern using the 
 * Curiously Recurring Template Pattern (CRTP). It solves the massive memory 
 * and speed bottlenecks of the traditional approach by using "Lazy Evaluation" 
 * and "Loop Fusion".
 * 
 * --- THE ARCHITECTURAL MAGIC:
 * 1. Expression Proxy: Operators (+, *) no longer perform immediate calculations. 
 *    Instead, they return lightweight proxy objects (representing the Abstract 
 *    Syntax Tree) at compile-time with zero allocations.
 * 2. Loop Fusion: The entire mathematical expression is fused into a single, 
 *    contiguous 'for' loop inside the assignment operator of the 'Vector' class.
 * 3. Zero-Allocation: No intermediate temporary Vector objects (like 8.94 GB 
 *    buffers) are created on the heap during the evaluation of the expression.
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <cassert>

// CALIBRATION CONSTANTS (Identical to Traditional.cpp for a fair benchmark)
const size_t VECTOR_SIZE       = 1'200'000'000;
const size_t WARMUP_ITERATIONS = 3'000'000'000;

//--------------------------------------------------------- 1. CRTP Base Class:
// This empty interface forces compile-time polymorphism without virtual tables.
// It ensures that all expressions share a common structural contract.
template <class Derived>
class VecExpression
{
public:
   size_t size() const { return static_cast<const Derived&>(*this).size(); }
   double operator[](size_t i) const { return static_cast<const Derived&>(*this)[i]; }
};

//--------------------------------------------------------- 2. Vector Container:
class Vector : public VecExpression<Vector>
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

   // Lazy Evaluation Expression Constructor
   // This constructor accepts ANY abstract expression tree and evaluates it
   // in a single, continuous, highly optimized loop.
   template <class Expression>
   Vector(const VecExpression<Expression>& expr) : data_(expr.size())
   {
      for (size_t i = 0; i < expr.size(); ++i)
         data_[i] = expr[i];
   }

   // Lazy Evaluation Assignment Operator
   // Triggers the actual evaluation of the expression tree directly into the target memory.
   template <class Expression>
   Vector& operator=(const VecExpression<Expression>& expr)
   {
      assert(size() == expr.size());
      for (size_t i = 0; i < expr.size(); ++i)
         data_[i] = expr[i]; // Loop fusion happens right here
      return *this;
   }
};

//--------------------------------------------------------- 3. Expression Node for Addition:
// Represents a delayed sum of two independent expressions.
template <class LHS_Expr, class RHS_Expr>
class VecSum : public VecExpression<VecSum<LHS_Expr, RHS_Expr>>
{
private:
   const LHS_Expr& l_; // Reference to the left-hand side expression operand
   const RHS_Expr& r_; // Reference to the right-hand side expression operand

public:
   VecSum(const LHS_Expr& lhs, const RHS_Expr& rhs) : l_(lhs), r_(rhs) { }

   size_t size() const { return l_.size(); }

   // Inline element access: propagates index requests down the expression tree
   double operator[](size_t i) const { return l_[i] + r_[i]; }
};

//--------------------------------------------------------- 4. Expression Node for Scaling:
// Represents a delayed multiplication of a scalar value and an expression.
template <class Expression>
class VecScale : public VecExpression<VecScale<Expression>>
{
private:
   double           scalar_; // The scaling factor
   const Expression& v_;     // Reference to the expression being scaled

public:
   VecScale(double s, const Expression& vec) : scalar_(s), v_(vec) { }

   size_t size() const { return v_.size(); }

   // Inline element access: performs scalar multiplication on the fly
   double operator[](size_t i) const { return scalar_ * v_[i]; }
};

//--------------------------------------------------------- 5. Operator Overloads:
// These operators DO NOT execute any loops or allocate heap memory.
// They simply deduce and assemble the type structure of the calculation tree.

// Non-member operator+ for two arbitrary expressions
template <class L, class R>
VecSum<L, R> operator+(const VecExpression<L>& a, const VecExpression<R>& b)
{
   return VecSum<L, R>(static_cast<const L&>(a), static_cast<const R&>(b));
}

// Non-member operator* for a scalar multiplying an arbitrary expression
template <class E>
VecScale<E> operator*(double scalar, const VecExpression<E>& vec)
{
   return VecScale<E>(scalar, static_cast<const E&>(vec));
}

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== EXPRESSION TEMPLATES: CRTP & LOOP FUSION ===\n" << std::endl;

   // 1. DATA INITIALIZATION (Identical sequence to traditional version)
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
    * THE EXPRESSION TEMPLATE MAGIC:
    * 
    * The compiler deduces the exact type structure of the operation as:
    * VecSum<VecSum<Vector, VecScale<Vector>>, VecScale<Vector>>
    * 
    * Visually, the compiled static AST looks like this:
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
