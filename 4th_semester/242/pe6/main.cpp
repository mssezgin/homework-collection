#include <iostream>
#include "person.h"
#include "corporation.h"
#include "villa.h"
#include "apartment.h"
#include "office.h"

using namespace std;

int main(int argc, char const *argv[]) {

    /*
    Person per = Person("Ahmet", 5000, 10);
    Corporation corp = Corporation("ACME", 5000, "cankaya");

    Villa est1 = Villa("Villa 1", 150, &per, 2, false);
    Apartment est2 = Apartment("Apartment 1", 200, &corp, 7, 1);
    Office est3 = Office("Apartment 2", 200, NULL, 5, 0);

    per.print_info();
    est1.print_info();

    cout << "----------------------------\n";
    per.list_properties();
    corp.list_properties();
    cout << "----------------------------\n";
    per.buy(&est2, &corp);
    per.sell(&est1, &corp);
    cout << "----------------------------\n";
    per.list_properties();
    corp.list_properties();
    cout << "----------------------------\n";
    */

    /*
    Person per = Person("Ahmet", 5000, 10);
    Corporation corp = Corporation("ACME", 5000, "cankaya");

    per.print_info();
    per.list_properties();
    corp.print_info();
    corp.list_properties();
    cout << "----------------------------\n";

    Villa est1 = Villa("Villa 1", 150, &per, 2, false);
    Apartment est2 = Apartment("Apartment 1", 200, &corp, 7, 1);
    Office est3 = Office("Apartment 2", 200, NULL, 5, 0);

    est1.print_info();
    est2.print_info();
    est3.print_info();
    cout << "----------------------------\n";
    cout << "----------------------------\n";

    per.list_properties();
    corp.list_properties();
    cout << "--------\n";
    est1.print_info();
    est2.print_info();
    est3.print_info();
    cout << "----------------------------\n";

    per.buy(&est2, &corp);
    cout << "----------------------------\n";
    
    per.list_properties();
    corp.list_properties();
    cout << "--------\n";
    est1.print_info();
    est2.print_info();
    est3.print_info();
    cout << "----------------------------\n";
    
    per.sell(&est1, &corp);
    cout << "----------------------------\n";

    per.list_properties();
    corp.list_properties();
    cout << "--------\n";
    est1.print_info();
    est2.print_info();
    est3.print_info();
    cout << "----------------------------\n";
    cout << "----------------------------\n";
    
    Office est4("First Office", 990, &corp, true, false);
    
    per.list_properties();
    corp.list_properties();
    cout << "--------\n";
    est4.print_info();
    cout << "----------------------------\n";
    
    corp.buy(&est4, &per);
    cout << "----------------------------\n";
    
    per.list_properties();
    corp.list_properties();
    cout << "--------\n";
    est4.print_info();
    cout << "----------------------------\n";
    */
    
    Person per = Person("Jeff Bezos", 543210, 57);
    Corporation corp = Corporation("Amazon", 1, "Seatlle, Washington");
    
    per.print_info();
    per.list_properties();
    corp.print_info();
    corp.list_properties();
    cout << "--------\n";
    
    Office off1 = Office("Good Office", 983, &corp, true, true);
    Office off2 = Office("Badd Office", 450, &corp, true, false);
    Office off3 = Office("Suit Office", 647, &corp, false, true);
    Office off4 = Office("Shit Office", 378, &corp, false, false);
    Office off5 = Office("Mini Office", 234, &corp, true, false);
    Office off6 = Office("Bigi Office", 10534, &corp, true, true);
    Office off7 = Office("Chep Office", 650, &corp, false, false);

    per.list_properties();
    corp.list_properties();
    cout << "--------\n";
    off1.print_info();
    off2.print_info();
    off3.print_info();
    off4.print_info();
    off5.print_info();
    off6.print_info();
    off7.print_info();
    cout << "---------------------------------\n";
    
    per.buy(&off5, &corp);
    cout << "---------------------------------\n";
    
    per.list_properties();
    corp.list_properties();
    cout << "--------\n";
    off1.print_info();
    off2.print_info();
    off3.print_info();
    off4.print_info();
    off5.print_info();
    off6.print_info();
    off7.print_info();
    cout << "---------------------------------\n";
    
    corp.sell(&off2, &per);
    corp.sell(&off1, &per);
    corp.sell(&off6, &per);
    cout << "---------------------------------\n";
    
    per.list_properties();
    corp.list_properties();
    cout << "--------\n";
    off1.print_info();
    off2.print_info();
    off3.print_info();
    off4.print_info();
    off5.print_info();
    off6.print_info();
    off7.print_info();
    cout << "---------------------------------\n";
    
    return 0;
}
