#ifndef LINKEDLIST_H
#define LINKEDLIST_H


#include <iostream>

#include "Node.h"

template<class T>
class LinkedList {
public: // DO NOT CHANGE THIS PART.
    LinkedList();
    LinkedList(const T arr[], int arrSize);
    LinkedList(const LinkedList<T> &obj);

    ~LinkedList();

    Node<T> *getFirstNode() const;
    Node<T> *getLastNode() const;
    Node<T> *getNode(const T &data) const;

    int getNumberOfNodes() const;
    bool isEmpty() const;
    bool contains(Node<T> *node) const;

    void insertAtTheHead(const T &data);
    void insertAtTheTail(const T &data);
    void insertSorted(const T &data);

    void removeNode(Node<T> *node);
    void removeNode(const T &data);
    void removeAllNodes();

    void print() const;

    T *toArray() const;

    LinkedList<T> &operator=(const LinkedList<T> &rhs);

private: // YOU MAY ADD YOUR OWN UTILITY MEMBER FUNCTIONS HERE.

private: // DO NOT CHANGE THIS PART.
    Node<T> *head;
    Node<T> *tail;
};

template<class T>
LinkedList<T>::LinkedList() {
    
    this->head = NULL;
    this->tail = NULL;
}

template<class T>
LinkedList<T>::LinkedList(const T arr[], int arrSize) {
    
    this->head = NULL;
    this->tail = NULL;
    
    while (arrSize--) {
        this->insertAtTheTail(*arr++);
    }
}

template<class T>
LinkedList<T>::LinkedList(const LinkedList<T> &obj) {
    
    this->head = NULL;
    this->tail = NULL;
    
    *this = obj;
}

template<class T>
LinkedList<T>::~LinkedList() {
    
    this->removeAllNodes();
}

template<class T>
Node<T> *LinkedList<T>::getFirstNode() const {
    
    return this->head;
}

template<class T>
Node<T> *LinkedList<T>::getLastNode() const {
    
    return this->tail;
}

template<class T>
Node<T> *LinkedList<T>::getNode(const T &data) const {
    
    Node<T> *tmp;
    
    for (tmp = this->head; tmp; tmp = tmp->next) {
        if (tmp->data == data) {
            break;
        }
    }
    
    return tmp;
}

template<class T>
int LinkedList<T>::getNumberOfNodes() const {
    
    int count;
    Node<T> *tmp;
    
    for (count = 0, tmp = this->head; tmp; count++, tmp = tmp->next);
    
    return count;
}

template<class T>
bool LinkedList<T>::isEmpty() const {
    
    return (this->head == NULL); // or this->tail == NULL
}

template<class T>
bool LinkedList<T>::contains(Node<T> *node) const {
    
    Node<T> *tmp;
    
    for (tmp = this->head; tmp; tmp = tmp->next) {
        if (tmp == node) {
            return true;
        }
    }
    
    return false;
}

template<class T>
void LinkedList<T>::insertAtTheHead(const T &data) {
    
    if (this->isEmpty()) {
        this->head = this->tail = new Node<T>(data, NULL, NULL);
    }
    else {
        this->head = new Node<T>(data, NULL, this->head);
        this->head->next->prev = this->head;
    }
}

template<class T>
void LinkedList<T>::insertAtTheTail(const T &data) {
    
    if (this->isEmpty()) {
        this->head = this->tail = new Node<T>(data, NULL, NULL);
    }
    else {
        this->tail = new Node<T>(data, this->tail, NULL);
        this->tail->prev->next = this->tail;
    }
}

template<class T>
void LinkedList<T>::insertSorted(const T &data) {
    
    if (this->isEmpty() || this->head->data >= data) {
        this->insertAtTheHead(data);
    }
    else if (this->tail->data <= data) {
        this->insertAtTheTail(data);
    }
    else {
        Node<T> *tmp;
        
        for (tmp = this->head; tmp->data < data; tmp = tmp->next);
        tmp->prev = tmp->prev->next = new Node<T>(data, tmp->prev, tmp);
    }
}

template<class T>
void LinkedList<T>::removeNode(Node<T> *node) {
    
    if (this->head == node && this->tail == node) {
        this->head = this->tail = NULL;
        delete node;
    }
    else if (this->head == node) {
        this->head = this->head->next;
        this->head->prev = NULL;
        delete node;
    }
    else if (this->tail == node) {
        this->tail = this->tail->prev;
        this->tail->next = NULL;
        delete node;
    }
    else if (this->contains(node)) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
        delete node;
    }
}

template<class T>
void LinkedList<T>::removeNode(const T &data) {
    
    Node<T> *node = this->getNode(data);
    
    if (node) {
        this->removeNode(node);
    }
}

template<class T>
void LinkedList<T>::removeAllNodes() {
    
    while (!(this->isEmpty())) {
        this->removeNode(this->head);
    }
}

template<class T>
void LinkedList<T>::print() const {
    std::cout << "Printing the linked list ..." << std::endl;

    if (this->isEmpty()) {
        std::cout << "empty" << std::endl;
        return;
    }

    Node<T> *node = this->head;

    std::cout << "[" << *node;
    node = node->next;
    while (node) {
        std::cout << ", " << *node;
        node = node->next;
    }
    std::cout << "]" << std::endl;
}

template<class T>
T *LinkedList<T>::toArray() const {
    
    int n = this->getNumberOfNodes();
    
    if (!n) {
        return NULL;
    }
    else {
        T *a, *array = new T[n];
        Node<T> *tmp;
        
        for (a = array, tmp = this->head; tmp; a++, tmp = tmp->next) {
            *a = tmp->data;
        }
        
        return array;
    }
}

template<class T>
LinkedList<T> &LinkedList<T>::operator=(const LinkedList<T> &rhs) {
    
    if (this != &rhs) {
        
        Node<T> *tmp;
        
        this->removeAllNodes();
        
        for (tmp = rhs.head; tmp; tmp = tmp->next) {
            this->insertAtTheTail(tmp->data);
        }
    }
    
    return *this;
}


#endif //LINKEDLIST_H
