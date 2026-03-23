/**
 * ============================================================================
 * File: Iterator.cpp (Classic GoF Version)
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the classic Object-Oriented Iterator pattern.
 * We build a custom collection (a Singly Linked List of Books). The client 
 * interacts only with the 'Iterator' interface (first, next, isDone, current) 
 * without knowing how the Books are stored in memory.
 * 
 * --- MULTIPLE TRAVERSALS:
 * Because iterators are separate objects (not a cursor inside the collection), 
 * we can instantiate multiple iterators to traverse the same collection 
 * simultaneously at different speeds or starting points.
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <memory>

//--------------------------------------------------------- Domain Object:
class Book
{
private:
   std::string title_;
   std::string author_;

public:
   Book(std::string title, std::string author) 
      : title_{std::move(title)}, author_{std::move(author)} { }

   std::string getTitle()  const { return title_; }
   std::string getAuthor() const { return author_; }
};

//--------------------------------------------------------- Iterator Interface:
template <typename T>
class Iterator
{
public:
   virtual ~Iterator() = default;
   virtual void first() = 0;
   virtual void next() = 0;
   virtual bool isDone() const = 0;
   virtual T currentItem() const = 0;
};

//--------------------------------------------------------- Aggregate Interface:
template <typename T>
class Aggregate
{
public:
   virtual ~Aggregate() = default;
   // Factory Method to create the appropriate iterator
   virtual std::unique_ptr<Iterator<T>> createIterator() const = 0;
};

//--------------------------------------------------------- Internal Node:
// This is hidden from the client. The client only sees 'Book'.
struct Node
{
   Book data;
   std::unique_ptr<Node> next;

   explicit Node(Book b) : data{std::move(b)}, next{nullptr} { }
};

//--------------------------------------------------------- Concrete Aggregate:
class BookCollection : public Aggregate<Book>
{
private:
   std::unique_ptr<Node> head_;
   Node* tail_{nullptr}; // Raw pointer for O(1) appending

public:
   void addBook(const std::string& title, const std::string& author)
   {
      auto newNode = std::make_unique<Node>(Book{title, author});
      Node* current = newNode.get();

      if (!head_) head_ = std::move(newNode);
      else        tail_->next = std::move(newNode);
      tail_ = current;
   }

   // The collection must provide access to its internal head for the iterator
   const Node* getHead() const { return head_.get(); }

   std::unique_ptr<Iterator<Book>> createIterator() const override;
};

//--------------------------------------------------------- Concrete Iterator:
class BookIterator : public Iterator<Book>
{
private:
   const BookCollection& collection_;
   const Node* current_;

public:
   explicit BookIterator(const BookCollection& collection) 
      : collection_{collection}, current_{collection.getHead()} { }

   void first() override
   {
      current_ = collection_.getHead();
   }

   void next() override
   {
      if (current_) current_ = current_->next.get();
   }

   bool isDone() const override
   {
      return current_ == nullptr;
   }

   Book currentItem() const override
   {
      if (isDone()) throw std::out_of_range("Iterator is out of bounds");
      return current_->data;
   }
};

// Now we can define the factory method since BookIterator is known
std::unique_ptr<Iterator<Book>> BookCollection::createIterator() const
{
   return std::make_unique<BookIterator>(*this);
}

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== ITERATOR PATTERN (CLASSIC GOF) ===\n" << std::endl;

   BookCollection library;
   library.addBook("Design Patterns", "Gang of Four");
   library.addBook("Clean Code", "Robert C. Martin");
   library.addBook("The Pragmatic Programmer", "Robert C. Martin");
   library.addBook("Effective C++", "Andy Hunt");

   // --- Test 1: Standard Traversal ---
   std::cout << "--- Library Inventory ---\n";
   auto it = library.createIterator();
   
   for (it->first(); !it->isDone(); it->next())
   {
      Book b = it->currentItem();
      std::cout << " - " << b.getTitle() << " (by " << b.getAuthor() << ")\n";
   }

   // --- Test 2: Multiple Simultaneous Traversals ---
   // We will use two independent iterators to find books by the same author.
   std::cout << "\n--- Finding multiple books by the same author ---\n";
   
   auto outerIt = library.createIterator();
   auto innerIt = library.createIterator();

   for (outerIt->first(); !outerIt->isDone(); outerIt->next())
   {
      Book currentBook = outerIt->currentItem();
      
      // Start the inner iterator ONE step ahead of the outer iterator
      innerIt->first();
      while (!innerIt->isDone() && innerIt->currentItem().getTitle() != currentBook.getTitle())
      {
         innerIt->next();
      }
      if (!innerIt->isDone()) innerIt->next(); // Move past the current book

      // Now search the rest of the collection
      while (!innerIt->isDone())
      {
         Book compareBook = innerIt->currentItem();
         if (currentBook.getAuthor() == compareBook.getAuthor())
         {
            std::cout << " Match found: '" << currentBook.getTitle() 
                      << "' and '" << compareBook.getTitle() 
                      << "' are both by " << currentBook.getAuthor() << ".\n";
         }
         innerIt->next();
      }
   }

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
