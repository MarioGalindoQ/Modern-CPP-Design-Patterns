# Visitor Pattern (Modern Variant - Extending Behaviors)

```mermaid
classDiagram
    class Paragraph {
        +string text_
    }
    class Image {
        +string url_
    }
    class Hyperlink {
        +string url_
        +string label_
    }
    class DocumentElement {
    }
    class HtmlExporter {
        +visit()
    }
    class TextExtractor {
        +visit()
    }
    class Client {
        +main()
    }

    DocumentElement ..> Paragraph
    DocumentElement ..> Image
    DocumentElement ..> Hyperlink

    HtmlExporter ..> Paragraph
    HtmlExporter ..> Image
    HtmlExporter ..> Hyperlink

    Client *-- "n" DocumentElement
    Client ..> HtmlExporter
    Client ..> TextExtractor
```

### Design Note:
In this modern C++ version, the Visitor pattern is completely non-intrusive. The
element classes (Paragraph, Image, Hyperlink) are treated as pure data and do
not need a base class. The 'DocumentElement' is a type-safe union
(std::variant). New operations like 'HtmlExporter' or 'TextExtractor' are
separate objects. The 'Client' processes the entire collection using
'std::visit', providing compile-time safety and high performance by avoiding
virtual function overhead.

**Author:** Mario Galindo Queralt, Ph.D.
