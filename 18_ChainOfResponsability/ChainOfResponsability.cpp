/**
 * ============================================================================
 * File: ChainOfResponsability.cpp
 * 
 * --- DESIGN OVERVIEW:
 * This program demonstrates the Chain of Responsibility pattern. Each handler 
 * in the chain decides whether to process a request or pass it to the next 
 * successor.
 * 
 * --- CHAIN LINKING:
 * We construct the chain using std::unique_ptr, where each handler 'owns' 
 * the next one. When the head of the chain is destroyed, the entire chain 
 * is cleaned up automatically.
 * 
 * --- BUILDER APPROACH:
 * The Chain class acts as a builder to construct the chain fluently. 
 * Handlers are linked dynamically, allowing the chain to be assembled 
 * in stages or at runtime while maintaining the correct priority order.
 * 
 * --- CHAIN LOGIC:
 * 1. Each 'Handler' knows about the 'next' handler.
 * 2. If a handler cannot process the request, it delegates to 'next'.
 * 3. The 'Chain' builder simplifies the construction process, maintaining 
 *    the order of priority (first added, first handled).
 * ============================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>

//--------------------------------------------------------- Base Handler:
class IHandler
{
protected:
   std::unique_ptr<IHandler> next_;

public:
   virtual ~IHandler() = default;

   void setNext(std::unique_ptr<IHandler> next)
   {
      next_ = std::move(next);
   }

   virtual void handle(int request)
   {
      if (next_)
      {
         next_->handle(request);
      }
      else
      {
         std::cout << " [System] Request " << request 
                   << " reached the end of the chain unhandled.\n";
      }
   }
};

//--------------------------------------------------------- Concrete Handler:
class Handler : public IHandler
{
private:
   int id_;
   std::string name_;

public:
   Handler(int id, std::string name) : id_{id}, name_{std::move(name)} { }

   void handle(int request) override
   {
      if (request == id_)
      {
         std::cout << " [Handler] " << name_ << " handled request: " << request << "\n";
      }
      else
      {
         std::cout << " [Handler] " << name_ << " passing request " << request << " forward.\n";
         IHandler::handle(request);
      }
   }
};

//--------------------------------------------------------- Chain Builder:
class Chain
{
private:
   std::unique_ptr<IHandler> head_;
   IHandler* tail_{nullptr};

public:
   Chain& add(std::unique_ptr<IHandler> handler)
   {
      IHandler* current = handler.get();
      
      if (!head_) head_ = std::move(handler);
      else        tail_->setNext(std::move(handler));
      
      tail_ = current;
      return *this;
   }

   void execute(int request) const
   {
      if(head_) head_->handle(request);
      else      std::cout << "Error: There are not chain to handle " << request << std::endl;
   }
};

//--------------------------------------------------------- Main Simulation:
int main()
{
   std::cout << "=== CHAIN OF RESPONSIBILITY (CHAINBUILDER) ===\n" << std::endl;

   Chain chain;

   // 1. First stage of construction
   chain.add(std::make_unique<Handler>(3, "Handler-3"))
        .add(std::make_unique<Handler>(5, "Handler-5"));

   // 2. Second stage of construction
   chain.add(std::make_unique<Handler>(8, "Handler-8"))
        .add(std::make_unique<Handler>(11, "Handler-11"));

   // Execute tests
   int requests[] = {3, 5, 4, 8, 11};
   for (int r : requests)
   {
      std::cout << "Testing request " << r << ":\n";
      chain.execute(r);
      std::cout << std::endl;
   }

   std::cout << "=== SIMULATION COMPLETED ===" << std::endl;
}

//================================================================================ END
