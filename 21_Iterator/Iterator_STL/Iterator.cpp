/**
 * ============================================================================
 * File: Iterator.cpp (Modern STL / Idiomatic C++ Version)
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * In modern C++, the Iterator pattern is built into the language itself.
 * Instead of using virtual interfaces (first, next, isDone), we use 
 * Operator Overloading (*, ++, !=) and Duck Typing (begin, end).
 * 
 * --- THE "RANGE-BASED FOR" MAGIC:
 * Because our BookCollection provides begin() and end(), and our BookIterator 
 * implements the required operators, we can use the elegant C++11 loop:
 *    for(const auto& book : library) { ... }
 * 
 * This approach has ZERO virtual overhead and provides maximum performance.
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <memory>
#include <unordered_set>
#include <vector>

//--------------------------------------------------------- Domain Object:
class Book
{
private:
   std::string title_;
   std::string author_;

   Book()                       = delete;      // 1 DC: Default Constructor
   Book(const Book&)            = delete;      // 2 CC: Copy Constructor
   Book& operator=(const Book&) = delete;      // 4 CA: Copy Assignment
   Book& operator=(Book&&)      = delete;      // 5 MA: Move Assignment

public:
   Book(Book&&)                 = default;     // 3 MC: Move Constructor
   ~Book()                      = default;     // 6 De: Destructor

   Book(std::string title, std::string author) // 7 PC: Particular Constructor 
      : title_{std::move(title)}, author_{std::move(author)} { }

   std::string getTitle()  const { return title_; }
   std::string getAuthor() const { return author_; }
};

//--------------------------------------------------------- Internal Node:
class Node
{
public:
   Book book;
   std::unique_ptr<Node> next;

   explicit Node(Book book) : book{std::move(book)}, next{nullptr} { }
};

//--------------------------------------------------------- STL-Style Iterator:
// We don't inherit from any interface. We just provide the expected operators.
class BookIterator
{
private:
   const Node* currentNode_;

public:
   // The iterator acts as a lightweight pointer wrapper
   explicit BookIterator(const Node* node) : currentNode_{node} { }

   // 1. Dereference operator (replaces currentItem())
   const Book& operator*() const
   {
      return currentNode_->book;
   }

   // 2. Pre-increment operator (replaces next())
   BookIterator& operator++()
   {
      if(currentNode_) currentNode_ = currentNode_->next.get();
      return *this;
   }

   // 3. Inequality operator (replaces !isDone())
   bool operator!=(const BookIterator& other) const
   {
      return currentNode_ != other.currentNode_;
   }
};

//--------------------------------------------------------- Concrete Aggregate:
class BookCollection
{
private:
   std::unique_ptr<Node> head_;
   Node* tail_{nullptr}; 

public:
   void addBook(const std::string& title, const std::string& author)
   {
      auto newNode = std::make_unique<Node>(Book{title, author});
      Node* current = newNode.get();

      if(!head_) head_ = std::move(newNode);
      else       tail_->next = std::move(newNode);
      
      tail_ = current;
   }

   // --- The magic methods for C++ Range-based for loops ---
   
   BookIterator begin() const
   {
      return BookIterator{head_.get()};
   }

   BookIterator end() const
   {
      return BookIterator{nullptr}; // The "done" state is a null pointer
   }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== ITERATOR PATTERN (IDIOMATIC C++ / STL) ===\n" << std::endl;

   BookCollection bookCollection;
   bookCollection.addBook("The C++ Programming Language", "Bjarne Stroustrup");
   bookCollection.addBook("Design Patterns", "Gang of Four");
   bookCollection.addBook("Clean Code", "Robert C. Martin");
   bookCollection.addBook("The Pragmatic Programmer", "Robert C. Martin");
   bookCollection.addBook("Effective C++", "Scott Meyers");
   bookCollection.addBook("Clean Agile: Back to Basics", "Robert C. Martin");
   bookCollection.addBook("Effective Modern C++", "Scott Meyers");

   // --- Test 1: Standard Traversal (The modern way) ---
   std::cout << "--- BookCollection Inventory (Range-based for loop) ---\n";
   
   // This single line replaces: for(iterator->first(); !iterator->isDone(); iterator->next())
   //                            {
   //                               Book book = iterator->currentItem();
   for(const auto& book : bookCollection)
      std::cout << " - " << book.getTitle() << " (by " << book.getAuthor() << ")\n";

   // --- Test 2: Multiple Traversals (Finding duplicates) ---
   // We will use two independent iterators to find books by the same author.
   std::cout << "\n--- Finding multiple books by the same author ---\n";

   std::unordered_set<std::string> processedAuthors;

   // We can still use iterators manually if we need fine-grained control
   for(auto outerIt = bookCollection.begin(); outerIt != bookCollection.end(); ++outerIt)
   {
      const Book& currentBook = *outerIt;

      if(!processedAuthors.contains(currentBook.getAuthor()))
      {
         std::vector<std::string> bookTitlesOfSameAuthor;

         processedAuthors.insert(currentBook.getAuthor());
         bookTitlesOfSameAuthor.push_back(currentBook.getTitle());
      
         // Start inner iterator one step ahead of the outer iterator
         auto innerIt = outerIt;
         ++innerIt;

         while(innerIt != bookCollection.end())
         {
            const Book& compareBook = *innerIt;
            if(currentBook.getAuthor() == compareBook.getAuthor())
               bookTitlesOfSameAuthor.push_back(compareBook.getTitle());
            ++innerIt;
         }

         if(bookTitlesOfSameAuthor.size() > 1)
         {
            std::cout << "Several books found by " << currentBook.getAuthor() << ":\n";
            for(const auto& title : bookTitlesOfSameAuthor)
               std::cout << " - " << title << std::endl;
            std::cout << std::endl;
         }
      } // if
   } // for

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
