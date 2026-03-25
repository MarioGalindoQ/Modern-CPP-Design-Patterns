/**
 * ============================================================================
 *                   --- THE ABSTRACT FACTORY PATTERN ---
 * Concept:
 * This program demonstrates how to create families of related objects 
 * that must always work together.
 * 
 * Game Concept:
 * This program simulates a multi-environment game. The game can take place 
 * either in "Outer Space" or "Deep Underwater". Each environment requires 
 * a specific "Family" of objects that are conceptually related:
 *   1. A Player Vehicle (Spaceship or Submarine)
 *   2. Environmental Hazards (Meteors or Sharks)
 * 
 * The Architecture:
 * The Abstract Factory pattern is used to provide an interface for creating 
 * families of related or dependent objects without specifying their concrete 
 * classes. This ensures that the game engine never accidentally mixes 
 * incompatible objects (like spawning a Shark in Outer Space).
 * 
 * Key Components of the Pattern in this Code:
 *    1. Abstract Products (Player, Hazard): Interfaces for the game elements.
 *    2. Concrete Products (Spaceship, Meteor, etc.): Specific implementations 
 *       for each environment.
 *    3. Abstract Factory (EnvironmentFactory): A base interface that declares 
 *       creation methods for all abstract products.
 *    4. Concrete Factories (SpaceEnvironment, UnderwaterEnvironment): These 
 *       classes implement the creation logic for one specific theme/environment.
 * 
 * Smart Pointer & Memory Management:
 * We use std::unique_ptr for automatic memory management. In this version, 
 * we create two separate Hazard objects (hazard1 and hazard2) to demonstrate 
 * that the factory generates independent instances. When the runEnvironment 
 * function ends, the destructors will trigger, showing the automatic 
 * cleanup of each individual object.
 * ============================================================================
 */

#include <iostream>
#include <memory>
#include <string>

// ==========================================
// 1. Abstract Products
// ==========================================

class Player {
public:
    virtual ~Player() = default;
    virtual void spawn() const = 0;
};

class Hazard {
public:
    virtual ~Hazard() = default; 
    virtual void spawn() const = 0;
};

// ==========================================
// 2. Space Environment Implementation
// ==========================================

class Spaceship : public Player {
public:
    void spawn() const override {
        std::cout << " [Player] Spaceship is ready for takeoff!" << std::endl;
    }
    ~Spaceship() {
        std::cout << " [Cleanup] Spaceship removed from memory." << std::endl;
    }
};

class Meteor : public Hazard {
public:
    void spawn() const override {
        std::cout << " [Hazard] A Meteor is drifting in the sector!" << std::endl;
    }
    ~Meteor() {
        std::cout << " [Cleanup] Meteor removed from memory." << std::endl;
    }
};

// ==========================================
// 3. Underwater Environment Implementation
// ==========================================

class Submarine : public Player {
public:
    void spawn() const override {
        std::cout << " [Player] Submarine is diving to the ocean floor!" << std::endl;
    }
    ~Submarine() {
        std::cout << " [Cleanup] Submarine removed from memory." << std::endl;
    }
};

class Shark : public Hazard {
public:
    void spawn() const override {
        std::cout << " [Hazard] A Shark is approaching the area!" << std::endl;
    }
    ~Shark() {
        std::cout << " [Cleanup] Shark removed from memory." << std::endl;
    }
};

// ==========================================
// 4. Abstract Factory
// ==========================================

class EnvironmentFactory {
public:
    virtual ~EnvironmentFactory() = default;
    virtual std::unique_ptr<Player> createPlayer() const = 0;
    virtual std::unique_ptr<Hazard> createHazard() const = 0;
};

// ==========================================
// 5. Concrete Factories
// ==========================================

class SpaceEnvironment : public EnvironmentFactory {
public:
    std::unique_ptr<Player> createPlayer() const override {
        return std::make_unique<Spaceship>();
    }
    std::unique_ptr<Hazard> createHazard() const override {
        return std::make_unique<Meteor>();
    }
   ~SpaceEnvironment(){
      std::cout << " [Cleanup] SpaceEnvironmentFactory removed from memory." << std::endl;
   }
};

class UnderwaterEnvironment : public EnvironmentFactory {
public:
    std::unique_ptr<Player> createPlayer() const override {
        return std::make_unique<Submarine>();
    }
    std::unique_ptr<Hazard> createHazard() const override {
        return std::make_unique<Shark>();
    }
   ~UnderwaterEnvironment(){
      std::cout << " [Cleanup] UnderwaterEnvironmentFactory removed from memory." << std::endl;
   }
};

// ==========================================
// 6. Generic Game Engine Subroutine
// ==========================================

void runEnvironment(std::unique_ptr<EnvironmentFactory> factory) {
    std::cout << " --- Initializing Environment Assets ---" << std::endl;

    // We create one player
    std::unique_ptr<Player> player = factory->createPlayer();

    // We use the factory twice to create TWO independent hazard objects
    // This demonstrates that the factory is a generator of new instances.
    std::unique_ptr<Hazard> hazard1 = factory->createHazard();
    std::unique_ptr<Hazard> hazard2 = factory->createHazard();

    // Spawn all elements
    player->spawn();
    
    // Each hazard is a separate object in memory
    std::cout << " (Hazard 1):";
    hazard1->spawn();
    
    std::cout << " (Hazard 2):";
    hazard2->spawn();

    std::cout << " --- Gameplay simulation in progress... ---" << std::endl;
    std::cout << " --- Environment Mission Completed! ---" << std::endl;

    // At this point, the function ends. 'player', 'hazard1', 'hazard2', 
    // and 'factory' are destroyed. You will see four separate cleanup messages!
}

// ==========================================
// 7. Main Entry Point
// ==========================================

int main() {
    std::cout << "=== VIRTUAL ENVIRONMENT SIMULATOR ===\n" << std::endl;

    // 1. Run Space Environment
    std::cout << ">>> Setting up Space Mission..." << std::endl;
    runEnvironment(std::make_unique<SpaceEnvironment>());

    std::cout << "\n------------------------------------------\n" << std::endl;

    // 2. Run Underwater Environment
    std::cout << ">>> Setting up Underwater Mission..." << std::endl;
    runEnvironment(std::make_unique<UnderwaterEnvironment>());

    std::cout << "\n=== SYSTEM SHUTDOWN COMPLETE ===" << std::endl;
}

//================================================================================ END
