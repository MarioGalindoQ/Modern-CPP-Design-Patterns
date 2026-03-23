// File: Visitor_RTTI.h

#ifndef VISITOR_RTTI_H
#define VISITOR_RTTI_H

#include <typeinfo>
#include <exception>
#include <string>

//--------------------------------------------------------- Exception Class:
class Dispatch_error : public std::exception
{
private:
   std::string message_;

public:
   Dispatch_error(const char* visitorName, const char* visitableName)
   {
      message_ = "dispatch_error: ";
      message_ += visitorName;
      message_ += " does not visit <";
      message_ += visitableName;
      message_ += ">";
   }

   explicit Dispatch_error(const char* className)
   {
      message_ = "dispatch_error: ";
      message_ += className;
      message_ += " not visited As<";
      message_ += className;
      message_ += ">";
   }

   const char* what() const noexcept override
   {
      return message_.c_str();
   }
};

//--------------------------------------------------------- Base Visitor:
class Visitor
{
public:
   virtual ~Visitor() = default;
};

//----------------------------------------- Visit Interface (Parameterized):
template<class SpecificVisitable>
class Visit
{
public:
   virtual ~Visit() = default;
   virtual void visit(SpecificVisitable&) = 0;
};

//--------------------------------------------------------- Accept Interface:
// This is the internal interface used for the second dispatch jump.
class Accept
{
public:
   virtual ~Accept() = default;
   virtual void accept(Visitor&) = 0;
};

//-------------------------------------------- As<SpecificVisitable> Helper:
/**
 * The 'As' template provides the actual implementation of the handshake.
 * It is a Mixin that Visitable classes must inherit from.
 */
template<class SpecificVisitable>
class As : public Accept
{
protected:
   As() = default; 

public:
   void accept(Visitor& visitor) override final
   {
      // (2) We cast 'this' to the specific type.
      SpecificVisitable& specific = static_cast<SpecificVisitable&>(*this);

      // (3) We check if the visitor implements the required Visit<T> interface.
      auto* v = dynamic_cast<Visit<SpecificVisitable>*>(&visitor);
      
      if (v)
      {
         v->visit(specific);
      }
      else
      {
         throw Dispatch_error(typeid(visitor).name(), typeid(*this).name());
      }
   }
};

//--------------------------------------------------------- Visitable Base:
/**
 * Note: Visitable does NOT inherit from Accept to avoid VTable ambiguity 
 * during multiple inheritance in concrete classes.
 */
class Visitable
{
public:
   virtual ~Visitable() = default;

   void accept(Visitor& visitor)
   {
      // (1) We attempt to find the Accept implementation via RTTI.
      // Concrete classes like Red will provide this through As<Red>.
      auto* a = dynamic_cast<Accept*>(this);
      if(a) a->accept(visitor);
      else  throw Dispatch_error(typeid(*this).name());
   }
};

//--------------------------------------------------------- Double Dispatch:
inline void double_dispatch(Visitable& visitable, Visitor& visitor)
{
   visitable.accept(visitor);
}

inline void double_dispatch(Visitor& visitor, Visitable& visitable)
{
   visitable.accept(visitor);
}

#endif

//================================================================================ END
