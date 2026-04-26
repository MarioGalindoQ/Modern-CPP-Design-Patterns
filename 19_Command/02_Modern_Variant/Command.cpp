/**
 * ============================================================================
 * File: Command.cpp (Modern Variant Version)
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates a non-intrusive Command pattern using std::variant.
 * Commands are represented as simple data structures, and their execution
 * is dispatched via std::visit.
 * 
 * --- ADVANTAGES:
 * 1. No inheritance required for commands.
 * 2. Commands are stored by value in the queue (no pointers/new/delete).
 * 3. The execution logic is centralized in the 'CommandExecutor' visitor.
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <variant>

//--------------------------------------------------------- Receivers:
class Cow
{
public:
   void moo() const { std::cout << " [Receiver] Cow says moo!\n"; }
};

class Dog
{
private:
   std::string name_;
public:
   explicit Dog(std::string name) : name_{std::move(name)} { }
   void bark() const { std::cout << " [Receiver] " << name_ << " barks!\n"; }
};

class Car
{
public:
   void turnOn()  const { std::cout << " [Receiver] Car engine ON.\n"; }
   void turnOff() const { std::cout << " [Receiver] Car engine OFF.\n"; }
   void rev()     const { std::cout << " [Receiver] Car: Vroom, vroom!\n"; }
};

//--------------------------------------------------------- Command Data:
// Commands are now just simple empty structs (Data markers)
struct CowCommand { };
struct DogCommand { };
struct CarCommand { };

using Command = std::variant<CowCommand, DogCommand, CarCommand>;

//--------------------------------------------------------- Command Executor:
// This is the "Visitor" that knows how to execute
// each command data type and owns the receivers.
class CommandExecutor
{
private:
   Cow cow;
   Dog dog{"Marshall"};
   Car car;

public:
   void operator()(const CowCommand&) const { cow.moo(); }
   void operator()(const DogCommand&) const { dog.bark(); }
   void operator()(const CarCommand&) const
   {
      car.turnOn();
      car.rev();
      car.turnOff();
   }
};

//---------------------------------------------------- CommandQueue:
class CommandQueue
{
private:
   std::vector<Command> queue_;
   CommandExecutor& commandExecutor_;

public:
   CommandQueue(CommandExecutor& commandExecutor) : commandExecutor_{commandExecutor} {}

   void addCommand(Command cmd) { queue_.push_back(std::move(cmd)); }

   void runAll() const
   {
      for(const auto& command : queue_) std::visit(commandExecutor_, command);
   }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== MODERN VARIANT COMMAND PATTERN SIMULATION ===\n" << std::endl;

   CommandExecutor commandExecutor;

   CommandQueue commandQueue{commandExecutor};

   commandQueue.addCommand(CowCommand{});
   commandQueue.addCommand(DogCommand{});
   commandQueue.addCommand(CarCommand{});
   commandQueue.addCommand(DogCommand{});

   std::cout << "Executing command queue:\n";
   commandQueue.runAll();

   std::cout << "\nExecuting command queue (again):\n";
   commandQueue.runAll();

   std::cout << "\n--- Executing command outside the queue ---\n";
   std::visit(commandExecutor, Command{CarCommand{}});

   std::cout << "\n=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
