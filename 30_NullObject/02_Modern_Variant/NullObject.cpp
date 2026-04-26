/**
 * ============================================================================
 * File: NullObject.cpp (Modern Variant Version)
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the Null Object pattern using std::variant 
 * and std::monostate. 
 * 
 * --- THE MODERN APPROACH:
 * Instead of creating a 'NullLogger' class via inheritance, we use 
 * 'std::monostate' as a type-safe alternative to represent the absence 
 * of a logger. 
 * 
 * --- THE ADVANTAGE:
 * The client code handles the logging through 'std::visit'. If the variant 
 * contains 'std::monostate', the "Null" behavior is executed (doing nothing). 
 * This eliminates raw pointers and prevents runtime crashes while maintaining
 * value semantics and stack allocation.
 * ============================================================================
 */

#include <iostream>
#include <variant>
#include <vector>
#include <string>

//--------------------------------------------------------- Real Logger:
// A simple struct that performs actual work. No inheritance required.
struct ConsoleLogger
{
   std::string name;

   void log(const std::string& message) const
   {
      std::cout << " [" << name << "] " << message << "\n";
   }
};

//--------------------------------------------------------- The Nullable Type:
// Logger can be either a real ConsoleLogger or "Nothing" (std::monostate)
// std::monostate acts as the Null Object here.
using Logger = std::variant<std::monostate, ConsoleLogger>;

//--------------------------------------------------------- The Dispatcher:
// This Visitor handles both the real logger and the Null Object case.
struct LogVisitor
{
   std::string message;

   // 1. Logic for the Real Object
   void operator()(const ConsoleLogger& logger) const
   {
      logger.log(message);
   }

   // 2. Logic for the Null Object (std::monostate)
   // This is the "do-nothing" behavior encapsulated as a type.
   void operator()(std::monostate) const
   {
      // Intentionally empty: no-op behavior
   }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== NULL OBJECT PATTERN (VARIANT VERSION) ===\n" << std::endl;

   // A collection of loggers using value semantics (no pointers!)
   std::vector<Logger> loggers;

   loggers.emplace_back(ConsoleLogger{"Console-A"}); // Real Object
   loggers.emplace_back(std::monostate{});           // The Null Object
   loggers.emplace_back(ConsoleLogger{"Console-B"}); // Real Object
   loggers.emplace_back(std::monostate{});           // The Null Object

   std::cout << "Broadcasting logs to all loggers (including the monostate Null Object):\n";

   // The compiler ensures both cases (Real and Null) are handled via std::visit
   for (const auto& logger : loggers)
      std::visit(LogVisitor{"System update in progress."}, logger);

   std::cout << "\n=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
