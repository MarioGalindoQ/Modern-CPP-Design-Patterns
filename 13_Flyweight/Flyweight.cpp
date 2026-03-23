/**
 * ============================================================================
 * File: Flyweight.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the Flyweight pattern. We separate the intrinsic 
 * state (Model) from the extrinsic state (Airplane).
 * 
 * Simulation:
 * We create a large number of 'Airplane' instances to show that memory usage 
 * is minimized: even with thousands of airplanes, only 3 'Model' objects 
 * are stored in memory.
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <stdexcept>

// Toggle this definition to show or hide memory cleanup messages
#define SHOW_CLEANUP

//--------------------------------------------------------- Intrinsic State:
class Model
{
private:
   std::string name_;
   int capacity_;
   int speed_;
 
public:
   Model(std::string name, int capacity, int speed)
      : name_{std::move(name)}, capacity_{capacity}, speed_{speed} { }
    
   ~Model()
   {
#ifdef SHOW_CLEANUP
      std::cout << " [Cleanup] Model '" << name_ << "' destroyed.\n";
#endif
   }

   void showDetails() const
   {
      std::cout << " Name: " << name_ 
                << ", Capacity: " << capacity_ 
                << ", Speed: " << speed_ << " knots";
   }
};

//--------------------------------------------------------- Flyweight Factory:
class FlyweightFactory
{
private:
   std::unordered_map<int, std::shared_ptr<Model>> cache_;
 
public:
   std::shared_ptr<Model> getModel(int type)
   {
      if (cache_.find(type) == cache_.end())
      {
         std::cout << " [Factory] Creating new model type: " << type << "\n";
         switch(type)
         {
            case 380: cache_[type] = std::make_shared<Model>("Airbus 380", 200, 800); break;
            case 787: cache_[type] = std::make_shared<Model>("Boeing 787", 600, 1000); break;
            case 797: cache_[type] = std::make_shared<Model>("Boeing 797", 1200, 1500); break;
            default:  throw std::invalid_argument("Unknown aeroplane type");
         }
      }
      return cache_[type];
   }
};

//--------------------------------------------------------- Extrinsic State:
class Airplane
{
private:
   std::shared_ptr<Model> model_; 
   std::string mfgDate_;
   int id_;
 
public:
   Airplane(std::shared_ptr<Model> model, std::string date, int id)
      : model_{std::move(model)}, mfgDate_{std::move(date)}, id_{id} { }
 
   ~Airplane()
   {
#ifdef SHOW_CLEANUP
      std::cout << " [Cleanup] Airplane " << id_ << " destroyed.\n";
#endif
   }

   void showDetails() const
   {
      model_->showDetails();
      std::cout << "\tMfgDate: " << mfgDate_ << ", Serial No: " << id_ << '\n';
   }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== FLYWEIGHT PATTERN (MASSIVE SCALING SIMULATION) ===\n" << std::endl;

   FlyweightFactory factory;
   std::vector<Airplane> fleet;

   // Create a large fleet of airplanes
   const int numAirplanes = 100;
   fleet.reserve(numAirplanes);

   std::cout << " [Factory] Creating " << numAirplanes << " airplanes..." << std::endl;

   for (int i = 0; i < numAirplanes; ++i)
   {
      // Distribute types among 380, 787, and 797
      int type = (i % 3 == 0) ? 380 : (i % 3 == 1) ? 787 : 797;
      fleet.emplace_back(factory.getModel(type), "Jan 2024", i);
   }

   std::cout << " [Factory] Total fleet created." << std::endl;
   std::cout << " Fleet size: " << fleet.size() << std::endl;
   
   // Check details of the last one to verify it works
   std::cout << "\n Last airplane details:\n ";
   fleet.back().showDetails();

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
