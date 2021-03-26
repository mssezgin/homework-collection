#include "Graph.h"
#include <iostream>
/*You can add additional libraries here to read and process the dataset*/
#include <fstream> // file read/write

using namespace std;

// tester
int main() {

    std::cout << "\nsimpleHashTable\n";

    int i, j;
    HashTable<int, int> simpleHashTable;

    for (i = 0; i < 12; i++) {

        simpleHashTable.Insert(i, i * (i + 1));
    }

    std::cout << "size: " << simpleHashTable._size << " capacity: " << simpleHashTable._capacity << "\n";
    for (i = 0; i < simpleHashTable._capacity; i++) {

        for (j = 0; j < BUCKET_SIZE; j++) {

            std::cout << simpleHashTable._table[i].entries[j].Key << " | " <<
                         simpleHashTable._table[i].entries[j].Value << " | " <<
                         simpleHashTable._table[i].entries[j].Deleted << " | " <<
                         simpleHashTable._table[i].entries[j].Active << "\n";
        }
    }

    std::cout << "\nsecondHashTable\n";
    HashTable<int, int> secondHashTable(simpleHashTable);

    for (i = 100; i < 120; i++) {

        secondHashTable.Insert(i, i);
    }


    std::cout << "size: " << secondHashTable._size << " capacity: " << secondHashTable._capacity << "\n";
    for (i = 0; i < secondHashTable._capacity; i++) {

        for (j = 0; j < BUCKET_SIZE; j++) {

            std::cout << secondHashTable._table[i].entries[j].Key << " | " <<
                         secondHashTable._table[i].entries[j].Value << " | " <<
                         secondHashTable._table[i].entries[j].Deleted << " | " <<
                         secondHashTable._table[i].entries[j].Active << "\n";
        }
    }

    std::cout << "\nthirdHashTable\n";
    HashTable<int, int> thirdHashTable;
    thirdHashTable = secondHashTable;


    std::cout << "size: " << thirdHashTable._size << " capacity: " << thirdHashTable._capacity << "\n";
    for (i = 0; i < thirdHashTable._capacity; i++) {

        for (j = 0; j < BUCKET_SIZE; j++) {

            std::cout << thirdHashTable._table[i].entries[j].Key << " | " <<
                         thirdHashTable._table[i].entries[j].Value << " | " <<
                         thirdHashTable._table[i].entries[j].Deleted << " | " <<
                         thirdHashTable._table[i].entries[j].Active << "\n";
        }
    }


    std::cout << "getting 104: " << thirdHashTable.Get(104) << "\n";
    try {
        // std::cout << "getting 103: " << thirdHashTable.Get(103) << "\n";
        std::cout << "getting 198: " << thirdHashTable.Get(198) << "\n";
    }
    catch(const ItemNotFoundException& e) {
        std::cerr << e.what() << "\n";
    }
    thirdHashTable.Delete(104);


    std::cout << "size: " << thirdHashTable._size << " capacity: " << thirdHashTable._capacity << "\n";
    for (i = 0; i < thirdHashTable._capacity; i++) {

        for (j = 0; j < BUCKET_SIZE; j++) {

            std::cout << thirdHashTable._table[i].entries[j].Key << " | " <<
                         thirdHashTable._table[i].entries[j].Value << " | " <<
                         thirdHashTable._table[i].entries[j].Deleted << " | " <<
                         thirdHashTable._table[i].entries[j].Active << "\n";
        }
    }


    thirdHashTable.Insert(104, 10444);
    thirdHashTable.Insert(119, 119119);

    std::cout << "size: " << thirdHashTable._size << " capacity: " << thirdHashTable._capacity << "\n";
    for (i = 0; i < thirdHashTable._capacity; i++) {

        for (j = 0; j < BUCKET_SIZE; j++) {

            std::cout << thirdHashTable._table[i].entries[j].Key << " | " <<
                         thirdHashTable._table[i].entries[j].Value << " | " <<
                         thirdHashTable._table[i].entries[j].Deleted << " | " <<
                         thirdHashTable._table[i].entries[j].Active << "\n";
        }
    }

    return 0;
}
