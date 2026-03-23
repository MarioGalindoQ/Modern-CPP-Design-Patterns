/**
 * ============================================================================
 * File: App.h
 * 
 * --- DESIGN OVERVIEW:
 * This is the core interface of our Framework. It defines the application 
 * skeleton by declaring the lifecycle methods (start, read, compute, write, 
 * result, final). 
 * 
 * The Framework follows the "Hollywood Principle": the Framework controls 
 * the execution flow and calls these methods when necessary. Concrete 
 * applications simply inherit from 'App' and implement the steps they need.
 * ============================================================================
 */

#ifndef APP_H
#define APP_H

class App
{
public:
   virtual ~App() = default;

   virtual void  start   ()  { }  // (1)
   // read must return EOF condition flag
   virtual bool  read    ()  = 0; // (2)
   virtual void  compute ()  { }  // (3)
   virtual void  write   ()  { }  // (4)
   virtual void  result  ()  { }  // (5)
   virtual void  final   ()  { }  // (6)
};

App* createApp();

#endif
//================================================================================ END
