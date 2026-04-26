/**
 * ============================================================================
 * File: FSM.h (Modern Finite State Machine with std::variant)
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This implementation demonstrates a state machine using std::variant (C++17).
 * It is functionally identical to the GoF version (02_CDPlayer_GoF) but 
 * replaces the dynamic registry and virtual hierarchy with type-safe 
 * value semantics.
 * 
 * --- RAII & TRANSITIONS:
 * We use constructors/destructors to simulate onEntry/onExit behaviors.
 * The 'transitionTo' method ensures a clean transition by explicitly 
 * destroying the current state before instantiating the next one.
 * 
 * --- TYPE SAFETY:
 * The compiler ensures that only valid states defined in the 'StateVariant' 
 * can be used, eliminating the risk of malformed strings or null pointers.
 * ============================================================================
 */

#ifndef FSM_H
#define FSM_H

#include <iostream>
#include <string>
#include <variant>
#include <memory>
#include <stdexcept>

class FSM; // Forward definition of Finite State Machine

//--------------------------------------------------------- State Data Structs:
//-------------------- Events (customise this part):
#define EVENTS                                                                               \
   void open(); void close();     void insert_CD(int n); void remove_CD(); void play();      \
   void stop(); void next_song(); void previous_song();  void pause();     void print_name();

//-------------------- States (customise this part):
struct Closed_without_CD { FSM* fsm; explicit Closed_without_CD(FSM* f); ~Closed_without_CD(); EVENTS };
struct Open_without_CD   { FSM* fsm; explicit Open_without_CD(FSM* f);   ~Open_without_CD();   EVENTS };
struct Open_with_CD      { FSM* fsm; explicit Open_with_CD(FSM* f);      ~Open_with_CD();      EVENTS };
struct Closed_with_CD    { FSM* fsm; explicit Closed_with_CD(FSM* f);    ~Closed_with_CD();    EVENTS };
struct Playing           { FSM* fsm; explicit Playing(FSM* f);           ~Playing();           EVENTS };
struct Paused            { FSM* fsm; explicit Paused(FSM* f);            ~Paused();            EVENTS };

// The variant of States, must include all states and std::monostate for transitions
using StateVariant = std::variant<
   std::monostate,    // Unit type intended for use as a well-behaved empty alternative in std::variant.
                      // Set as its first alternative, makes the variant itself default-constructible
                      // (not used in this example). But also, it allows to call the destructor of state_
                      // to execute the transition action before constructing a new state.
   Closed_without_CD, 
   Open_without_CD, 
   Open_with_CD, 
   Closed_with_CD, 
   Playing, 
   Paused
>;

//--------------------------------------------------------- FSM (Context):
class FSM // Finite State Machine
{
private:
   StateVariant state_{Closed_without_CD(this)};

public:
   //-------------------- State variables (customise this part):
   std::string name     {""};
   bool tray_open       {false};
   bool playing         {false};
   bool green_light_on  {false};
   bool yellow_light_on {false};
   bool cian_light_on   {false};
   bool CD_on_tray      {false};
   int nSongs           {0};  // Number of songs in the CD
   int iSong            {0};  // Song that is being played

   FSM(std::string name);

   // Transition helper
   template <class NewState>
   void transitionTo(void(FSM::*transitionAction)() = nullptr) // transitionAction is a pointer to a method of FSM
   {                                                           // with no parameters that returns void (default = nullptr).
      // 1. Force destruction of current state (calls destructor / onExit)
      state_ = std::monostate{}; 

      // 2. Execute transitionAction
      if(transitionAction != nullptr) (this->*transitionAction)();

      // 3. Construct new state (calls constructor / onEntry)
      state_.emplace<NewState>(this); // emplace: destroy current variant and creates a new variant value in-place.

      // 4. Print current state name
      std::get<NewState>(state_).print_name();
   }

   // Events (Public API)
   EVENTS

   // Transition actions
   void open_tray()  { std::cout << name << ": Opening tray.\n"; tray_open = true; }
   void close_tray() { std::cout << name << ": Closing tray.\n"; tray_open = false; }
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
