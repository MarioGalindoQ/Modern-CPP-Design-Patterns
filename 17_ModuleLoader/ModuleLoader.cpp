/**
 * ============================================================================
 * File: ModuleLoader.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This is the Host application. It dynamically loads 'libModule.so' 
 * at runtime using the POSIX <dlfcn.h> API.
 * 
 * --- MODERN C++ IMPROVEMENTS:
 * 1. RAII Library Handle: A small wrapper ensures 'dlclose' is always 
 *    called, preventing resource leaks even if exceptions occur.
 * 2. Smart Pointers with Custom Deleter: 'std::unique_ptr' is used 
 *    with the plugin's 'destroy_module' function to guarantee safe 
 *    deallocation across the ABI boundary.
 * ============================================================================
 */

// Load a module (an object from a .so file) and call a method from this module.

#include "IModule.h"

#include <dlfcn.h> // This is a C library
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

// RAII wrapper for the dynamic library handle
class DynamicLibrary
{
private:
   void* handle_{nullptr};

public:
   explicit DynamicLibrary(const char* filename)
   {
      std::cout << " [Host] Loading shared library: " << filename << "...\n";
      handle_ = dlopen(filename, RTLD_LAZY);
      if (!handle_) throw std::runtime_error(dlerror());
   }

   ~DynamicLibrary()
   {
      if (handle_)
      {
         std::cout << " [Host] Unloading shared library...\n";
         dlclose(handle_);
      }
   }

   void* getSymbol(const char* symbolName) const
   {
      void* symbol = dlsym(handle_, symbolName);
      if (!symbol) throw std::runtime_error(dlerror());
      return symbol;
   }
};

int main(int ac, char** av)
{
   std::cout << "=== DYNAMIC MODULE LOADER ===\n\n";

   //---------------------------------------------------------------- Verify input:
   if (ac < 2)
   {
      std::cerr << "usage: ModuleLoader ./libModule.so\n";
      return 1;
   }

   dlerror(); // Clear all previous errors

   try
   {
      //-------------------------------------------------------------- Open a library:
      // Utilizing RAII to ensure the library is closed automatically
      DynamicLibrary lib(av[1]);

      //---------------------------------------------------------------- Define types:
      using Module_constructor = IModule*(*)(const char*);
      using Module_destructor  = void(*)(IModule*);

      //---------------------------------------- Get build and destroy module symbols:
      std::cout << " [Host] Resolving symbols (build_module, destroy_module)...\n";
      
      auto build_module = reinterpret_cast<Module_constructor>(lib.getSymbol("build_module"));
      auto destroy_module = reinterpret_cast<Module_destructor>(lib.getSymbol("destroy_module"));

      //------------------------------------------------ Create a new module instance:
      std::cout << " [Host] Requesting new module instance...\n";
      IModule* raw_module = build_module("MathProcessor_v1");
      
      if (!raw_module)
      {
         std::cerr << "build(): returned NULL instead of a new module\n";
         return 1;
      }

      // Bind the raw pointer to a unique_ptr with a custom deleter.
      // This guarantees 'destroy_module' is called when the pointer goes out of scope.
      std::unique_ptr<IModule, Module_destructor> module(raw_module, destroy_module);

      //------------------------------------------------------- Call Module functions:
      std::cout << "\n [Host] Executing module operation with data: 42...\n";
      module->processData(42);

      std::cout << "\n [Host] Destroying module instance...\n";
      
      //--------------------------------------------------------------- Delete Module:
      // The unique_ptr will automatically call destroy_module(module.get()) here 
      // when it goes out of scope, right before the DynamicLibrary is destroyed.
   }
   catch (const std::exception& e)
   {
      std::cerr << "[Error] " << e.what() << "\n";
      return 1;
   }

   std::cout << "\n=== EXECUTION COMPLETED ===\n";
}

//======================================================================================== END
