// File: Closed_without_CD.cpp
#include "FSM.h"

class Closed_without_CD : public IState_FSM
{
public:
   void open(FSM* fsm) override { fsm->changeState("Open_without_CD", &FSM::open_tray); }
};

// Auto-register
static inline bool registered_ = []()
 {
    FSM::registerState<Closed_without_CD>("Closed_without_CD");
    return true;
 }();

//================================================================================ END
