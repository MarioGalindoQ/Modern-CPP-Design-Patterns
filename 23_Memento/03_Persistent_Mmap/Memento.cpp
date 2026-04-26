/**
 * ============================================================================
 * File: Memento.cpp (Persistent Mmap with Auto-Save & Crash Simulation)
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates a robust Memento pattern with disk persistence.
 * 1. Auto-Save: Components notify the Caretaker of every change.
 * 2. Crash Recovery: The Caretaker restores the system state from the 
 *    binary file (".bin") upon instantiation.
 * 3. Time Travel: Full Undo/Redo support. New actions after an Undo 
 *    will overwrite any existing "future" history.
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdexcept>
#include <memory>

//--------------------------------------------------------- Binary Memento (POD):
struct FileHeader
{
   size_t count;  // Total valid states in the timeline // (8 bytes)
   size_t cursor; // Current position in time           // (8 bytes), Total = 16 bytes
};

struct SystemState
{
   int valB;      // ( 4 bytes)
   char strA[64]; // (64 bytes), Total = 68 bytes
};

//--------------------------------------------------------- Concrete Originators:
class Caretaker; // Forward declaration

class ComponentA
{
private:
   std::string state_{""};
   Caretaker& caretaker_;

public:
   ComponentA(Caretaker& c) : caretaker_{c} { }
   void setState(std::string s); // Defined after Caretaker 
   void internalSet(std::string s) { state_ = std::move(s); }
   void print() const { std::cout << " Current A (string):  \"" << state_ << "\"\n"; }
   std::string getState() const { return state_; }
};

class ComponentB
{
private:
   int value_{0};
   Caretaker& caretaker_;

public:
   ComponentB(Caretaker& c) : caretaker_{c} { }
   void setValue(int v); // Defined after Caretaker
   void internalSet(int v) { value_ = v; }
   void print() const { std::cout << " Current B (integer): " << value_ << "\n"; }
   int getValue() const { return value_; }
};

//--------------------------------------------------------- Persistent Caretaker:
class Caretaker
{
private:
   constexpr static size_t MAX_STATES = 120; // FILE_SIZE = 16 + 68*120 = 8176 almost 8kB
   constexpr static size_t FILE_SIZE = sizeof(FileHeader) + (sizeof(SystemState) * MAX_STATES);

   int fd_;
   void* mappedRegion_;
   FileHeader* header_;
   SystemState* history_;
   
   ComponentA* a_{nullptr};
   ComponentB* b_{nullptr};
   bool isRestoring_{false}; 

public:
   explicit Caretaker(const std::string& filename)
   {
      fd_ = open(filename.c_str(), O_RDWR | O_CREAT, 0644);
      if(fd_ < 0) throw std::runtime_error("Could not open persistence file.");

      off_t currentSize = lseek(fd_, 0, SEEK_END);
      if(currentSize == 0)
         if(ftruncate(fd_, FILE_SIZE) != 0) throw std::runtime_error("File allocation failed.");

      mappedRegion_ = mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
      if(mappedRegion_ == MAP_FAILED) throw std::runtime_error("mmap failed.");

      header_ = static_cast<FileHeader*>(mappedRegion_);
      history_ = reinterpret_cast<SystemState*>((char*)mappedRegion_ + sizeof(FileHeader));
   }

   ~Caretaker()
   {
      munmap(mappedRegion_, FILE_SIZE);
      close(fd_);
   }

   void setComponents(ComponentA* a, ComponentB* b)
   {
      a_ = a; b_ = b;
      if(header_->count > 0)
      {
         std::cout << " [System] Recovery: Restoring state " << header_->cursor << " from disk.\n";
         applyState(history_[header_->cursor]);
      }
   }

   void save()
   {
      if(isRestoring_) return;

      if(header_->cursor + 1 >= MAX_STATES) 
         throw std::runtime_error("History file is full! Cannot save more states.\n");

      SystemState systemSate;

      systemSate.valB = b_->getValue();

      std::strncpy(systemSate.strA, a_->getState().c_str(), 63);
      systemSate.strA[63] = '\0';

      if(header_->count > 0) header_->cursor++;
      
      history_[header_->cursor] = systemSate;
      header_->count = header_->cursor + 1;

      std::cout << "          ... Automatic checkpoint saved (State " << header_->cursor << ").\n";
   }

   void undo()
   {
      if(header_->cursor > 0)
      {
         isRestoring_ = true;
         header_->cursor--;
         applyState(history_[header_->cursor]);
         std::cout << " [Undo] System rolled back to state " << header_->cursor << "\n";
         isRestoring_ = false;
      }
      else std::cout << " [System] Cannot Undo: start of history reached.\n";
   }

   void redo()
   {
      if(header_->cursor < header_->count - 1)
      {
         isRestoring_ = true;
         header_->cursor++;
         applyState(history_[header_->cursor]);
         std::cout << " [Redo] System moved forward to state " << header_->cursor << "\n";
         isRestoring_ = false;
      }
      else std::cout << " [System] Cannot Redo: latest state reached.\n";
   }

   void applyState(const SystemState& s)
   {
      a_->internalSet(s.strA);
      b_->internalSet(s.valB);
   }

   void resetFile()
   {
      header_->count = 0;
      header_->cursor = 0;
   }
};

//--------------------------------------------------------- Implementation:
void ComponentA::setState(std::string s) 
{ 
   state_ = std::move(s); 
   std::cout << " [Action] Component A set to \"" << state_ << "\"\n";
   caretaker_.save(); 
}

void ComponentB::setValue(int v) 
{ 
   value_ = v; 
   std::cout << " [Action] Component B set to " << value_ << "\n";
   caretaker_.save(); 
}

//--------------------------------------------------------- Main Simulation:
int main()
{
   try
   {
      std::cout << "=== MEMENTO PATTERN (MMAP PERSISTENCE & CRASH SIMULATION) ===\n" << std::endl;
      const std::string dbFile = "memento_history.bin";

      {
         std::cout << "--- PHASE 1: INITIAL EXECUTION & SAVE ---\n";
         auto caretaker = std::make_unique<Caretaker>(dbFile);
         caretaker->resetFile(); 

         ComponentA a(*caretaker);
         ComponentB b(*caretaker);
         caretaker->setComponents(&a, &b);

         a.setState("Alpha");  // A="Alpha", B=0   (State 0)
         a.setState("Beta");   // A="Beta",  B=0   (State 1)
         b.setValue(50);       // A="Beta",  B=50  (State 2)
         b.setValue(100);      // A="Beta",  B=100 (State 3)
         a.print(); b.print();
         std::cout << " [CRASH] Program terminated unexpectedly!\n\n";
      } 

      {
         std::cout << "--- PHASE 2: RESTART & RECOVERY ---\n";
         auto caretaker = std::make_unique<Caretaker>(dbFile);

         ComponentA a(*caretaker);
         ComponentB b(*caretaker);
         caretaker->setComponents(&a, &b); // Recovers State 3 (A="Beta", B=100)
         a.print(); b.print();
      
         std::cout << "\n--- PHASE 3: UNDO & REDO ---\n";
         caretaker->undo();    // A="Beta",  B=50  (State 2)
         a.print(); b.print();

         caretaker->redo();    // A="Beta",  B=100 (State 3)
         a.print(); b.print();

         std::cout << "\n--- PHASE 4: CONTINUING ACTIONS ---\n";
         a.setState("Gamma");  // A="Gamma", B=100 (State 4)
         b.setValue(200);      // A="Gamma", B=200 (State 5)
         b.setValue(800);      // A="Gamma", B=800 (State 6)
         a.print(); b.print();

         std::cout << "\n--- PHASE 5: MULTIPLE UNDOS ---\n";
         caretaker->undo();    // A="Gamma", B=200 (State 5)
         a.print(); b.print();

         caretaker->undo();    // A="Gamma", B=100 (State 4)
         a.print(); b.print();

         caretaker->undo();    // A="Beta",  B=100 (State 3)
         a.print(); b.print();

         std::cout << "\n--- PHASE 6: REDO TEST ---\n";
         caretaker->redo();    // A="Gamma", B=100 (State 4)
         a.print(); b.print();

         std::cout << "\n--- PHASE 7: NEW ACTION (KILLING REDO) ---\n";
         b.setValue(600);      // A="Gamma", B=600 (New State 5, old States 5 & 6 are lost)
         a.print(); b.print();
      
         std::cout << "\n--- PHASE 8: ATTEMPTING TO REDO (SHOULD FAIL) ---\n";
         caretaker->redo();
         a.print(); b.print();

         std::cout << "\n--- PHASE 9: UNDO & REDO ---\n";
         caretaker->undo();    // A="Gamma", B=100 (State 4)
         a.print(); b.print();

         caretaker->redo();    // A="Gamma", B=600 (New State 5)
         a.print(); b.print();
      }
   }
   catch(const std::runtime_error& e)
   {
      std::cerr << "\nCaught error: " << e.what() << std::endl;
   }

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
