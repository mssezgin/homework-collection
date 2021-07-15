#include "Action.h"
#include "Exception.h"

Action::Action() {
	// TODO
	// pass
}

Action::~Action() {
	// TODO
	// pass
}

/********************** CompleteAction **************************/

// helper
Node* helpComplete(Node* left, Node* right) {
    
    Node* root = *left & *right;
    vector<Node*>& leftChildren = left->getChildren();
    vector<Node*>& rightChildren = right->getChildren();
    int i, j, s = leftChildren.size(), t = rightChildren.size();
    
    for (i = 0; i < s; ++i) {
        int lid = leftChildren[i]->getId();
        for (j = 0; j < t; ++j) {
            if (lid == rightChildren[j]->getId())
                break;
        }
        // *root += *(leftChildren[i] & rightChildren[j]);
        *root += *helpComplete(leftChildren[i], rightChildren[j]);
    }
    
    return root;
}
// helper end

CompleteAction::CompleteAction(const Node& node) {
	// TODO
	try {
        node.getData();
        this->root = new DataNode(*(const DataNode*) &node);
	}
	catch (InvalidRequest e) {
        this->root = new Node(node);
	}
}

CompleteAction::~CompleteAction() {
	// TODO
	// delete this->root;
	try {
        this->root->getData();
        delete (DataNode*) this->root;
	}
	catch (InvalidRequest e) {
        delete this->root;
	}
}

Node* CompleteAction::act(const Node* node) const {
	// TODO
	Node* nonConstNode;
	try {
	    node->getData();
	    nonConstNode = new DataNode(*(const DataNode*) node);
	}
	catch (InvalidRequest e) {
	    nonConstNode = new Node(*node);
	}
	Node* returned = helpComplete(this->root, nonConstNode);
	
	try {
	    node->getData();
	    delete (DataNode*) nonConstNode;
	}
	catch (InvalidRequest e) {
	    delete nonConstNode;
	}
	return returned;
	/* Node nonConstNode(*node);
	return helpComplete(this->root, &nonConstNode); */
}

/************************* CutAction ***************************/

// helper
int cutHelper(char data, Node*& root) {
    
    if (root == NULL)
        return 0;
    
    vector<Node*>& children = root->getChildren();
    
    if (children.size() == 0) {
        return 0;
	}
    else {
        
        int countChildren = 0, countGrandChildren = 0;
		int i, s = children.size();
        
		for (i = 0; i < s; ++i) {
			countGrandChildren += cutHelper(data, children[i]);
			try {
                if (children[i]->getData() == data)
                    ++countChildren;
            }
            catch (InvalidRequest e) {}
		}
		
		if ((countChildren + countGrandChildren) >= 2) {
			Node* byeNode = root;
			root = new DataNode(root->getId(), data);
			// delete byeNode; // try catch
			try {
                byeNode->getData();
                delete (DataNode*) byeNode;
        	}
        	catch (InvalidRequest e) {
                delete byeNode;
        	}
			return 0;
		}
		else {
			return countChildren;
		}
    }
}
// helper end

CutAction::CutAction(char character) {
	// TODO
	this->data = character;
}

CutAction::~CutAction() {
	// TODO
	// pass
}

Node* CutAction::act(const Node* node) const {
	// TODO
	Node* root;
	try {
	    node->getData();
	    root = new DataNode(*(const DataNode*) node);
	}
	catch (InvalidRequest e) {
	    root = new Node(*node);
	}
	cutHelper(this->data, root);
	return root;
}

/************************* CompositeAction ***************************/

CompositeAction::CompositeAction() {
	// TODO
	actions = NULL;
}

CompositeAction::~CompositeAction() {
	// TODO
	// delete[] actions;
}

CompositeAction* CompositeAction::addAction(const Action* action) {
	// TODO
	return this;
}

Node* CompositeAction::act(const Node* node) const {
	// TODO
	// return NULL; // pass
}