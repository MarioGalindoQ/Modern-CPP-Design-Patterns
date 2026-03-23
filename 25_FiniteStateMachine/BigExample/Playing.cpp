// File: Playing.cpp

#include "FSM.h"

class Playing : public IState_FSM
{
public:
   void onEntry(FSM* fsm) override
   {
      std::cout << fsm->name << ": Turning ON green light.\n";
      fsm->green_light_on = true;
      std::cout << fsm->name << ": Playing song number " << fsm->iSong << '\n';
      fsm->playing = true;
   }

   void onExit(FSM* fsm) override
   {
      std::cout << fsm->name << ": Stop playing.\n";
      fsm->playing = false;
      std::cout << fsm->name << ": Turning OFF green light.\n";
      fsm->green_light_on = false;
   }

   void open(FSM* fsm) override { fsm->changeState("Open_with_CD", &FSM::open_tray); }
    
   void play(FSM* fsm) override { fsm->changeState("Playing", &FSM::reset); }

   void stop(FSM* fsm) override { fsm->changeState("Closed_with_CD"); }

   void next_song(FSM* fsm) override
   {
      fsm->iSong = (fsm->iSong < fsm->nSongs) ? fsm->iSong + 1 : 1;
      std::cout << fsm->name << ": Playing song number " << fsm->iSong << '\n';
   }

   void previous_song(FSM* fsm) override
   {
      fsm->iSong = (fsm->iSong > 1) ? fsm->iSong - 1 : fsm->nSongs;
      std::cout << fsm->name << ": Playing song number " << fsm->iSong << '\n';
   }

   void pause(FSM* fsm) override { fsm->changeState("Paused"); }
};

// Auto-register
static inline bool registered_ = []()
 {
    FSM::registerState<Playing>("Playing");
    return true;
 }();

//================================================================================ END
