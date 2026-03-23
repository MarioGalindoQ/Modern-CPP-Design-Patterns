/**
 * ============================================================================
 * File: SameParameters.cpp
 * 
 * --- DESIGN OVERVIEW:
 * It is not possible to create two constructors with the exact same 
 * parameter types (e.g., float, float). To solve this ambiguity, we use 
 * Named Constructors (static methods) that clearly describe the input.
 * ============================================================================
 */

#include <cmath>
#include <iostream>

class Point
{
private:
   float x_;
   float y_;

   // The true constructor is private
   // It internally stores rectangular coordinates
   Point(float x, float y) : x_{x}, y_{y} { } 

public:
   // Named constructors:
   static Point rectangular(float x, float y)
   {
      std::cout << " [System] A rectangular coordinate has been created.\n";
      return Point(x, y);
   }

   static Point polar(float radius, float angle)
   {
      std::cout << " [System] A polar coordinate has been created.\n";
      return Point(radius * std::cos(angle), radius * std::sin(angle));
   }

   void print() const
   {
      std::cout << "  -> Point(x: " << x_ << ", y: " << y_ << ")\n";
   }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== NAMED CONSTRUCTORS (SAME PARAMETERS) ===\n" << std::endl;

   Point p1 = Point::rectangular(5.7f, 1.2f);
   p1.print();

   std::cout << "\n";

   Point p2 = Point::polar(5.7f, 1.2f);
   p2.print();

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
