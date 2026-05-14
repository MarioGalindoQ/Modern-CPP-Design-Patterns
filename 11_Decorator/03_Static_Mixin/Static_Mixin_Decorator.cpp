/**
 * ============================================================================
 * File: Static_Mixin_Decorator.cpp
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the Static Decorator pattern, also known as 
 * "Mixin Inheritance". Unlike the Dynamic versions (Simple/Advanced), 
 * the composition is defined at compile-time using C++ Templates.
 * 
 * --- RELATIONSHIP NOTE:
 * In the Dynamic Decorator, the relationship is 'is a' AND 'has a' (composition).
 * In the Static Decorator, the relationship is strictly 'is a' (inheritance).
 * The static decorator "is a" Decorated, but it doesn't "have a" Decorated; it wraps the 
 * functionality by becoming part of the class hierarchy itself, eliminating 
 * the need for internal pointers.
 * 
 * --- THE PROBLEM:
 * In an industrial coffee machine factory, we design specialized machines 
 * that only produce one specific recipe (e.g., "The Sweet Latte Machine"). 
 * Since the recipe never changes during the machine's life, we don't need 
 * the flexibility of pointers or virtual tables. We need maximum performance 
 * and a small memory footprint.
 * 
 * --- THE SOLUTION:
 * We use templates where decorators inherit from their template argument:
 * 'class Milk : public Decorated'. This allows the compiler to "flatten" the 
 * inheritance chain, enabling aggressive optimizations like inlining.
 * This implementation also ensures that the natural language description 
 * ("with", "and", ",") is maintained with zero runtime overhead.
 *
 * Note:
 * Since we don't have a common 'Decorator' base class with a pointer, 
 * each template class handles the formatting logic independently to 
 * maintain the "Zero-overhead" principle.
 * ============================================================================
 */

#include <iostream>
#include <string>

//--------------------------------------------------------- Base Component:
class Coffee_ToBeDecorated
{
public:
   std::string getDescription() const
   {
      return "Coffee";
   }

   double getCost() const
   {
      return 2.0;
   }
};

//--------------------------------------------------------- Static Decorator A:
template <typename Decorated>
class Milk : public Decorated // Static Decorator "is a" Decorated,
{                             // but doesn't "have a" Decorated
public:
   std::string getDescription() const
   {
      std::string current = Decorated::getDescription();
      size_t andPos = current.find(" and ");
      size_t withPos = current.find(" with ");

      if(andPos != std::string::npos)
      {
         current.replace(andPos, 5, ", ");
         return current + " and Milk";
      }
      if(withPos != std::string::npos)
      {
         return current + " and Milk";
      }
      return current + " with Milk";
   }

   double getCost() const
   {
      return Decorated::getCost() + 0.5;
   }
};

//--------------------------------------------------------- Static Decorator B:
template <typename Decorated>
class Sugar : public Decorated // Static Decorator "is a" Decorated,
{                              // but doesn't "have a" Decorated
public:
   std::string getDescription() const
   {
      std::string current = Decorated::getDescription();
      size_t andPos = current.find(" and ");
      size_t withPos = current.find(" with ");

      if(andPos != std::string::npos)
      {
         current.replace(andPos, 5, ", ");
         return current + " and Sugar";
      }
      if(withPos != std::string::npos)
      {
         return current + " and Sugar";
      }
      return current + " with Sugar";
   }

   double getCost() const
   {
      return Decorated::getCost() + 0.2;
   }
};

//--------------------------------------------------------- Static Decorator C:
template <typename Decorated>
class Vanilla : public Decorated // Static Decorator "is a" Decorated,
{                                // but doesn't "have a" Decorated
public:
   std::string getDescription() const
   {
      std::string current = Decorated::getDescription();
      size_t andPos = current.find(" and ");
      size_t withPos = current.find(" with ");

      if(andPos != std::string::npos)
      {
         current.replace(andPos, 5, ", ");
         return current + " and Vanilla";
      }
      if(withPos != std::string::npos)
      {
         return current + " and Vanilla";
      }
      return current + " with Vanilla";
   }

   double getCost() const
   {
      return Decorated::getCost() + 0.7;
   }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== INDUSTRIAL COFFEE FACTORY (STATIC DECORATORS) ===\n" << std::endl;

   // Recipe 1: Basic Latte (Milk<Coffee_ToBeDecorated>)
   Milk<Coffee_ToBeDecorated> latte;
   std::cout << " Recipe 1 (Latte): " << latte.getDescription() << std::endl;
   std::cout << " Static Cost: $" << latte.getCost() << std::endl;

   // Recipe 2: Sweet Latte (Sugar<Milk<Coffee_ToBeDecorated>>)
   Sugar<Milk<Coffee_ToBeDecorated>> sweetLatte;
   std::cout << "\n Recipe 2 (Sweet Latte): " << sweetLatte.getDescription() << std::endl;
   std::cout << " Static Cost: $" << sweetLatte.getCost() << std::endl;

   // Recipe 3: Vanilla Dream (Vanilla<Sugar<Milk<Coffee_ToBeDecorated>>>)
   // Now this will correctly print: "Coffee_ToBeDecorated with Milk, Sugar and Vanilla"
   Vanilla<Sugar<Milk<Coffee_ToBeDecorated>>> vanillaDream;
   std::cout << "\n Recipe 3 (Vanilla Dream): " << vanillaDream.getDescription() << std::endl;
   std::cout << " Static Cost: $" << vanillaDream.getCost() << std::endl;

   std::cout << "\n--- Production lines are optimized. No pointers used. ---\n";
}

//================================================================================ END
