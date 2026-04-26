/**
 * ============================================================================
 * File: BlurProcessor.cpp
 * Author: Mario Galindo Queralt, Ph.D.
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
   Image process(const Image& image) const override
   {
      return image + " blured";
   }
};

// Auto-register using the helper
static Register<BlurProcessor> reg("Blur");

//================================================================================ END
