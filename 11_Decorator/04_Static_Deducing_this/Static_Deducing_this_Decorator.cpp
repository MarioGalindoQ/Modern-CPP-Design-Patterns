/**
 * ============================================================================
 * File: Static_Deducing_this_Decorator.cpp (Modern C++23 Version)
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This program implements a cutting-edge version of the Static Decorator pattern 
 * (Mixin Inheritance) using C++23 "Deducing This". It models a professional 
 * Industrial Sensor Analytics system where raw hardware readings are enhanced 
 * through a chain of compile-time Mixins (Averaging and Alarms).
 * 
 * --- THE ARCHITECTURAL PROBLEM (INTERFACE LOSS):
 * Traditional Static Decorators (see Example 03) suffer from "Interface Loss". 
 * If a method defined in the deepest Base class is invoked, it typically 
 * returns a reference to the Base class. This "breaks the chain" because the 
 * compiler "forgets" about the outer decorators, preventing the client from 
 * accessing decorator-specific methods in the same fluent expression.
 * 
 * --- THE C++23 SOLUTION (CALIBRATABLE STATIC MIXINS):
 * By using 'this auto&& self' in both the Base class and every Decorator, we 
 * achieve "Interface Persistence". The 'self' parameter captures the 
 * "most derived" type (the outermost decorator) at the call site. This allows 
 * for a perfect, perfectly-typed Fluent Interface where methods from 
 * different layers can be interleaved seamlessly.
 * 
 * Furthermore, this architecture enables "Runtime Calibration": the structure 
 * is fixed at compile-time for maximum performance, but functional parameters 
 * (like alarm limits or filter windows) remain adjustable at runtime without 
 * re-instantiating the object.
 * 
 * --- TECHNICAL MECHANICS:
 * 1. Zero-Overhead Polymorphism: No VTables or pointers are used. The compiler 
 *    flattens the inheritance chain, enabling total inlining of the 
 *    processing logic and zero runtime indirection.
 * 2. Perfect Forwarding: Every setter returns 'std::forward<decltype(self)>(self)', 
 *    ensuring the value category (lvalue/rvalue) is preserved across the chain.
 * 3. Functional Enhancement: The decorators transform behavior (e.g., from 
 *    point readings to moving averages) via static dispatch.
 * 4. Deterministic Simulation: The base sensor uses a fixed data array to 
 *    ensure predictable and testable hardware readings.
 * 5. Total Hierarchy Visibility: Since 'self' always refers to the "most 
 *    derived" object, every layer (including the Base class) has full access 
 *    to the methods of all other layers. This creates a transparent 
 *    composition where the order of template nesting does not affect 
 *    functional behavior—provided method names are unique—as every component 
 *    of the composite object is visible to the entire hierarchy through the 
 *    explicit object parameter.
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <array>
#include <utility> // Required for std::forward

//--------------------------------------------------------- Base Component:
/**
 * AnalogSensor: Models the physical hardware.
 * It contains a simulated buffer of raw voltage readings.
 */
class AnalogSensor {
private:
   int sensorID_{0};
   size_t readIndex_{0};

   // Simulated deterministic hardware readings (Voltages)
   // In real life, these values ​​are not deterministic.
   static constexpr std::array<double, 16> simulatedData_{
      1.2, 1.5, 2.8, 2.5, 1.1, 0.9, 3.2, 3.5, 1.8, 1.4, 2.1, 3.2, 1.5, 1.1, 0.2, 0.5,
   };

public:
   // C++23: Every method in the base returns the outermost decorator!
   auto&& setID(this auto&& self, int id) noexcept {
      self.sensorID_ = id;
      return std::forward<decltype(self)>(self);
   }

   /**
    * Returns the next raw value from the hardware buffer.
    */
   double readRaw() {
      double val = simulatedData_[readIndex_];
      readIndex_ = (readIndex_ + 1) % simulatedData_.size(); // Wrap around
      return val;
   }

   int getID() const { return sensorID_; }
};

//-------------------------------------------------- Static Decorator Averager:
/**
 * Averager Decorator:
 * Injects a Moving Average filter into the sensor pipeline.
 * It intercepts the reading logic to provide a smoothed value
 * based on a configurable window size.
 */
template <typename Decorated>
class Averager : public Decorated {
private:
   int windowSize_{1};

public:
   // Runtime Calibration: Window size can be adjusted on the fly
   auto&& setWindowSize(this auto&& self, int size) noexcept {
      self.windowSize_ = (size > 10) ? 10 : (size < 1) ? 1 : size; // Constraint check
      return std::forward<decltype(self)>(self);
   }

   /**
    * Performs the average calculation by pulling multiple raw values 
    * from the underlying sensor logic.
    */
   double read(this auto&& self) {
      double sum = 0.0;
      for (int i = 0; i < self.windowSize_; ++i) {
         sum += self.readRaw(); 
      }
      double average = sum / self.windowSize_;
      std::cout << " [Filter] Averaged " << self.windowSize_ 
                << " samples. Result: " << average << "V\n";
      return average;
   }
};

//-------------------------------------------- Static Decorator ThresholdAlarm:
/**
 * ThresholdAlarm Decorator:
 * Adds safety monitoring to the sensor pipeline.  It checks
 * if the processed sensor value exceeds a defined safety
 * limit.
 */
template <typename Decorated>
class ThresholdAlarm : public Decorated {
private:
   double limit_{5.0};

public:
   // Runtime Calibration: Alarm limits are dynamic
   auto&& setAlarmLimit(this auto&& self, double limit) noexcept {
      self.limit_ = limit;
      return std::forward<decltype(self)>(self);
   }

   /**
    * High-level logic that coordinates the reading and the safety check.
    */
   void monitor(this auto&& self) {
      double currentVal = self.read(); // Static dispatch to Averager
      if (currentVal > self.limit_) {
         std::cout << " [ALARM] Sensor " << self.getID() 
                   << " reporting " << currentVal << "V. EXCEEDS LIMIT (" 
                   << self.limit_ << "V)!\n";
      } else {
         std::cout << " [Monitor] Sensor " << self.getID() 
                   << " is stable at " << currentVal << "V.\n";
      }
   }
};

//------------------------------------------------------------------- Main:
int main() {
   std::cout << "=== INDUSTRIAL SENSOR ANALYTICS (CALIBRATABLE STATIC DECORATORS) ===\n"
             << std::endl;

   /**
    * CONSTRUCTION OF THE SMART SENSOR:
    * We wrap the AnalogSensor with an Averager, then with a ThresholdAlarm.
    * 
    * THE POWER OF DEDUCING THIS:
    * Notice how we interleave calls from different hierarchy levels:
    * - .setID (Base)
    * - .setWindowSize (Decorator A)
    * - .setAlarmLimit (Decorator B)
    */
   
   std::cout << "--- PHASE 1: Initialization & Calibration ---\n";

   /**
    * ARCHITECTURAL NOTE ON COMPOSITION ORDER:
    * Due to C++23 "Deducing This", the order of template nesting is functionally 
    * agnostic in this specific implementation (e.g., Averager<ThresholdAlarm<...>> 
    * would yield the same result as ThresholdAlarm<Averager<...>>). 
    *
    * Since 'self' always represents the most derived (outermost) type, every 
    * layer has total visibility of the entire hierarchy's API. This transforms 
    * the "Onion-Layer" Decorator into a "Transparent Composite" where any part 
    * can invoke any other part seamlessly.
    */
// auto smartSensor = Averager<ThresholdAlarm<AnalogSensor>>{}
   auto smartSensor = ThresholdAlarm<Averager<AnalogSensor>>{}
                        .setID(101)            // Defined in AnalogSensor (Base)
                        .setWindowSize(4)      // Defined in Averager (Decorator A)
                        .setAlarmLimit(2.2);   // Defined in ThresholdAlarm (Decorator B)

   std::cout << " Sensor " << smartSensor.getID() << " configured.\n";

   std::cout << "\n--- PHASE 2: High Sensitivity Cycle ---\n";
   smartSensor.monitor();

   std::cout << "\n Taking another reading of data:\n";
   smartSensor.monitor();

   std::cout << "\n--- PHASE 3: Hot Re-Calibration (Values defined at execution time) ---\n";
   // We adjust the filter and the safety limit in a single fluent chain
   smartSensor.setWindowSize(3)
              .setAlarmLimit(1.8)
              .monitor();

   std::cout << "\n--- PHASE 4: Identity Preservation ---\n";
   // Even changing the ID doesn't break the decorator's API
   smartSensor.setID(292).setAlarmLimit(1.9).monitor();

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
