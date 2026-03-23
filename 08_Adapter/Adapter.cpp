/**
 * ============================================================================
 * File: Adapter.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the Adapter Pattern. It allows a modern 
 * interface that expects coordinates (x1, y1, x2, y2) to work with a legacy 
 * class that expects (x, y, width, height).
 * 
 * The RectangleAdapter "translates" the point-based coordinates into 
 * dimension-based values required by the Legacy library.
 * ============================================================================
 */

#include <iostream>
#include <memory>

//--------------------------------------------------------- Legacy (Adaptee):
// This is the old library we cannot modify.
class LegacyRectangle
{
public:
   // Added 'const' to allow calls from a const Adapter.
   void oldDraw(int x, int y, int w, int h) const
   {
      std::cout << " [Legacy Library] Drawing rectangle..." << std::endl;
      std::cout << "  -> Origin: (" << x << ", " << y << ")" << std::endl;
      std::cout << "  -> Dimensions: " << w << "x" << h << std::endl;
   }
};

//------------------------------------------------- Modern Interface (Target):
// This is the interface our modern system understands.
class ModernRectangle
{
public:
   virtual ~ModernRectangle() = default;
   
   // Marked as 'const' to be compatible with const references.
   virtual void draw(int x1, int y1, int x2, int y2) const = 0;
};

//--------------------------------------------------------- The Adapter:
// The Adapter translates Modern calls into Legacy calls.
class RectangleAdapter : public ModernRectangle
{
private:
   // This is created automatically when RectangleAdapter is instantiated.
   LegacyRectangle legacyInstance_;

public:
   // Implementation of the modern interface.
   void draw(int x1, int y1, int x2, int y2) const override
   {
      std::cout << " [Adapter] Converting coordinates to dimensions..." << std::endl;
      
      int width  = x2 - x1;
      int height = y2 - y1;

      // Delegate the call to the internal legacy instance.
      legacyInstance_.oldDraw(x1, y1, width, height);
   }
};

//--------------------------------------------------------- Main Engine:
// This represents the client that only knows about the modern interface.
void render(const ModernRectangle& rect)
{
   // Now this works because 'draw' is a const method.
   rect.draw(10, 10, 50, 30);
}

int main()
{
   std::cout << "=== ADAPTER PATTERN SIMULATION ===\n" << std::endl;

   // 1. We instantiate the Adapter.
   // 2. This automatically instantiates the private 'legacyInstance_' inside it.
   std::unique_ptr<ModernRectangle> myRectangle = std::make_unique<RectangleAdapter>();

   // The system calls 'render' which expects a ModernRectangle reference.
   render(*myRectangle);

   std::cout << "\n=== SIMULATION COMPLETED ===" << std::endl;

   return 0;
}

//================================================================================ END
