#include "Graph.h"

/*Do not add new libraries or files*/


Graph::Graph() {
    // TODO: IMPLEMENT THIS FUNCTION.

}

Graph::Graph(const Graph& rhs) {

    *this = rhs;
}

Graph& Graph::operator=(const Graph& rhs) {

    if (this != &rhs) {

        this->adjList = rhs.adjList;
    }

    return *this;
}

Graph::~Graph() {
    // TODO: IMPLEMENT THIS FUNCTION.

}


void Graph::addNode(const Node &node) {

    this->adjList.Insert(node.getCountry(), list<Edge>());
}

void Graph::addConnection(const Node& headNode, const Node& tailNode, int import) {

    list<Edge> *edgesOfHeadNode;

    try {
        edgesOfHeadNode = &this->adjList.Get(headNode.getCountry());
    }
    catch (ItemNotFoundException &e) {
        return;
    }

    edgesOfHeadNode->push_back(Edge(tailNode, import));
}

list<Node> Graph::getAdjacentNodes(const Node& node) {

    list<Edge> *edgesOfHeadNode;

    try {
        edgesOfHeadNode = &this->adjList.Get(node.getCountry());
    }
    catch(ItemNotFoundException &e) {
        throw e;
    }

    list<Edge>::iterator itr = edgesOfHeadNode->begin();
    list<Node> adjacentNodesOfHeadNode;

    for (; itr != edgesOfHeadNode->end(); itr++) {

        adjacentNodesOfHeadNode.push_back(itr->getTailNode());
    }

    return adjacentNodesOfHeadNode;
}

long Graph::getTotalImports(const Node& node) {

    long totalImports = 0;
    list<Edge> *edgesOfHeadNode;

    try {
        edgesOfHeadNode = &this->adjList.Get(node.getCountry());
    }
    catch(ItemNotFoundException &e) {
        return totalImports;
    }

    list<Edge>::iterator itr = edgesOfHeadNode->begin();

    for (; itr != edgesOfHeadNode->end(); itr++) {

        totalImports += itr->getImport();
    }

    return totalImports;
}

void Graph::deleteNode(const Node& node) {

    try {
        this->adjList.Delete(node.getCountry());
    }
    catch (ItemNotFoundException &e) {
        return;
    }

    int i = this->adjList.Size();
    string *k, *keys = new string[i];

    this->adjList.getKeys(keys);

    for (k = keys; i--; k++) {

        list<Edge> *edgesOfK;

        try {
            edgesOfK = &this->adjList.Get(*k);
        }
        catch(ItemNotFoundException &e) {
            continue;
        }

        list<Edge>::iterator itr = edgesOfK->begin();

        while (itr != edgesOfK->end()) {

            if (itr->getTailNode().getCountry() == node.getCountry()) {

                itr = edgesOfK->erase(itr);
            }
            else {

                itr++;
            }
        }
    }

    delete[] keys;
}

list<string> Graph::findLeastCostPath(const Node& srcNode, const Node& destNode) {

    priority_queue<pqElement, vector<pqElement>, greater<pqElement> > pq;
    HashTable<string, pqElement> ht;

    pq.push(pqElement(srcNode.getCountry(), 0, "", false));
    ht.Insert(srcNode.getCountry(), pqElement(srcNode.getCountry(), 0, "", false));

    while (!pq.empty()) {

        if (pq.top().country == destNode.getCountry()) {

            ht.Get(pq.top().country).visited = true;
            break;
        }

        pqElement pqTop = pq.top();
        ht.Get(pqTop.country).visited = true;
        pq.pop();

        list<Edge> &neighborList = this->adjList.Get(pqTop.country);
        list<Edge>::iterator itr = neighborList.begin();

        for (; itr != neighborList.end(); itr++) {

            long totalCost = pqTop.costFromSrc + itr->getImport();
            pqElement currNeighbor(itr->getTailNode().getCountry(), totalCost, pqTop.country, false);
            pqElement *currNeighborInHT;

            try {
                currNeighborInHT = &ht.Get(currNeighbor.country);
            }
            catch (ItemNotFoundException &e) {
                currNeighborInHT = NULL;
            }

            if (currNeighborInHT) { // currNeighbor exists in ht

                if (!currNeighborInHT->visited) { // currNeighbor hasn't been visited yet

                    if (totalCost < currNeighborInHT->costFromSrc) { // new cost is smaller, updating

                        currNeighborInHT->costFromSrc = totalCost;
                        currNeighborInHT->prev = currNeighbor.prev;
                        pq.push(currNeighbor);
                    }
                    //else; // new cost is greater
                }
                //else; // currNeighbor has already been visited
            }
            else { // currNeighbor doesn't exist in ht, no calculations done previously

                ht.Insert(currNeighbor.country, currNeighbor);
                pq.push(currNeighbor);
            }
        }
    }

    list<string> path;
    string *p = (string *) &destNode.getCountry();

    try {
        ht.Get(*p);
    }
    catch (ItemNotFoundException &e) { // no path exists
        *p = "";
    }

    while (*p != "") {

        path.push_front(*p);
        p = &ht.Get(*p).prev;
    }

    return path;
}


bool Graph::isCyclic() {

    bool result;
    string *k, *end, *keys = new string[this->adjList.Size()];
    HashTable<string, bool> stackHT; // <key, true if traversed currently>, deleted if visited completely

    this->adjList.getKeys(keys);

    for (k = keys, end = k + this->adjList.Size(); k < end; k++) { // initializing stackHT

        stackHT.Insert(*k, false);
    }

    for (k = keys; k < end; k++) { // finding next unvisited node

        try {
            stackHT.Get(*k);
        }
        catch (ItemNotFoundException &e) { // node was deleted from stackHT since no loop was found
            continue;
        }

        if ((result = this->isCyclic(*k, stackHT))) {

            break;
        }
    }

    delete[] keys;

    return result;
}


bool Graph::isCyclic(const string &key, HashTable<string, bool> &stackHT) {

    if (stackHT.Get(key)) { // came to previously traversed neighbor, found a loop

        return true;
    }

    stackHT.Get(key) = true;

    list<Edge> &neighborList = this->adjList.Get(key);
    list<Edge>::iterator itr = neighborList.begin();

    for (; itr != neighborList.end(); itr++) {

        try {
            stackHT.Get(itr->getTailNode().getCountry());
        }
        catch (ItemNotFoundException &e) {
            continue;
        }

        if (this->isCyclic(itr->getTailNode().getCountry(), stackHT)) {

            return true;
        }
    }

    stackHT.Delete(key);

    return false;

    /* for (; keys < end; keys++) { // finding next unvisited node

        try {
            stackHT.Get(*keys);
        }
        catch (ItemNotFoundException &e) {
            continue;
        }

        break;
    }

    if (keys >= end) { // all nodes are visited

        return false;
    }

    list<Edge> &neighborList = this->adjList.Get(*keys);
    list<Edge>::iterator itr = neighborList.begin();

    stackHT.Get(*keys) = true;

    for (; itr != neighborList.end(); itr++) {

        bool *wasTraversed;

        try {
            wasTraversed = &stackHT.Get(itr->getTailNode().getCountry());
        }
        catch (ItemNotFoundException &e) { // node was deleted from stackHT
            wasTraversed = NULL;
        }

        if (wasTraversed) { // wasTraversed != NULL

            if (*wasTraversed) { // came to previously traversed neighbor, found a loop

                return true;
            }
            else { // continue searching

                *wasTraversed = true;
                this->isCyclic(keys, end, stackHT);
            }
        }
        else { // wasTraversed == NULL

            return false;
        }
    } */
}


list<string> Graph::getBFSPath(const Node& srcNode, const Node& destNode) {

    list<string> path;
    queue<const string *> queue;
    HashTable<string, bool> hashTable; // <country, visited>
    list<Edge> *adjacents;
    list<Edge>::iterator itr;

    queue.push(&srcNode.getCountry());
    hashTable.Insert(srcNode.getCountry(), true);

    while (!queue.empty()) {

        path.push_back(*queue.front());

        if (*queue.front() == destNode.getCountry()) {

            break;
        }
        else {

            adjacents = &this->adjList.Get(*queue.front());

            for (itr = adjacents->begin(); itr != adjacents->end(); itr++) {

                bool b;

                try {
                    b = hashTable.Get(itr->getTailNode().getCountry());
                }
                catch (ItemNotFoundException &e) {
                    b = false;
                }

                if (!b) {
                    
                    queue.push(&itr->getTailNode().getCountry());
                    hashTable.Insert(itr->getTailNode().getCountry(), true);
                }
            }
        }

        queue.pop();
    }

    return path;
}