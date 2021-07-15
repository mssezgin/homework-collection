#include <iostream>
#include "office.h"
#include "owner.h"

using namespace std;

Office::Office(const string &property_name, int area, Owner *owner, bool having_wifi, bool having_reception) {
    this->property_name = property_name;
    this->area = area;
    this->owner = owner;
    if (owner)
        owner->add_property(this);
    this->having_wifi = having_wifi;
    this->having_reception = having_reception;
}

float Office::valuate() {
    return area * 5 * (having_wifi ? 1.3 : 1) * (having_reception ? 1.5 : 1);
}