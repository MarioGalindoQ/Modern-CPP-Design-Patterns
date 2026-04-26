// **************************************************************************
// Observer.h - Generic Observer Pattern Framework using libsigc++
// Author: Mario Galindo Queralt, Ph.D.
// **************************************************************************
// This file defines the template base classes 'Observer' and 'Subject'.
// It implements a type-safe callback system for the Observer pattern.
//
// Key features:
// - Template-based: Supports any ConcreteSubject type.
// - libsigc++ integration: Uses sigc::signal and sigc::trackable.
// - Memory safety: Automatic disconnection when objects are destroyed.
// - Modern C++: Uses std::static_cast and connection management.
// **************************************************************************

#ifndef OBSERVER_H
#define OBSERVER_H

#include <sigc++/sigc++.h>

template<class ConcreteSubject>
class Observer : public sigc::trackable 
{
public:
   /**
    * Pure virtual function called by the subject when its state changes.
    * @param subject The instance of the subject that changed.
    */
   virtual void update(ConcreteSubject&) = 0;
   virtual ~Observer() = default;
};

template<class ConcreteSubject>
class Subject
{
private:
   sigc::signal<void(ConcreteSubject&)> observers;
   Subject() = default; 
   friend ConcreteSubject; 

protected:
   /**
    * Emits the signal to all attached observers.
    */
   void notify()
   {
      observers.emit(*static_cast<ConcreteSubject*>(this));
   }

public:
   /**
    * Connects an observer to this subject.
    * @param obs The observer instance to be attached.
    * @return A sigc::connection object to manage the lifecycle of the link.
    */
   sigc::connection attach(Observer<ConcreteSubject>& obs)
   {
      return observers.connect(sigc::mem_fun(obs, &Observer<ConcreteSubject>::update));
   }
};

#endif

//================================================================================ END
