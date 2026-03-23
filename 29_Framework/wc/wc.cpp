/**
 * ============================================================================
 * File: wc.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This program implements a word and character count utility using the 'App'
 * framework. It showcases how different applications can reuse the same 
 * execution skeleton ('Framework/Framework.cpp') while providing unique 
 * processing logic.
 * ============================================================================
 */

#include "../Framework/App.h"

#include <iostream>

// Define your new customized application
class wc : public App 
{
private:
   char c_;
   int nChar_{0}, nWord_{1}, nLine_{1};

public:
   bool read() override // (2)
   {
      std::cin.get(c_);       // Read a character
      return std::cin.good(); // Return EOF cond.
   }

   void compute() override // (3)
   {
      ++nChar_;
      if (c_ == ' ' || c_ == '\n') ++nWord_;
      if (c_ == '\n') ++nLine_;
   }

   void result() override // (5)
   {
      std::cout << "lines=" << nLine_ << ' ';
      std::cout << "words=" << nWord_ << ' ';
      std::cout << "chars=" << nChar_ << '\n';
   }
};

App* createApp() 
{
   return new wc;
}

//========================================================================= END
