#ifndef __HASHTABLE__
#define __HASHTABLE__

#include "HashUtils.h"
#include "ItemNotFoundException.h"
/* Do not add new libraries or files */

#define BUCKET_SIZE 2

// Do not modify the public interface of this class.
// Otherwise, your code will note compile!
template <class K, class T>
class HashTable {
    struct Entry {
        K Key;             // the key of the entry
        T Value;   // the value of the entry
        bool Deleted;        // flag indicating whether this entry is deleted
        bool Active;         // flag indicating whether this item is currently used

        Entry() : Key(), Value(), Deleted(false), Active(false) {}
    };

    struct Bucket {
        Entry entries[BUCKET_SIZE];
    };

    int _capacity; // INDICATES THE TOTAL CAPACITY OF THE TABLE
    int _size; // INDICATES THE NUMBER OF ITEMS IN THE TABLE

    Bucket* _table; // THE HASH TABLE

    // == DEFINE HELPER METHODS & VARIABLES BELOW ==


public:
    // TODO: IMPLEMENT THESE FUNCTIONS.
    // CONSTRUCTORS, ASSIGNMENT OPERATOR, AND THE DESTRUCTOR
    HashTable();
    // COPY THE WHOLE CONTENT OF RHS INCLUDING THE KEYS THAT WERE SET AS DELETED
    HashTable(const HashTable<K, T>& rhs);
    HashTable<K, T>& operator=(const HashTable<K, T>& rhs);
    ~HashTable();

    // TODO: IMPLEMENT THIS FUNCTION.
    // INSERT THE ENTRY IN THE HASH TABLE WITH THE GIVEN KEY & VALUE
    // IF THE GIVEN KEY ALREADY EXISTS, THE NEW VALUE OVERWRITES
    // THE ALREADY EXISTING ONE. IF THE LOAD FACTOR OF THE TABLE IS GREATER THAN 0.6,
    // RESIZE THE TABLE WITH THE NEXT PRIME NUMBER.
    void Insert(const K& key, const T& value);

    // TODO: IMPLEMENT THIS FUNCTION.
    // DELETE THE ENTRY WITH THE GIVEN KEY FROM THE TABLE
    // IF THE GIVEN KEY DOES NOT EXIST IN THE TABLE, THROW ItemNotFoundException()
    void Delete(const K& key);

    // TODO: IMPLEMENT THIS FUNCTION.
    // IT SHOULD RETURN THE VALUE THAT CORRESPONDS TO THE GIVEN KEY.
    // IF THE KEY DOES NOT EXIST, THROW ItemNotFoundException()
    T& Get(const K& key) const;

    // TODO: IMPLEMENT THIS FUNCTION.
    // AFTER THIS FUNCTION IS EXECUTED THE TABLE CAPACITY MUST BE
    // EQUAL TO newCapacity AND ALL THE EXISTING ITEMS MUST BE REHASHED
    // ACCORDING TO THIS NEW CAPACITY.
    // WHEN CHANGING THE SIZE, YOU MUST REHASH ALL OF THE ENTRIES
    void Resize(int newCapacity);

    // THE IMPLEMENTATION OF THESE FUNCTIONS ARE GIVEN TO YOU
    // DO NOT MODIFY!
    int Capacity() const; // RETURN THE TOTAL CAPACITY OF THE TABLE
    int Size() const; // RETURN THE NUMBER OF ACTIVE ITEMS
    void getKeys(K* keys); // PUT THE ACTIVE KEYS TO THE GIVEN INPUT PARAMETER
};


template <class K, class T>
HashTable<K, T>::HashTable() {

    this->_capacity = NextCapacity(0);
    this->_size = 0;
    this->_table = new Bucket[this->_capacity];
}

template <class K, class T>
HashTable<K, T>::HashTable(const HashTable<K, T>& rhs) {

    this->_capacity = 0;
    this->_size = 0;
    this->_table = NULL;
    *this = rhs;
}

template <class K, class T>
HashTable<K, T>& HashTable<K, T>::operator=(const HashTable<K, T>& rhs) {

    if (this != &rhs) {

        int i;
        Bucket *lt, *rt, *end;

        this->_capacity = rhs._capacity;
        this->_size = rhs._size;

        if (this->_table) { // _table != NULL

            delete[] this->_table;
        }

        this->_table = new Bucket[this->_capacity];

        for (lt = this->_table, rt = rhs._table, end = rt + rhs._capacity; rt < end; lt++, rt++) {

            for (i = 0; i < BUCKET_SIZE; i++) {
                
                Entry &le = lt->entries[i];
                Entry &re = rt->entries[i];

                if (re.Active) {

                    le.Key = re.Key;
                    le.Value = re.Value;
                    le.Deleted = re.Deleted;
                    le.Active = re.Active;
                }
                else if (re.Deleted) {

                    le.Deleted = true;
                }
            }
        }
    }

    return *this;
}

template <class K, class T>
HashTable<K, T>::~HashTable() {

    this->_capacity = 0;
    this->_size = 0;
    delete[] this->_table;
    this->_table = NULL;
}

template <class K, class T>
void HashTable<K, T>::Insert(const K& key, const T& value) {

    // std::cout << "--- Insert(" << key << ", " << value << ")\n";

    if ((float)this->_size / (float)this->_capacity >= 0.6f * BUCKET_SIZE) {

        this->Resize(NextCapacity(this->_capacity));
    }

    int hashVal = Hash(key);
    int i, probe;
    Bucket *b;

    for (probe = 0; true; probe++) {

        b = this->_table + (hashVal + probe * probe) % this->_capacity;

        for (i = 0; i < BUCKET_SIZE; i++) {

            Entry &e = b->entries[i];

            if (e.Active && e.Key == key) {

                e.Value = value;

                break;
            }

            if (!e.Active) {

                e.Key = key;
                e.Value = value;
                e.Deleted = false;
                e.Active = true;
                this->_size++;

                break;
            }
        }

        if (i < BUCKET_SIZE) // break happened, breaking again
            break;
    }
}

template <class K, class T>
void HashTable<K, T>::Delete(const K& key) {

    // std::cout << "--- Delete(" << key << ")\n";

    int hashVal = Hash(key);
    int i, probe;
    Bucket *b;

    for (probe = 0; true; probe++) {

        b = this->_table + (hashVal + probe * probe) % this->_capacity;

        for (i = 0; i < BUCKET_SIZE; i++) {

            Entry &e = b->entries[i];

            if (e.Active) { // found key

                if (e.Key == key) {

                    e.Deleted = true;
                    e.Active = false;
                    this->_size--;

                    break;
                }
            }
            else if (!e.Deleted) { // key does not exist
                
                throw ItemNotFoundException();
                break;
            }
        }

        if (i < BUCKET_SIZE) // break happened, breaking again
            break;
    }
}

template <class K, class T>
T& HashTable<K, T>::Get(const K& key) const {

    // std::cout << "--- Get(" << key << ")\n";

    int hashVal = Hash(key);
    int i, probe;
    Bucket *b;

    for (probe = 0; true; probe++) {

        b = this->_table + (hashVal + probe * probe) % this->_capacity;

        for (i = 0; i < BUCKET_SIZE; i++) {

            Entry &e = b->entries[i];

            if (e.Active) { // found key

                if (e.Key == key) {

                    return e.Value;
                }
            }
            else if (!e.Deleted) { // key does not exist

                throw ItemNotFoundException();
                break;
            }
        }

        if (i < BUCKET_SIZE) // break happened, breaking again // ---------BURASI GEREKSIZZZZZZZZZZZZZZZZZZZZ
            break;
    }
}


template <class K, class T>
void HashTable<K, T>::Resize(int newCapacity) {

    // std::cout << "--- Resize(" << newCapacity << ")\n";

    int i, j;
    Bucket *oldTable;

    i = this->_capacity; // swapping old capacity and new capacity for insert function to work properly
    this->_capacity = newCapacity;
    newCapacity = i;

    oldTable = this->_table;
    this->_table = new Bucket[this->_capacity];
    this->_size = 0;

    for (i = 0; i < newCapacity; i++, oldTable++) { // newCapacity holds old capacity

        for (j = 0; j < BUCKET_SIZE; j++) {

            if (oldTable->entries[j].Active) {

                this->Insert(oldTable->entries[j].Key, oldTable->entries[j].Value);
            }
        }
    }

    oldTable -= newCapacity;
    delete[] oldTable;
}


template <class K, class T>
int HashTable<K, T>::Capacity() const {
    return _capacity;
}

template <class K, class T>
int HashTable<K, T>::Size() const {
    return _size;
}


template <class K, class T>
void HashTable<K, T>::getKeys(K* keys) {
    int index = 0;

    for (int i = 0; i < _capacity; i++) {
        Bucket& bucket = _table[i];
        for (int j = 0; j < BUCKET_SIZE; j++) {
            if (bucket.entries[j].Active && !bucket.entries[j].Deleted) {
                keys[index++] = bucket.entries[j].Key;
            }
        }
    }
}

#endif