/**
 * ============================================================================
 * File: FSM.cpp (Simple Example)
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the State pattern using a Finite State Machine (FSM).
 * Instead of using complex 'if-else' or 'switch' logic in the main class,
 * we move state-specific behavior into separate State classes.
 * 
 * --- DYNAMIC TRANSITIONS:
 * The FSM Context holds a pointer to the current State. When an event occurs,
 * the Context delegates the call to the current State object. If a transition
 * is required, the State object tells the Context to change to a new State.
 * 
 * --- AUTOMATIC REGISTRATION:
 * Each State class registers itself into the FSM's internal map during
 * static initialization, making the machine modular and extensible.
 * ============================================================================
 */

#include "FSM.h"

int main()
{
   std::cout << "=== FINITE STATE MACHINE (SIMPLE EXAMPLE) ===\n" << std::endl;

   FSM fsm1("fsm1", "State_A");

   fsm1.event_change();

   std::cout << '\n';

   FSM fsm2("fsm2", "State_A");

   fsm2.event_stay();
   fsm2.event_only_A();
   fsm2.event_change();

   fsm2.event_stay();
   try
   {
      fsm2.event_only_A();
   }
   catch (const std::exception& e)
   {
      std::cout << e.what();
   }
   fsm2.event_change();

   fsm2.event_stay();
   fsm2.event_only_A();

   std::cout << "\n=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
