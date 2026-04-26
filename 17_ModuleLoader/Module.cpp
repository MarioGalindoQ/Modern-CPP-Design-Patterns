/**
 * ============================================================================
 * File: Module.cpp
 * Author: Mario Galindo Queralt, Ph.D.
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
   int factor_;

public:
   // Constructor now accepts configuration from the Host
   explicit Module(const char* name, const int factor) : name_{name}, factor_{factor}
   {
      std::cout << "    -> [Plugin] Module '" << name_ << "' initialized.\n";
      std::cout << "    -> [Plugin] with factor = " << factor_ << "\n";
   }

   ~Module() override
   {
      std::cout << "    -> [Plugin] Module '" << name_ << "' destroyed.\n";
   }

   int processData(int data) override
   {
      std::cout << "    -> [Plugin] Processing data: " << data << "\n";
      std::cout << "    -> [Plugin] Returning calculated value: " << data*factor_ << "\n";
      return data*factor_;
   }
};

// The following "C" functions must be into the library:

extern "C"
{
   IModule* build_module(const char* name, const int factor)
   {
      std::cout << "    -> [Plugin] build_module called.\n";
      return new Module(name, factor);
   }

   void destroy_module(IModule* module_ptr)
   {
      if(module_ptr)
      {
         std::cout << "    -> [Plugin] destroy_module called.\n";
         delete module_ptr;
      }
   }
}

//========================================================================= END
