/**
 * ============================================================================
 * File: Employee.cpp (Pimpl Idiom / Bridge)
 * Author: Mario Galindo Queralt, Ph.D.
 *
 * --- DESIGN OVERVIEW:
 * This program implements the Pimpl (Pointer to Implementation) idiom, which
 * is a specific C++ application of the Bridge pattern.
 *
 * --- MEMORY MANAGEMENT:
 * We use 'std::unique_ptr' to manage the 'pimpl' object. Unlike raw pointers,
 * this ensures that the implementation is cleaned up automatically,
 * respecting RAII principles and preventing memory leaks.
 *
 * --- INVARIANT:
 * The 'pimpl' pointer must never be nullptr. To enforce this, we manually
 * implement the move constructor and move assignment operator to ensure
 * that moved-from objects are immediately re-initialized with a valid
 * implementation, preventing crashes when accessed after a move operation.
 *
 * --- COMPILATION FIREWALL:
 * The definition of 'class Employee::Impl' is hidden in this .cpp file.
 * Changes to 'Impl' do not require recompilation of any client code that
 * only includes 'Employee.h'.
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <memory>
#include <utility>

//---------------------------------------------------------------------------- Employe
class Employee
{
public:
   Employee();                                     // 1 DC: Default constructor
   Employee(std::string name, std::string id);     // 7 PC: Parametric Constructor

   // In Pimpl with unique_ptr, we must manually define Copy logic
   // because unique_ptr is move-only.
   Employee(const Employee& other);                // 2 CC: Copy constructor (Deep Copy)
   Employee& operator=(const Employee& other);     // 4 CA: Copy assignment

   // Move logic can be defaulted, but must be defined in the .cpp
   Employee(Employee&& other) noexcept;            // 3 MC: Move constructor
   Employee& operator=(Employee&& other) noexcept; // 5 MA: Move assignment

   ~Employee(); // Defined in .cpp where Impl is complete // 6 De: Destructor

   std::string getName() const;
   void setName(std::string&& name);

   std::string getId() const;
   void setId(std::string&& id);

private:
   class Impl;
   std::unique_ptr<Impl> pimpl; // The Bridge. This pointer can't be nullptr!
};

//------------------------------------------------- Nested Struct Definition:
class Employee::Impl
{
private:
   std::string name_{"No name"};
   std::string id_{"No id"};

public:
   Impl() = default; // 1 DC: Default constructor
   Impl(std::string name, std::string id) : name_{std::move(name)}, id_{std::move(id)} {} // 7 PC

   // --- RULE OF FIVE vs. RULE OF ZERO ---
   // According to the "Rule of Zero", we could leave all five of the following
   // special member functions undefined, and the compiler would generate them
   // automatically. However, per the "Rule of Five", if even one is explicitly
   // defined, it is best practice to define all five to ensure consistent and
   // safe behavior. In this case, we explicitly default all five functions to
   // prevent compiler warnings when at least one is defined and to clarify intent,
   // although defining none of them would also have been a valid approach.

   // (The numbering follows my "Rule of 7" mnemonic system)

   Impl(const Impl&)                = default;  // 2 CC: Copy Constructor
   Impl(Impl&&) noexcept            = default;  // 3 MC: Move Constructor
   Impl& operator=(const Impl&)     = default;  // 4 CA: Copy Assignment
   Impl& operator=(Impl&&) noexcept = default;  // 5 MA: Move Assignment
   ~Impl()                          = default;  // 6 De: Destructor

   std::string getName() const {return name_;}
   void setName(std::string&& name) {name_ = std::move(name);}

   std::string getId() const {return id_;}
   void setId(std::string&& id) {id_ = std::move(id);}
};

//------------------------------------------------- Employee Implementation:

// 1 DC: Default constructor
Employee::Employee() : pimpl{std::make_unique<Impl>()}
{
   std::cout << " 1 DC: Default constructor\n";
}

// 7 PC: Parametric Constructor
Employee::Employee(std::string name, std::string id)
   : pimpl{std::make_unique<Impl>(std::move(name), std::move(id))}
{
   std::cout << " 7 PC: Parametric Constructor\n";
}

// 2 CC: Copy constructor (Deep Copy)
Employee::Employee(const Employee& other)
   : pimpl{std::make_unique<Impl>(*other.pimpl)}
{
   std::cout << " 2 CC: Copy constructor\n";
}

// 4 CA: Copy assignment
Employee& Employee::operator=(const Employee& other)
{
   std::cout << " 4 CA: Copy assignment\n";
   if (this != &other)
   {
      *pimpl = *other.pimpl; // Uses default Impl copy assignment operator
   }
   return *this;
}

// 3 MC: Move constructor
Employee::Employee(Employee&& other) noexcept
   : pimpl{std::move(other.pimpl)} // Transfers ownership via unique_ptr move constructor
{
   std::cout << " 3 MC: Move constructor\n";
// Instead of leaving 'other.pimpl' as nullptr (default move constructor behavior),
// we re-initialize it to maintain our "Can't be nullptr" invariant.
   other.pimpl = std::make_unique<Impl>();
}

// 5 MA: Move assignment
Employee& Employee::operator=(Employee&& other) noexcept
{
   std::cout << " 5 MA: Move assignment\n";
   if (this != &other)
   {
      // Using swap is an elegant way to maintain the non-null invariant
      std::swap(pimpl, other.pimpl);
   }
   return *this;
}

// 6 De: Destructor
Employee::~Employee() = default;

std::string Employee::getName() const
{
   return pimpl->getName(); // Here the bridge is used
}

void Employee::setName(std::string&& name)
{
   pimpl->setName(std::move(name)); // Here the bridge is used
}

std::string Employee::getId() const
{
   return pimpl->getId(); // Here the bridge is used
}

void Employee::setId(std::string&& id)
{
   pimpl->setId(std::move(id)); // Here the bridge is used
}

//------------------------------------------------------------------------------- Main
int main()
{
   std::cout << "=== BRIDGE (PIMPL) PATTERN SIMULATION ===\n\n";

   std::cout << " 1 DC: Employee():\n";
   Employee e1;                     // 1 DC: Employee()

   std::cout << "\n 7 PC: Employee(std::string name, std::string id):\n";
   Employee e2{"Jimmy", "1-653-9"}; // 7 PC: Employee(std::string name, std::string id)

   std::cout << "\n 2 CC: Employee(Employee const& other):\n";
   Employee e3{e2};                 // 2 CC: Employee(Employee const& other)

   std::cout << "\n 3 MC: Employee(Employee&& other):\n";
   Employee e4{std::move(e2)};      // 3 MC: Employee(Employee&& other)

   std::cout << "\n 4 CA: operator=(Employee const& other):\n";
   e2 = e4;                         // 4 CA: operator=(Employee const& other)

   std::cout << "\n 5 MA: operator=(Employee&& other):\n";
   e1 = std::move(e4);              // 5 MA: operator=(Employee&& other)

   std::cout << "\n Print:\n";
   std::cout << " e1: " << e1.getName() << ", " << e1.getId() << '\n';
   std::cout << " e2: " << e2.getName() << ", " << e2.getId() << '\n';
   std::cout << " e3: " << e3.getName() << ", " << e3.getId() << '\n';
   std::cout << " e4: " << e4.getName() << ", " << e4.getId() << '\n';

   std::cout << "\n Fill and Print e4:\n";
   e4.setName("Mario");
   e4.setId("3-593-1");
   std::cout << " e4: " << e4.getName() << ", " << e4.getId() << '\n';

   std::cout << "\n=== SIMULATION COMPLETED ===\n";  // 6 De: ~Employee()
}

//================================================================================ END
