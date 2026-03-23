/**
 * ============================================================================
 * File: Command.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This implementation uses a generic 'Invoker' that manages a queue of 
 * 'Command' objects. The 'Receivers' (Cow, Dog, Car) remain decoupled from 
 * the 'Invoker' (CommandQueue).
 * 
 * --- MEMORY MANAGEMENT:
 * The 'CommandQueue' owns the commands via 'std::unique_ptr', ensuring 
 * automatic cleanup of all issued requests.
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>

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
private:
   bool engineOn_{false};

public:
   void turnOn() { engineOn_ = true; std::cout << " [Receiver] Car engine ON.\n"; }
   void turnOff() { engineOn_ = false; std::cout << " [Receiver] Car engine OFF.\n"; }
   void rev() const
   {
      if (engineOn_) std::cout << " [Receiver] Car: Vroom, vroom!\n";
      else std::cout << " [Receiver] Car: Silence!\n";
   }
};

//--------------------------------------------------------- Command Interface:
class Command
{
public:
   virtual ~Command() = default;
   virtual void execute() const = 0;
};

//--------------------------------------------------------- Concrete Commands:
class CowCommand : public Command
{
private:
   Cow& receiver_;

public:
   explicit CowCommand(Cow& receiver) : receiver_{receiver} { }
   void execute() const override { receiver_.moo(); }
};

class DogCommand : public Command
{
private:
   Dog& receiver_;

public:
   explicit DogCommand(Dog& receiver) : receiver_{receiver} { }
   void execute() const override { receiver_.bark(); }
};

class CarCommand : public Command
{
private:
   Car& receiver_;

public:
   explicit CarCommand(Car& receiver) : receiver_{receiver} { }
   void execute() const override
   {
      receiver_.turnOn();
      receiver_.rev();
      receiver_.turnOff();
   }
};

//--------------------------------------------------------- Invoker:
class CommandQueue
{
private:
   std::vector<std::unique_ptr<Command>> queue_;

public:
   void addCommand(std::unique_ptr<Command> cmd) { queue_.push_back(std::move(cmd)); }

   void runAll() const
   {
      for (const auto& cmd : queue_) cmd->execute();
   }
};

//--------------------------------------------------------- Main:
int main()
{
   std::cout << "=== COMMAND PATTERN (JOB QUEUE SIMULATION) ===\n" << std::endl;

   Cow cow;
   Dog dog{"Marshall"};
   Car car;

   CommandQueue invoker;

   invoker.addCommand(std::make_unique<CowCommand>(cow));
   invoker.addCommand(std::make_unique<DogCommand>(dog));
   invoker.addCommand(std::make_unique<CarCommand>(car));

   std::cout << "Executing command queue:\n";
   invoker.runAll();

   std::cout << "\nExecuting command queue (again):\n";
   invoker.runAll();

   std::cout << "\n--- Executing command outside the queue ---\n";
   std::make_unique<CarCommand>(car)->execute();

   std::cout << "\n=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
