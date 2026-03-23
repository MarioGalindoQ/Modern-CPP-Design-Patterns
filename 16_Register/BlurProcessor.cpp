/**
 * ============================================================================
 * File: BlurProcessor.cpp
 * 
 * --- DESIGN OVERVIEW:
 * Concrete implementation of the Processor interface. Demonstrates how 
 * easy it is to add new features to the system by simply including a new 
 * file and registering it.
 * ============================================================================
 */

#include "Register.h"

class BlurProcessor : public Processor
{
public:
   std::string process(const std::string& input) const override
   {
      return input + " blured";
   }

   std::string getName() const override 
   { 
      return "Blur"; 
   }
};

// Auto-register using the helper
static Register<BlurProcessor> reg("Blur");

//================================================================================ END
