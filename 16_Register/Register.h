/**
 * ============================================================================
 * File: Register.h
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * The Registry pattern acts as a central repository for service creation.
 * By using 'std::function' as a factory, we decouple the Registry from the
 * concrete constructors of each service, making the system highly modular.
 * 
 * --- AUTO-REGISTRATION:
 * Each concrete processor registers itself at runtime using the 'Register'
 * helper, allowing new implementations to be added to the system simply
 * by including their file, without modifying the core engine.
 * 
 * --- DYNAMIC CREATION:
 * The Registry stores "Creators" (factories), not just instances. This 
 * provides the flexibility to return either a shared Singleton instance 
 * or a fresh Prototype instance on demand.
 *
 * --- CONFIGURATION:
 * Use #define REGISTRY_SINGLETON or #define REGISTRY_PROTOTYPE to toggle 
 * the instantiation policy.
 * ============================================================================
 */

#ifndef REGISTER_H
#define REGISTER_H

//=============================================================================
// POLICY: Uncomment exactly one of the lines below to define the policy
//=============================================================================
#define REGISTRY_SINGLETON
//#define REGISTRY_PROTOTYPE

#if !defined(REGISTRY_SINGLETON) && !defined(REGISTRY_PROTOTYPE)
   #error "You must define either REGISTRY_SINGLETON or REGISTRY_PROTOTYPE in Register.h"
#endif

#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <functional>
#include <stdexcept>

using Image = std::string;

//--------------------------------------------------------- Processor Interface:
class Processor
{
public:
   virtual ~Processor() = default;
   virtual std::string process(const Image& image) const = 0;
};

//--------------------------------------------------------- Registry:
class Registry
{
public:
   // Factory type: a function that returns a new Processor instance
   using Creator = std::function<std::shared_ptr<Processor>()>;

private:
   static inline std::map<std::string, Creator> registryMap_;

public:
   // Registers a creation function for a given processor name
   static void registerProcessor(const std::string& processorName, Creator creator)
   {
      registryMap_[processorName] = std::move(creator);
      std::cout << " [Registry] Registered: " << processorName << "\n";
   }

   // Returns a new instance using the registered factory function
   static std::shared_ptr<Processor> create(const std::string& processorName)
   {
      if(registryMap_.find(processorName) == registryMap_.end())
         throw std::runtime_error("Processor not found: " + processorName);
      
      return registryMap_[processorName](); // Execute the ConcreteCreator function
   }
};

//--------------------------------------------------------- Auto-Registration Helper:
template<class ConcreteProcessor>
class Register
{
public:
   Register(const std::string& processorName)
   {
#ifdef REGISTRY_SINGLETON
      // Singleton: Cache the instance and return the same one
      static std::shared_ptr<ConcreteProcessor> instance = std::make_shared<ConcreteProcessor>();
      Registry::registerProcessor(processorName, []() { return instance; });
#else // REGISTRY_PROTOTYPE
      // Prototype: Always return a new instance
      Registry::registerProcessor(processorName, []() { return std::make_shared<ConcreteProcessor>(); });
#endif
   }
};

#endif

//================================================================================ END
