/**
 * ============================================================================
 * File: LazyInitialization.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the Lazy Initialization pattern. The creation of 
 * heavy 'Fruit' objects is deferred until the first time they are requested.
 * 
 * --- LAZY INITIALIZATION:
 * We use an 'unordered_map' to cache instances. If a requested fruit does not 
 * exist, it is created (Lazy) and stored (Cached).
 * 
 * --- MEMORY MANAGEMENT:
 * We use 'std::shared_ptr' because 'Fruit' instances are shared resources. 
 * The factory acts as the primary owner, but clients can hold their own 
 * shared references to these objects safely.
 * ============================================================================
 */

#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <memory>

class Fruit
{
private:
   std::string type_;

   // Note: Constructor is private to force the use of the static factory method
   explicit Fruit(std::string type) : type_{std::move(type)}
   {
      std::cout << " [System] Fruit instance created: " << type_ << "\n";
   }

public:
   ~Fruit()
   {
      std::cout << " [Cleanup] Fruit instance destroyed: " << type_ << "\n";
   }

   // Lazy Factory method: gets the Fruit instance associated with a certain type.
   // Creates new ones only as needed (Lazy Initialization).
   static std::shared_ptr<Fruit> getFruit(std::string_view type);

   static void printCurrentTypes()
   {
      std::cout << " Number of shared instances: " << cache_.size() << "\n";
      for(const auto& [type, fruit] : cache_) std::cout << " - " << type << "\n";
      std::cout << "\n";
   }

private:
   // Using shared_ptr as the cache owner allows shared access by clients
   static inline std::map<std::string, std::shared_ptr<Fruit>> cache_;
};

// Definition of the factory method
std::shared_ptr<Fruit> Fruit::getFruit(std::string_view type)
{
   std::string key{type};
   auto it = cache_.find(key);
   
   if (it == cache_.end())
   {
      auto fruit = std::shared_ptr<Fruit>(new Fruit(key));
      cache_[key] = fruit;
      return fruit;
   }
   return it->second;
}

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== LAZY INITIALIZATION PATTERN ===\n" << std::endl;

   // Verify a clean cache:
   std::cout << "Requesting number of shared instances in the cache:\n";
   Fruit::printCurrentTypes();

   // 1st request: Creates a new "Banana"
   std::cout << "Requesting 'Banana' (1st time):\n";
   auto f1 = Fruit::getFruit("Banana");
   Fruit::printCurrentTypes();

   // 2nd request: Creates a new "Apple"
   std::cout << "Requesting 'Apple' (1st time):\n";
   auto f2 = Fruit::getFruit("Apple");
   Fruit::printCurrentTypes();

   // 3rd request: Returns pre-existing "Banana" instance from the cache
   std::cout << "Requesting 'Banana' (2nd time):\n";
   auto f3 = Fruit::getFruit("Banana");
   Fruit::printCurrentTypes();

   std::cout << "=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
