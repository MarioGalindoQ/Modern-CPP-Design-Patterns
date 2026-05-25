/**
 * ============================================================================
 * File: Component.cpp
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates an advanced Entity-Component-System (ECS) 
 * architecture built on Data-Oriented Design (DOD) principles. It represents 
 * a shift from traditional Object-Oriented programming to a model that 
 * prioritizes memory layout and CPU cache efficiency.
 * 
 * --- STATIC TYPE-TO-STORAGE MAPPING:
 * Instead of using inheritance-based lists or dynamic maps of pointers, this 
 * architecture utilizes C++ templates to map each unique 'ComponentType' 
 * to its own 'static std::vector'. This provides:
 * 1. Zero-Overhead: Component retrieval is resolved at compile-time, 
 *    eliminating runtime lookups and virtual function (vtable) overhead.
 * 2. Type Safety: The system is intrinsically type-safe; the compiler 
 *    guarantees that you always receive the exact vector type requested.
 * 
 * --- ENTITY IDENTITY & UNIQUENESS:
 * Entities are not objects, but simple 'uint32_t' identifiers. The Registry 
 * (acting as the World Container) manages an internal counter through 
 * 'createEntity()' to guarantee that every entity receives a strictly 
 * unique ID, preventing identity collisions in the global data buckets.
 * 
 * --- PERFORMANCE & CACHE LOCALITY:
 * Components are stored in contiguous memory blocks. This is vital because:
 * 1. Cache Locality: Systems (like Physics or AI) process these arrays 
 *    sequentially. The CPU can effectively prefetch data, drastically 
 *    reducing 'Cache Misses'.
 * 2. Data Normalization: Mathematical components (Position, Velocity) are 
 *    kept "lean" (small PODs) to fit more elements into a single CPU cache 
 *    line. Human-readable names or metadata are relegated to a separate 
 *    'Label' component, processed only when necessary (e.g., during logging).
 * 
 * --- PARALLEL ARRAY BUILDER:
 * To ensure data integrity, we utilize a Static Builder pattern (Pattern 01). 
 * This builder guarantees "Parallel Array Alignment": every time an entity 
 * is built, a new entry is pushed into EVERY component vector simultaneously. 
 * This allows Systems to use a shared index 'i' to access related data across 
 * different buckets in O(1) time without searching for IDs.
 * 
 * --- SINGLETON ARCHITECTURE:
 * The 'Registry' is implemented as a Meyers Singleton. This centralizes 
 * identity management and data storage, ensuring a single, consistent 
 * "World State" across the entire application. It prevents data 
 * fragmentation and provides a global access point for specialized Systems.
 * 
 * --- DYNAMIC STATE SIMULATION:
 * In DOD, objects don't "decide" to change state. Instead, logic systems or 
 * game controllers mutate the data in the vectors. Behavioural changes are 
 * the side effect of updated values being processed in the next cycle.
 * ============================================================================
 */

#include <vector>      // Provides contiguous memory storage for the static component buckets
#include <iostream>    // Used for simulation reporting and console output
#include <type_traits> // Required for std::decay_t to strip references and const from types
#include <string>      // Used for entity labels and human-readable metadata
#include <cstdint>     // Provides fixed-width integer types like uint32_t for entity IDs
#include <utility>     // Required for std::forward (perfect forwarding) and std::move

//--------------------------------------------------------- Component Types:
// Pure data structures (POD) for maximum cache efficiency.
struct Label     { uint32_t entityId; std::string name; };
struct Position  { uint32_t entityId; float x, y; };
struct Velocity  { uint32_t entityId; float vx, vy; };
struct AIControl { uint32_t entityId; int state; }; // 0:Idle, 1:Patrol, 2:Attack, 3:Defense, 4:Dead

//--------------------------------------------------------- Component Registry:
class Registry // A Meyers' Singleton class
{
private:
   uint32_t nextEntityId_{1};

   Registry() = default;

   Registry(const Registry&)            = delete;
   Registry& operator=(const Registry&) = delete;

   // Internal storage: one static vector per unique component type.
   // We use this second template to ensure that even if getComponents is called 
   // with references or const types, they all map to the same physical vector.
   template<class ComponentType>
   std::vector<ComponentType>& getInternalVector()
   {
      static std::vector<ComponentType> componentVector;
      return componentVector;
   }

public:
   static Registry& getInstance()
   {
      static Registry instance;
      return instance;
   }

   // Implementation of the Inertnal Static Builder for Entity Creation
   class EntityBuilder
   {
   private:
      uint32_t    id_;
      std::string name_{"Unknown"};
      float       x_{0}, y_{0}, vx_{0}, vy_{0};
      int         state_{0};

      // Private constructor: ensures only Registry can start the building process.
      // This protects the integrity of the nextEntityId_ counter.
      explicit EntityBuilder(uint32_t id) : id_{id} { }
      
      // We grant friendship to the outer class
      friend class Registry;

   public:
      EntityBuilder& setName(std::string name)
      {
         name_ = std::move(name);
         return *this;
      }

      EntityBuilder& setPosition(float x, float y)
      {
         x_ = x; y_ = y;
         return *this;
      }

      EntityBuilder& setVelocity(float vx, float vy)
      {
         vx_ = vx; vy_ = vy;
         return *this;
      }

      EntityBuilder& setAIState(int state)
      {
         state_ = state;
         return *this;
      }

      // The build method ensures all parallel arrays are updated at once.
      // This alignment is what allows O(1) access by index in the systems.
      uint32_t build()
      {
         auto& world = Registry::getInstance();
         world.addComponent(Label{id_, std::move(name_)});
         world.addComponent(Position{id_, x_, y_});
         world.addComponent(Velocity{id_, vx_, vy_});
         world.addComponent(AIControl{id_, state_});
         return id_;
      }
   };

   // Entry point to start the fluent building process
   EntityBuilder createEntity()
   {
      return EntityBuilder{nextEntityId_++};
   }

   template<class ComponentType>
   void addComponent(ComponentType&& component)
   {
      using CleanComponentType = std::decay_t<ComponentType>;
      getInternalVector<CleanComponentType>().emplace_back(std::forward<ComponentType>(component));
   }

   template<class ComponentType>
   std::vector<std::decay_t<ComponentType>>& getComponents()
   {
      return getInternalVector<std::decay_t<ComponentType>>();
   }
};

//--------------------------------------------------------- Systems:

// ScenarioSystem: Handles the world timeline by identifying entities by their Labels.
class ScenarioSystem
{
public:
   void update(int frame)
   {
      auto& world    = Registry::getInstance();
      auto& aiStates = world.getComponents<AIControl>();
      auto& labels   = world.getComponents<Label>();

      // Data mutation logic based on aligned indices
      // In a real simulation, this system would analyze the environment 
      // (proximity, line of sight, health) to trigger state changes. 
      // For this example, we simulate these triggers based on the frame timeline.
      for(size_t i = 0; i < labels.size(); ++i)
      {
         // Logic for Mario
         if(labels[i].name == "Mario")
         {
            if(frame == 3) aiStates[i].state = 3; // Mario detects danger and shields
            if(frame == 5) aiStates[i].state = 0; // Threat neutralized, back to Idle
         }

         // Logic for the Drone
         if(labels[i].name == "Aggressive Drone")
         {
            if(frame == 2) aiStates[i].state = 2; // Drone starts attack
            if(frame == 4) aiStates[i].state = 4; // Drone receives critical damage
         }
      }
   }
};

class PhysicsSystem
{
public:
   void update()
   {
      auto& world      = Registry::getInstance();
      auto& positions  = world.getComponents<Position>();
      auto& velocities = world.getComponents<Velocity>();
      auto& aiStates   = world.getComponents<AIControl>();
      auto& labels     = world.getComponents<Label>();

      std::cout << " [System] Updating Physics...\n";
      
      // Using shared index 'i' for O(1) access to parallel arrays
      for(size_t i = 0; i < positions.size(); ++i)
      {
         if(aiStates[i].state == 4) continue; // Skip processing for dead entities

         positions[i].x += velocities[i].vx;
         positions[i].y += velocities[i].vy;
         
         std::cout << "  -> [" << labels[i].name << "] moved to ("
                   << positions[i].x << ", " << positions[i].y << ")\n";
      }
   }
};

/**
 * AISystem along with ScenarioSystem, manages the intelligence for 
 * decision-making regarding game events. In a complex application, these 
 * systems can be implemented using Artificial Intelligence (AI) algorithms 
 * (e.g., Finite State Machines, Behavior Trees, or Neural Networks) to 
 * analyze data buckets and determine tactical actions.
 */

class AISystem
{
public:
   void update()
   {
      auto& world    = Registry::getInstance();
      auto& aiStates = world.getComponents<AIControl>();
      auto& labels   = world.getComponents<Label>();

      std::cout << " [System] Updating AI Decisions...\n";
      for(size_t i = 0; i < aiStates.size(); ++i)
      {
         const std::string& name = labels[i].name;
         switch(aiStates[i].state)
         {
            case 0: std::cout << "  -> [" << name << "] is searching for the treasure.\n";                break;
            case 1: std::cout << "  -> [" << name << "] is patrolling the area.\n";                       break;
            case 2: std::cout << "  -> [" << name << "] is ATTACKING!\n";                                 break;
            case 3: std::cout << "  -> [" << name << "] is in DEFENSE stance (Shields Up and Firing).\n"; break;
            case 4: std::cout << "  -> [" << name << "] HAS CRASHED (Dead).\n";                           break;
         }
      }
   }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== COMPONENT REGISTRY (DOD / SINGLETON / BUILDER) ===\n" << std::endl;

   Registry& world = Registry::getInstance();

   // 1. Create Entities using the Fluent Static Builder (Pattern 01)
   // This ensures all parallel vectors are perfectly aligned.

   [[maybe_unused]]
   auto mario = world.createEntity()
                     .setName("Mario")
                     .setPosition(0.0f, 0.0f)
                     .setVelocity(1.0f, 2.0f)
                     .setAIState(0)
                     .build();

   [[maybe_unused]]
   auto guard = world.createEntity()
                     .setName("Enemy Guard")
                     .setPosition(10.0f, 8.0f)
                     .setVelocity(-1.5f, 1.0f)
                     .setAIState(1)
                     .build();

   [[maybe_unused]]
   auto drone = world.createEntity()
                     .setName("Aggressive Drone")
                     .setPosition(7.0f, 2.5f)
                     .setVelocity(-2.0f, 1.0f)
                     .setAIState(1)
                     .build();

   // 2. Systems Initialization
   ScenarioSystem scenario;
   PhysicsSystem  physics;
   AISystem       intelligence;

   // 3. Main Loop: Processing 5 frames of aligned simulation
   for(int frame = 1; frame <= 5; ++frame)
   {
      std::cout << "--- Processing Frame " << frame << " ---\n";

      // The ScenarioSystem manages data mutations for this frame
      scenario.update(frame);

      // The PhysicsSystem manages the physical simulation
      physics.update();

      // The AISystem manages the game's artificial intelligence
      intelligence.update();

      std::cout << std::endl;
   }

   std::cout << "=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
