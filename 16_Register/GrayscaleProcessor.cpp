/**
 * ============================================================================
 * File: GrayscaleProcessor.cpp
 * 
 * --- DESIGN OVERVIEW:
 * Concrete implementation of the Processor interface. This component 
 * automatically registers itself into the Registry upon initialization.
 * ============================================================================
 */

#include "Register.h"

class GrayscaleProcessor : public Processor
{
public:
   std::string process(const std::string& input) const override
   {
      return input + " grayscaled";
   }

   std::string getName() const override 
   { 
      return "Grayscale"; 
   }
};

// Auto-register using the helper
static Register<GrayscaleProcessor> reg("Grayscale");

//================================================================================ END
