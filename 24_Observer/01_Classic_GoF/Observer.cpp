/**
 * ============================================================================
 * File: Observer.cpp (Classic GoF Version)
 * 
 * --- DESIGN OVERVIEW:
 * This implementation follows the classic Gang of Four (GoF) approach. 
 * It uses a polymorphic base class 'Observable' and 'Observer'. 
 * The Observable maintains a collection of Observers and notifies them 
 * via a single 'update(Observable*)' method.
 * 
 * --- THE GOF TRADE-OFF:
 * Since the 'update' method accepts a base pointer, the ConcreteObserver 
 * must cast this pointer to the concrete type (e.g., 'NumberObservable') to
 * access specific data. This is the traditional way to implement the pattern
 * before modern C++ techniques like CRTP were popularized.
 * 
 * --- ARCHITECTURE:
 * 1. Observable: Manages the list of Observers.
 * 2. Observer: Defines the interface for update notifications.
 * 3. Concrete Implementation: Observers query the Observable to synchronize.
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>

//--------------------------------------------------------- Observer Interface:
class Observable; // Forward declaration

class Observer
{
public:
   virtual ~Observer() = default;
   virtual void update(Observable* observable) = 0;
};

//--------------------------------------------------------- Observable Base:
class Observable
{
private:
   std::vector<Observer*> observers_;

public:
   virtual ~Observable() = default;

   void attach(Observer* obs)
   {
      observers_.push_back(obs);
   }

   void detach(Observer* obs)
   {
      observers_.erase(std::remove(observers_.begin(), observers_.end(), obs), 
                       observers_.end());
   }

protected:
   void notify()
   {
      for (auto* obs : observers_) obs->update(this);
   }
};

//--------------------------------------------------------- Concrete Observable:
class NumberObservable : public Observable
{
private:
   int val_{0};

public:
   void setVal(int newVal)
   {
      val_ = newVal;
      notify();
   }

   int getVal() const { return val_; }
};

//--------------------------------------------------------- Concrete Observers:
class DivObserver : public Observer
{
private:
   int div_;

public:
   explicit DivObserver(int div) : div_{div} { }

   void update(Observable* obs) override
   {
      // The classic GoF way: cast the base pointer to the concrete type
      auto* numberObservable = static_cast<NumberObservable*>(obs);
      int n = numberObservable->getVal();
      std::cout << "  [DivObserver] " << n << " div " << div_ << " is " << n / div_ << "\n";
   }
};

class ModObserver : public Observer
{
private:
   int div_;

public:
   explicit ModObserver(int div) : div_{div} { }

   void update(Observable* obs) override
   {
      auto* numberObservable = static_cast<NumberObservable*>(obs);
      int n = numberObservable->getVal();
      std::cout << "  [ModObserver] " << n << " mod " << div_ << " is " << n % div_ << "\n";
   }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== OBSERVER PATTERN (CLASSIC GOF) ===\n" << std::endl;

   NumberObservable numberObservable; // One Observable

   DivObserver divObserver{4};        // Two Observers
   ModObserver modObserver{3};
    
   numberObservable.attach(&divObserver);
   numberObservable.attach(&modObserver);
    
   std::cout << "--- Changing numberObservable state to 14 (with two observers):\n";
   numberObservable.setVal(14);
   
   std::cout << "\n--- Detaching divObserver and changing numberObservable to 20:\n";
   numberObservable.detach(&divObserver);
   numberObservable.setVal(20);

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
