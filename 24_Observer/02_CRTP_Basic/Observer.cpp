/**
 * ============================================================================
 * File: Observer.cpp (Without Topics)
 * 
 * --- DESIGN OVERVIEW:
 * This implementation uses the CRTP (Curiously Recurring Template Pattern) 
 * to inject the concrete observable type into the base Observable and 
 * Observer classes. This allows the Observer to interact directly with 
 * the specific Observable without needing a dynamic_cast.
 * 
 * --- CROSS-REFERENCE (CRTP):
 * For a deep dive into how Static Polymorphism and the Curiously Recurring 
 * Template Pattern (CRTP) work in C++, please see folder '32_CRTP/'.
 * 
 * --- MULTIPLE OBSERVABLES:
 * This implementation supports multiple Observables for each Observer. 
 * The Observable passed to the 'update' operation allows the Observer 
 * to interact with the specific Observable that has changed.
 * 
 * --- TERMINOLOGY:
 * We use 'Observable' instead of the classic GoF term 'Subject' because 
 * it describes the role of the watched object much more clearly.
 * ============================================================================
 */

#include <iostream>
#include <set>
#include <string>

//--------------------------------------------------------- Observer Interface:
template<class ConcreteObservable>
class Observer
{
private:
   template<class> friend class Observable;

protected:
   virtual ~Observer() = default;

   // Called by the Observable when its state changes
   virtual void update(ConcreteObservable&) = 0;
};

//--------------------------------------------------------- Observable Base:
// Uses CRTP to know its own concrete type at compile time
template<class ConcreteObservable>
class Observable
{
private:
   // Non-owning pointers. Observers must outlive or detach themselves.
   std::set<Observer<ConcreteObservable>*> observers_;

   // Force constructor to be private to ensure only ConcreteObservable can build it
   Observable() = default;
   friend ConcreteObservable;

protected:
   virtual ~Observable() = default;

   void notify()
   {
      for (auto obs : observers_) obs->update(*static_cast<ConcreteObservable*>(this));
   }

public:
   void attach(Observer<ConcreteObservable>& obs) { observers_.insert(&obs); }
   void detach(Observer<ConcreteObservable>& obs) { observers_.erase(&obs); }
};

//--------------------------------------------------------- Concrete Observable:
class NumberObservable : public Observable<NumberObservable>
{
private:
   int val_{0};

public:
   void setVal(int i)
   {
      val_ = i;
      notify(); // Must be the last line of the state change
   }

   int getVal() const { return val_; }
};

//--------------------------------------------------------- Concrete Observers:
class DivObserver : public Observer<NumberObservable>
{
private:
   int div_;

protected:
   void update(NumberObservable& num) override
   {
      int n = num.getVal();
      std::cout << "  [DivObserver] " << n << " div " << div_ << " is " << n / div_ << '\n';
   }

public:
   explicit DivObserver(int div) : div_{div} { }
};

class ModObserver : public Observer<NumberObservable>
{
private:
   int div_;

protected:
   void update(NumberObservable& num) override
   {
      int n = num.getVal();
      std::cout << "  [ModObserver] " << n << " mod " << div_ << " is " << n % div_ << '\n';
   }

public:
   explicit ModObserver(int div) : div_{div} { }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== OBSERVER PATTERN (WITHOUT TOPICS) ===\n" << std::endl;

   // Create two observables
   NumberObservable numberObservable_1;
   NumberObservable numberObservable_2;

   // Create three observers
   DivObserver observer_1{4};
   DivObserver observer_2{3};
   ModObserver observer_3{3};
    
   // Attach observers to observables (demonstrating multiple Observables per Observer)
   numberObservable_1.attach(observer_1);
   numberObservable_1.attach(observer_2);
   numberObservable_1.attach(observer_3);
    
   numberObservable_2.attach(observer_1);
   numberObservable_2.attach(observer_2);
   numberObservable_2.attach(observer_3);
    
   std::cout << "--- With three observers attached to numberObservables 1 and 2:\n";
   std::cout << "Setting numberObservable_1 to 14:\n";
   numberObservable_1.setVal(14);
   
   std::cout << "\nSetting numberObservable_2 to 18:\n";
   numberObservable_2.setVal(18);

   std::cout << "\n--- Detaching observer_2 from numberObservable_1:\n";
   numberObservable_1.detach(observer_2);
   std::cout << "Setting numberObservable_1 to 14 again:\n";
   numberObservable_1.setVal(14);
    
   std::cout << "\n--- Detaching observers 1 and 3, and reattaching observer_2 to numberObservable_1:\n";
   numberObservable_1.detach(observer_1);
   numberObservable_1.detach(observer_3);
   numberObservable_1.attach(observer_2);
   std::cout << "Setting numberObservable_1 to 14 again:\n";
   numberObservable_1.setVal(14);

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
