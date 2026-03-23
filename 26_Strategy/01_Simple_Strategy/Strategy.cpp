/**
 * ============================================================================
 * File: Strategy.cpp (Simple Version)
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the Strategy pattern. It defines a family of 
 * sorting algorithms, encapsulates each one, and makes them interchangeable 
 * at runtime.
 * 
 * --- DESIGN PRINCIPLES:
 * - Dependency Injection: The strategy is provided to the context, 
 *                         decoupling the sorting algorithm from the data
 *                         container.
 * - Open/Closed Principle: New sorting algorithms can be added without 
 *                          modifying the 'Sorter' class.
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

//--------------------------------------------------------- Strategy Interface:
class SortStrategy
{
public:
   virtual ~SortStrategy() = default;
   virtual void sort(std::vector<int>& data) const = 0;
};

//--------------------------------------------------------- Concrete Strategy A:
class BubbleSort : public SortStrategy
{
public:
   void sort(std::vector<int>& data) const override
   {
      std::cout << " [Strategy] Applying Bubble Sort\n";
      for (size_t i = data.size(); i > 0; --i)
         for (size_t j = 0; j < i - 1; ++j)
            if (data[j] > data[j + 1])
               std::swap(data[j], data[j + 1]);
   }
};

//--------------------------------------------------------- Concrete Strategy B:
class ShellSort : public SortStrategy
{
public:
   void sort(std::vector<int>& data) const override
   {
      std::cout << " [Strategy] Applying Shell Sort\n";
      int n = static_cast<int>(data.size());
      for (int g = n / 2; g > 0; g /= 2)
         for (int i = g; i < n; ++i)
            for (int j = i - g; j >= 0; j -= g)
               if (data[j] > data[j + g])
                  std::swap(data[j], data[j + g]);
   }
};

//--------------------------------------------------------- Context:
class Sorter
{
private:
   std::unique_ptr<SortStrategy> strategy_;

public:
   void setStrategy(std::unique_ptr<SortStrategy> strategy)
   {
      strategy_ = std::move(strategy);
   }

   void sortVector(std::vector<int>& data) const
   {
      if (strategy_) strategy_->sort(data);
      else std::cout << " [System] No strategy set!\n";
   }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== STRATEGY PATTERN (SIMPLE VERSION) ===\n" << std::endl;

   std::vector<int> data = {30, 21, 6, 14, 8, 11, 10, 26, 12};
   Sorter sorter;

   // 1. Use Bubble Sort
   sorter.setStrategy(std::make_unique<BubbleSort>());
   sorter.sortVector(data);

   std::cout << " Result: ";
   for (int n : data) std::cout << n << " ";
   std::cout << "\n\n";

   // 2. Use Shell Sort
   sorter.setStrategy(std::make_unique<ShellSort>());
   sorter.sortVector(data);

   std::cout << " Result: ";
   for (int n : data) std::cout << n << " ";
   std::cout << "\n" << std::endl;

   std::cout << "=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
