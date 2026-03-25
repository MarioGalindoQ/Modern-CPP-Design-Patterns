/**
 * ============================================================================
 * File: Facade.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the Facade pattern, which provides a simplified 
 * interface to a complex system. The goal is to hide the complexity 
 * of booting a computer from the client.
 * ============================================================================
 */

#include <iostream>
#include <memory>

//--------------------------------------------------------- Subsystem Classes:
// These classes represent the complex internals of the computer.
class CPU
{
public:
   void freeze() const { std::cout << " [CPU] Freezing CPU...\n"; }
   void jump(long position) const { std::cout << " [CPU] Jumping to address: " << position << "\n"; }
   void execute() const { std::cout << " [CPU] Executing instructions...\n"; }
};

class HardDrive
{
public:
   const char* read(long lba, int size) const
   {
      static constexpr char data[] = "System loaded from disk";
      std::cout << " [HardDrive] Reading " << size << " bytes from sector " << lba << "...\n";
      return data;
   }
};

class Memory
{
public:
   void load(long position, const char* data) const
   {
      std::cout << " [Memory] Loading '" << data << "' into memory at " << position << "...\n";
   }
};

//--------------------------------------------------------- Facade Class:
// The Facade provides a simplified interface to the complex subsystem.
class ComputerFacade
{
private:
   // The Facade *HAS A* CPU, Memory, and HardDrive.
   std::unique_ptr<CPU>       cpu_        = std::make_unique<CPU>();
   std::unique_ptr<Memory>    memory_     = std::make_unique<Memory>();
   std::unique_ptr<HardDrive> hard_drive_ = std::make_unique<HardDrive>();

public:
   void start() const
   {
      std::cout << "=== BOOTING COMPUTER ===\n";
      
      const long kBootAddress = 300;
      const long kBootSector  = 49;
      const int  kSectorSize  = 1024;

      cpu_->freeze();
      memory_->load(kBootAddress, hard_drive_->read(kBootSector, kSectorSize));
      cpu_->jump(kBootAddress);
      cpu_->execute();

      std::cout << "\n=== COMPUTER BOOTED SUCCESSFULLY! ===\n";
   }
};

//--------------------------------------------------------- Client Code (main):
int main()
{
   std::cout << "=== FACADE PATTERN SIMULATION ===\n" << std::endl;

   ComputerFacade computer;
   computer.start();

   std::cout << "\n=== END OF DEMONSTRATION ===\n";
}

//================================================================================ END
