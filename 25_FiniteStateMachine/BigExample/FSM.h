// File: FSM.h (Finite State Machine Pattern)

#ifndef FSM_H
#define FSM_H

#include <iostream>
#include <string>
#include <string_view>
#include <map>
#include <memory>
#include <stdexcept>

class FSM; // Finite State Machine

//-------------------------------------------------------------------- IState_FSM (State Interface):
class IState_FSM
{
public:
   virtual ~IState_FSM() = default;

   virtual void onEntry(FSM*) { }
   virtual void onExit(FSM*)  { }

   //-------------------- Events (customise this part):
   virtual void open(FSM*)           { throw std::runtime_error("open: invalid event"); }
   virtual void close(FSM*)          { throw std::runtime_error("close: invalid event"); }
   virtual void insert_CD(FSM*, int) { throw std::runtime_error("insert_CD: invalid event"); }
   virtual void remove_CD(FSM*)      { throw std::runtime_error("remove_CD: invalid event"); }
   virtual void play(FSM*)           { throw std::runtime_error("play: invalid event"); }
   virtual void stop(FSM*)           { throw std::runtime_error("stop: invalid event"); }
   virtual void next_song(FSM*)      { throw std::runtime_error("next_song: invalid event"); }
   virtual void previous_song(FSM*)  { throw std::runtime_error("previous_song: invalid event"); }
   virtual void pause(FSM*)          { throw std::runtime_error("pause: invalid event"); }
};

//---------------------------------------------------------------------- FSM (Finite State Machine):
class FSM
{
private:
   // Registry of states (Flyweight/Singleton states):
   // Each state is instantiated once and shared across all FSM instances.
   // Managed via unique_ptr to ensure automatic memory cleanup.
   static inline std::map<std::string, std::unique_ptr<IState_FSM>> stateMap_;
   IState_FSM* state_{nullptr};

public:
   //-------------------- State variables (customise this part):
   std::string name     {""};
   bool tray_open       {false};
   bool playing         {false};
   bool green_light_on  {false};
   bool yellow_light_on {false};
   bool cian_light_on   {false};
   bool CD_on_tray      {false};
   int nSongs           {0}; // Number of songs in the CD
   int iSong            {0}; // Song that is being played

   //------------------------------- Constructor and destructor:
   FSM(std::string name, const std::string& initialStateName)
      : name{std::move(name)}
   {
      state_ = stateMap_.at(initialStateName).get();
      state_->onEntry(this);
   }

   ~FSM() 
   { 
      state_->onExit(this); 
   }

   //------------------------------------------- Register State:
   template<class StateClass>
   static void registerState(const std::string& name)
   {
      stateMap_[name] = std::make_unique<StateClass>();
   }

   //--------------------------------------------- Change State:
   void changeState(const std::string& nextStateName, void(FSM::*transitionAction)() = nullptr)
   {
      state_->onExit(this);
      if (transitionAction) (this->*transitionAction)();
      state_ = stateMap_.at(nextStateName).get();
      state_->onEntry(this);
   }
    
   //-------------------- Events (customise this part):
   void open()           { state_->open(this); }
   void close()          { state_->close(this); }
   void insert_CD(int n) { state_->insert_CD(this, n); }
   void remove_CD()      { state_->remove_CD(this); }
   void play()           { state_->play(this); }
   void stop()           { state_->stop(this); }
   void next_song()      { state_->next_song(this); }
   void previous_song()  { state_->previous_song(this); }
   void pause()          { state_->pause(this); }

   //-------------------- Transition actions (customise this part):
   void open_tray()         { std::cout << name << ": Opening tray.\n"; tray_open = true; }
   void close_tray()        { std::cout << name << ": Closing tray.\n"; tray_open = false; }
   void close_tray_and_dir()
   {
      close_tray();
      std::cout << name << ": Reading CD directory with " << nSongs << " songs.\n";
      iSong = 1;
   }
   void reset() { std::cout << name << ": Resetting to play first song.\n"; iSong = 1; }
};

#endif
//================================================================================ END
