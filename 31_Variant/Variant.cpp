/**
 * ============================================================================
 * File: Variant.cpp
 * 
 * --- DESIGN OVERVIEW:
 * Demonstrates the use of std::variant and std::visit (C++17).
 * Includes a custom 'Logger' class implementing "The Gang of Seven" to
 * trace memory lifecycle and ownership changes within the variant.
 * ============================================================================
 */

#include <iostream>
#include <variant>
#include <stack>
#include <string>
#include <utility>

//---------------------------------------------------------------------------------------- Logger:

// As an example, define a logger class to follow it:
struct Logger
{
   std::string s;
  
   // Define the Gang of seven:
   Logger()
   {
      std::cout << "----- Logger 1 DC: Default Constructor\n";
   }

   Logger(Logger const& other) : s{other.s}
   {
      std::cout << "----- Logger 2 CC: Copy Constructor: " << s << '\n';
   }

   Logger(Logger&& other) noexcept : s{std::move(other.s)}
   {
      std::cout << "----- Logger 3 MC: Move Constructor: " << s << '\n';
   }

   Logger& operator=(Logger const& other)
   {
      std::cout << "----- Logger 4 CA: Copy Assignment: " << other.s << '\n';
      s = other.s;
      return *this;
   }

   Logger& operator=(Logger&& other) noexcept
   {
      std::cout << "----- Logger 5 MA: Move Assignment: " << other.s << '\n';
      s = std::move(other.s);
      return *this;
   }

   ~Logger()
   {
      std::cout << "----- Logger 6 De: Destructor: " << s << '\n';
   }

   explicit Logger(std::string s) : s{std::move(s)}
   {
      std::cout << "----- Logger 7 PC: Particular Constructor: " << this->s << '\n';
   }
};

std::ostream& operator << (std::ostream& os, Logger const& g)
{
   os << g.s;
   return os;
}

//------------------------------------------------------------------------------------------ Main:

int main()
{
   //----------------------------------------------------- Declare the variant (union):
   using var_t = std::variant<int, double, char, Logger, const char*, std::string>;
  
   //----------------------------------------------- Write the sizes of the components:

   std::cout << "\n----------------------- Size of components:\n";

   std::cout << "size of int:         " << sizeof(int) << '\n';
   std::cout << "size of double:      " << sizeof(double) << '\n';
   std::cout << "size of char:        " << sizeof(char) << '\n';
   std::cout << "size of Logger:      " << sizeof(Logger) << '\n';
   std::cout << "size of const char*: " << sizeof(const char*) << '\n';
   std::cout << "size of std::string: " << sizeof(std::string) << '\n';

   std::cout << "\nsize of var_t:       " << sizeof(var_t) << '\n';
  
   //------------------ Create a visitor (define a operator() to work with each type):
   struct Visitor
   {
      int in{0}, db{0}, ch{0}, lo{0}, cp{0}, st{0};
 
      void operator()(int i)                { std::cout << ">> int: "         << i << '\n'; ++in; }
      void operator()(double d)             { std::cout << ">> double: "      << d << '\n'; ++db; }
      void operator()(char c)               { std::cout << ">> char: "        << c << '\n'; ++ch; }
      void operator()(const Logger& l)      { std::cout << ">> Logger: "      << l << '\n'; ++lo; }
      void operator()(const char* p)        { std::cout << ">> const char*: " << p << '\n'; ++cp; }
      void operator()(const std::string& s) { std::cout << ">> std::string: " << s << '\n'; ++st; }
   } visitor;

   //----------------------------------------------------- Create a stack of variants:
   std::stack<var_t> myStack;
   
   //--------------------------------------------- Operate with the stack of variants:

   std::cout << "\n--------------------------- Fill the stack:\n";

   using namespace std::string_literals;
  
   myStack.emplace(12);
   myStack.emplace(8.3);
   myStack.emplace('c');
   myStack.emplace(9);
   myStack.emplace(Logger{"I am a Logger"});
   myStack.emplace("C string 1");
   myStack.emplace("I am std::string"s);
   myStack.emplace("C string 2");
  
   std::cout << "\n--------------------------- Read the stack:\n";

   while (myStack.size() > 0)
   {
      var_t v{std::move(myStack.top())};
      myStack.pop();

      //---- Visit the variant:
      std::visit(visitor, v);
   }
  
   std::cout << "\n---------------------------------- Results:\n";
  
   std::cout << "Num int:         " << visitor.in << '\n';
   std::cout << "Num double:      " << visitor.db << '\n';
   std::cout << "Num char:        " << visitor.ch << '\n';
   std::cout << "Num Logger:      " << visitor.lo << '\n';
   std::cout << "Num const char*: " << visitor.cp << '\n';
   std::cout << "Num string:      " << visitor.st << '\n';
    
   std::cout << "\n--------------------------------------- END\n";
}

//============================================================================================ END
