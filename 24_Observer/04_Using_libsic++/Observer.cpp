// ******************** Observer Pattern ********************

// This implementation supports multiple Subjects for each
// Observer. The Subject passed to the Update operation allows
// the Observer interact with the Subject that has changed.

#include "Objects.h"

int main()
{
   Number number_1, number_2;

   DivObserver observer_1{4}; // Create Observers
   DivObserver observer_2{3};
   ModObserver observer_3{3};

   sigc::connection con_1 = number_1.attach(observer_1); // Attach Observers
   sigc::connection con_2 = number_1.attach(observer_2);
   sigc::connection con_3 = number_1.attach(observer_3);

   number_2.attach(observer_1);
   number_2.attach(observer_2);
   number_2.attach(observer_3);

   std::cout << "With three observers:\n";
   number_1.setVal(14);
   number_2.setVal(18);

   std::cout << "\nWith two observers:\n";
   con_2.disconnect();
   number_1.setVal(14);

   std::cout << "\nWith one observer:\n";
   con_1.disconnect();
   con_3.disconnect();
   number_1.attach(observer_2);
   number_1.setVal(14);
}
