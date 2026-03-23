// File: Paused.cpp

#include "FSM.h"

class Paused : public IState_FSM
{
public:
   void onEntry(FSM* fsm) override
   {
      std::cout << fsm->name << ": Turning ON cyan light.\n";
      fsm->cian_light_on = true;
   }

   void onExit(FSM* fsm) override
   {
      std::cout << fsm->name << ": Turning OFF cyan light.\n";
      fsm->cian_light_on = false;
   }
    
   void play(FSM* fsm) override 
   {
      fsm->changeState("Playing");
   }
};

// Auto-register
static inline bool registered_ = []()
 {
    FSM::registerState<Paused>("Paused");
    return true;
 }();

//================================================================================ END
