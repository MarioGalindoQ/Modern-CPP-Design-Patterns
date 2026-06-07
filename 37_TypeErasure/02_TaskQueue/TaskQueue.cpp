/**
 * ============================================================================
 * File: TaskQueue.cpp
 * Author: Mario Galindo Queralt, Ph.D.
 *
 * --- DESIGN OVERVIEW:
 * This program demonstrates Type Erasure applied to an execution pipeline.
 * We create an 'AnyTaskEntity' wrapper that can store and execute any "Callable"
 * object (lambdas, functors, or function pointers) without a shared
 * inheritance hierarchy.
 *
 * --- C++20 CONCEPTS:
 * We define a 'Callable' concept to ensure that only types providing
 * an 'operator()' can be wrapped as a task.
 *
 * --- THE RULE OF SEVEN:
 * 'AnyTaskEntity' manages its internal 'StorageInterface' via unique_ptr,
 * implementing the full lifecycle to allow tasks to be stored in
 * standard containers, moved between queues, or duplicated.
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <utility>
#include <type_traits>
#include <concepts>

//--------------------------------------------------------- 0. Formal Contract:
// A type is Callable if it can be invoked with no arguments.
template <class T>
concept Callable = requires(T& object)
{
   object();
};

//--------------------------------------------------------- 1. Type erasure container:
class AnyTaskEntity
{
private:
   // --- Internal Infrastructure ---

   class StorageInterface
   {
   public:
      virtual ~StorageInterface() = default;
      virtual void execute()                                  = 0;
      virtual std::unique_ptr<StorageInterface> clone() const = 0;
   };

   // Only accepts types that satisfy the 'Callable' concept.
   template <Callable TaskType>
   class Model final : public StorageInterface
   {
   private:
      TaskType function_;

   public:
      explicit Model(TaskType&& value) : function_{std::move(value)} { }
      explicit Model(const TaskType& value) : function_{value} { }

      // --- Domain Operation (Internal/Contractual) ---
      // Delegates directly to the business class. The wrapped type must natively 
      // implement this method to satisfy the compile-time 'Serializable' concept.
      void execute() override
      {
         function_();
      }

      // --- Extraneous Extension (External/Non-Intrusive) ---
      // This is an external capability injected solely by the Type Erasure wrapper.
      // The business classes are completely unaware of 'clone()', demonstrating how 
      // this pattern allows adding new polymorphic behaviors without altering the original classes.
      std::unique_ptr<StorageInterface> clone() const override
      {
         return std::unique_ptr< Model<TaskType> >(new Model<TaskType>(function_));
      }
   };

   std::unique_ptr<StorageInterface> pimpl_;

public:
   // --- APPLYING THE RULE OF SEVEN ---

   // 1:DC -  Default Constructor: Disabled.
   AnyTaskEntity() = delete;

   // 2:CC - Copy Constructor: Deep copy for duplicating tasks
   AnyTaskEntity(const AnyTaskEntity& other)
      : pimpl_{other.pimpl_ ? other.pimpl_->clone() : nullptr}
   {
      std::cout << " [Rule of Seven] 2:CC - Deep Copy Constructor.\n";
   }

   // 3:MC - Move Constructor
   AnyTaskEntity(AnyTaskEntity&& other) noexcept : pimpl_{std::move(other.pimpl_)}
   {
      std::cout << " [Rule of Seven] 3:MC - Move Constructor.\n";
   }

   // 4:CA - Copy Assignment
   AnyTaskEntity& operator=(const AnyTaskEntity& other)
   {
      std::cout << " [Rule of Seven] 4:CA - Copy Assignment.\n";
      if(this != &other)
      {
         AnyTaskEntity temp(other);
         std::swap(pimpl_, temp.pimpl_);
      }
      return *this;
   }

   // 5:MA - Move Assignment
   AnyTaskEntity& operator=(AnyTaskEntity&& other) noexcept
   {
      std::cout << " [Rule of Seven] 5:MA - Move Assignment.\n";
      if(this != &other) pimpl_ = std::move(other.pimpl_);
      return *this;
   }

   // 6:De - Destructor
   ~AnyTaskEntity()
   {
      if(pimpl_) std::cout << " [Rule of Seven] 6:De - Destructor - Task released.\n";
   }

   // 7:PC - Parametric Constructor (Template): Captures any Callable type.
   // Only accepts types that satisfy the 'Callable' concept.
   template <Callable TaskType>
   AnyTaskEntity(TaskType&& object)
      : pimpl_{std::make_unique< Model<std::decay_t<TaskType>> >(std::forward<TaskType>(object))}
   {
      std::cout << " [Rule of Seven] 7:PC - Parametric Constructor - Task captured via StorageInterface.\n";
   }

   // --- Public Interface ---
   void execute()
   {
      if(pimpl_) pimpl_->execute();
   }
};

//--------------------------------------------------- 2. Custom Functor (Task):
// Example of a class that is NOT designed for this system but is compatible.
class LogWorker
{
private:
   std::string id_;
public:
   explicit LogWorker(std::string id) : id_{std::move(id)} { }
   void operator()() { std::cout << " -> [LogWorker " << id_ << "] is processing logs.\n"; }
};

//--------------------------------------------------------- 3. Main simulation:
int main()
{
   std::cout << "=== TYPE ERASURE: ASYNCHRONOUS TASK QUEUE ===\n" << std::endl;

   std::vector<AnyTaskEntity> taskQueue;

   std::cout << "--- PHASE 1: Loading heterogeneous tasks ---\n";

   // Task A: A lambda with capture
   std::string secret = "Data_42";
   taskQueue.push_back([secret]()
   {
      std::cout << " -> [Lambda] Accessing captured state: " << secret << "\n";
   });

   // Task B: A custom functor object
   taskQueue.push_back(LogWorker{"Alpha"});

   // Task C: Another lambda
   taskQueue.push_back([]()
   {
      std::cout << " -> [Lambda] Performing a quick calculation: " << 10 + 20 << "\n";
   });

   std::cout << "\n--- PHASE 2: Duplicating the Queue (Deep Copy) ---\n";
   auto backupQueue = taskQueue;

   std::cout << "\n--- PHASE 3: Executing the Primary Queue ---\n";
   for(auto& task : taskQueue) task.execute();

   std::cout << "\n--- PHASE 4: Executing the Backup Queue ---\n";
   for(auto& task : backupQueue) task.execute();

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
