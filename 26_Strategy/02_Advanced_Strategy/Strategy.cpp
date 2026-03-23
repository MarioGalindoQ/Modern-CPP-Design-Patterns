/**
 * ============================================================================
 * File: Strategy.cpp (Advanced Version)
 * 
 * --- DESIGN OVERVIEW:
 * This implementation demonstrates "Strategy Composition". A single context 
 * (SortAndPrint) delegates work to two different strategy families: 
 * SortStrategy and PrintStrategy.
 * 
 * --- DEPENDENCY INJECTION:
 * The client injects concrete strategies into the context at runtime, 
 * allowing the sorting and printing behavior to vary independently without 
 * modifying the core logic.
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <memory>
#include <iomanip>
#include <algorithm>

//--------------------------------------------------------- Sort Strategies:
class SortStrategy
{
public:
   virtual ~SortStrategy() = default;
   virtual void sort(std::vector<int>& data) const = 0;
};

class BubbleSort : public SortStrategy
{
public:
   void sort(std::vector<int>& data) const override
   {
      std::cout << " [Strategy] Applying Bubble Sort\n";
      for (size_t i = data.size(); i > 0; --i)
         for (size_t j = 0; j < i - 1; ++j)
            if (data[j] > data[j + 1]) std::swap(data[j], data[j + 1]);
   }
};

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
               if (data[j] > data[j + g]) std::swap(data[j], data[j + g]);
   }
};

//--------------------------------------------------------- Print Strategies:
class PrintStrategy
{
public:
   virtual ~PrintStrategy() = default;
   virtual void print(const std::vector<int>& data) const = 0;
};

class FreePrint : public PrintStrategy
{
public:
   void print(const std::vector<int>& data) const override
   {
      for (int n : data) std::cout << n << ' ';
      std::cout << '\n';
   }
};

class WidthPrint : public PrintStrategy
{
public:
   void print(const std::vector<int>& data) const override
   {
      for (int n : data) std::cout << std::setw(2) << std::setfill('0') << n << ' ';
      std::cout << '\n';
   }
};

//--------------------------------------------------------- Context:
class SortAndPrint
{
private:
   std::unique_ptr<SortStrategy>  sortImpl_;  // Abstract sort  strategy
   std::unique_ptr<PrintStrategy> printImpl_; // Abstract print strategy

public:
   void setSort(std::unique_ptr<SortStrategy> sortImpl) // Dependency Injection
   {
      sortImpl_ = std::move(sortImpl);
   }
  
   void setPrint(std::unique_ptr<PrintStrategy> printImpl) // Dependency Injection
   {
      printImpl_ = std::move(printImpl);
   }
  
   void execute(std::vector<int>& data) const
   {
      if (sortImpl_) sortImpl_->sort(data);
      if (printImpl_) printImpl_->print(data);
   }
};

//--------------------------------------------------------- Main:
int main()
{
   std::cout << "=== STRATEGY PATTERN (ADVANCED COMPOSITION) ===\n" << std::endl;

   std::vector<int> data1 = {30, 21, 6, 14, 8, 11, 10, 26, 12};
   std::vector<int> data2 = data1; // Copy for the second test

   SortAndPrint sorter;
  
   // --- Sort with Bubble sort and FreePrint
   std::cout << "Testing Bubble + FreePrint:\n";
   sorter.setSort(std::make_unique<BubbleSort>());
   sorter.setPrint(std::make_unique<FreePrint>());
   sorter.execute(data1);

   // --- Sort with Shell sort and WidthPrint
   std::cout << "\nTesting Shell + WidthPrint:\n";
   sorter.setSort(std::make_unique<ShellSort>());
   sorter.setPrint(std::make_unique<WidthPrint>());
   sorter.execute(data2);

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
