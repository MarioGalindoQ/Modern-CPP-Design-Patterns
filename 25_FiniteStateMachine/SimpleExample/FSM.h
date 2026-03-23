// File: FSM.h (Finite State Machine Pattern)

#ifndef FSM_H
#define FSM_H

#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>
#include <map>

class FSM;

//--------------------------------------------------------- IState_FSM:
// Interface for the state-specific behavior
class IState_FSM
{
public:
   virtual ~IState_FSM() = default;

   virtual void onEntry(FSM*) { }
   virtual void onExit(FSM*)  { }

   // Events (customise this part)
   virtual void event_stay(FSM*)   { throw std::runtime_error(">>> Event_stay out of order\n"); }
   virtual void event_change(FSM*) { throw std::runtime_error(">>> Event_change out of order\n"); }
   virtual void event_only_A(FSM*) { throw std::runtime_error(">>> Event_only_A out of order\n"); }
};

//--------------------------------------------------------- FSM (Context):
class FSM
{
private:
   // Using a map to own the state objects
   static inline std::map<std::string, std::unique_ptr<IState_FSM>> stateMap_;
   
   IState_FSM* currentState_{nullptr};

public:
   std::string name;
   int i{1};

   FSM(std::string name, const std::string& initialStateName)
      : name{std::move(name)}
   {
      currentState_ = stateMap_.at(initialStateName).get();
      currentState_->onEntry(this);
   }

   ~FSM()
   {
      currentState_->onExit(this);
   }
    
   // Register a state in the machine
   template<class StateClass>
   static void registerState(const std::string& stateName)
   {
      stateMap_[stateName] = std::make_unique<StateClass>();
   }

   void changeState(const std::string& nextStateName, void(FSM::*transitionAction)() = nullptr)
   {
      currentState_->onExit(this);
      
      if (transitionAction) (this->*transitionAction)();
      
      currentState_ = stateMap_.at(nextStateName).get();
      currentState_->onEntry(this);
   }

   // Transition actions (customise this part)
   void transition_AtoB() { std::cout << name << ": Transition_AtoB, i=" << i++ << "\n\n"; }
   void transition_BtoA() { std::cout << name << ": Transition_BtoA, i=" << i++ << "\n\n"; }
    
   // Events (customise this part)
   void event_stay()   { currentState_->event_stay(this); }
   void event_change() { currentState_->event_change(this); }
   void event_only_A() { currentState_->event_only_A(this); }
};

#endif
//================================================================================ END
