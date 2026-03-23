#include "Observer.h"
#include <iostream>

class Number : public Subject<Number>
{
private:
   int n;

public:
   Number() {}
   void setVal(int i)
   {
      n = i;
      notify(); // Must be the last line of setVal();
   }
   int getVal() {return n;}
};

class DivObserver: public Observer<Number>
{
private:
   int div;
   void update(Number& num) override
   {
      int n = num.getVal();
      std::cout << n << " div " << div << " is " << n / div << '\n';
   }

public:
   DivObserver(int div) : div{div} {}
};

class ModObserver: public Observer<Number>
{
private:
   int div;
   void update(Number& num) override
   {
      int n = num.getVal();
      std::cout << n << " mod " << div << " is " << n % div << '\n';
   }

public:
   ModObserver(int div) : div{div} {}
};
