/**
 * ============================================================================
 * File: NullObject.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the Null Object pattern. We use a 'NullLogger' 
 * as a safe substitute for a real logger. 
 * 
 * --- ARCHITECTURAL NOTE:
 * By implementing 'NullLogger' as a Singleton, we ensure that we don't 
 * waste memory creating multiple "do-nothing" objects. The client code 
 * treats the 'NullLogger' exactly like a real 'Logger', eliminating the 
 * need for explicit 'if (logger != nullptr)' checks.
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <memory>
#include <vector>

//--------------------------------------------------------- Logger Interface:
class Logger
{
public:
   virtual ~Logger() = default;
   virtual void log(const std::string& message) const = 0;
};

//--------------------------------------------------------- Real Logger:
class ConsoleLogger : public Logger
{
private:
   std::string name_;

public:
   explicit ConsoleLogger(std::string name) : name_{std::move(name)} { }

   void log(const std::string& message) const override
   {
      std::cout << " [" << name_ << "] " << message << "\n";
   }
};

//--------------------------------------------------------- Null Logger:
// The Null Object provides "do-nothing" behavior.
// We implement it as a Singleton since it has no state.
class NullLogger : public Logger
{
private:
   NullLogger() = default;

public:
   static NullLogger& getInstance()
   {
      static NullLogger instance;
      return instance;
   }

   // No-op implementation: does nothing.
   void log(const std::string&) const override { }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== NULL OBJECT PATTERN ===\n" << std::endl;

   // Two different real loggers
   ConsoleLogger logger1{"Console-A"};
   ConsoleLogger logger2{"Console-B"};
   
   // The shared Null Object (Singleton)
   NullLogger& null = NullLogger::getInstance(); // Only one instance

   std::vector<Logger*> loggers;
   loggers.push_back(&logger1);
   loggers.push_back(&null);
   loggers.push_back(&logger2);
   loggers.push_back(&null);

   std::cout << "Broadcasting logs to all loggers (both real and null):\n";
   for (const auto* logger : loggers) logger->log("System update in progress.");

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
