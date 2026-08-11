/*
 * ============================================================================
 * File: Singleton.cpp
 * Author: Mario Galindo Queralt, Ph.D.
 *
 * --- DESIGN OVERVIEW:
 * This implementation demonstrates the "Multiton" pattern combined with a
 * Registry mechanism. Unlike a standard Singleton (which manages one instance),
 * this pattern manages a collection of named instances.
 *
 * --- ENCAPSULATED AUTO-REGISTRATION:
 * Registration is an encapsulated concern of each concrete class. We use a
 * private static boolean member initialized by an immediate lambda to trigger
 * the registration BEFORE the 'main()' function starts.
 *
 * --- COMPILATION & ACCESS CONTROL:
 * By defining the static member's initialization after the class is fully
 * defined, we avoid "incomplete type" errors. This approach allows the
 * constructors to remain 'private', ensuring that instances are only created
 * through the controlled, automatic registration process.
 *
 * --- ARCHITECTURAL NOTE:
 * This approach achieves decoupling: the registry handles the lifecycle
 * and lookup, while the client simply requests instances by a unique
 * identifier. This is a powerful technique for modular systems where
 * components can register themselves dynamically.
 *
 * --- MEMORY MANAGEMENT & STORAGE:
 * The Singleton instances in this example are stored in the **Static Data
 * Segment** (specifically the .data or .bss sections of the executable).
 * Unlike stack variables, they are not destroyed when a function ends;
 * and unlike heap variables, they do not require 'new' or 'delete'. They are
 * allocated by the system loader when the program starts and persist until
 * the process terminates.
 * ============================================================================
 */

#include <string>
#include <string_view>
#include <map>
#include <iostream>
#include <stdexcept>

//--------------------------------------------------------------------- Singleton Base
class Singleton
{
private:
   static inline std::map<std::string, Singleton*> singletonMap;

protected:
   static Singleton *getSingleton(std::string_view name)
   {
      auto it = singletonMap.find(std::string(name));
      if(it == singletonMap.end()) return nullptr;
      return it->second;
   }

public:
   Singleton()                             = delete;  // 1:DC Default Constructor
   Singleton(const Singleton&)             = delete;  // 2:CC Copy Constructor
   Singleton(Singleton&&)                  = delete;  // 3:MC Move Constructor
   Singleton& operator=(const Singleton&)  = delete;  // 4:CA Copy Assigment
   Singleton& operator=(const Singleton&&) = delete;  // 5:MA Copy Assigment

   virtual ~Singleton()                    = default; // 6:De Destructor

   Singleton(const std::string& name)                 // 7:PC Parametric Constructor
   {
      auto [it, success] = singletonMap.insert({name, this});
      if(!success) throw std::invalid_argument("Duplicate singleton name: " + name);
   }
};

//---------------------------------------------------------------------------- Service
class Service : public Singleton
{
public:
   using Singleton::Singleton;
   virtual ~Service() = default;

   static Service *getService(std::string_view name)
   {
      return static_cast<Service*>(getSingleton(name));
   }

   virtual void method_1() { std::cout << "Default method_1\n"; }
   virtual void method_2() { std::cout << "Default method_2\n"; }
};

//------------------------------------------------------------------------ Singleton A
class Singleton_A final : public Service
{
private:
   Singleton_A(const std::string& name) : Service{name}
   {
      std::cout << " [System] Singleton_A registered as a Service.\n";
   }

   ~Singleton_A() override { std::cout << " [System] Singleton_A destroyed.\n"; }

   void method_1() override { std::cout << " -> Running Singleton_A::method_1\n"; }

   static bool registered_;
};

bool Singleton_A::registered_ = []()
{
   static Singleton_A instance{"Singleton_A"};
   return true;
}();

//------------------------------------------------------------------------ Singleton B
class Singleton_B final : public Service
{
private:
   Singleton_B(const std::string& name) : Service{name}
   {
      std::cout << " [System] Singleton_B registered as a Service.\n";
   }

   ~Singleton_B() override { std::cout << " [System] Singleton_B destroyed.\n"; }

   void method_2() override { std::cout << " -> Running Singleton_B::method_2\n"; }

   static bool registered_;
};

bool Singleton_B::registered_ = []()
{
   static Singleton_B instance{"Singleton_B"};
   return true;
}();

//------------------------------------------------------------------------------- Main

int main()
{
   std::cout << "=== SINGLETON WITH ENCAPSULATED AUTO-REGISTER ===\n" << std::endl;

   Service *sA1 = Service::getService("Singleton_A");
   if(sA1) sA1->method_1();

   Service *sB = Service::getService("Singleton_B");
   if(sB) sB->method_2();

   Service *sC = Service::getService("Singleton_C");
   if(!sC) std::cout << " [Error] Singleton_C not found in registry.\n";

   std::cout << "\nRequesting Singleton A instance for the second time...\n";
   Service *sA2 = Service::getService("Singleton_A");

   std::cout << "\nVerification:\n";
   std::cout << " Address of sA1: " << sA1 << "\n";
   std::cout << " Address of sA2: " << sA2 << "\n";

   if(sA1 == sA2)
   {
      std::cout << " SUCCESS: Identity confirmed. Only one instance exists.\n";
   }
   else
   {
      std::cout << " FAILURE: Multiple instances detected!\n";
   }

   std::cout << "\n=== END OF MAIN ===\n";
}

//================================================================================ END
