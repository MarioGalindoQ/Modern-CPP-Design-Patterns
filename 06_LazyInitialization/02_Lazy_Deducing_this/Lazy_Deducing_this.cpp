/**
 * ============================================================================
 * File: Lazy_Deducing_this.cpp (Modern C++23 Version)
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates a "Transparent Object-level" Lazy Initialization 
 * using the C++23 "Deducing This" feature. It allows heavy objects to defer 
 * resource-intensive loading until the exact moment a business method is 
 * invoked, completely hiding this complexity from the client.
 * 
 * --- THE ARCHITECTURAL EVOLUTION:
 * 1. Factory-Level (Example 01): Focuses on the "when" of object creation 
 *    using a global cache and static factories.
 * 2. Object-Level (Example 02): Focuses on the "internal state" of the object. 
 *    The object exists as a lightweight shell, only "heavying up" its internal 
 *    resources upon the first functional call.
 * 
 * --- THE C++23 REVOLUTION (ZERO-OVERHEAD POLYMORPHISM):
 * Traditionally, a Base class could only trigger logic in a Derived class 
 * using either:
 *   A) Virtual Functions: Incurs the cost of a VTable and dynamic dispatch.
 *   B) CRTP: Incurs complex, circular template syntax and increased code bloat.
 * 
 * C++23 "Deducing This" enables "Static Dispatch" with a clean, non-template 
 * hierarchy. The 'LazyComponent' base class remains a simple class, yet its 
 * methods can "reach down" into the derived class at compile-time. This 
 * achieves the same goal as virtual functions but with zero runtime overhead 
 * and much simpler code.
 * 
 * --- TECHNICAL MECHANICS:
 * 1. Mixin Infrastructure: 'LazyComponent' provides 'ensure_initialized(this 
 *    auto&& self)'. 
 * 2. Static Injection: The 'self' parameter deduces the 'HeavyResource' type 
 *    at the call site, allowing the base to invoke private child methods 
 *    (via friendship) without knowing the child's type during its own 
 *    definition.
 * 3. Total Transparency: The client interacts with the public API normally. 
 *    The "Lazy" check is an invisible safety layer injected by the Mixin.
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <utility>

//----------------------------------------------------- Infrastructure Layer:
/**
 * LazyComponent Mixin:
 * Provides the machinery to ensure an object is loaded exactly once.
 */
class LazyComponent {
private:
   bool initialized_{false};

protected:
   /**
    * Using Deducing This to access the derived class's private members.
    * This method is called internally by the derived class's public API.
    */
   void ensure_initialized(this auto&& self) {
      if(!self.initialized_) {
         std::cout << " [Mixin] Lazy check: Resource not loaded. Initializing now...\n";
         self.load_resources(); // Static dispatch to Derived::load_resources
         self.initialized_ = true;
      }
   }
};

//---------------------------------------------------------- Business Layer:
class HeavyResource : public LazyComponent {
   // Allows the Mixin to call private load_resources()
   friend class LazyComponent;

private:
   std::string name_;
   int data_value_{0};

   /**
    * Private loading logic. 
    * Only accessible via the LazyComponent infrastructure.
    */
   void load_resources() {
      std::cout << " [System] Loading heavy data for: " << name_ << "...\n";
      data_value_ = 42; // Emulated heavy data
   }

public:
   explicit HeavyResource(std::string name) : name_{std::move(name)} {
      std::cout << " [System] HeavyResource created (dormant): " << name_ <<
                   " (Resource NOT loaded yet)\n";
   }

   /**
    * Public API: Transparently handles initialization.
    */
   void process() {
      ensure_initialized(); // Internal check
      std::cout << " [System] Processing data: " << data_value_ << " in " << name_ << "\n";
   }

   void update(int new_val) {
      ensure_initialized(); // Internal check
      data_value_ = new_val;
      std::cout << " [System] Data updated to: " << data_value_ << " in " << name_ << "\n";
   }
};

//--------------------------------------------------------- Main Simulation:
int main() {
   std::cout << "=== MODERN LAZY INITIALIZATION: DEDUCING THIS (TRANSPARENT) ===\n" << std::endl;

   std::cout << "--- PHASE 1: Object Creation ---\n";
   HeavyResource resource("MainDatabase");
   // The object exists but its heavy resources are not yet loaded.

   std::cout << "\n--- PHASE 2: Transparent Access (First Call) ---\n";
   // The user doesn't know about 'ensure_initialized'. It just works.
   resource.process();

   std::cout << "\n--- PHASE 3: Subsequent Access (No reload) ---\n";
   resource.update(100);
   resource.process();

   std::cout << "\n--- PHASE 4: Temporary Object Access ---\n";
   // Works perfectly with rvalues too!
   HeavyResource("TemporaryBuffer").process();

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
