#include <iostream>

#include "LinkedList.h"

int main() {

    int array[10] = {99, 98, 97, 96, 95, 94, 93, 92, 91, 90};
    LinkedList<int> llist(array, 10);
    LinkedList<float> fltlist;

    llist.print();
    std::cout << llist.getFirstNode()->data << " " << llist.getLastNode()->data << std::endl;
    std::cout << llist.getNumberOfNodes() << std::endl;
    fltlist.print();
    std::cout << fltlist.getNumberOfNodes() << std::endl;

    llist.insertAtTheTail(6);
    llist.print();
    std::cout << llist.getFirstNode()->data << " " << llist.getLastNode()->data << std::endl;

    llist.insertAtTheHead(2);
    llist.print();
    std::cout << llist.getFirstNode()->data << " " << llist.getLastNode()->data << std::endl;

    llist.insertSorted(4);
    llist.print();
    std::cout << llist.getFirstNode()->data << " " << llist.getLastNode()->data << std::endl;

    llist.removeNode(4);
    llist.print();
    std::cout << llist.getFirstNode()->data << " " << llist.getLastNode()->data << std::endl;

    return 0;
}
