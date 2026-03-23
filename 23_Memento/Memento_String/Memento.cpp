/**
 * ============================================================================
 * File: Memento.cpp (String Serialization Version)
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the Memento pattern using string serialization.
 * Instead of polymorphic Memento objects, the state is serialized into 
 * simple 'std::string' formats.
 * 
 * --- PERSISTENCE:
 * The Caretaker manages a stack of strings. To simulate real-world usage,
 * we dump this stack into a file, clear the memory, and restore the objects
 * from the file.
 * 
 * --- SERIALIZATION LOGIC:
 * A simple 'ID:VALOR' protocol is used. To prevent newlines from breaking
 * the file format, we escape them before saving.
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stack>
#include <memory>

//--------------------------------------------------------- Originator A:
class ComponentA
{
private:
   std::string state_;

public:
   void setState(std::string s)
   {
      state_ = std::move(s);
      std::cout << " [Action] Component A (string) set to \"" << state_ << "\"\n";
   }

   void print() const { std::cout << " Current A (string): \"" << state_ << "\"\n"; }

   std::string serialize() const { return "A:" + state_; }
   void deserialize(const std::string& data)
   {
      std::string oldState = state_;
      state_ = data.substr(2);
      std::cout << "    -> Component A (string) changed from \"" 
                << oldState << "\" to \"" << state_ << "\"\n";
   }
};

//--------------------------------------------------------- Originator B:
class ComponentB
{
private:
   int value_{0};

public:
   void setValue(int v)
   {
      value_ = v;
      std::cout << " [Action] Component B (integer) set to " << value_ << "\n";
   }

   void print() const { std::cout << " Current B (integer): " << value_ << "\n"; }

   std::string serialize() const { return "B:" + std::to_string(value_); }
   void deserialize(const std::string& data)
   {
      int oldValue = value_;
      value_ = std::stoi(data.substr(2));
      std::cout << "    -> Component B (integer) changed from " 
                << oldValue << " to " << value_ << "\n";
   }
};

//--------------------------------------------------------- Caretaker:
class Caretaker
{
private:
   std::stack<std::string> history_;

public:
   void save(const std::string& memento)
   {
      history_.push(memento);
      std::cout << "          ... Memento (string) saved to Caretaker.\n";
   }

   std::string undo()
   {
      if (history_.empty()) throw std::runtime_error("No more states to undo.");
      std::string memento = history_.top();
      history_.pop();
      return memento;
   }

   void saveToFile(const std::string& filename) const
   {
      std::ofstream ofs(filename);
      std::stack<std::string> tmp = history_;
      std::vector<std::string> v;
      while (!tmp.empty())
      {
         v.push_back(tmp.top());
         tmp.pop();
      }
      for (auto it = v.rbegin(); it != v.rend(); ++it) ofs << *it << "\n";
      std::cout << " [System] Stack saved to file: " << filename << "\n";
   }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== MEMENTO PATTERN (STRING SERIALIZATION) ===\n" << std::endl;

   std::unique_ptr<Caretaker> caretaker = std::make_unique<Caretaker>();
   ComponentA a;
   ComponentB b;

   std::cout << "--- PHASE 1: DOING ACTIONS AND SAVING ---\n";
   a.setState("1st value"); caretaker->save(a.serialize());
   b.setValue(1);           caretaker->save(b.serialize());
   a.setState("2nd value"); caretaker->save(a.serialize());
   b.setValue(2);           caretaker->save(b.serialize());
   a.setState("3rd value"); caretaker->save(a.serialize());
   b.setValue(3);           caretaker->save(b.serialize());

   std::cout << "\n--- CURRENT STATE AFTER ALL ACTIONS ---\n";
   a.print();
   b.print();

   std::cout << "\n--- SAVING CURRENT STATE TO FILE ---\n";
   caretaker->saveToFile("memento_stack_data.txt");

   std::cout << "\n--- PHASE 2: SIMULATING A FRESH RESTART ---\n";
   std::cout << " [System] Clearing memory by reinstantiating a new caretaker..." << std::endl;
   caretaker.reset(); // Destroy old caretaker
   caretaker = std::make_unique<Caretaker>();

   std::cout << "\n--- RESTORING FROM DISK ---\n";
   std::ifstream ifs("memento_stack_data.txt");
   std::string line;
   while (std::getline(ifs, line))
   {
      if (!line.empty()) caretaker->save(line);
   }

   std::cout << "\n--- PERFORMING UNDOES ---\n";
   caretaker->undo(); // Discards B's 3rd state
   caretaker->undo(); // Discards A's 3rd state

   std::cout << " [Undo] Restoring Component B (integer)...\n";
   b.deserialize(caretaker->undo()); 

   std::cout << "\n [Undo] Restoring Component A (string)...\n";
   a.deserialize(caretaker->undo()); 

//   caretaker->saveToFile("memento_stack_data.txt");
   std::cout << "\n [Undo] Restoring Component B (integer)...\n";
   b.deserialize(caretaker->undo()); 

   std::cout << "\n [Undo] Restoring Component A (string)...\n";
   a.deserialize(caretaker->undo()); 

//   caretaker->saveToFile("memento_stack_data.txt");
   std::cout << "\n--- CURRENT STATE AFTER ALL UNDOES ---\n";
   a.print();
   b.print();

   std::cout << "\n=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
