// File: Open_with_CD.cpp

#include "FSM.h"

class Open_with_CD : public IState_FSM
{
public:
   void close(FSM* fsm) override { fsm->changeState("Closed_with_CD", &FSM::close_tray_and_dir); }

   void remove_CD(FSM* fsm) override
   {
      fsm->nSongs = 0;
      std::cout << fsm->name << ": A CD has been removed.\n";
      fsm->CD_on_tray = false;
      fsm->changeState("Open_without_CD");
   }
    
   void play(FSM* fsm) override { fsm->changeState("Playing", &FSM::close_tray_and_dir); }
};

// Auto-register
static inline bool registered_ = []()
 {
    FSM::registerState<Open_with_CD>("Open_with_CD");
    return true;
 }();

//================================================================================ END
