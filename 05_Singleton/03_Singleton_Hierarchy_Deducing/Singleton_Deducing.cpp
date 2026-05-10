/**
 * ============================================================================
 * File: Singleton_Deducing.cpp (Modern C++23 Version)
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates a Singleton Hierarchy using C++23 "Deducing This" 
 * (Explicit Object Parameters).
 * 
 * --- THE ARCHITECTURAL PROBLEM:
 * When Singletons inherit from a common Base class (e.g., a generic Service 
 * Manager), calling a Base method traditionally returns a reference to the 
 * Base class. This breaks the "Fluent Interface" because the compiler 
 * "forgets" the derived type, preventing further calls to derived-specific 
 * methods in the same chain.
 * 
 * --- THE C++23 SOLUTION:
 * By using 'this auto&& self' in the Base class methods, we can capture the 
 * exact type of the calling Singleton at compile-time. This allows the Base 
 * class to return a reference to the Derived type automatically, maintaining 
 * the chain without using CRTP or virtual functions.
 * 
 * --- TECHNICAL MECHANICS:
 * 1. Base Utility: 'ManagerBase' provides common logic (e.g., 'initialize') 
 *    to all services.
 * 2. Static Dispatch: 'self' in 'initialize(this auto&& self)' deduces 
 *    whether it's being called by 'Logger' or 'Database'.
 * 3. Meyer's Singleton: We still use the safe, thread-safe static local 
 *    instance within each derived class.
 * ============================================================================
 */

#include <iostream>
#include <string>

//------------------------------------------------- Common Service Manager:
/**
 * The Base class is NO LONGER a template. 
 * "Deducing This" handles the polymorphic return types.
 */
class ManagerBase {
public:
   // Common initialization logic for all Singletons
   auto&& initialize(this auto&& self) {
      std::cout << " [Base] Global Service Initialization sequence started...\n";
      // Imagine complex setup logic here...
      return self; // Returns the derived Singleton!
   }

   auto&& setLogLevel(this auto&& self, int level) {
      std::cout << " [Base] Setting system-wide log level to: " << level << "\n";
      return self;
   }
};

//------------------------------------------------------- Logger Singleton:
class Logger : public ManagerBase {
public:
   // Standard Meyer's Singleton access
   static Logger& instance() {
      static Logger inst;
      return inst;
   }

   // --- THE RULE OF SEVEN (Singleton constraints) ---
   Logger(const Logger&)            = delete;
   Logger(Logger&&)                 = delete;
   Logger& operator=(const Logger&) = delete;
   Logger& operator=(Logger&&)      = delete;

   // Specific Logger behavior
   Logger& log(const std::string& msg) {
      std::cout << " [Logger] Event: " << msg << "\n";
      return *this;
   }

private:
   Logger() { std::cout << " [Constructor] Logger Service Ready.\n"; }
};

//----------------------------------------------------- Database Singleton:
class Database : public ManagerBase {
public:
   static Database& instance() {
      static Database inst;
      return inst;
   }

   // --- THE RULE OF SEVEN ---
   Database(const Database&)            = delete;
   Database(Database&&)                 = delete;
   Database& operator=(const Database&) = delete;
   Database& operator=(Database&&)      = delete;

   // Specific Database behavior
   Database& connect(const std::string& connectionString) {
      std::cout << " [Database] Connected to " << connectionString << "\n";
      return *this;
   }

private:
   Database() { std::cout << " [Constructor] Database Service Ready.\n"; }
};

//------------------------------------------------------------------- Main:
int main() {
   std::cout << "=== SINGLETON HIERARCHY: C++23 DEDUCING THIS ===\n\n";

   /**
    * MAGIC AT WORK:
    * 'Logger::instance().initialize()' returns a 'Logger&' even though 
    * 'initialize' is defined in 'ManagerBase'. 
    * This allows us to chain '.log()' immediately.
    */
   std::cout << "--- Testing Logger Chain ---\n";
   Logger::instance()
      .initialize()
      .setLogLevel(2)
      .log("Singleton hierarchy is working perfectly.");

   std::cout << "\n--- Testing Database Chain ---\n";
   /**
    * Here, 'initialize' returns a 'Database&', allowing the call to '.connect()'.
    */
   Database::instance()
      .initialize()
      .connect("prod_server_db_01");

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
