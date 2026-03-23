/**
 * ============================================================================
 * File: Composite.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the Composite Pattern. It treats single 
 * components (CPU, RAM, SDD) and compositions (Motherboard, Case, Diskrack)
 * uniformly through the 'Equipment' interface.
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <memory>
#include <vector>

//----------------------------------------------------------- Equipment (Component):
class Equipment
{
protected:
   std::string name_;

public:
   explicit Equipment(std::string name) : name_{std::move(name)} { }
   virtual ~Equipment() = default;

   virtual void print(int indentation = 0) const = 0;
};

//--------------------------------------------------------------------- Leaf Elements:
class CPU : public Equipment
{
public:
   using Equipment::Equipment;
   
   void print(int indentation = 0) const override
   {
      for (int i = 0; i < indentation; ++i) std::cout << "  ";
      std::cout << "- CPU: " << name_ << std::endl;
   }
};

class RAM : public Equipment
{
public:
   using Equipment::Equipment;
   
   void print(int indentation = 0) const override
   {
      for (int i = 0; i < indentation; ++i) std::cout << "  ";
      std::cout << "- RAM: " << name_ << std::endl;
   }
};

class SSD : public Equipment
{
public:
   using Equipment::Equipment;
   
   void print(int indentation = 0) const override
   {
      for (int i = 0; i < indentation; ++i) std::cout << "  ";
      std::cout << "- SSD: " << name_ << std::endl;
   }
};

//--------------------------------------------------------------------- Composite:
class Composite : public Equipment                    // Is an Equipment that ...
{
private:
   std::vector<std::unique_ptr<Equipment>> children_; // has several Equipments

public:
   using Equipment::Equipment;

   void add(std::unique_ptr<Equipment> component)
   {
      children_.push_back(std::move(component));
   }

   void print(int indentation = 0) const override
   {
      for (int i = 0; i < indentation; ++i) std::cout << "  ";
      std::cout << "+ Composite: " << name_ << std::endl;

      for (const auto& child : children_)
      {
         child->print(indentation + 1);
      }
   }
};

//-------------------------------------------------------------------------- Main:
int main()
{
   std::cout << "=== COMPUTER ASSEMBLY (COMPOSITE PATTERN) ===\n" << std::endl;

   // 1. Create the main composite
   auto mainBox = std::make_unique<Composite>("Tower Case");

   // 2. Add simple leaf to main box
   mainBox->add(std::make_unique<SSD>("Samsung 980 Pro"));

   // 3. Create a motherboard (composite)
   auto motherboard = std::make_unique<Composite>("ASUS ROG Motherboard");
   
   // 4. Add leaves to motherboard
   motherboard->add(std::make_unique<CPU>("Intel i9-13900K"));
   motherboard->add(std::make_unique<RAM>("Corsair Vengeance 32GB"));
   motherboard->add(std::make_unique<RAM>("Corsair Vengeance 32GB"));

   // 5. Create a disk rack memory (composite)
   auto diskrack = std::make_unique<Composite>("Disk Rack SDD Server Memory");

   // 6. Add SSD memory into diskrack
   diskrack->add(std::make_unique<SSD>("One Tera SSD disk"));
   diskrack->add(std::make_unique<SSD>("One Tera SSD disk"));
   diskrack->add(std::make_unique<SSD>("One Tera SSD disk"));

   // 7. Add the diskrack (composite) into motherboard (composite)
   motherboard->add(std::move(diskrack));
   
   // 8. Add the motherboard (composite) into the box (composite)
   mainBox->add(std::move(motherboard));

   // 6. Print the whole structure uniformly starting at level 0
   mainBox->print();

   std::cout << "\n=== ASSEMBLY COMPLETED ===" << std::endl;
}

//================================================================================ END
