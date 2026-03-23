/**
 * ============================================================================
 * File: Memento.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the classic GoF Memento pattern.
 * 1. Originator: Creates and restores its state from a Memento.
 * 2. Memento: An opaque object for everyone except its Creator.
 * 3. Caretaker: Manages the history (stack) of Mementos without inspecting.
 * 
 * --- ENCAPSULATION NOTE:
 * By nesting the concrete Memento struct inside the Originator class, 
 * we guarantee that ONLY the specific Originator knows the internal 
 * structure of its own Memento. The Caretaker only sees a generic 
 * base pointer (std::unique_ptr<Memento>).
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <memory>
#include <stack>
#include <stdexcept>

//--------------------------------------------------------- Memento Interface:
// Opaque interface for the Caretaker
class Memento
{
public:
   virtual ~Memento() = default;
};

using MementoPtr = std::unique_ptr<Memento>;

//--------------------------------------------------------- Originator Interface:
class Originator
{
public:
   virtual ~Originator() = default;
   virtual MementoPtr createMemento() const = 0;
   virtual void restoreMemento(MementoPtr memento) = 0;
};

//--------------------------------------------------------- Caretaker:
// Manages the safekeeping of Mementos without inspecting them.
class Caretaker
{
private:
   std::stack<MementoPtr> history_;

public:
   void save(MementoPtr memento)
   {
      history_.push(std::move(memento));
      std::cout << "          ... Memento saved to Caretaker.\n";
   }

   MementoPtr undo()
   {
      if (history_.empty()) throw std::out_of_range("No more states to undo.");
      
      MementoPtr memento = std::move(history_.top());
      history_.pop();
      return memento;
   }

   void discardLatest()
   {
      if (!history_.empty()) history_.pop();
   }
};

//--------------------------------------------------------- Concrete Originator A:
class ComponentA : public Originator
{
private:
   std::string state_;

   // The concrete Memento is hidden inside the Originator
   struct MementoA : public Memento
   {
      std::string savedState;
      explicit MementoA(std::string s) : savedState{std::move(s)} { }
   };

public:
   void setState(std::string s)
   {
      state_ = std::move(s);
      std::cout << " [Action] Component A (string) set to \"" << state_ << "\"\n";
   }

   void print() const
   {
      std::cout << " Current A (string): \"" << state_ << "\"\n";
   }

   MementoPtr createMemento() const override
   {
      return std::make_unique<MementoA>(state_);
   }

   void restoreMemento(MementoPtr memento) override
   {
      auto* m = dynamic_cast<MementoA*>(memento.get());
      if (!m) throw std::invalid_argument("Invalid Memento passed to ComponentA");

      std::string oldState = state_;
      state_ = std::move(m->savedState);
      
      std::cout << "    -> Component A (string) changed from \"" 
                << oldState << "\" to \"" << state_ << "\"\n";
   }
};

//--------------------------------------------------------- Concrete Originator B:
class ComponentB : public Originator
{
private:
   int value_{0};

   struct MementoB : public Memento
   {
      int savedValue;
      explicit MementoB(int v) : savedValue{v} { }
   };

public:
   void setValue(int v)
   {
      value_ = v;
      std::cout << " [Action] Component B (integer) set to " << value_ << "\n";
   }

   void print() const
   {
      std::cout << " Current B (integer): " << value_ << "\n";
   }

   MementoPtr createMemento() const override
   {
      return std::make_unique<MementoB>(value_);
   }

   void restoreMemento(MementoPtr memento) override
   {
      auto* m = dynamic_cast<MementoB*>(memento.get());
      if (!m) throw std::invalid_argument("Invalid Memento passed to ComponentB");

      int oldValue = value_;
      value_ = m->savedValue;
      
      std::cout << "    -> Component B (integer) changed from " 
                << oldValue << " to " << value_ << "\n";
   }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== MEMENTO PATTERN (CLASSIC GOF) ===\n" << std::endl;

   Caretaker caretaker;
   ComponentA a;
   ComponentB b;

   std::cout << "--- PHASE 1: DOING ACTIONS AND SAVING MEMENTOS ---\n";

   a.setState("1st value"); 
   caretaker.save(a.createMemento());
   
   b.setValue(1); 
   caretaker.save(b.createMemento());
   std::cout << "\n";

   a.setState("2nd value"); 
   caretaker.save(a.createMemento());
   
   b.setValue(2); 
   caretaker.save(b.createMemento());
   std::cout << "\n";

   a.setState("3rd value"); 
   caretaker.save(a.createMemento());
   
   b.setValue(3); 
   caretaker.save(b.createMemento());

   std::cout << "\n--- CURRENT STATE AFTER ALL ACTIONS ---\n";
   a.print();
   b.print();

   std::cout << "\n--- PHASE 2: UNDOING ACTIONS ---\n";

   try 
   {
      std::cout << " [System] Discarding the latest saved states to perform Undo...\n\n";
      caretaker.discardLatest(); // Discards B's 3rd state
      caretaker.discardLatest(); // Discards A's 3rd state

      std::cout << " [Undo] Restoring Component B (integer)...\n";
      b.restoreMemento(caretaker.undo()); 

      std::cout << "\n [Undo] Restoring Component A (string)...\n";
      a.restoreMemento(caretaker.undo()); 

      std::cout << "\n [Undo] Restoring Component B (integer)...\n";
      b.restoreMemento(caretaker.undo());

      std::cout << "\n [Undo] Restoring Component A (string)...\n";
      a.restoreMemento(caretaker.undo());

      std::cout << "\n--- CURRENT STATE AFTER ALL UNDOES ---\n";
      a.print();
      b.print();
   }
   catch (const std::exception& e)
   {
      std::cerr << " [Error] " << e.what() << "\n";
   }

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
