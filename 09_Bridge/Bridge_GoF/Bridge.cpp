/**
 * ============================================================================
 * File: Bridge.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the Bridge Pattern by decoupling a "Shape" 
 * (the Abstraction) from its "Renderer" (the Implementation).
 * 
 * Instead of creating VectorCircle, RasterCircle, VectorSquare, etc., 
 * we create two independent hierarchies and connect them via a pointer 
 * (the Bridge) inside the Shape class.
 * ============================================================================
 */

#include <iostream>
#include <memory>
#include <string>

//------------------------------------------------- Implementer (Interface):
// This defines how a shape is actually drawn on a specific platform.
class Renderer
{
public:
   virtual ~Renderer() = default;
   virtual void renderCircle(float radius) const = 0;
   virtual void renderSquare(float side) const = 0;
};

//----------------------------------------------- Concrete Implementer A:
class VectorRenderer : public Renderer
{
public:
   void renderCircle(float radius) const override
   {
      std::cout << " [Vector] Drawing a circle of radius " << radius << std::endl;
   }

   void renderSquare(float side) const override
   {
      std::cout << " [Vector] Drawing a square of side " << side << std::endl;
   }
};

//----------------------------------------------- Concrete Implementer B:
class RasterRenderer : public Renderer
{
public:
   void renderCircle(float radius) const override
   {
      std::cout << " [Raster] Drawing pixels for a circle of radius " << radius << std::endl;
   }

   void renderSquare(float side) const override
   {
      std::cout << " [Raster] Drawing pixels for a square of side " << side << std::endl;
   }
};

//------------------------------------------------- Abstraction (Base):
// This defines the high-level logic. It holds the "Bridge" to a Renderer.
class Shape
{
protected:
   // The Bridge: a pointer to the implementation
   std::unique_ptr<Renderer> rendererBridge_;

public:
   explicit Shape(std::unique_ptr<Renderer> renderer) 
      : rendererBridge_{std::move(renderer)} { }

   virtual ~Shape() = default;
   virtual void draw() const = 0;
   virtual void resize(float factor) = 0;
};

//------------------------------------------------- Refined Abstraction 1:
class Circle : public Shape
{
private:
   float radius_;

public:
   Circle(std::unique_ptr<Renderer> renderer, float radius)
      : Shape{std::move(renderer)}, radius_{radius} { }

   void draw() const override
   {
      rendererBridge_->renderCircle(radius_);
   }

   void resize(float factor) override
   {
      radius_ *= factor;
   }
};

//------------------------------------------------- Refined Abstraction 2:
class Square : public Shape
{
private:
   float side_;

public:
   Square(std::unique_ptr<Renderer> renderer, float side)
      : Shape{std::move(renderer)}, side_{side} { }

   void draw() const override
   {
      rendererBridge_->renderSquare(side_);
   }

   void resize(float factor) override
   {
      side_ *= factor;
   }
};

//--------------------------------------------------------- Main Engine:
int main()
{
   std::cout << "=== BRIDGE PATTERN SIMULATION ===\n" << std::endl;

   // 1. Create a Vector Circle
   std::unique_ptr<Shape> circle = std::make_unique<Circle>(
      std::make_unique<VectorRenderer>(), 5.0f
   );

   // 2. Create a Raster Square
   std::unique_ptr<Shape> square = std::make_unique<Square>(
      std::make_unique<RasterRenderer>(), 10.0f
   );

   circle->draw();
   square->draw();

   std::cout << "\n --- Resizing Shapes ---" << std::endl;
   circle->resize(2.0f);
   square->resize(0.5f);

   circle->draw();
   square->draw();

   std::cout << "\n=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
