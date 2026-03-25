/**
 * Game Concept & Example Overview:
 *
 * This is a multi-level space game where the difficulty progressively 
 * increases as the player advances. In each new level, the game generates 
 * meteorites that are larger and travel at faster initial speeds, making 
 * them increasingly challenging to deal with.
 *
 * This code example is intentionally kept simple and focuses exclusively 
 * on demonstrating the "Factory Method" design pattern (as defined by the 
 * Gang of Four). It illustrates how to elegantly manage the creation of 
 * different meteor types as the level increases, without tightly coupling 
 * the game logic to concrete meteor classes. 
 *
 * Instead of implementing a full game engine with graphics and collision 
 * detection, this program serves as a summary of what would happen in a 
 * much longer, complete game. Whenever a meteor is spawned, the program 
 * simply calls its showInfo() method, which prints a text description of 
 * the meteor's size and speed to the console.
 */

#include <iostream>
#include <memory>
#include <string>

// ==========================================
// 1. Abstract Product
// ==========================================
class Meteor {
public:
    virtual ~Meteor() = default;
    virtual void showInfo() const = 0;
};

// ==========================================
// 2. Concrete Products
// ==========================================
class SmallMeteor : public Meteor {
public:
    void showInfo() const override {
        std::cout << " -> Meteor spawned![Size: Small | Speed: Slow]" << std::endl;
    }
};

class MediumMeteor : public Meteor {
public:
    void showInfo() const override {
        std::cout << " -> Meteor spawned![Size: Medium | Speed: Normal]" << std::endl;
    }
};

class LargeMeteor : public Meteor {
public:
    void showInfo() const override {
        std::cout << " -> Meteor spawned! [Size: Large | Speed: Fast]" << std::endl;
    }
};

// ==========================================
// 3. Abstract Creator
// ==========================================
class FactoryMethod {
public:
    virtual ~FactoryMethod() = default;
    
    // The Factory Method
    virtual std::unique_ptr<Meteor> createMeteor() const = 0;
};

// ==========================================
// 4. Concrete Creators (Levels)
// ==========================================
class FactoryLevel1 : public FactoryMethod {
public:
    std::unique_ptr<Meteor> createMeteor() const override {
        return std::make_unique<SmallMeteor>();
    }
};

class FactoryLevel2 : public FactoryMethod {
public:
    std::unique_ptr<Meteor> createMeteor() const override {
        return std::make_unique<MediumMeteor>();
    }
};

class FactoryLevel3 : public FactoryMethod {
public:
    std::unique_ptr<Meteor> createMeteor() const override {
        return std::make_unique<LargeMeteor>();
    }
};

// ==========================================
// 5. Game Logic / Client Code
// ==========================================
// Passing std::unique_ptr by value transfers ownership to this function.
// The function does not need to know the specific factory or level type.
void playLevel(std::unique_ptr<FactoryMethod> factory) {
    std::cout << " [Game Engine] Spawning meteors for this level...\n";
    
    // std::unique_ptr overloads the '->' operator, allowing us to safely 
    // access the methods of the managed FactoryMethod object.
    std::unique_ptr<Meteor> m1 = factory->createMeteor();
    std::unique_ptr<Meteor> m2 = factory->createMeteor();
    
    m1->showInfo();
    m2->showInfo();
    
    std::cout << " [Game Engine] Level Cleared!\n\n";
} // 'factory' goes out of scope here. The managed object is destroyed automatically.

// ==========================================
// 6. Main Flow
// ==========================================
int main() {
    std::cout << "=== SPACE METEOR DEFENSE ===\n\n";

    std::unique_ptr<FactoryMethod> currentFactory;

    // --- LEVEL 1 ---
    std::cout << "--- STARTING LEVEL 1 ---\n";
    currentFactory = std::make_unique<FactoryLevel1>();
    
    // We use std::move() to explicitly transfer ownership of the factory.
    // After this call, 'currentFactory' becomes nullptr.
    playLevel(std::move(currentFactory)); 

    // --- LEVEL 2 ---
    std::cout << "--- STARTING LEVEL 2 ---\n";
    // We can safely reuse the variable by assigning a new factory object.
    currentFactory = std::make_unique<FactoryLevel2>();
    playLevel(std::move(currentFactory)); 

    // --- LEVEL 3 ---
    std::cout << "--- STARTING LEVEL 3 ---\n";
    // We can also create and pass the unique_ptr directly in one line.
    // The compiler implicitly handles the move semantics for temporary objects.
    playLevel(std::make_unique<FactoryLevel3>());

    std::cout << "=== CONGRATULATIONS! YOU WIN! ===\n";
}

//================================================================================ END
