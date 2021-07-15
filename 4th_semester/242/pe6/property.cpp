#include <iostream>
#include "property.h"
#include "owner.h"

using namespace std;

Property::Property() {
    this->area = 0;
    this->owner = NULL;
}

Property::Property(const string &property_name, int area, Owner *owner) {
    this->property_name = property_name;
    this->area = area;
    this->owner = owner;
    if (owner)
        owner->add_property(this);
}

void Property::set_owner(Owner *owner) {
    this->owner = owner;
    // owner->owner->add_property(this);
}

float Property::valuate() {
    return 0.f; // pass
}

string &Property::get_name() {
    return property_name;
}

void Property::print_info() {
    cout << property_name << " (" << area << " m2) Owner: " << (owner ? owner->get_name() : "No owner") << "\n";
}