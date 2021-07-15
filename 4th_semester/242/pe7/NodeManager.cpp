
#include "NodeManager.h"

// helper
Node** findNode(int id, vector<Node*>& children) {
    
    int i, s = children.size();
    Node** found = NULL;

	for (i = 0; i < s; ++i) {
        if (id == children[i]->getId())
            return &children[i];
        else if ((found = findNode(id, children[i]->getChildren())) != NULL)
            return found;
	}
	return found;
}
// helper end

NodeManager::NodeManager() {
	// TODO
	// pass
}

NodeManager::~NodeManager() {
	// TODO
	int i, s = roots.size();
	/* for (i = 0; i < s; ++i) {
	    delete roots[i]; // try catch
	    roots[i] = NULL;
	}
	roots.clear(); */
	for (i = 0; i < s; ++i) {
	    try {
            roots[i]->getData();
            delete (DataNode*) roots[i];
        }
        catch (InvalidRequest e) {
            delete roots[i];
        }
        roots[i] = NULL;
    }
    roots.clear();
}

void NodeManager::addRelation(int parent, int child) {
	// TODO
	int i, s = roots.size();
	/* Node* parentP = *findNode(parent, roots);
	
	if (!parentP)
	    roots.push_back((parentP = new Node(parent)));
	
	for (i = 0; i < s; ++i) {
	    if (child == roots[i]->getId()) {
	        *parentP += *roots[i];
	        roots.erase(roots.begin() + i);
	        return;
	    }
	}
	
	*parentP += *(new Node(child)); */
	Node** parentPP = findNode(parent, roots);
	Node* parentP = NULL;
	
	if (parentPP) // != NULL
	    parentP = *parentPP;
	
	if (parentP == NULL)
	    roots.push_back((parentP = new Node(parent)));
	
	for (i = 0; i < s; ++i) {
	    if (child == roots[i]->getId()) {
	        *parentP += *roots[i];
	        roots.erase(roots.begin() + i);
	        return;
	    }
	}
	
	*parentP += *(new Node(child));
}

void NodeManager::setDataToNode(int id, char data) {
	// TODO
	Node** node = findNode(id, roots);
	
	if (node == NULL) {
	    roots.push_back(new DataNode(id, data));
    }
	else { // if (node != NULL) {
        Node* byeNode = *node;
        *node = new DataNode(**node, data);
        // delete byeNode;
        try {
            byeNode->getData();
            delete (DataNode*) byeNode;
        }
        catch (InvalidRequest e) {
            delete byeNode;
        }
	}
}

const Node& NodeManager::getNode(int id) {
	// TODO
    return **findNode(id, roots);
}
