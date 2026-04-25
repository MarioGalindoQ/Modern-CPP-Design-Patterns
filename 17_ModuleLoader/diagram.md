# Module Loader / Plugin Pattern

```mermaid
classDiagram
   class Client {
      +main()
   }

   class IModule {
      <<interface>>
      +int* processData(int)
   }

   class Module {
      -string name_
      -int factor_
      +Module(const char* name, const int factor)
      +processData(int) int
   }

   class DynamicLibrary {
      -void* libraryHandle_
      +DynamicLibrary(const char* filename)
      +void* getSymbol(const char*)
   }

   class `extern "C"` {
      +IModule* build_module(char* name, int factor)
      +void destroy_module(IModule* module_ptr)
   }

   %% Inheritance (Implements)
   IModule <|.. Module

   %% The Client owns the RAII wrapper for the library
   Client *-- DynamicLibrary : manages library

   %% The Client owns the module instance via unique_ptr
   Client *-- IModule : uses plugin

   %% The Client depends on the factory functions exported by the .so
   Client --> IModule : creates via build_module

   %%Module is managed via extern "C" funcions
   Module .. `extern "C"`
```

### Design Note:
This diagram illustrates the separation between the 'Host' (Client and
DynamicLibrary) and the 'Plugin' (Module). The Client only has a compile-time
dependency on the 'IModule' interface. The concrete 'Module' class is loaded at
runtime from the Shared Object (.so), and its lifecycle is managed through a
smart pointer with a custom deleter provided by the library's symbols.

**Author:** Mario Galindo Queralt, Ph.D.
