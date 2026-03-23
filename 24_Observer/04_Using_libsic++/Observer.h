#ifndef OBSERVER_H
#define OBSERVER_H

#include <sigc++/sigc++.h>

template<class ConcreteSubject>
class Observer : public sigc::trackable // Must inherit from sigc::trackable
{
   template<class> friend class Subject;
   virtual void update(ConcreteSubject&) = 0;
};

template<class ConcreteSubject>
class Subject
{
private:
   sigc::signal<void(ConcreteSubject&)> observers;
   Subject() = default; // Force constructor to be private!!!
   friend ConcreteSubject; // Only ConcreteSubject can construct Subject.

protected:
   void notify()
   {
      observers.emit(*static_cast<ConcreteSubject*>(this));
   }

public:
   sigc::connection attach(Observer<ConcreteSubject>& obs)
   {
      return observers.connect(sigc::mem_fun(&obs, &Observer<ConcreteSubject>::update));
   }
};

#endif
