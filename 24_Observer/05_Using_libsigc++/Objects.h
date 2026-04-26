// **************************************************************************
// Objects.h - Implementation of Concrete Subjects and Observers
// Author: Mario Galindo Queralt, Ph.D.
// **************************************************************************
// This file defines the 'Number' class as a Concrete Subject and the 
// 'DivObserver'/'ModObserver' classes as Concrete Observers.
//
// Key features:
// - Inherits from the Observer/Subject template framework.
// - Uses sigc::trackable for safe signal-slot connection management.
// - Demonstrates state-sharing between multiple subjects and observers.
// **************************************************************************

#include "Observer.h"
#include <iostream>

class Number : public Subject<Number>
{
private:
   int n;

public:
   Number() : n(0) {}

   /**
    * Updates the internal value and notifies all connected observers.
    * @param i The new integer value to be stored.
    */
   void setVal(int i)
   {
      n = i;
      notify(); // Must be the last line of setVal();
   }

   /**
    * Returns the current internal value.
    * @return The current integer state of the subject.
    */
   int getVal() const { return n; }
};

class DivObserver : public Observer<Number>
{
private:
   int div;

   /**
    * Handles the notification from a Subject.
    * Performs a division operation and prints the result to stdout.
    * @param num Reference to the Number subject that triggered the update.
    */
   void update(Number& num) override
   {
      int n = num.getVal();
      std::cout << n << " div " << div << " is " << n / div << '\n';
   }

public:
   DivObserver(int div) : div{div} {}
};

class ModObserver : public Observer<Number>
{
private:
   int div;

   /**
    * Handles the notification from a Subject.
    * Performs a modulo operation and prints the result to stdout.
    * @param num Reference to the Number subject that triggered the update.
    */
   void update(Number& num) override
   {
      int n = num.getVal();
      std::cout << n << " mod " << div << " is " << n % div << '\n';
   }

public:
   ModObserver(int div) : div{div} {}
};

//================================================================================ END
