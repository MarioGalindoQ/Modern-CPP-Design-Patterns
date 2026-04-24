# Visitor Pattern (Traditional GoF - Extending Behaviors)

```mermaid
classDiagram
   class Visitable {
      <<interface>>
      +accept(Visitor&)*
   }

   class Paragraph {
      -string text_
      +accept(Visitor&)
      +getText() string
   }

   class Image {
      -string url_
      +accept(Visitor&)
      +getUrl() string
   }

   class Hyperlink {
      -string url_
      -string label_
      +accept(Visitor&)
      +getUrl() string
      +getLabel() string
   }

   class Visitor {
      <<interface>>
      +visit(Paragraph&)*
      +visit(Image&)*
      +visit(Hyperlink&)*
   }

   class HtmlExport {
      +visit(Paragraph&)
      +visit(Image&)
      +visit(Hyperlink&)
   }

   class TextExtract {
      +visit(Paragraph&)
      +visit(Image&)
      +visit(Hyperlink&)
   }

   class Client {
      +main()
   }

   %% Inheritance (Is_a)
   Visitable <|.. Paragraph
   Visitable <|.. Image
   Visitable <|.. Hyperlink

   Visitor <|.. HtmlExport
   Visitor <|.. TextExtract

   %% Composition (Has_a)
   Client *-- "n" Visitable : document collection

   %% Double Dispatch Handshake
   Paragraph ..> Visitor : calls visit()
   Image ..> Visitor : calls visit()
   Hyperlink ..> Visitor : calls visit()

   %% Client dependencies
   Client ..> Visitor : executes operation
```

### Design Note:
In this "Extending Behaviors" scenario, the object structure (Paragraph, Image,
Hyperlink) is considered stable. The power of the GoF Visitor lies in its
ability to add completely new operations, like 'HtmlExport' or 'TextExtract',
without changing the code of the data elements. The 'accept' method provides the
entry point for the visitor to perform its specific logic on each concrete type.
