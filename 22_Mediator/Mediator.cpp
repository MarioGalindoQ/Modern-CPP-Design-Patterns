/**
 * ============================================================================
 * File: Mediator.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the Mediator Pattern. It evaluates the math 
 * expression: (1/a)*(b+c) + d
 * 
 * Since in this example, math operations are assumed to be expensive, the
 * 'Expression' (Mediator) acts as a smart cache coordinator. When a Number
 * (Colleague)  changes its value, it notifies the Mediator. The Mediator then
 * recalculates ONLY the parts of the expression affected by that specific variable.
 * 
 * --- MODERN C++:
 * - The Mediator identifies which Colleague sent the notification by simply 
 *   comparing memory addresses (e.g., if colleague == a_), which is the 
 *   fastest and safest way to handle identity in C++.
 * ============================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>

class Colleague;

//--------------------------------------------------------- Mediator Interface:
class Mediator
{
public:
   virtual ~Mediator() = default;

   // Communication channel from Colleague to Mediator
   virtual void changed(Colleague* colleague) = 0;
};

//--------------------------------------------------------- Colleague Base:
class Colleague
{
protected:
   Mediator* mediator_; // Not owner, safe since lifespans are tied in the stack

   explicit Colleague(Mediator* mediator) : mediator_{mediator} { }

   void notify() 
   {
      if (mediator_) mediator_->changed(this);
   }

public:
   virtual ~Colleague() = default;
};

//------------------------------------------------ Double (Concrete Colleague):
class Double : public Colleague
{
private:
   double val_;

public:
   Double(Mediator* mediator, double val) : Colleague{mediator}, val_{val} { }

   void setVal(double newVal)
   {
      std::cout << " [Variable] Double changed to " << newVal << '\n';
      val_ = newVal;
      notify(); // Must be the last line to inform the Mediator
   }

   double getVal() const { return val_; }
};

//----------------------------------------------- Integer (Concrete Colleague):
class Integer : public Colleague
{
private:
   int val_;

public:
   Integer(Mediator* mediator, int val) : Colleague{mediator}, val_{val} { }

   void setVal(int newVal)
   {
      std::cout << " [Variable] Integer changed to " << newVal << '\n';
      val_ = newVal;
      notify(); // Must be the last line to inform the Mediator
   }

   int getVal() const { return val_; }
};

//--------------------------------------------- Expression (Concrete Mediator):
class Expression : public Mediator // (1/a)*(b+c) + d
{
private:
   // Pointers to Concrete Colleagues
   Double*  a_{nullptr};
   Double*  b_{nullptr};
   Integer* c_{nullptr};
   Integer* d_{nullptr};

   // Cache and flags for lazy evaluation
   bool doInv_{true}, doMul_{true}, doAd1_{true}, doAd2_{true};
   double invVal_{0}, mulVal_{0}, ad1Val_{0}, ad2Val_{0};

   bool verify() const 
   {
      return a_ != nullptr && b_ != nullptr && c_ != nullptr && d_ != nullptr;
   }

public:
   // Type-safe registration methods (replaces dynamic_cast and switch)
   void setA(Double* a)  { a_ = a; }
   void setB(Double* b)  { b_ = b; }
   void setC(Integer* c) { c_ = c; }
   void setD(Integer* d) { d_ = d; }

   // The Mediator coordinates the cache invalidation based on memory identity
   void changed(Colleague* colleague) override
   {
      if      (colleague == a_)                    doInv_ = doMul_ = doAd2_ = true;
      else if (colleague == b_ || colleague == c_) doAd1_ = doMul_ = doAd2_ = true;
      else if (colleague == d_)                    doAd2_ = true;
   }

   // Evaluates (1/a)*(b+c) + d
   double evaluate() 
   {
      if (!verify()) throw std::invalid_argument("Error: Expression not well defined.");

      if (doInv_)
      {
         invVal_ = 1.0 / a_->getVal(); 
         std::cout << "\tInverting (1/a)\n";
      }
      if (doAd1_)
      {
         ad1Val_ = b_->getVal() + c_->getVal(); 
         std::cout << "\tAdding (b+c)\n";
      }
      if (doMul_)
      {
         mulVal_ = invVal_ * ad1Val_; 
         std::cout << "\tMultiplying\n";
      }
      if (doAd2_)
      {
         ad2Val_ = mulVal_ + d_->getVal(); 
         std::cout << "\tAdding (+d)\n";
      }

      // Reset flags after evaluation
      doInv_ = doMul_ = doAd1_ = doAd2_ = false;

      std::cout << " Result = ";
      return ad2Val_;
   }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== MEDIATOR PATTERN SIMULATION ===\n" << std::endl;

   try
   {
      Expression expression; // (1/a)*(b+c) + d

      // 1. Create Colleagues
      Double  a{&expression, 2.0};
      Double  b{&expression, 8.0};
      Integer c{&expression, 3};
      Integer d{&expression, 1};

      // 2. Type-safe registration with the Mediator
      expression.setA(&a);
      expression.setB(&b);
      expression.setC(&c);
      expression.setD(&d);

      std::cout << "Initial Evaluation:\n";
      std::cout << expression.evaluate() << "\n\n";

      std::cout << "Second Evaluation (Cache hit, no operations should run):\n";
      std::cout << expression.evaluate() << "\n\n";

      a.setVal(4.4);
      std::cout << "Evaluating after 'a' changes:\n";
      std::cout << expression.evaluate() << "\n\n";

      c.setVal(10);
      std::cout << "Evaluating after 'c' changes:\n";
      std::cout << expression.evaluate() << "\n\n";

      d.setVal(6);
      std::cout << "Evaluating after 'd' changes:\n";
      std::cout << expression.evaluate() << "\n\n";

      a.setVal(3.1); d.setVal(4);
      std::cout << "Evaluating after 'a' and 'c' changes:\n";
      std::cout << expression.evaluate() << "\n";
   }
   catch(const std::exception& e)
   {
      std::cerr << e.what() << std::endl;
   }
}

//================================================================================ END
