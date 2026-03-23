/**
 * ============================================================================
 * File: Framework.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This is the core engine of the Framework. It implements the "Hollywood 
 * Principle" (Don't call us, we call you) by controlling the application 
 * lifecycle.
 * 
 * --- EXECUTION FLOW:
 * The framework executes the algorithm steps defined in 'App.h' in a 
 * fixed order: start -> read -> compute -> write -> result -> final.
 * Clients only provide the concrete implementation of these steps.
 * ============================================================================
 */

#include "App.h"
#include <memory>

int main()
{
   // Here is created your own App
   // We use unique_ptr to manage the App lifetime automatically
   std::unique_ptr<App> app{createApp()};

   // (1) Do initial actions
   app->start();

   // (2) Read from input
   while (app->read())
   {
      // (3) Perform some computations
      app->compute();

      // (4) Write some output
      app->write();
   }

   // (5) Produce a final result
   app->result();

   // (6) Do final actions
   app->final();
}

//========================================================================= END
