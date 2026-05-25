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
struct Label
{
   uint32_t entityId;
   std::string name;
};

struct Position
{
   uint32_t entityId;
   float x, y;
};

struct Velocity
{
   uint32_t entityId;
   float vx, vy;
};

struct AIControl
{
   uint32_t entityId;
   int state; // 0: Idle, 1: Patrol, 2: Attack, 3: Defense, 4: Dead
};

//--------------------------------------------------------- Component Registry:
class Registry
{
private:
   uint32_t nextEntityId_{1};

   Registry() = default;

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

   Registry(const Registry&)            = delete;
   Registry& operator=(const Registry&) = delete;

   // Centralized entity creation with identity guarantee
   uint32_t createEntity(std::string name = "")
   {
      uint32_t id = nextEntityId_++;
      if(!name.empty()) addComponent(Label{id, std::move(name)});
      return id;
   }

   // Resolves an Entity ID to its Label name for human-readable output
   std::string getEntityName(uint32_t id)
   {
      auto& labels = getComponents<Label>();
      for(const auto& label : labels)
         if(label.entityId == id) return label.name;
      
      return "Entity " + std::to_string(id);
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

class MovementSystem
{
public:
   void update()
   {
      auto& world      = Registry::getInstance();
      auto& positions  = world.getComponents<Position>();
      auto& velocities = world.getComponents<Velocity>();
      auto& aiStates   = world.getComponents<AIControl>();

      std::cout << " [System] Updating Physics...\n";
      
      for(size_t i = 0; i < positions.size(); ++i)
      {
         uint32_t id = positions[i].entityId;
         
         // Logic: Check if the entity is dead in the AI data bucket
         bool isDead = false;
         for(const auto& ai : aiStates)
            if(ai.entityId == id && ai.state == 4) isDead = true;

         if(isDead) continue;

         positions[i].x += velocities[i].vx;
         positions[i].y += velocities[i].vy;
         
         std::string name = world.getEntityName(id);
         std::cout << "  -> [" << name << "] moved to ("
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
      
      std::cout << " [System] Updating AI Decisions...\n";
      for(const auto& ai : aiStates)
      {
         std::string name = world.getEntityName(ai.entityId);
         switch (ai.state)
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

// ScenarioSystem: Handles the world timeline and state mutations.
class ScenarioSystem
{
public:
   void update(int frame, uint32_t marioId, uint32_t droneId)
   {
      auto& world    = Registry::getInstance();
      auto& aiStates = world.getComponents<AIControl>();

      // In a real simulation, this system would analyze the environment 
      // (proximity, line of sight, health) to trigger state changes. 
      // For this example, we simulate these triggers based on the frame timeline.
      for(auto& ai : aiStates)
      {
         if(frame == 2 && ai.entityId == droneId) ai.state = 2; // Drone starts attack
         if(frame == 3 && ai.entityId == marioId) ai.state = 3; // Mario detects danger and shields
         if(frame == 4 && ai.entityId == droneId) ai.state = 4; // Drone receives critical damage
         if(frame == 5 && ai.entityId == marioId) ai.state = 0; // Threat neutralized, back to Idle
      }
   }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== COMPONENT REGISTRY (DOD / SINGLETON / SIMULATION) ===\n" << std::endl;

   Registry& world = Registry::getInstance();

   // 1. Create Entities using automatic ID generation
   auto mario = world.createEntity("Mario");
   world.addComponent(Position{mario, 0.0f, 0.0f});
   world.addComponent(Velocity{mario, 1.0f, 2.0f});
   world.addComponent(AIControl{mario, 0}); 

   auto guard = world.createEntity("Enemy Guard");
   world.addComponent(Position{guard, 10.0f, 8.0f});
   world.addComponent(Velocity{guard, -1.5f, 1.0f});
   world.addComponent(AIControl{guard, 1}); 

   auto drone = world.createEntity("Aggressive Drone");
   world.addComponent(Position{drone, 7.0f, 2.5f});
   world.addComponent(Velocity{drone, -2.0f, 1.0f});
   world.addComponent(AIControl{drone, 1}); 

   // 2. Systems Initialization
   ScenarioSystem scenario;
   MovementSystem physics;
   AISystem       intelligence;

   // 3. Main Loop: Processing 6 frames of simulation
   for(int frame = 1; frame <= 5; ++frame)
   {
      std::cout << "--- Processing Frame " << frame << " ---\n";

      // The ScenarioSystem manages data mutations for this frame
      scenario.update(frame, mario, drone);

      // Execute standard systems over the mutated data
      physics.update();
      intelligence.update();
      std::cout << std::endl;
   }

   std::cout << "=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
