#include <iostream>
#include <string>
#include <vector>
#include "owner.h"

using namespace std;

Owner::Owner() {
    this->balance = 0;
}

Owner::Owner(const string &name, float balance) {
    this->name = name;
    this->balance = balance;
}

void Owner::print_info() {
    return; // pass
}

string &Owner::get_name() {
    return name;
}

void Owner::add_property(Property *property) {
    properties.push_back(property);
}

void Owner::buy(Property *property, Owner *seller) {
    
    if (!property || !seller)
        return;
    
    bool owns_property = false;
    float property_value = property->valuate();
    vector<Property *>::iterator itr = seller->properties.begin();
    
    cout << "[BUY] Property: " << property->get_name() << " Value: " << property_value
         << "$ " << seller->name << "--->" << this->name << "\n";
    
    if (this->balance < property_value) {
        cout << "[ERROR] Unaffordable  property\n";
        return;
    }
    
    for (; itr != seller->properties.end(); ++itr) {
        if (*itr == property) {
            owns_property = true;
            break;
        }
    }
    
    if (!owns_property) {
        cout << "[ERROR] Transaction  on  unowned  property\n";
        return;
    }
    
    this->balance -= property_value;
    seller->balance += property_value;
    this->properties.push_back(*itr);
    seller->properties.erase(itr);
    property->set_owner(this);
}

void Owner::sell(Property *property, Owner *buyer) {
    
    if (!property || !buyer)
        return;
    
    bool owns_property = false;
    float property_value = property->valuate();
    vector<Property *>::iterator itr = this->properties.begin();
    
    cout << "[SELL] Property: " << property->get_name() << " Value: " << property_value
         << "$ " << this->name << "--->" << buyer->name << "\n";
    
    if (buyer->balance < property_value) {
        cout << "[ERROR] Unaffordable  property\n";
        return;
    }
    
    for (; itr != this->properties.end(); ++itr) {
        if (*itr == property) {
            owns_property = true;
            break;
        }
    }
    
    if (!owns_property) {
        cout << "[ERROR] Transaction  on  unowned  property\n";
        return;
    }
    
    buyer->balance -= property_value;
    this->balance += property_value;
    buyer->properties.push_back(*itr);
    this->properties.erase(itr);
    property->set_owner(buyer);
}

void Owner::list_properties() {
    
    int i = 1;
    vector<Property *>::iterator itr = properties.begin();
    
    cout << "Properties of " << name << ":\n";
    cout << "Balance: " << balance << "$\n";
    for (; itr != properties.end(); ++itr, ++i) {
        cout << i << ". " << (*itr)->get_name() << "\n";
    }
}