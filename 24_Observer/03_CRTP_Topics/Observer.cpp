/**
 * ============================================================================
 * File: Observer.cpp (With Topics)
 *
 * --- DESIGN OVERVIEW:
 * This advanced implementation introduces "Topics" (or Events). Instead
 * of broadcasting every change to all observers, observers subscribe only
 * to specific topics they care about (e.g., Value changes vs Color changes).
 * This significantly reduces unnecessary updates and improves efficiency.
 *
 * --- CROSS-REFERENCE (CRTP):
 * For a deep dive into Static Polymorphism and the Curiously Recurring
 * Template Pattern (CRTP) used here, please see folder '32_CRTP/'.
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
#include <unordered_set> // In this case is faster than <set>
#include <vector>
#include <type_traits>

//--------------------------------------------------------- Topics (Events):
// We use 'enum class' for modern type safety.
// 'END' must be the last element to properly size the vector of sets.
enum class Topic
{
   Value,
   Color,
   END // Must be the last topic
};

//--------------------------------------------------------- Forward Declaration:
template<class ConcreteObservable, Topic TopicType>
class Observer;

//--------------------------------------------------------- Observable Base:
template<class ConcreteObservable>
class Observable
{
private:
   // A vector of unordered_sets. Each index in the vector represents a Topic.
   // We store void* because different topics might have different Observer types.
   std::vector<std::unordered_set<void*>> observers_;

   Observable()
   {
      // Resize the vector to accommodate all topics in O(1) access time
      observers_.resize(static_cast<size_t>(Topic::END));
   }
   
   friend ConcreteObservable;

protected:
   virtual ~Observable() = default;

template<size_t Index = 0>
void dispatchNotify(void* obs, Topic t)
{
   if constexpr (Index < static_cast<size_t>(Topic::END))
   {
      if (Index == static_cast<size_t>(t))
      {
         constexpr Topic TopicValue = static_cast<Topic>(Index);
         static_cast<Observer<ConcreteObservable, TopicValue>*>(obs)->update(*static_cast<ConcreteObservable*>(this), t);
      }
      else
         dispatchNotify<Index + 1>(obs, t);
   }
}

void notify(Topic t)
{
   size_t index = static_cast<size_t>(t);
   for (auto obs : observers_[index])
      dispatchNotify(obs, t);
}

public:
   // Auto-detect and attach based on Observer inheritance
   template<class T>
   void attach(T& obs)
   {
      if constexpr (std::is_base_of_v<Observer<ConcreteObservable, Topic::Value>, T>)
         observers_[static_cast<size_t>(Topic::Value)].insert(&obs);

      if constexpr (std::is_base_of_v<Observer<ConcreteObservable, Topic::Color>, T>)
         observers_[static_cast<size_t>(Topic::Color)].insert(&obs);
   }

   // Detach from all potential topic sets
   template<class T>
   void detach(T& obs)
   {
      for (auto& observer_set : observers_) observer_set.erase(&obs);
   }
};

//--------------------------------------------------------- Observer Interface:
template<class ConcreteObservable, Topic TopicType>
class Observer
{
   template<class> friend class Observable;

protected:
   virtual ~Observer() = default;

   // The update method receives both the specific Observable and the Topic
   virtual void update(ConcreteObservable&, Topic) = 0;
};

//--------------------------------------------------------- Concrete Observable:
class NumberObservable : public Observable<NumberObservable>
{
private:
   int  value_{0};
   char color_{'A'};

public:
   void setValue(int newValue)
   {
      value_ = newValue;
      notify(Topic::Value); // Notify only subscribers to 'Value'
   }

   void setColor(char newColor)
   {
      color_ = newColor;
      notify(Topic::Color); // Notify only subscribers to 'Color'
   }

   int  getValue() const {return value_;}
   char getColor() const {return color_;}
};

//--------------------------------------------------------- Concrete Observers:
// This observer listens to BOTH Value and Color changes.
class ValueColorObserver : public Observer<NumberObservable, Topic::Value>,
                           public Observer<NumberObservable, Topic::Color>
{
protected:
   void update(NumberObservable& num, Topic t) override
   {
      switch (t)
      {
         case Topic::Value:
            std::cout << "  [ValueColorObserver] Value changed to " << num.getValue() << '\n';
            break;
         case Topic::Color:
            std::cout << "  [ValueColorObserver] Color changed to " << num.getColor() << '\n';
            break;
         default:
            break;
      }
   }
};

// This observer listens ONLY to Value changes.
class ValueObserver : public Observer<NumberObservable, Topic::Value>
{
protected:
   void update(NumberObservable& num, Topic) override
   {
      std::cout << "  [ValueObserver] Value changed to " << num.getValue() << '\n';
   }
};

// This observer listens ONLY to Color changes.
class ColorObserver : public Observer<NumberObservable, Topic::Color>
{
protected:
   void update(NumberObservable& num, Topic) override
   {
      std::cout << "  [ColorObserver] Color changed to " << num.getColor() << '\n';
   }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== OBSERVER PATTERN (WITH TOPICS) ===\n" << std::endl;

   // Create two observables with two topics each (Value and Color)
   NumberObservable numberObservable_1;
   NumberObservable numberObservable_2;

   // Create three observers
   ValueColorObserver observer_1;
   ValueObserver      observer_2;
   ColorObserver      observer_3;

   // Attach observers automatically. No need to specify the Topic!
   numberObservable_1.attach(observer_1);
   numberObservable_1.attach(observer_2);
   numberObservable_1.attach(observer_3);

   std::cout << "--- Initial state changes for numberObservable_1:\n";
   std::cout << "Setting Value to 14 (Triggers ValueColorObserver and ValueObserver):\n";
   numberObservable_1.setValue(14);
   
   std::cout << "\nSetting Color to 'A' (Triggers ValueColorObserver and ColorObserver):\n";
   numberObservable_1.setColor('A');

   std::cout << "\n--- Detaching observer_1 from both topics on numberObservable_1...\n";
   numberObservable_1.detach(observer_1);
   
   std::cout << "--- Attaching observer_1 to numberObservable_2...\n";
   numberObservable_2.attach(observer_1);

   std::cout << "\nSetting Value to 18 on numberObservable_1 (Should only trigger ValueObserver):\n";
   numberObservable_1.setValue(18);
   
   std::cout << "\nSetting Color to 'B' on numberObservable_1 (Should only trigger ColorObserver):\n";
   numberObservable_1.setColor('B');
   
   std::cout << "\nSetting Value to 15 and Color to 'C' on numberObservable_2 (Triggers ValueColorObserver):\n";
   numberObservable_2.setValue(15);
   numberObservable_2.setColor('C');

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
