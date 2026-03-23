/**
 * ============================================================================
 * File: up.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This is a concrete implementation of the 'App' framework.
 * It reads a text file line by line and converts every character to 
 * uppercase, demonstrating how a specific client fills the framework 
 * skeleton with concrete functionality.
 * ============================================================================
 */

#include "../Framework/App.h"

#include <iostream>
#include <fstream>
#include <cctype>   // std::toupper
#include <string>

class up : public App 
{
private:
   char c_{0};
   std::ifstream fs_;

public:
   void start() override // (1)
   {
      std::string filename;
      std::getline(std::cin, filename);
      fs_.open(filename);
      if (!fs_)
      {
         std::cerr << "Can't open " << filename << '\n';
         std::exit(1);
      }
   }

   bool read() override // (2)
   {
      c_ = static_cast<char>(fs_.get()); // Read a character
      return fs_.good();                 // Return EOF condition
   }

   void write() override // (4)
   {
      std::cout << static_cast<char>(std::toupper(c_));
   }
  
   void final() override // (6)
   {
      fs_.close();
   }
};

// Factory method to create the concrete application instance
App* createApp() 
{
   return new up;
}

//================================================================================ END
