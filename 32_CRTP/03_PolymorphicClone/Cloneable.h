/**
 * ============================================================================
 * File: Cloneable.h
 * 
 * --- INFRASTRUCTURE:
 * This utility provides a non-intrusive cloning mechanism using CRTP.
 * It uses 'dynamic_cast' to perform cross-casting between unrelated 
 * branches of a multiple inheritance hierarchy.
 * 
 * --- TYPE SAFETY (CRTP PROTECTION):
 * To prevent "identity hijacking" (e.g., Circle inheriting from 
 * Cloneable<Square>), the Mixin uses a private constructor and friendship.
 * Only the class specified in the template argument 'T' has permission 
 * to inherit from Cloneable<T>.
 * 
 * --- ERROR HANDLING:
 * The 'Cloneable_clone' function is designed to fail-fast. If the baseSource 
 * object does not implement the Cloneable Mixin, it throws a 
 * std::runtime_error to prevent null pointer propagation.
 * ============================================================================
 */

#ifndef CLONEABLE_H
#define CLONEABLE_H

#include <iostream>
#include <memory>
#include <stdexcept>
#include <typeinfo>

//--------------------------------------------------------- System Interface:
// Non-template base to allow dynamic_cast across different branches.
class ICloneable
{
public:
   virtual ~ICloneable() = default;
   
   // Internal polymorphic entry point. Returns raw pointer to handle 
   // covariance during the cross-casting process.
   virtual ICloneable* clone_internal() const = 0;
};

//--------------------------------------------------------- Cloneable Mixin:
template <class Concrete>
class Cloneable : public ICloneable
{
private:
   // The "Lock": Only the intended class Concrete can call this constructor
   friend Concrete;
   Cloneable() = default;

public:
   // Returns a raw pointer that will be immediately wrapped by the dispatcher
   ICloneable* clone_internal() const override
   {
      // Invokes the copy constructor of the class Concrete
      return new Concrete(static_cast<const Concrete&>(*this));
   }
};

//--------------------------------------------------------- Dispatch Utility:
/**
 * Cloneable_clone: The smart dispatcher.
 * It attempts to clone any polymorphic object by looking for the 
 * ICloneable interface in its hierarchy.
 */
template <class Base>
std::unique_ptr<Base> Cloneable_clone(const Base* baseSource)
{
   if(!baseSource) throw std::invalid_argument("Attempted to clone a null pointer.");

   // 1. Cross-cast to find if the object supports cloning
   const auto* cloner = dynamic_cast<const ICloneable*>(baseSource);

   if(!cloner)
      throw std::runtime_error("Object of type [" +
                               std::string(typeid(*baseSource).name()) +
                               "] is NOT cloneable.");

   // 2. Perform the clone (returns ICloneable*)
   ICloneable* raw_clone = cloner->clone_internal();

   // 3. Cross-cast the result back to the Business Base
   Base* casted_clone = dynamic_cast<Base*>(raw_clone);

   if (!casted_clone)
   {
      // Clean up the raw pointer if the cast fails to prevent leaks
      delete raw_clone;
      throw std::runtime_error("Type mismatch: The cloned object is not compatible "
                               "with the requested Base interface.");
   }

   // 4. Wrap into unique_ptr for safe automatic management
   return std::unique_ptr<Base>(casted_clone);
}

#endif

//================================================================================ END
