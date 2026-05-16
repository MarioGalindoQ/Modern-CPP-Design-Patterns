# Iterator Pattern (GoF Version)

```mermaid
classDiagram
    class Book {
        -string title_
        -string author_
        +getTitle() string
        +getAuthor() string
    }

    class Iterator~Book~ {
        <<interface>>
        +first()*
        +next()*
        +isDone()* bool
        +currentItem()* Book
        +clone()* unique_ptr~Iterator~
    }

    class Node {
        +Book book
        +unique_ptr~Node~ next
    }

    class BookCollection {
        -unique_ptr~Node~ head_
        -Node* tail_
        +addBook(string title, string author)
        +createIterator() unique_ptr~BookIterator~
    }

    class BookIterator {
        -BookCollection& bookCollection_
        -Node* currentNode_
        +first()
        +next()
        +isDone() bool
        +currentItem() Book
        +clone() unique_ptr~BookIterator~
    }

    class Client {
        +main()
    }

    note for Client "for(iterator->first(); !iterator->isDone(); iterator->next()) {...}"

    Iterator~Book~ <|.. BookIterator

    BookCollection *-- Node : head_
    BookCollection ..> Node : tail_
    Node *-- Book : book
    Node *-- Node : next

    BookIterator ..> BookCollection : bookCollection_
    BookIterator ..> Node : currentNode_
    
    Client ..> BookCollection
    Client ..> Iterator
```

### Design Note:
This diagram illustrates the classic decoupled approach. The 'BookCollection'
(Aggregate) is responsible for storing the data in a linked-list of 'Nodes',
while the 'BookIterator' manages the traversal state. By providing a 'clone()'
method, we allow multiple independent iterators to exist and duplicate their
positions efficiently, as shown in the author's search simulation.

**Author:** Mario Galindo Queralt, Ph.D.
