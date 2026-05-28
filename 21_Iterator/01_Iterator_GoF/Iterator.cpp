/**
 * ============================================================================
 * File: Iterator.cpp (Classic GoF Version)
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the classic Object-Oriented Iterator pattern.
 * We build a custom collection (a Singly Linked List of Books). The client 
 * interacts only with the 'Iterator' interface (first, next, isDone, current) 
 * without knowing how the Books are stored in memory.
 * 
 * ============================================================================
 */
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
 *
 * --- CLONING CAPABILITY:
 * To match the efficiency of modern C++ iterators, this implementation 
 * includes a 'clone()' method. This allows creating a new iterator at 
 * the exact same position as an existing one, facilitating efficient 
 * nested traversals.
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

   Book(std::string title, std::string author) // 7 PC: Parametric Constructor
      : title_{std::move(title)}, author_{std::move(author)} { }

   std::string getTitle()  const { return title_; }
   std::string getAuthor() const { return author_; }
};

//--------------------------------------------------------- Iterator Interface:
template <typename T>
class Iterator
{
public:
   virtual ~Iterator()                  = default;
   virtual void first()                 = 0;
   virtual void next()                  = 0;
   virtual bool isDone()          const = 0;
   virtual const T& currentBook() const = 0;

   // Prototype-like method to duplicate the iterator state
   virtual std::unique_ptr<Iterator<T>> clone() const = 0;
};

//--------------------------------------------------------- Internal Node:
class Node
{
public:
   Book book;
   std::unique_ptr<Node> next{nullptr};

   explicit Node(Book book) : book{std::move(book)} { }
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
      Node* currentNode = newNode.get();

      if(!head_) head_ = std::move(newNode);
      else       tail_->next = std::move(newNode);
      
      tail_ = currentNode;
   }

   const Node* getHead() const { return head_.get(); }

   std::unique_ptr<Iterator<Book>> createIterator() const;
};

//--------------------------------------------------------- Concrete Iterator:
class BookIterator : public Iterator<Book>
{
private:
   const BookCollection& bookCollection_;
   const Node* currentNode_;

public:
   explicit BookIterator(const BookCollection& bookCollection) 
      : bookCollection_{bookCollection}, currentNode_{bookCollection.getHead()} { }

   // Internal constructor for cloning
   BookIterator(const BookCollection& col, const Node* curr) 
      : bookCollection_{col}, currentNode_{curr} { }

   void first() override { currentNode_ = bookCollection_.getHead(); }

   void next() override
   {
      if(currentNode_) currentNode_ = currentNode_->next.get();
   }

   bool isDone() const override { return currentNode_ == nullptr; }

   const Book& currentBook() const override
   {
      if(isDone()) throw std::out_of_range("Iterator is out of bounds");
      return currentNode_->book;
   }

   std::unique_ptr<Iterator<Book>> clone() const override
   {
      return std::make_unique<BookIterator>(bookCollection_, currentNode_);
   }
};

std::unique_ptr<Iterator<Book>> BookCollection::createIterator() const
{
   return std::make_unique<BookIterator>(*this);
}

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== ITERATOR PATTERN (CLASSIC GOF) ===\n" << std::endl;

   BookCollection bookCollection;
   bookCollection.addBook("The C++ Programming Language", "Bjarne Stroustrup");
   bookCollection.addBook("Design Patterns", "Gang of Four");
   bookCollection.addBook("Clean Code", "Robert C. Martin");
   bookCollection.addBook("The Pragmatic Programmer", "Robert C. Martin");
   bookCollection.addBook("Effective C++", "Scott Meyers");
   bookCollection.addBook("Clean Agile: Back to Basics", "Robert C. Martin");
   bookCollection.addBook("Effective Modern C++", "Scott Meyers");

   // --- Test 1: Standard Traversal ---
   std::cout << "--- bookCollection Inventory ---\n";
   auto iterator = bookCollection.createIterator();
   
   for(iterator->first(); !iterator->isDone(); iterator->next())
   {
      const Book& book = iterator->currentBook();
      std::cout << " - " << book.getTitle() << " (by " << book.getAuthor() << ")\n";
   }

   // --- Test 2: Multiple Traversals (Finding duplicates) ---
   // We will use two independent iterators to find books by the same author.
   std::cout << "\n--- Finding multiple books by the same author ---\n";

   std::unordered_set<std::string> processedAuthors;
   auto outerIt = bookCollection.createIterator();

   for(outerIt->first(); !outerIt->isDone(); outerIt->next())
   {
      const Book& currentBook = outerIt->currentBook();

      if(!processedAuthors.contains(currentBook.getAuthor()))
      {
         std::vector<std::string> bookTitlesOfSameAuthor;

         processedAuthors.insert(currentBook.getAuthor());
         bookTitlesOfSameAuthor.push_back(currentBook.getTitle());

         // Use cloning to start the inner iterator at the same position
         auto innerIt = outerIt->clone();
         innerIt->next(); // Move one step ahead

         while(!innerIt->isDone())
         {
            const Book& compareBook = innerIt->currentBook();
            if(currentBook.getAuthor() == compareBook.getAuthor())
               bookTitlesOfSameAuthor.push_back(compareBook.getTitle());
            innerIt->next();
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
