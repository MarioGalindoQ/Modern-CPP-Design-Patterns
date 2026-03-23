// File: Visitor.h

#ifndef VISITOR_H
#define VISITOR_H

#include <typeinfo>  // For typeid
#include <exception> // For std::exception
#include <string>

//--------------------------------------------------------- Exception Class:
// This class handles errors when a Visitor does not implement a 
// specific visit method for a Visitable type.
class dispatch_error : public std::exception
{
private:
   std::string msg_;

public:
   dispatch_error(const char* visitorName, const char* visitableName)
   {
      msg_ = "dispatch_error: ";
      msg_ += visitorName;
      msg_ += " does not visit <";
      msg_ += visitableName;
      msg_ += ">";
   }

   const char* what() const noexcept override 
   { 
      return msg_.c_str(); 
   }
};

//------------------------------------------------------- Visitable Interface:
class Visitor; // Forward declaration

class Visitable
{
protected:
   Visitable() = default;

public:
   virtual ~Visitable() = default;
   
   // Every Visitable must implement this to accept a Visitor
   virtual void accept(Visitor& v) = 0;
};

//--------------------------------------------------------- Double Dispatch:
/**
 * Symmetry in Dispatch:
 * These two functions ensure that the user doesn't need to remember 
 * the order of arguments. The pattern is encapsulated, always initiating 
 * the handshake via the Visitable's accept method.
 */

inline void double_dispatch(Visitable& visited, Visitor& visitor)
{
   visited.accept(visitor);
}

inline void double_dispatch(Visitor& visitor, Visitable& visited)
{
   visited.accept(visitor);
}

#endif 

//================================================================================ END
