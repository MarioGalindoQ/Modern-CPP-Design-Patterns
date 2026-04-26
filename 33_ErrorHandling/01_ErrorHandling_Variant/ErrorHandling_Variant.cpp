/**
 * ============================================================================
 * File: ErrorHandling_Variant.cpp
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the "Either" pattern using std::variant (C++17).
 * It represents a safe way to handle errors without using exceptions or 
 * ambiguous return codes.
 * 
 * --- THE SCENARIO:
 * A function attempts to parse a string into a number and then calculates 
 * its square root. There are two failure points: 
 * 1. The input is not a number (Parsing error).
 * 2. The number is negative (Mathematical error).
 * 
 * --- THE VISITOR APPROACH:
 * Using std::visit ensures that the caller MUST handle both the success 
 * and the error case, providing total type safety at runtime.
 * ============================================================================
 */

#include <iostream>
#include <variant>
#include <string>
#include <cmath>
#include <iomanip>

//--------------------------------------------------------- Error Structure:
struct Error
{
   std::string message;
};

//--------------------------------------------------------- Result Type:
// A Result can be either the expected double or an Error object.
using Result = std::variant<double, Error>;

//--------------------------------------------------------- Logic:
Result safeSqrt(const std::string& input)
{
   double value;
   
   // 1. Try to parse the string
   try 
   {
      size_t pos;
      value = std::stod(input, &pos);
      // Ensure the whole string was a number
      if (pos < input.size()) return Error{"Input contains non-numeric characters."};
   }
   catch (...)
   {
      return Error{"Invalid input: '" + input + "' is not a number."};
   }

   // 2. Check for mathematical validity
   if (value < 0) return Error{"Math error: cannot calculate sqrt of " + std::to_string(value)};

   // 3. Success
   return std::sqrt(value);
}

//--------------------------------------------------------- Result Processor:
struct ResultHandler
{
   void operator()(double value) const
   {
      std::cout << " [Success] Result: " << std::fixed << std::setprecision(4) << value << "\n";
   }

   void operator()(const Error& error) const
   {
      std::cout << " [Failure] " << error.message << "\n";
   }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== ERROR HANDLING (std::variant VERSION) ===\n" << std::endl;

   std::string testInputs[] = {"16.0", "-4.0", "abc", "25.0"};

   for (const auto& input : testInputs)
   {
      std::cout << "Calculating sqrt of '" << input << "':\n";
      
      // The function returns a Variant (The "Either" pattern)
      Result res = safeSqrt(input);

      // Process the variant using a Visitor
      std::visit(ResultHandler{}, res);
      
      std::cout << std::endl;
   }

   std::cout << "=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
