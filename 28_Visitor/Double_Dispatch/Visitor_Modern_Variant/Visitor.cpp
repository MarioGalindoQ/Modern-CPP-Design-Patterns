/**
 * ============================================================================
 * File: Visitor_Modern.cpp (Optimized Variant Version)
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the Visitor pattern using std::variant.
 * We optimize the "Double Dispatch" matrix to avoid redundant calculations:
 * 1. Intersection(A, B) is treated as the same as Intersection(B, A).
 * 2. We skip self-intersection (A, A) if not desired.
 * 3. We use nested loops: for (i=0; i<N; ++i) for (j=i+1; j<N; ++j).
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <variant>
#include <algorithm>

//--------------------------------------------------------- Plain Data Structs:
struct Circle   { int id_; explicit Circle(int id) : id_{id} {} };
struct Triangle { int id_; explicit Triangle(int id) : id_{id} {} };

using Shape = std::variant<Circle, Triangle>;

//--------------------------------------------------------- Interaction Logic:
struct CollisionEngine
{
   // Helper to perform the actual math once and reuse it
   void intersect(int id1, int id2, const std::string& type) const
   {
      std::cout << " [System] Collision detected between " << type 
                << " [" << id1 << "] and [" << id2 << "]\n";
   }

   // Implementation for Circle <-> Circle
   void operator()(const Circle& a, const Circle& b) const
   {
      if (a.id_ != b.id_) intersect(a.id_, b.id_, "Circle-Circle");
   }

   // Implementation for Triangle <-> Triangle
   void operator()(const Triangle& a, const Triangle& b) const
   {
      if (a.id_ != b.id_) intersect(a.id_, b.id_, "Triangle-Triangle");
   }

   // Implementation for Circle <-> Triangle
   void operator()(const Circle& a, const Triangle& b) const
   {
      intersect(a.id_, b.id_, "Circle-Triangle");
   }

   // Implementation for Triangle <-> Circle (Reverse order)
   // We simply redirect to the Circle-Triangle logic to maintain OCP
   void operator()(const Triangle& a, const Circle& b) const
   {
      (*this)(b, a);
   }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== MODERN VARIANT DISPATCH (OPTIMIZED) ===\n" << std::endl;

   std::vector<Shape> shapes;
   shapes.emplace_back(Circle{1});
   shapes.emplace_back(Triangle{2});
   shapes.emplace_back(Circle{3});
   shapes.emplace_back(Triangle{4});

   // Optimized O(N(N-1)/2) loop
   for (size_t i = 0; i < shapes.size(); ++i)
   {
      for (size_t j = i + 1; j < shapes.size(); ++j)
      {
         std::visit(CollisionEngine{}, shapes[i], shapes[j]);
      }
   }

   std::cout << "\n=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
