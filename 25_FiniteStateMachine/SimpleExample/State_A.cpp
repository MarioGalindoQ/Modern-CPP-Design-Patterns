// File: State_A.cpp

#include "FSM.h"

class State_A : public IState_FSM
{
public:
   // Static registration method that must be called before using the state
   static inline bool registered = []()
   {
      FSM::registerState<State_A>("State_A");
      return true;
   }();

   void onEntry(FSM* fsm) override
   {
      std::cout << fsm->name << ": State_A::onEntry, i=" << fsm->i++ << "\n";
   }
    
   void onExit(FSM* fsm) override
   {
      std::cout << fsm->name << ": State_A::onExit, i=" << fsm->i++ << "\n\n";
   }

   void event_stay(FSM* fsm) override
   {
      std::cout << fsm->name << ": State_A, event_stay, i=" << fsm->i++ << "\n";
   }
    
   void event_change(FSM* fsm) override
   {
      std::cout << fsm->name << ": State_A, event_change, i=" << fsm->i++ << "\n";
      fsm->changeState("State_B", &FSM::transition_AtoB);
   }

   void event_only_A(FSM* fsm) override
   {
      std::cout << fsm->name << ": State_A, event_only_A, i=" << fsm->i++ << "\n";
   }
};

//================================================================================ END
