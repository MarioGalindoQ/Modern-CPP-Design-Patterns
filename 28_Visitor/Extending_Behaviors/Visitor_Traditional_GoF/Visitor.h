// File: Visitor.h

#ifndef VISITOR_H
#define VISITOR_H

class Paragraph;
class Image;
class Hyperlink;

class Visitor
{
public:
   virtual ~Visitor() = default;
   virtual void visit(Paragraph& p) = 0;
   virtual void visit(Image& i) = 0;
   virtual void visit(Hyperlink& h) = 0;
};

class Visitable
{
public:
   virtual ~Visitable() = default;
   virtual void accept(Visitor& v) = 0;
};

#endif
//================================================================================ END
