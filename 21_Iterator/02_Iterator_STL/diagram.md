# Iterator Pattern (Modern STL Version)

```mermaid
classDiagram
    class Book {
        -string title_
        -string author_
        +getTitle() string
        +getAuthor() string
    }

    class Node {
        +Book book
        +unique_ptr~Node~ next
    }

    class BookIterator {
        -Node* current_
        +operator*() Book&
        +operator++() BookIterator&
        +operator!=(BookIterator&) bool
    }

    class BookCollection {
        -unique_ptr~Node~ head_
        -Node* tail_
        +addBook(string, string)
        +begin() BookIterator
        +end() BookIterator=nullptr
    }

    class Client {
        +main()
    }

    note for Client "for(const auto& book : bookCollection) {...}"

    %% Relationships using your symbology reference:

    BookCollection *-- Node : head_
    BookCollection ..> Node : tail_
    Node *-- Book : book
    Node *-- Node : next

    BookIterator ..> Node : currentNode_

    Client ..> BookCollection
    Client ..> BookIterator
```

### Design Note:
In the Modern C++ (STL) version, the Iterator pattern is implemented without
inheritance. The 'BookCollection' provides 'begin()' and 'end()' methods that
return 'BookIterator' instances. The C++ compiler uses these methods and the
overloaded operators (*, ++, !=) to enable the 'Range-based for loop'
syntax. This approach follows the Zero-Overhead Principle, as all calls are
resolved at compile-time without virtual table lookups.

**Author:** Mario Galindo Queralt, Ph.D.
