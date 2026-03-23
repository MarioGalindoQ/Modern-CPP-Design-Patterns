/**
 * ============================================================================
 * File: Decorator.cpp (Advanced Version)
 * 
 * --- DESIGN OVERVIEW:
 * This advanced implementation of the Decorator pattern simulates a dynamic
 * beverage ordering system. 
 * 
 * Features:
 * 1. Base components: Coffee and Tea.
 * 2. Parameterized Decorators: Milk (with types), Sugar & Vanilla (portions).
 * 3. State Decorator: Temperature (Hot/Iced/ExtraHot). 
 * 4. Automatic Grammar: Logic to handle "with", "and", and commas correctly.
 * 5. Memory Management: std::unique_ptr ensures no leaks.
 * 6. Configuration: Use #define SHOW_CLEANUP to toggle destructor messages.
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <memory>

// Toggle this definition to show or hide memory cleanup messages
//#define SHOW_CLEANUP

//--------------------------------------------------------- Base Interface:
class Decorated
{
public:
   virtual ~Decorated() = default;
   virtual std::string getDescription() const = 0;
   virtual double getCost() const = 0;
};

//----------------------------------------------------- Concrete Decorated A:
class Coffee : public Decorated
{
public:
   ~Coffee() override 
   { 
#ifdef SHOW_CLEANUP
      std::cout << " [Cleanup] Coffee base destroyed.\n"; 
#endif
   }
   
   std::string getDescription() const override { return "Coffee"; }
   double getCost() const override { return 2.0; }
};

//----------------------------------------------------- Concrete Decorated B:
class Tea : public Decorated
{
public:
   ~Tea() override 
   { 
#ifdef SHOW_CLEANUP
      std::cout << " [Cleanup] Tea base destroyed.\n"; 
#endif
   }
   
   std::string getDescription() const override { return "Tea"; }
   double getCost() const override { return 1.5; }
};

//-------------------------------------------------------- Base Decorator:
class Decorator : public Decorated          // "is a" Decorated and ...
{
protected:
   std::unique_ptr<Decorated> decorated_;   // "has a" Decorated

   // Helper for ingredients list (with / and / ,)
   std::string addIngredient(const std::string& ingredient) const
   {
      std::string current = decorated_->getDescription();
      size_t andPos = current.find(" and ");
      size_t withPos = current.find(" with ");

      if (andPos != std::string::npos)
      {
         current.replace(andPos, 5, ", ");
         return current + " and " + ingredient;
      }
      if (withPos != std::string::npos)
      {
         return current + " and " + ingredient;
      }
      return current + " with " + ingredient;
   }

public:
   explicit Decorator(std::unique_ptr<Decorated> target)
      : decorated_{std::move(target)} { }

   ~Decorator() override 
   { 
#ifdef SHOW_CLEANUP
      std::cout << " [Cleanup] Decorator wrapper destroyed.\n"; 
#endif
   }

   std::string getDescription() const override { return decorated_->getDescription(); }
   double getCost() const override { return decorated_->getCost(); }
};

//----------------------------------------------------- Milk Decorator:
enum class MilkType { Whole, Soy, Almond };

class Milk : public Decorator
{
private:
   MilkType type_;

public:
   Milk(std::unique_ptr<Decorated> target, MilkType type)
      : Decorator{std::move(target)}, type_{type} { }

   std::string getDescription() const override
   {
      std::string name;
      switch (type_)
      {
         case MilkType::Soy:    name = "Soy Milk";    break;
         case MilkType::Almond: name = "Almond Milk"; break;
         default:               name = "Milk";        break;
      }
      return addIngredient(name);
   }

   double getCost() const override
   {
      double extra = (type_ == MilkType::Almond) ? 0.9 : 0.5;
      return decorated_->getCost() + extra;
   }
};

//----------------------------------------------------- Sugar Decorator:
class Sugar : public Decorator
{
private:
   int portions_;

public:
   Sugar(std::unique_ptr<Decorated> target, int portions)
      : Decorator{std::move(target)}, portions_{portions} { }

   std::string getDescription() const override
   {
      std::string label = (portions_ > 1) ? " portions of Sugar" : " portion of Sugar";
      return addIngredient(std::to_string(portions_) + label);
   }

   double getCost() const override
   {
      return decorated_->getCost() + (portions_ * 0.15);
   }
};

//----------------------------------------------------- Vanilla Decorator:
class Vanilla : public Decorator
{
private:
   int portions_;

public:
   Vanilla(std::unique_ptr<Decorated> target, int portions)
      : Decorator{std::move(target)}, portions_{portions} { }

   std::string getDescription() const override
   {
      std::string label = (portions_ > 1) ? " portions of Vanilla" : " portion of Vanilla";
      return addIngredient(std::to_string(portions_) + label);
   }

   double getCost() const override
   {
      return decorated_->getCost() + (portions_ * 0.40);
   }
};

//----------------------------------------------------- Temperature Decorator:
enum class Temp { Hot, Iced, ExtraHot };

class Temperature : public Decorator
{
private:
   Temp temp_;

public:
   Temperature(std::unique_ptr<Decorated> target, Temp temp)
      : Decorator{std::move(target)}, temp_{temp} { }

   std::string getDescription() const override
   {
      std::string prefix;
      switch (temp_)
      {
         case Temp::Iced:     prefix = "Iced ";      break;
         case Temp::ExtraHot: prefix = "Extra Hot "; break;
         default:             prefix = "Hot ";       break;
      }
      return prefix + decorated_->getDescription();
   }
   /* Temperature doesn't need to decorete getCost if Temperature is free.
   double getCost() const override
   {
      // Iced has a surcharge, Hot and Extra Hot are free
      double extra = (temp_ == Temp::Iced) ? 0.35 : 0.0;
      return decorated_->getCost() + extra;
      }
   */
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== ADVANCED COFFEE SHOP (DYNAMIC DECORATORS) ===\n" << std::endl;

   std::unique_ptr<Decorated> drink;

   std::cout << "\n------------------------------------------\n" << std::endl;

   // --- ORDER 1: Complex Iced Coffee ---
   std::cout << "--- Order 1 ---\n";
   drink = std::make_unique<Coffee>(); 
   drink = std::make_unique<Milk>(std::move(drink), MilkType::Almond);
   drink = std::make_unique<Vanilla>(std::move(drink), 2);
   drink = std::make_unique<Temperature>(std::move(drink), Temp::Iced);
   
   std::cout << " Receipt: " << drink->getDescription() << std::endl;
   std::cout << " Total Cost: $" << drink->getCost() << std::endl;

   std::cout << "\n------------------------------------------\n" << std::endl;

   // --- ORDER 2: Simple Hot Tea ---
   std::cout << "--- Order 2 ---\n";
   drink = std::make_unique<Tea>();
   drink = std::make_unique<Sugar>(std::move(drink), 1);
   drink = std::make_unique<Temperature>(std::move(drink), Temp::Hot);

   std::cout << " Receipt: " << drink->getDescription() << std::endl;
   std::cout << " Total Cost: $" << drink->getCost() << std::endl;

   std::cout << "\n------------------------------------------\n" << std::endl;

   // --- ORDER 3: Loaded Soy Coffee ---
   std::cout << "--- Order 3 ---\n";
   drink = std::make_unique<Coffee>();
   drink = std::make_unique<Milk>(std::move(drink), MilkType::Soy);
   drink = std::make_unique<Sugar>(std::move(drink), 3);
   drink = std::make_unique<Vanilla>(std::move(drink), 1);
   drink = std::make_unique<Temperature>(std::move(drink), Temp::Hot);

   std::cout << " Receipt: " << drink->getDescription() << std::endl;
   std::cout << " Total Cost: $" << drink->getCost() << std::endl;

   std::cout << "\n------------------------------------------\n" << std::endl;

   // --- ORDER 4: Loaded Soy Coffee (Increased Temperature) ---
   std::cout << "--- Order 4 (Same as 3 but Extra Hot) ---\n";
   drink = std::make_unique<Coffee>();
   drink = std::make_unique<Milk>(std::move(drink), MilkType::Soy);
   drink = std::make_unique<Sugar>(std::move(drink), 3);
   drink = std::make_unique<Vanilla>(std::move(drink), 1);
   drink = std::make_unique<Temperature>(std::move(drink), Temp::ExtraHot);

   std::cout << " Receipt: " << drink->getDescription() << std::endl;
   std::cout << " Total Cost: $" << drink->getCost() << std::endl;

#ifdef SHOW_CLEANUP
   std::cout << "\n--- Closing Shop. Objects will be destroyed below ---" << std::endl;
#endif
   std::cout << "\n------------------------------------------\n" << std::endl;
}

//================================================================================ END
