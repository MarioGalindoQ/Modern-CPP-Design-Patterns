/**
 * ============================================================================
 * File: Command.cpp
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This implementation uses a generic 'CommandQueue' that manages a queue of 
 * 'Command' objects. The 'Receivers' (Cow, Dog, Car) remain decoupled from 
 * the 'CommandQueue'.
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
   void turnOn()  const { std::cout << " [Receiver] Car engine ON.\n"; }
   void turnOff() const { std::cout << " [Receiver] Car engine OFF.\n"; }
   void rev()     const { std::cout << " [Receiver] Car: Vroom, vroom!\n"; }
};

//--------------------------------------------------------- Command Interface:
class ICommand
{
public:
   virtual ~ICommand() = default;
   virtual void execute() const = 0;
};

//--------------------------------------------------------- Concrete Commands:
class CowCommand : public ICommand
{
private:
   Cow& receiver_;

public:
   explicit CowCommand(Cow& receiver) : receiver_{receiver} { }
   void execute() const override { receiver_.moo(); }
};

class DogCommand : public ICommand
{
private:
   Dog& receiver_;

public:
   explicit DogCommand(Dog& receiver) : receiver_{receiver} { }
   void execute() const override { receiver_.bark(); }
};

class CarCommand : public ICommand
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

//---------------------------------------------------- CommandQueue:
class CommandQueue
{
private:
   std::vector<std::unique_ptr<ICommand>> queue_;

public:
   void addCommand(std::unique_ptr<ICommand> cmd) { queue_.push_back(std::move(cmd)); }

   void runAll() const
   {
      for(const auto& command : queue_) command->execute();
   }
};

//--------------------------------------------------------- Main:
int main()
{
   std::cout << "=== COMMAND PATTERN SIMULATION ===\n" << std::endl;

   Cow cow;
   Dog dog{"Marshall"};
   Car car;

   CommandQueue commandQueue;

   commandQueue.addCommand(std::make_unique<CowCommand>(cow));
   commandQueue.addCommand(std::make_unique<DogCommand>(dog));
   commandQueue.addCommand(std::make_unique<CarCommand>(car));
   commandQueue.addCommand(std::make_unique<DogCommand>(dog));

   std::cout << "Executing command queue:\n";
   commandQueue.runAll();

   std::cout << "\nExecuting command queue (again):\n";
   commandQueue.runAll();

   std::cout << "\n--- Executing command outside the queue ---\n";
   std::make_unique<CarCommand>(car)->execute();

   std::cout << "\n=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
