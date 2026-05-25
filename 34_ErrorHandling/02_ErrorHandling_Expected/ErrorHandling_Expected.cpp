/**
 * ============================================================================
 * File: ErrorHandling_Expected.cpp
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the "Result" pattern using std::expected (C++23).
 * This is the industry-standard way to handle operations that can fail 
 * without resorting to exceptions for logic flow.
 * 
 * --- ADVANTAGES OVER std::variant:
 * 1. Semantic Clarity: The type signature 'expected<double, Error>' 
 *    explicitly states what is the result and what is the failure.
 * 2. Ergonomics: Provides methods like .has_value(), .value(), and .error() 
 *    which make the code much more readable than a Visitor.
 * 3. Functional Style: It behaves like a pointer (bool conversion and 
 *    operator*) but with total type safety.
 * ============================================================================
 */

#include <iostream>
#include <expected> // C++23 Feature
#include <string>
#include <cmath>
#include <iomanip>

//--------------------------------------------------------- Error Structure:
struct Error
{
   std::string message;
};

//--------------------------------------------------------- Logic:
// The function signature is now extremely expressive.
std::expected<double, Error> safeSqrt(const std::string& input)
{
   double value;
   
   // 1. Try to parse the string
   try 
   {
      size_t pos;
      value = std::stod(input, &pos);
      if (pos < input.size()) return std::unexpected(Error{"Input contains non-numeric characters."});
   }
   catch (...)
   {
      return std::unexpected(Error{"Invalid input: '" + input + "' is not a number."});
   }

   // 2. Check for mathematical validity
   if (value < 0) return std::unexpected(Error{"Math error: cannot calculate sqrt of " + std::to_string(value)});

   // 3. Success: Just return the value
   return std::sqrt(value);
}

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== ERROR HANDLING (std::expected VERSION - C++23) ===\n" << std::endl;

   std::string testInputs[] = {"16.0", "-4.0", "abc", "25.0"};

   for (const auto& input : testInputs)
   {
      std::cout << "Calculating sqrt of '" << input << "':\n";
      
      // The function returns an 'expected' object
      auto res = safeSqrt(input);

      // Checking success is as simple as checking a boolean
      if (res)
         std::cout << " [Success] Result: " << std::fixed << std::setprecision(4) << *res << "\n";
      else
         std::cout << " [Failure] " << res.error().message << "\n";
      
      std::cout << std::endl;
   }

   std::cout << "=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
