/**
 * ============================================================================
 * File: Singleton.cpp
 * 
 * --- EDUCATIONAL NOTE:
 * "We use the Heap (via unique_ptr) when the Singleton is very large, when we 
 * want to use polymorphism (deciding the implementation at runtime) or when we 
 * need total control over dynamic memory. We use Meyers' Singleton for 
 * lightweight and simple Singletons due to its elegance and native thread 
 * safety."
 * 
 * --- CONFIGURATION:
 * Toggle the definition below to switch between implementations.
 * Only ONE implementation of getInstance() will be compiled at a time,
 * preserving the "Single Instance" rule of the pattern.
 * ============================================================================
 */

#include <iostream>
#include <memory>

// Comment or uncomment this line to switch between Meyers' and Heap version
//#define USE_HEAP_SINGLETON 

class Singleton
{
private:
   // Private constructor prevents direct instantiation.
   Singleton()
   {
      std::cout << " [System] Singleton instance created at address: " << this << "\n"; 
   }

public:
   // Rule of Three/Five: Delete copy constructor and assignment operator.
   Singleton(const Singleton&) = delete;
   Singleton& operator=(const Singleton&) = delete;

   virtual ~Singleton()
   {
      std::cout << " [System] Singleton instance destroyed.\n"; 
   }

   /**
    * The Singleton Access Point
    * The logic changes based on the #define at the top of the file.
    */
   static Singleton& getInstance()
   {
        
#ifdef USE_HEAP_SINGLETON
      // APPROACH: Heap-based Singleton (Dynamic Memory)
      // Managed by std::unique_ptr for automatic cleanup.
      // We use 'new' because make_unique cannot access a private constructor.
      static std::unique_ptr<Singleton> instance(new Singleton());
      std::cout << " [Info] Using Heap-based implementation (unique_ptr).\n";
      return *instance;
#else
      // APPROACH: Meyers' Singleton (Static/Data Segment)
      // Simplest, thread-safe (C++11+), and stack-efficient.
      static Singleton instance; 
      std::cout << " [Info] Using Meyers' implementation (Static Segment).\n";
      return instance;
#endif
   }

   void talk() const
   {
      std::cout << " -> Hello! I am the only instance of this class.\n";
   }
};

// ==========================================
// Main Execution Flow
// ==========================================
int main()
{
   std::cout << "=== SINGLETON IMPLEMENTATION TEST ===\n\n";

#ifdef USE_HEAP_SINGLETON
   std::cout << "CONFIGURATION: HEAP MODE ENABLED\n";
#else
   std::cout << "CONFIGURATION: MEYERS MODE ENABLED\n";
#endif
   std::cout << "-------------------------------------\n\n";

   // First access: The object is created here.
   std::cout << "Requesting instance for the first time...\n";
   Singleton& s1 = Singleton::getInstance();
   s1.talk();

   // Second access: Returns the existing instance.
   std::cout << "\nRequesting instance for the second time...\n";
   Singleton& s2 = Singleton::getInstance();
   s2.talk();

   // Verification: Both pointers must be identical.
   std::cout << "\nVerification:\n";
   std::cout << " Address of s1: " << &s1 << "\n";
   std::cout << " Address of s2: " << &s2 << "\n";

   if (&s1 == &s2)
   {
      std::cout << " SUCCESS: Identity confirmed. Only one instance exists.\n";
   }
   else
   {
      std::cout << " FAILURE: Multiple instances detected!\n";
   }

   std::cout << "\n=== END OF MAIN ===\n";
    
   // Cleanup happens after main() returns as the program terminates.
}

//================================================================================ END
