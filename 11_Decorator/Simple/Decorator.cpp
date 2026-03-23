/**
 * ============================================================================
 * File: Decorator.cpp (Simple Version)
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the Decorator pattern with advanced string 
 * formatting logic. It builds a natural description:
 * 0 base:         "Coffe"
 * 1 supplement:   "Coffee with Milk"
 * 2 supplements:  "Coffee with Milk and Sugar"
 * 3+ supplements: "Coffee with Milk, Sugar and Vanilla"
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <memory>

//--------------------------------------------------------- Base Interface:
class Decorated
{
public:
   virtual ~Decorated() = default;
   virtual std::string getDescription() const = 0;
   virtual double getCost() const = 0;
};

//----------------------------------------------------- Concrete Decorated:
class Coffee : public Decorated
{
public:
   ~Coffee() override
   {
      std::cout << " [Cleanup] Coffee base object destroyed.\n";
   }

   std::string getDescription() const override
   {
      return "Coffee";
   }

   double getCost() const override
   {
      return 2.0; 
   }
};

//-------------------------------------------------------- Base Decorator:
class Decorator : public Decorated         // "is a" Decorated
{
protected:
   std::unique_ptr<Decorated> decorated_;  //  and "has a" Decorated

   // Smart logic to build a natural English sentence
   std::string wrapDescription(const std::string& ingredient) const
   {
      std::string current = decorated_->getDescription();
      
      size_t andPos = current.find(" and ");
      size_t withPos = current.find(" with ");

      // Case 1: We already have an "and" (e.g., "Coffee with Milk and Sugar")
      // Replace the old " and " with ", " and add the new " and ingredient"
      if (andPos != std::string::npos)
      {
         current.replace(andPos, 5, ", ");
         return current + " and " + ingredient;
      }

      // Case 2: We have a "with" but no "and" (e.g., "Coffee with Milk")
      // Just add " and ingredient"
      if (withPos != std::string::npos)
      {
         return current + " and " + ingredient;
      }

      // Case 3: No supplements yet (e.g., "Coffee")
      // Add the first " with ingredient"
      return current + " with " + ingredient;
   }

public:
   explicit Decorator(std::unique_ptr<Decorated> target)
      : decorated_{std::move(target)} { }

   ~Decorator() override
   {
      std::cout << " [Cleanup] Ingredient wrapper destroyed.\n";
   }

   std::string getDescription() const override
   {
      return decorated_->getDescription();
   }

   double getCost() const override
   {
      return decorated_->getCost();
   }
};

//----------------------------------------------------- Concrete Decorator A:
class Milk : public Decorator
{
public:
   using Decorator::Decorator;

   std::string getDescription() const override
   {
      return wrapDescription("Milk");
   }

   double getCost() const override
   {
      return decorated_->getCost() + 0.5;
   }
};

//----------------------------------------------------- Concrete Decorator B:
class Sugar : public Decorator
{
public:
   using Decorator::Decorator;

   std::string getDescription() const override
   {
      return wrapDescription("Sugar");
   }

   double getCost() const override
   {
      return decorated_->getCost() + 0.2;
   }
};

//----------------------------------------------------- Concrete Decorator C:
class Vanilla : public Decorator
{
public:
   using Decorator::Decorator;

   std::string getDescription() const override
   {
      return wrapDescription("Vanilla");
   }

   double getCost() const override
   {
      return decorated_->getCost() + 0.7;
   }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== COFFEE SHOP (SIMPLE DECORATOR) ===\n" << std::endl;

   // We build a Coffee with Milk, Sugar and Vanilla.
   // --- Step-by-step ordering (Dynamic Decoration) ---
   
   // 1. Start with a basic Coffee
   std::unique_ptr<Decorated> myDrink = std::make_unique<Coffee>();
   std::cout << " Order: " << myDrink->getDescription();
   std::cout << ", Cost: $" << myDrink->getCost() << std::endl;

   // 2. Add Milk
   myDrink = std::make_unique<Milk>(std::move(myDrink));
   std::cout << " Order: " << myDrink->getDescription();
   std::cout << ", Cost: $" << myDrink->getCost() << std::endl;

   // 3. Add Sugar
   myDrink = std::make_unique<Sugar>(std::move(myDrink));
   std::cout << " Order: " << myDrink->getDescription();
   std::cout << ", Cost: $" << myDrink->getCost() << std::endl;

   // 4. Add Vanilla
   myDrink = std::make_unique<Vanilla>(std::move(myDrink));
   std::cout << " Order: " << myDrink->getDescription();
   std::cout << ", Cost: $" << myDrink->getCost() << std::endl;

   std::cout << "\n--- Closing the shop. Cleaning up orders... ---\n";
}

//================================================================================ END
