/**
 * ============================================================================
 * File: TemplateMethod.cpp
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the Template Method pattern. The base class 
 * 'SortAndPrint' defines the algorithm skeleton 'sortAndPrintVector', 
 * while concrete subclasses implement the specific 'sort' and 'print' steps.
 * 
 * --- HOLLYWOOD PRINCIPLE:
 * "Don't call us, we will call you." The base class controls the flow 
 * and calls the subclasses when needed.
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <memory>

//--------------------------------------------------------- Base Template Class:
class SortAndPrint
{
private:
   // Steps requiring peculiar implementations are private placeholders.
   // Derived classes override these to provide the "how".
   virtual void sort(std::vector<int>& data) = 0;
   virtual void print(const std::vector<int>& data) const = 0;

public:
   virtual ~SortAndPrint() = default;

   /**
    * THE TEMPLATE METHOD (Algorithm Skeleton)
    * This method implements the Non-Virtual Interface (NVI) idiom.
    * It defines a stable sequence of steps that subclasses cannot bypass.
    */
   void sortAndPrintVector(std::vector<int>& data)
   {
      // 1. [INVARIANT CODE] Pre-processing:
      // Perform mandatory administrative tasks such as input validation, 
      // logging the start of the operation, or starting performance timers.
      
      sort(data); // Call to the specific "how" (variant part)

      // 2. [INVARIANT CODE] Intermediate Logic:
      // Execute global consistency checks or verify invariants to ensure 
      // the algorithm step (sort) left the system in a valid state.
      
      print(data); // Call to the second specific step (variant part)

      // 3. [INVARIANT CODE] Post-processing:
      // Finalize administrative actions such as measuring elapsed time, 
      // reporting telemetry metrics, or releasing global resources.
   }
};

//--------------------------------------------------------- Concrete Classes:
class BubbleSort : public SortAndPrint
{
private:
   void sort(std::vector<int>& data) override
   {
      for (size_t i = data.size(); i > 0; --i)
         for (size_t j = 0; j < i - 1; ++j)
            if (data[j] > data[j + 1]) std::swap(data[j], data[j + 1]);
   }

   void print(const std::vector<int>& data) const override
   {
      std::cout << "Bubble: ";
      for (int n : data) std::cout << n << ' ';
      std::cout << '\n';
   }
};

class ShellSort : public SortAndPrint
{
private:
   void sort(std::vector<int>& data) override
   {
      int n = static_cast<int>(data.size());
      for (int g = n / 2; g > 0; g /= 2)
         for (int i = g; i < n; ++i)
            for (int j = i - g; j >= 0; j -= g)
               if (data[j] > data[j + g]) std::swap(data[j], data[j + g]);
   }

   void print(const std::vector<int>& data) const override
   {
      std::cout << "Shell:  ";
      for (int n : data) std::cout << std::setw(2) << std::setfill('0') << n << ' ';
      std::cout << '\n';
   }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== TEMPLATE METHOD PATTERN ===\n" << std::endl;

   std::vector<int> data1 = {30, 21, 6, 14, 8, 11, 10, 26, 12, 1, 15, 20, 8, 11, 5, 26, 30, 3};
   std::vector<int> data2 = data1;

   std::cout << "Initial Vector: ";
   for (int n : data1) std::cout << n << ' ';
   std::cout << "\n\n";

   // We use unique_ptr for safety
   std::unique_ptr<SortAndPrint> sorter;

   // 1. Sort with Bubble Sort
   sorter = std::make_unique<BubbleSort>();
   sorter->sortAndPrintVector(data1);

   // 2. Sort with Shell Sort
   sorter = std::make_unique<ShellSort>();
   sorter->sortAndPrintVector(data2);

   std::cout << "\n=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
