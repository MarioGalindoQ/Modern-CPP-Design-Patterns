/**
 * ============================================================================
 * File: Static_Strategy.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This implementation demonstrates the Static Strategy pattern using templates
 * (Mixin Inheritance). Instead of virtual dispatch at runtime, the algorithm
 * is bound at compile-time.
 * 
 * --- PERFORMANCE:
 * Because there are no virtual functions, the compiler can inline the 
 * sorting and printing logic directly into the 'execute' method, resulting
 * in zero runtime overhead (Zero-Overhead Principle).
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

//--------------------------------------------------------- Sort Strategies:
struct BubbleSort
{
   void sort(std::vector<int>& data) const
   {
      std::cout << " [Strategy] Applying Bubble Sort\n";
      for (size_t i = data.size(); i > 0; --i)
         for (size_t j = 0; j < i - 1; ++j)
            if (data[j] > data[j + 1]) std::swap(data[j], data[j + 1]);
   }
};

struct ShellSort
{
   void sort(std::vector<int>& data) const
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
struct FreePrint
{
   void print(const std::vector<int>& data) const
   {
      for (int n : data) std::cout << n << ' ';
      std::cout << '\n';
   }
};

struct WidthPrint
{
   void print(const std::vector<int>& data) const
   {
      for (int n : data)
         std::cout << std::setw(2) << std::setfill('0') << n << ' ';
      std::cout << '\n';
   }
};

//--------------------------------------------------------- Context (Static):
template <typename SortAlgo, typename PrintAlgo>
class SortAndPrint : private SortAlgo, private PrintAlgo
{
public:
   void execute(std::vector<int>& data) const
   {
      SortAlgo::sort(data);
      PrintAlgo::print(data);
   }
};

//--------------------------------------------------------- Main:
int main()
{
   std::cout << "=== STRATEGY PATTERN (STATIC TEMPLATES) ===\n" << std::endl;

   std::vector<int> data1 = {30, 21, 6, 14, 8, 11, 10, 26, 12};
   std::vector<int> data2 = data1;

   // 1. Compile-time composition: No pointers, no virtual functions.
   SortAndPrint<BubbleSort, FreePrint> bubbleSorter;
   std::cout << "Testing Bubble + FreePrint:\n";
   bubbleSorter.execute(data1);

   // 2. Another static composition
   SortAndPrint<ShellSort, WidthPrint> shellSorter;
   std::cout << "\nTesting Shell + WidthPrint:\n";
   shellSorter.execute(data2);

   std::cout << "\n=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
