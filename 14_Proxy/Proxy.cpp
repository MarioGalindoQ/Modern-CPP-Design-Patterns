/**
 * ============================================================================
 * File: Proxy.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the Proxy pattern. The 'ProxyCar' acts as an 
 * interface for 'Car'. It controls access to the real object by checking 
 * the driver's age before allowing the call to proceed.
 * 
 * --- KEY FEATURES:
 * - Protection Proxy: Controls access based on conditions (age).
 * - Lazy Initialization: The real 'Car' is only created when needed.
 * - Uniform Interface: The client interacts with 'ICar' without knowing 
 *   if it is talking to a Proxy or the real object.
 * ============================================================================
 */

#include <iostream>
#include <memory>

//--------------------------------------------------------- Subject Interface:
class ICar
{
public:
   virtual ~ICar() = default;
   virtual void drive() const = 0;
};

//------------------------------------------------ Car (real object):
class Car : public ICar
{
public:
   Car() { std::cout << " [System] Real Car created.\n"; }
   ~Car() override { std::cout << " [Cleanup] Real Car destroyed.\n"; }

   void drive() const override 
   { 
      std::cout << " -> Driving the car!\n"; 
   }
};

//--------------------------------------------------------- ProxyCar:
class ProxyCar : public ICar
{
private:
   mutable std::unique_ptr<Car> realCar_; // Lazy initialized
   int driverAge_;

public:
   explicit ProxyCar(int driverAge) : driverAge_{driverAge} { }

   void drive() const override
   {
      if (driverAge_ > 16)
      {
         // Lazy Initialization: Create the real object only when needed
         if (!realCar_) realCar_ = std::make_unique<Car>();
         realCar_->drive();
      }
      else
         std::cout << " [Proxy] Access denied: Driver is too young.\n";
   }
};

//--------------------------------------------------------- Main:
int main()
{
   std::cout << "=== PROXY PATTERN SIMULATION ===\n" << std::endl;

   std::cout << "--- Attempt 1: Young driver ---\n";
   std::unique_ptr<ICar> proxy1 = std::make_unique<ProxyCar>(16);
   proxy1->drive();

   std::cout << "\n--- Attempt 2: Adult driver ---\n";
   std::unique_ptr<ICar> proxy2 = std::make_unique<ProxyCar>(25);
   proxy2->drive();

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
