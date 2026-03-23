/**
 * ============================================================================
 * File: Singleton.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This implementation demonstrates the "Multiton" pattern combined with a 
 * Registry mechanism. Unlike a standard Singleton (which manages one instance), 
 * this pattern manages a collection of named instances.
 * 
 * --- AUTO-REGISTRATION:
 * We use global static instances to trigger the constructor of our 
 * Singletons BEFORE the 'main()' function starts. This automatically 
 * populates the registry map, allowing us to retrieve specific instances 
 * by name ('Singleton_A', 'Singleton_B') without manual wiring.
 * 
 * --- ARCHITECTURAL NOTE:
 * This approach achieves decoupling: the registry handles the lifecycle 
 * and lookup, while the client simply requests instances by a unique 
 * identifier. This is a powerful technique for modular systems where 
 * components can register themselves dynamically.
 * ============================================================================
 */

#include <string>
#include <string_view>
#include <map>
#include <iostream>
#include <stdexcept> // invalid_argument

//--------------------------------------------------------------------- Singleton Base
class Singleton
{
private:
   // C++17 inline static: prevents the need for a .cpp to define the map
   static inline std::map<std::string, Singleton*> singletonMap;

protected:
   // Use string_view for efficient lookups without copies
   static Singleton* getSingleton(std::string_view name)
   {
      auto it = singletonMap.find(std::string(name));
      if (it == singletonMap.end()) return nullptr;
      return it->second;
   }

public:
   virtual ~Singleton() = default; // Mandatory virtual destructor

   Singleton(const std::string& name)
   {
      auto [it, success] = singletonMap.insert({name, this});
      if (!success) throw std::invalid_argument("Duplicate singleton name: " + name);
   }

   // Disable copy/move for safety
   Singleton(const Singleton&) = delete;
   Singleton& operator=(const Singleton&) = delete;
};

//-------------------------------------------------------------------------- Interface
class Interface : public Singleton
{
public:
   using Singleton::Singleton;
   virtual ~Interface() = default;

   static Interface* getInterface(std::string_view name)
   {
      return static_cast<Interface*>(getSingleton(name));
   }

   virtual void method_1() { std::cout << "Default method_1\n"; }
   virtual void method_2() { std::cout << "Default method_2\n"; }
};

//------------------------------------------------------------------------ Singleton A
class Singleton_A final : public Interface
{
public:
   Singleton_A(const std::string& name) : Interface{name}
   {
      std::cout << " [System] Singleton_A registered.\n";
   }

   ~Singleton_A() override { std::cout << " [System] Singleton_A destroyed.\n"; }

private:
   void method_1() override { std::cout << " -> Running Singleton_A::method_1\n"; }
};

// Auto-register Singleton_A using a lambda
static inline bool reg_A = []()
{
   static Singleton_A instance{"Singleton_A"};
   return true;
}();

//------------------------------------------------------------------------ Singleton B
class Singleton_B final : public Interface
{
public:
   Singleton_B(const std::string& name) : Interface{name}
   {
      std::cout << " [System] Singleton_B registered.\n";
   }
    
   ~Singleton_B() override { std::cout << " [System] Singleton_B destroyed.\n"; }

private:
   void method_2() override { std::cout << " -> Running Singleton_B::method_2\n"; }
};

// Auto-register Singleton_B using a lambda
static inline bool reg_B = []()
{
   static Singleton_B instance{"Singleton_B"};
   return true;
}();

//------------------------------------------------------------------------------- Main

int main() {
    std::cout << "=== SINGLETON WITH AUTO-REGISTER ===\n\n";

    Interface* sA = Interface::getInterface("Singleton_A");
    if (sA) sA->method_1();
    
    Interface* sB = Interface::getInterface("Singleton_B");
    if (sB) sB->method_2();

    // Testing non-existent
    Interface* sC = Interface::getInterface("Singleton_C");
    if (!sC) std::cout << " [Error] Singleton_C not found in registry.\n";

    std::cout << "\n=== END OF MAIN ===\n";
}

//================================================================================ END
