#include <iostream>
#include "apartment.h"
#include "owner.h"

using namespace std;

Apartment::Apartment(const string &property_name, int area, Owner *owner, int floor, bool having_elevator) {
    this->property_name = property_name;
    this->area = area;
    this->owner = owner;
    if (owner)
        owner->add_property(this);
    this->floor = floor;
    this->having_elevator = having_elevator;
}

float Apartment::valuate() {
    return area * floor * (having_elevator ? 1.5 : 1);
}