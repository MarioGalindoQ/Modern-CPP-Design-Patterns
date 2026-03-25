/**
 * ============================================================================
 * File: SimpleFactoryMethod.cpp
 * 
 * --- EDUCATIONAL OVERVIEW: THE "SIMPLE FACTORY" IDIOM ---
 * 
 * Game Concept:
 * This is a multi-level space game where meteors increase in size and speed 
 * as the player advances. Whenever a meteor is spawned, the program simply 
 * calls its showInfo() method to print its stats.
 * 
 * The Architecture (Simple / Parameterized Factory):
 * In this approach, we use a SINGLE concrete factory class (MeteorFactory). 
 * The client changes the factory's internal state by calling setLevel(int). 
 * When asked to create a meteor, the factory uses a control structure (like a 
 * 'switch' or 'if/else' statement) to decide which concrete meteor class to 
 * instantiate based on the current level.
 * 
 * --- DEFICIENCIES & WHY THIS IS NOT THE GoF FACTORY METHOD ---
 * 
 * While this approach is extremely common in game development because it is 
 * simple and intuitive, it has significant architectural flaws as the project grows:
 * 
 * 1. Violation of the Open/Closed Principle (OCP):
 *    This is the biggest flaw. SOLID principles state that code should be 
 *    "open for extension, but closed for modification". 
 *    If we want to add a Level 4 with a new "FireMeteor", we are FORCED to 
 *    open this file and modify the existing 'switch' statement inside the 
 *    MeteorFactory class. We are modifying existing, working code, which 
 *    increases the risk of introducing bugs.
 * 
 * 2. Scalability Issues (The "God Object" Anti-pattern):
 *    Imagine a game with 100 levels and 50 different enemy types. The 
 *    createMeteor() method would become a massive, hundreds-of-lines-long 
 *    switch statement. The factory becomes a "God Object" that knows about 
 *    every single meteor type in the entire game, tightly coupling all 
 *    dependencies into one file.
 * 
 * 3. Lack of Polymorphic Delegation:
 *    The Gang of Four (GoF) Factory Method pattern solves these issues by 
 *    making the Factory an abstract base class, and letting subclass factories 
 *    (e.g., Level1Factory, Level2Factory) decide what to instantiate. Here, 
 *    creation is hardcoded into a single class's logic.
 * 
 * Conclusion for Students:
 * Use the Simple Factory for small, limited scopes. But if your game or app 
 * will expand with many new types over time, you should upgrade to the true 
 * GoF Factory Method to keep your code modular, scalable, and safe.
 * ============================================================================
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
        std::cout << " -> Meteor spawned! [Size: Medium | Speed: Normal]" << std::endl;
    }
};

class LargeMeteor : public Meteor {
public:
    void showInfo() const override {
        std::cout << " -> Meteor spawned! [Size: Large | Speed: Fast]" << std::endl;
    }
};

// ==========================================
// 3. The Simple Factory (Concrete Class)
// ==========================================
class MeteorFactory {
private:
    int currentLevel;

public:
    MeteorFactory() : currentLevel(1) {}

    // Method to update the internal state of the factory
    void setLevel(int level) {
        currentLevel = level;
    }

    // The creation method uses hardcoded logic (a switch) to decide what to build.
    // WARNING: Adding Level 4 requires modifying this exact method (Violates OCP).
    std::unique_ptr<Meteor> createMeteor() const {
        switch (currentLevel) {
            case 1:
                return std::make_unique<SmallMeteor>();
            case 2:
                return std::make_unique<MediumMeteor>();
            case 3:
                return std::make_unique<LargeMeteor>();
            default:
                std::cout << " [Warning] Unknown level! Defaulting to Small Meteor.\n";
                return std::make_unique<SmallMeteor>();
        }
    }
};

// ==========================================
// 4. Game Logic / Client Code
// ==========================================
// In this version, the function takes a constant reference to the single factory.
// It doesn't take ownership (no std::unique_ptr or std::move needed for the factory), 
// because the exact same factory object will be reused for all levels.
void playLevel(const MeteorFactory& factory) {
    std::cout << " [Game Engine] Spawning meteors for this level...\n";

    // We use the '.' operator because 'factory' is a reference to the object.
    std::unique_ptr<Meteor> m1 = factory.createMeteor();
    std::unique_ptr<Meteor> m2 = factory.createMeteor();

    m1->showInfo();
    m2->showInfo();

    std::cout << " [Game Engine] Level Cleared!\n\n";
}

// ==========================================
// 5. Main Flow
// ==========================================
int main() {
    std::cout << "=== SPACE METEOR DEFENSE (SIMPLE FACTORY) ===\n\n";

    // We instantiate exactly ONE factory object for the entire game session.
    MeteorFactory gameFactory;

    // --- LEVEL 1 ---
    std::cout << "--- STARTING LEVEL 1 ---\n";
    gameFactory.setLevel(1);
    playLevel(gameFactory); // Passing the factory by reference

    // --- LEVEL 2 ---
    std::cout << "--- STARTING LEVEL 2 ---\n";
    // We just tell the SAME factory that the level has changed.
    gameFactory.setLevel(2);
    playLevel(gameFactory); 

    // --- LEVEL 3 ---
    std::cout << "--- STARTING LEVEL 3 ---\n";
    gameFactory.setLevel(3);
    playLevel(gameFactory);

    std::cout << "=== CONGRATULATIONS! YOU WIN! ===\n";
}

//================================================================================ END
