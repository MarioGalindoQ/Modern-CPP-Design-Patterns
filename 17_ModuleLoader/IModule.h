/**
 * ============================================================================
 * File: IModule.h
 * 
 * --- DESIGN OVERVIEW:
 * This is the Contract (Interface) shared between the Host application 
 * and the dynamically loaded Plugin. Both sides must compile against 
 * the exact same interface to ensure ABI compatibility.
 * ============================================================================
 */

//----------------------------- Module interface

#ifndef IMODULE_H
#define IMODULE_H

class IModule
{
public:
   IModule() = default;
   virtual ~IModule() = default;
   
   // Updated to accept a parameter for bidirectional communication
   virtual void processData(int data) = 0;
};

#endif

//========================================================================= END
