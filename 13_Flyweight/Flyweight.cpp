/**
 * ============================================================================
 * File: Flyweight.cpp
 * Author: Mario Galindo Queralt, Ph.D.
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
#include <random> // Essential for modern random logic

// Toggle this definition to show or hide memory cleanup messages
#define SHOW_CLEANUP

//--------------------------------------------------------- Intrinsic State:
class Model
{
private:
   std::string name_;
   int         capacity_;
   int         speed_;
   std::string deployDate_;
 
public:
   Model(std::string name, int capacity, int speed, std::string date)
      : name_{std::move(name)}, capacity_{capacity},
        speed_{speed}, deployDate_{std::move(date)} { }

   ~Model()
   {
#ifdef SHOW_CLEANUP
      std::cout << " [~Model] Model '" << name_ << "' destroyed.\n";
#endif
   }

   void showDetails() const
   {
      std::cout << " Name: "         << name_ 
                << ", Capacity: "    << capacity_    << " passengers"
                << ", Speed: "       << speed_       << " knots"
                << ", Deploy date: " << deployDate_;
   }

   std::string getName()
   {
      return name_;
   }
};

//--------------------------------------------------------- Flyweight Factory:
class FlyweightFactory
{
private:
   std::unordered_map<int, std::shared_ptr<Model>> modelCache_;
 
public:
   std::shared_ptr<Model> getModel(int type)
   {
      if(modelCache_.find(type) == modelCache_.end())
      {
         std::cout << " [Factory] Creating new model type: " << type << "\n";
         switch(type)
         {
         case 747: modelCache_[type] = std::make_shared<Model>("Boeing 747", 467, 495, "Sep 1968"); break;
         case 380: modelCache_[type] = std::make_shared<Model>("Airbus 380", 545, 510, "Apr 2005"); break;
         case 787: modelCache_[type] = std::make_shared<Model>("Boeing 787", 330, 488, "Dec 2009"); break;
         case 220: modelCache_[type] = std::make_shared<Model>("Airbus 220", 150, 450, "Sep 2013"); break;
         default:  throw std::invalid_argument("Unknown aeroplane type");
         }
      }
      return modelCache_[type];
   }
};

//--------------------------------------------------------- Extrinsic State:
class Airplane
{
private:
   std::shared_ptr<Model> model_; // Almost everything is stored here.
   int id_;                       // Only a few unique values ​​are stored here
 
public:
   Airplane(std::shared_ptr<Model> model, int id) : model_{std::move(model)}, id_{id} { }
 
   ~Airplane()
   {
#ifdef SHOW_CLEANUP
      std::cout << " [~Airplane] Airplane " << id_ << " model " << model_->getName() << " destroyed.\n";
#endif
   }

   void showDetails() const
   {
      model_->showDetails();
      std::cout << ", Serial No: " << id_ << '\n';
   }
};

int getRandomModelType()
{
    static const int modelTypes[] = {747, 380, 787, 220};
    static std::random_device rd; 
    static std::mt19937 gen(rd()); 
    static std::uniform_int_distribution<int> dist(0, 3);

    // Pick a random index and return the model type
    return modelTypes[dist(gen)];
}

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
      // Choose random types from 747, 380, 787 and 220.
      int type = getRandomModelType();
      fleet.emplace_back(factory.getModel(type), i+1);
   }

   std::cout << " [Factory] Total fleet created." << std::endl;
   std::cout << " Fleet size: " << fleet.size() << std::endl;
   
   // Check details of the first and last one to verify it works
   std::cout << "\n First airplane details:\n ";
   fleet.front().showDetails();

   std::cout << "\n Last airplane details:\n ";
   fleet.back().showDetails();

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
