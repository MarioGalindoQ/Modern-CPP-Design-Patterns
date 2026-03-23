/**
 * ============================================================================
 * File: Visitor_Modern.cpp (std::variant Version)
 * 
 * --- DESIGN OVERVIEW:
 * This implementation uses C++17 'std::variant' and 'std::visit' to replace 
 * traditional virtual inheritance with type-safe unions.
 * 
 * THE MODERN ADVANTAGE:
 * 1. Non-Intrusive: The element classes (Paragraph, etc.) don't need 'accept()'.
 * 2. Static Safety: 'std::visit' forces you to handle all types in the variant.
 * 3. Zero Inheritance: We don't need a base Visitable class, reducing 
 *    the complexity of the object structure.
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <variant>

//--------------------------------------------------------- Plain Data Structs:
// No inheritance, no virtual methods. Pure data elements.
struct Paragraph
{
   std::string text_;
   explicit Paragraph(std::string text) : text_{std::move(text)} {}
};

struct Image
{
   std::string url_;
   explicit Image(std::string url) : url_{std::move(url)} {}
};

struct Hyperlink
{
   std::string url_;
   std::string label_;
   Hyperlink(std::string url, std::string label) 
      : url_{std::move(url)}, label_{std::move(label)} {}
};

// Define the DocumentElement as a variant of all possible types
using DocumentElement = std::variant<Paragraph, Image, Hyperlink>;

//--------------------------------------------------------- Modern Visitors:
// A Visitor in C++17 is simply a struct/class with overloaded operator()
struct HtmlExporter
{
   void operator()(const Paragraph& p) const { std::cout << "<p>" << p.text_ << "</p>\n"; }
   void operator()(const Image& i)     const { std::cout << "<img src=\"" << i.url_ << "\" />\n"; }
   void operator()(const Hyperlink& h) const { std::cout << "<a href=\"" << h.url_ << "\">" << h.label_ << "</a>\n"; }
};

struct TextExtractor
{
   void operator()(const Paragraph& p) const { std::cout << "Text: " << p.text_ << "\n"; }
   void operator()(const Image& i)     const { std::cout << "Image found at: " << i.url_ << "\n"; }
   void operator()(const Hyperlink& h) const { std::cout << "Link: " << h.label_ << " [" << h.url_ << "]\n"; }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== VISITOR PATTERN (MODERN VARIANT) ===\n" << std::endl;

   // The document is now a vector of variants
   std::vector<DocumentElement> document;
   document.push_back(Paragraph("Hello World!"));
   document.push_back(Image("logo.png"));
   document.push_back(Hyperlink("https://cpp.org", "C++ Site"));

   // 1. HTML Export
   std::cout << "--- HTML Export:\n";
   for (const auto& element : document)
   {
      std::visit(HtmlExporter{}, element);
   }

   // 2. Text Extraction
   std::cout << "\n--- Text Extraction:\n";
   for (const auto& element : document)
   {
      std::visit(TextExtractor{}, element);
   }

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}

//================================================================================ END
