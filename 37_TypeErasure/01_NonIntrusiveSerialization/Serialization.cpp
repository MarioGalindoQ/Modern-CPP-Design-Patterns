/**
 * ============================================================================
 * File: Serialization.cpp
 * Author: Mario Galindo Queralt, Ph.D.
 *
 * --- DESIGN OVERVIEW:
 * This program demonstrates the Type Erasure pattern. It achieves "Open
 * Polymorphism" by allowing unrelated classes to be treated as a single
 * type without requiring a common base class (non-intrusive).
 *
 * --- C++20 CONCEPTS:
 * We use a formal compile-time contract (concept) to ensure that only classes
 * providing a 'serialize' method can be wrapped, improving compiler error
 * messages without adding runtime overhead.
 *
 * --- THE ARCHITECTURAL TRIAD:
 * 1. The Wrapper (SerializableEntity): The public-facing class that the
 *    user interacts with. It manages the "Rule of Seven".
 * 2. The StorageInterface (Base): A private, internal abstract interface that
 *    defines the behavior to be erased.
 * 3. The Model (Template): A private, internal template that implements
 *    the StorageInterface for any specific type (BusinessType).
 *
 * --- THE RULE OF SEVEN:
 * Type Erasure is the ultimate test for object lifecycle management. We
 * implement all seven key methods to ensure deep copies, efficient moves,
 * and safe destruction of the opaque internal pointer.
 * ============================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <type_traits>
#include <concepts>

//--------------------------------------------------------- 0. Formal Contract:
// Defines what it means to be "Serializable" at compile-time.
template <class T>
concept Serializable = requires(const T& object, std::ostream& os)
{
   object.serialize(os);
};

//--------------------------------------------------------- 1. Business classes:
// These classes are 100% independent. No inheritance, no virtual methods.

class User
{
private:
   std::string name_;

public:
   explicit User(std::string name) : name_{std::move(name)} { }

   void serialize(std::ostream& os) const
   {
      os << "{ \"user\": \"" << name_ << "\" }";
   }
};

class NetworkConfig
{
private:
   int port_;

public:
   explicit NetworkConfig(int port) : port_{port} { }

   void serialize(std::ostream& os) const
   {
      os << "{ \"port\": " << port_ << " }";
   }
};

//--------------------------------------------------------- 2. Type erasure container:
class SerializableEntity
{
private:
   // --- Internal Infrastructure ---

   class StorageInterface
   {
   public:
      virtual ~StorageInterface() = default;
      virtual void serialize(std::ostream& os)          const = 0;
      virtual std::unique_ptr<StorageInterface> clone() const = 0;
   };

   // Only accepts types that satisfy the 'Serializable' concept.
   template <Serializable BusinessType>
   class Model final : public StorageInterface
   {
   private:
      BusinessType data_;

   public:
      explicit Model(BusinessType&& value) : data_{std::move(value)} { }
      explicit Model(const BusinessType& value) : data_{value} { }

      void serialize(std::ostream& os) const override
      {
         data_.serialize(os);
      }

      std::unique_ptr<StorageInterface> clone() const override
      {
         return std::unique_ptr< Model<BusinessType> >(new Model<BusinessType>(data_));
      }
   };

   std::unique_ptr<StorageInterface> pimpl_;

public:
   // --- APPLYING THE RULE OF SEVEN ---

   // 1:DC - Default Constructor: Disabled.
   SerializableEntity() = delete;

   // 2:CC - Copy Constructor: Deep copy via virtual clone()
   SerializableEntity(const SerializableEntity& other)
      : pimpl_{other.pimpl_ ? other.pimpl_->clone() : nullptr}
   {
      std::cout << " [Rule of Seven] 2:CC - Copy Constructor (Deep Copy).\n";
   }

   // 3:MC - Move Constructor: Zero-cost pointer transfer
   SerializableEntity(SerializableEntity&& other) noexcept : pimpl_{std::move(other.pimpl_)}
   {
      std::cout << " [Rule of Seven] 3:MC - Move Constructor.\n";
   }

   // 4:CA - Copy Assignment: Copy-and-Swap idiom for strong exception safety
   SerializableEntity& operator=(const SerializableEntity& other)
   {
      std::cout << " [Rule of Seven] 4:CA - Copy Assignment.\n";
      if(this != &other)
      {
         SerializableEntity temp(other); // 2:CC
         std::swap(pimpl_, temp.pimpl_);
      }
      return *this;
   }

   // 5:MA - Move Assignment: Efficient ownership transfer
   SerializableEntity& operator=(SerializableEntity&& other) noexcept
   {
      std::cout << " [Rule of Seven] 5:MA - Move Assignment.\n";
      if(this != &other) pimpl_ = std::move(other.pimpl_);
      return *this;
   }

   // 6:De - Destructor
   ~SerializableEntity()
   {
      if(pimpl_) std::cout << " [Rule of Seven] 6:De - Destructor (Internal pimpl released).\n";
   }

   // 7:PC - Parametric Constructor (Template): Captures any Serializable type.
   // Only accepts types that satisfy the 'Serializable' concept.
   template <Serializable BusinessType>
   SerializableEntity(BusinessType&& object)
      : pimpl_{std::make_unique< Model<std::decay_t<BusinessType>> >(std::forward<BusinessType>(object))}
   {
      std::cout << " [Rule of Seven] 7:PC - Parametric Constructor triggered via StorageInterface.\n";
   }

   // --- Public Interface ---
   void save(std::ostream& os) const
   {
      if(pimpl_) pimpl_->serialize(os);
   }
};

//--------------------------------------------------------- 3. Main simulation:
int main()
{
   std::cout << "=== TYPE ERASURE: NON-INTRUSIVE SERIALIZATION (C++20) ===\n" << std::endl;

   std::vector<SerializableEntity> archive;

   std::cout << "--- PHASE 1: Adding objects to the archive ---\n";
   archive.push_back(User{"Mario"});
   archive.push_back(NetworkConfig{8080});
   archive.push_back(User{"Bjarne"});

   std::cout << "\n--- PHASE 2: Demonstrating Deep Copy (Rule 2:CC) ---\n";
   auto archive_backup = archive;

   std::cout << "\n--- PHASE 3: Processing the Archive ---\n";
   for(const auto& entity : archive)
   {
      std::cout << " -> Serializing: ";
      entity.save(std::cout);
      std::cout << "\n";
   }

   std::cout << "\n--- PHASE 4: Processing the Backup ---\n";
   for(const auto& entity : archive_backup)
   {
      std::cout << " -> Backup Data: ";
      entity.save(std::cout);
      std::cout << "\n";
   }

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
