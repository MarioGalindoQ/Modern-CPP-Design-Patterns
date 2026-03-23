// File: State_B.cpp

#include "FSM.h"

class State_B : public IState_FSM
{
public:
   // Static registration method
   static inline bool registered = []()
   {
      FSM::registerState<State_B>("State_B");
      return true;
   }();

   void onEntry(FSM* fsm) override
   {
      std::cout << fsm->name << ": State_B::onEntry, i=" << fsm->i++ << "\n";
   }

   void onExit(FSM* fsm) override
   {
      std::cout << fsm->name << ": State_B::onExit, i=" << fsm->i++ << "\n\n";
   }

   void event_stay(FSM* fsm) override
   {
      std::cout << fsm->name << ": State_B, event_stay, i=" << fsm->i++ << "\n";
   }

   void event_change(FSM* fsm) override
   {
      std::cout << fsm->name << ": State_B, event_change, i=" << fsm->i++ << "\n";
      fsm->changeState("State_A", &FSM::transition_BtoA);
   }
};

//================================================================================ END
