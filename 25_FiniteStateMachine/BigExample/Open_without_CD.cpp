// File: Open_without_CD.cpp
#include "FSM.h"

class Open_without_CD : public IState_FSM
{
public:
   void close(FSM* fsm) override { fsm->changeState("Closed_without_CD", &FSM::close_tray); }

   void insert_CD(FSM* fsm, int n) override
   {
      fsm->nSongs = n;
      std::cout << fsm->name << ": A CD has been inserted.\n";
      fsm->CD_on_tray = true;
      fsm->changeState("Open_with_CD");
   }
};

// Auto-register
static inline bool registered_ = []()
 {
    FSM::registerState<Open_without_CD>("Open_without_CD");
    return true;
 }();

//================================================================================ END
