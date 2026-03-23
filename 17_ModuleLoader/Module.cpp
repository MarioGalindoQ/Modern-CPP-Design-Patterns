/**
 * ============================================================================
 * File: Module.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This is the concrete Plugin implementation. It gets compiled into a 
 * Shared Object (.so). It exports two flat C functions (build_module 
 * and destroy_module) to bypass C++ name mangling, allowing the Host 
 * to instantiate and destroy the object safely.
 * ============================================================================
 */

//------------------------ Module implementation

#include "IModule.h"
#include <iostream>
#include <string>

class Module : public IModule
{
private:
   std::string name_;

public:
   // Constructor now accepts configuration from the Host
   explicit Module(const char* name) : name_{name}
   {
      std::cout << "    -> [Plugin] Module '" << name_ << "' initialized.\n";
   }

   ~Module() override
   {
      std::cout << "    -> [Plugin] Module '" << name_ << "' destroyed.\n";
   }

   void processData(int data) override
   {
      std::cout << "    -> [Plugin] Processing data: " << data 
                << " (Calculated: " << data * 2 << ")\n";
   }
};

// The following "C" functions must be into the library:

extern "C"
{
   IModule* build_module(const char* name)
   {
      return new Module(name);
   }

   void destroy_module(IModule* module_ptr)
   {
      if (module_ptr) delete module_ptr;
   }
}

//========================================================================= END
