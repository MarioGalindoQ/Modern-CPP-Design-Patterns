/**
 * ============================================================================
 * File: Register.cpp (Registry Pattern)
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the Registry pattern, providing a centralized 
 * lookup for service creation. The client requests processors by name 
 * without knowing their concrete implementation.
 * 
 * --- AUTO-REGISTRATION:
 * Each concrete class registers itself into the 'Registry' using the 
 * 'Register<T>' helper during static initialization.
 * 
 * --- VERIFICATION:
 * We test the registry by requesting the same processor twice. Depending on 
 * the compilation flag (REGISTRY_SINGLETON or REGISTRY_PROTOTYPE), you can 
 * verify if the instance returned is shared or a fresh copy.
 * ============================================================================
 */

#include "Register.h"
#include <iostream>
#include <string>

int main()
{
   std::cout << "=== REGISTRY PATTERN SIMULATION ===\n" << std::endl;

   std::string image = "I am an image";
   std::cout << "Original: " << image << "\n\n";

   try
   {
      // The client asks the Registry for processors by name
      auto p1 = Registry::create("Grayscale");
      auto p2 = Registry::create("Blur");

      // Process the image
      std::string result;
      result = p1->process(image);
      std::cout << "Result: " << result << std::endl;

      result = p2->process(result);
      std::cout << "Result: " << result << std::endl;

      // Verification: Check if repeated request returns the same instance
      std::cout << "\n--- Verifying Instance Policy ---\n";
      auto p3 = Registry::create("Grayscale");
      std::cout << " Pointer 1 (p1): " << p1.get() << "\n";
      std::cout << " Pointer 2 (p3): " << p3.get() << "\n";
      
      if (p1 == p3) std::cout << " Policy: Singleton (Instance shared)";
      else          std::cout << " Policy: Prototype (New instance created)";
      std::cout << " - Defined in Register.h\n";

      // Testing non-existent processor
      std::cout << "\nAttempting to get an unknown processor:\n";
      Registry::create("Sepia");
   }
   catch (const std::exception& e)
   {
      std::cerr << " [Error] " << e.what() << "\n";
   }

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
