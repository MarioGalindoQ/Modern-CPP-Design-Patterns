// File: Closed_with_CD.cpp

#include "FSM.h"

class Closed_with_CD : public IState_FSM
{
public:
   void onEntry(FSM* fsm) override
   {
      std::cout << fsm->name << ": Turning ON yellow light.\n";
      fsm->yellow_light_on = true;
   }

   void onExit(FSM* fsm) override
   {
      std::cout << fsm->name << ": Turning OFF yellow light.\n";
      fsm->yellow_light_on = false;
   }

   void open(FSM* fsm) override 
   {
      fsm->changeState("Open_with_CD", &FSM::open_tray);
   }
    
   void play(FSM* fsm) override 
   {
      fsm->changeState("Playing", &FSM::reset);
   }

};

// Auto-register
static inline bool registered_ = []()
 {
    FSM::registerState<Closed_with_CD>("Closed_with_CD");
    return true;
 }();

//================================================================================ END
