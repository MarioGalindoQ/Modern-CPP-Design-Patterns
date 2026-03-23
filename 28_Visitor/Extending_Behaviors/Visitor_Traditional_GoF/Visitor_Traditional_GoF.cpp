/**
 * ============================================================================
 * File: Visitor_Traditional_GoF.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This implementation demonstrates the classic GoF Visitor pattern.
 * We have a Document structure (Paragraphs, Images, Hyperlinks) and we
 * perform different operations on it (HTML Export, Text Extraction) without
 * modifying the element classes themselves.
 * ============================================================================
 */

#include "Visitor.h"
#include <iostream>
#include <vector>
#include <memory>
#include <string>

//--------------------------------------------------------- Concrete Visitables:
class Paragraph : public Visitable
{
private:
   std::string text_;

public:
   explicit Paragraph(std::string text) : text_{std::move(text)} {}
   void accept(Visitor& v) override { v.visit(*this); }
   std::string getText() const { return text_; }
};

class Image : public Visitable
{
private:
   std::string url_;

public:
   explicit Image(std::string url) : url_{std::move(url)} {}
   void accept(Visitor& v) override { v.visit(*this); }
   std::string getUrl() const { return url_; }
};

class Hyperlink : public Visitable
{
private:
   std::string url_;
   std::string label_;

public:
   Hyperlink(std::string url, std::string label) 
      : url_{std::move(url)}, label_{std::move(label)} {}
   void accept(Visitor& v) override { v.visit(*this); }
   std::string getUrl() const { return url_; }
   std::string getLabel() const { return label_; }
};

//--------------------------------------------------------- Concrete Visitors:
class HtmlExport : public Visitor
{
public:
   void visit(Paragraph& p) override
   {
      std::cout << "<p>" << p.getText() << "</p>\n";
   }

   void visit(Image& i) override
   {
      std::cout << "<img src=\"" << i.getUrl() << "\" />\n";
   }

   void visit(Hyperlink& h) override
   {
      std::cout << "<a href=\"" << h.getUrl() << "\">" << h.getLabel() << "</a>\n";
   }
};

class TextExtract : public Visitor
{
public:
   void visit(Paragraph& p) override
   {
      std::cout << "Text: " << p.getText() << "\n";
   }

   void visit(Image& i) override
   {
      std::cout << "Image found at: " << i.getUrl() << "\n";
   }

   void visit(Hyperlink& h) override
   {
      std::cout << "Link: " << h.getLabel() << " [" << h.getUrl() << "]\n";
   }
};

//--------------------------------------------------------- Main:
int main()
{
   std::cout << "=== VISITOR PATTERN (TRADITIONAL GOF) ===\n" << std::endl;

   std::vector<std::unique_ptr<Visitable>> document;
   document.push_back(std::make_unique<Paragraph>("Hello World!"));
   document.push_back(std::make_unique<Image>("logo.png"));
   document.push_back(std::make_unique<Hyperlink>("https://cpp.org", "C++ Site"));

   HtmlExport htmlExporter;
   TextExtract textExtractor;

   std::cout << "--- HTML Export:\n";
   for (auto& element : document)
   {
      element->accept(htmlExporter);
   }

   std::cout << "\n--- Text Extraction:\n";
   for (auto& element : document)
   {
      element->accept(textExtractor);
   }

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
   return 0;
}

//================================================================================ END
