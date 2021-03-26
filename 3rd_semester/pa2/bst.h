#ifndef BINARY_SEARCH_TREE
#define BINARY_SEARCH_TREE

#include <functional>
#include <cstddef>
#include <ostream>
#include <list>
#include <vector>
#include <stack>
#include <queue>
#include <cmath>
#include <utility>

#include <iostream>   // you may use it for debugging your code


class DefaultBalanceCondition    // default BST, do not change
{
  public:
    bool operator() (int current_height, int ideal_height) const
    {
      return true;
    }
};


template <typename Key, typename Object,
          typename BalanceCondition=DefaultBalanceCondition,
	      typename Comparator=std::less<Key> >
class BinarySearchTree
{
  public:  /* DO NOT CHANGE */
    struct Node   // core structure of BinarySearchTree
    {
       Key key;         // unique key
       Object data;     // data to be stored, characterized by key
       Node * left;     // pointer to left subtree
       Node * right;    // pointer to right subtree
       size_t height;   // height of the node
       size_t subsize;  // size of the subtree node roots including itself

       Node(const Key &, const Object &, 
            Node *, Node *, size_t =0, size_t =1);   // Node constructor
    };

  public:  /* DO NOT CHANGE PROTOTYPES */
    BinarySearchTree();  // zero-parameter constructor
    BinarySearchTree(const std::list<std::pair<Key, Object> > &); // list is always sorted!
    ~BinarySearchTree();  // destructor

  public:  /* DO NOT CHANGE PROTOTYPES */
    void insert(const Key &, const Object &);  // insert new Key-Object
    void remove(const Key &);  // remove Node characterized by Key
    void toCompleteBST();  // convert into a complete BST

  public:  /* DO NOT CHANGE PROTOTYPES */
    Node * find(const Key &) const; // single item
    std::list<Node *> find(const Key &, const Key &) const; // range queries
    int height() const;  // return the height of the tree
    size_t size() const; // return the number of items in the tree
    bool empty() const;  // return whether the tree is empty or not
    Node * getRoot() const; // return a pointer to the root of the tree
    void print(std::ostream &) const;  // print tree structure into an output stream

  private:  /* DO NOT CHANGE DATA TYPES AND IDENTIFIERS */
    Node * root;                     // designated root
    size_t numNodes;                 // size
    Comparator isLessThan;           // operator upon which BST nodes are arranged
    BalanceCondition isBalanced;     // signals whether the signal is balanced
  
  private:  /* DO NOT CHANGE PROTOTYPES */
    Node * find(const Key &, Node *) const;
    int height(Node *) const;
    int subsize(Node *) const;
    void print(Node *, std::ostream &) const;

    // Define your const private utility functions below this line
    void createArrayFromTree(Node *&, Node **&) const;
    void helperFind(const Key &, const Key &, Node *, std::list<Node *> &) const;

  private:
    void makeEmpty(Node * &);       // utility for destructor
    
    // Define your private utility functions below this line
    void helperToConstructWithList(Node *&, size_t, int);
    // void helperToFill(Node * &, std::list<std::pair<Key, Object>> &);
    Node *toCompleteFromArray(Node **, size_t, int);
    void toCompleteBST(Node *&);
    bool helperInsert(const Key &, const Object &, Node *&);
    bool helperRemove(const Key &, Node *&);
    Node *helperExtractMin(Node *&);


  private: /* DO NOT CHANGE PROTOTYPES: compiler defaults are blocked */  
    BinarySearchTree(const BinarySearchTree &);
    const BinarySearchTree & operator=(const BinarySearchTree &);

  private:  // static utility functions
    template <typename T> //static utility function
    static const T & max(const T &, const T &);

    // Define more below this line if needed

};

#endif


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
BinarySearchTree<K,O,B,C>::Node::
Node(const K & _k, const O & _d, Node * _l, Node * _r, size_t _h, size_t _s)
  : key(_k), data(_d), left(_l), right(_r), height(_h), subsize(_s)
{
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
BinarySearchTree<K,O,B,C>::BinarySearchTree()
  : root(NULL), numNodes(0)  //comparator(C() ??)
{
}


// IMPLEMENT
template <typename K, typename O, typename B, typename C>
BinarySearchTree<K,O,B,C>::BinarySearchTree(const std::list<std::pair<K,O>> & datalist) {

    size_t n = datalist.size();

    this->root = NULL;
    this->numNodes = 0;

    if (n) {

        typename std::list<std::pair<K,O>>::const_iterator it = datalist.begin(); // BURASI CALISIYORRRRRRRRRRRRRRRRRRRRRRRRRRRRR
        // auto it = datalist.begin();
        std::stack<Node *> forRecursion1;
        std::stack<int> forRecursion2;

        this->helperToConstructWithList(this->root, n, (size_t) log2(n)); // constructing an empty complete tree
        // this->helperToFill(this->root, datalist); // filling through inorder traversal

        forRecursion1.push(this->root);
        forRecursion2.push(0);

        while (!forRecursion1.empty()) {

            Node *top = forRecursion1.top();

            if (!top) { // top == NULL
                forRecursion1.pop();
                forRecursion2.pop();
                continue;
            }

            if (forRecursion2.top() == 0) { // left subtree haven't been traversed yet
                forRecursion2.pop();
                forRecursion2.push(1);
                forRecursion1.push(top->left);
                forRecursion2.push(0);
                continue;
            }

            if (forRecursion2.top() == 1) { // right subtree haven't been traversed yet
                top->key = it->first;
                top->data = it->second;
                it++;
                
                forRecursion2.pop();
                forRecursion2.push(2);
                forRecursion1.push(top->right);
                forRecursion2.push(0);
                continue;
            }

            forRecursion1.pop(); // both subtrees have been traversed
            forRecursion2.pop(); // popping elements and returning to parent
        }
    }
}


template <typename K, typename O, typename B, typename C>
void BinarySearchTree<K,O,B,C>::helperToConstructWithList(Node * & t, size_t n, int idealHeight) {

    if (n == 0) {
        t = NULL;
    }
    else {

        t = new Node(K(), O(), NULL, NULL, 0, 1);

        if (idealHeight <= 0) {

            t->left = NULL;
            t->right = NULL;
        }
        else {

            double bigSubtree = pow(2, idealHeight) - 1; // max no of nodes in a subtree with height idealHeight - 1
            double smallSubtree = pow(2, idealHeight - 1) - 1; // max no of nodes in a subtree with height idealHeight - 2

            if (n - 1 <= bigSubtree + smallSubtree) {

                this->helperToConstructWithList(t->left, n - 1 - smallSubtree, idealHeight - 1);
                this->helperToConstructWithList(t->right, smallSubtree, idealHeight - 2);
            }
            else { // n - 1 > bigSubtree + smallSubtree

                this->helperToConstructWithList(t->left, bigSubtree, idealHeight - 1);
                this->helperToConstructWithList(t->right, n - 1 - bigSubtree, idealHeight - 1);
            }

            t->height = idealHeight;
            t->subsize = n;
        }
    }
}


/*template <typename K, typename O, typename B, typename C>
void BinarySearchTree<K,O,B,C>::helperToFill(Node * & t, std::list<std::pair<K, O>> &datalist) {

    // static size_t m = 0;
    static auto it = datalist.begin();

    if (t && m < n) {

        helperToFill(t->left, datalist, n);
        t->key = it->first;
        t->data = it->second;
        // m++;
        it++;
        helperToFill(t->right, datalist, n);
    }
}*/


// IMPLEMENT
template <typename K, typename O, typename B, typename C>
void
BinarySearchTree<K,O,B,C>::toCompleteBST() {

    this->toCompleteBST(this->root);
}


template <typename K, typename O, typename B, typename C>
void BinarySearchTree<K,O,B,C>::toCompleteBST(Node *&r) {

    if (r && r->subsize > 1) { // if r == NULL || subsize == 1, do nothing

        size_t n = r->subsize;
        Node **arr = new Node *[n];
        Node **a = arr;

        this->createArrayFromTree(r, a);
        a = arr;
        r = this->toCompleteFromArray(a, n, (size_t) log2(n));

        delete[] arr;
    }
}


template <typename K, typename O, typename B, typename C>
void BinarySearchTree<K,O,B,C>::createArrayFromTree(Node *&r, Node **&a) const {

    if (r) {

        this->createArrayFromTree(r->left, a);
        *a++ = r;
        this->createArrayFromTree(r->right, a);
    }
}


template <typename K, typename O, typename B, typename C>
typename BinarySearchTree<K,O,B,C>::Node *
BinarySearchTree<K,O,B,C>::toCompleteFromArray(Node **a, size_t n, int idealHeight) {

    if (n == 0) {
        return NULL;
    }
    else if (idealHeight <= 0) { // n == 1

        (*a)->left = NULL;
        (*a)->right = NULL;
        (*a)->height = 0;
        (*a)->subsize = 1;

        return *a;
    }
    else { // n >= 2 and idealHeight >= 1

        double bigSubtree = pow(2, idealHeight) - 1; // max no of nodes in a subtree with height idealHeight - 1
        double smallSubtree = pow(2, idealHeight - 1) - 1; // max no of nodes in a subtree with height idealHeight - 2
        size_t rootIndex;
        Node *rootP;

        if (n - 1 <= bigSubtree + smallSubtree) {

            rootIndex = n - smallSubtree - 1;
            rootP = a[rootIndex];
            rootP->left = this->toCompleteFromArray(a, rootIndex, idealHeight - 1);
            rootP->right = this->toCompleteFromArray(a + rootIndex + 1, smallSubtree, idealHeight - 2);
        }                                         // ^ a + rootIndex yerine rootP mi yazsam, yok yaaa
        else { // n - 1 > bigSubtree + smallSubtree

            rootIndex = bigSubtree;
            rootP = a[rootIndex];
            rootP->left = this->toCompleteFromArray(a, rootIndex, idealHeight - 1);
            rootP->right = this->toCompleteFromArray(a + rootIndex + 1, n - rootIndex - 1, idealHeight - 1);
        }

        rootP->height = idealHeight;
        rootP->subsize = n;

        return rootP;
    }
}


// IMPLEMENT
template <typename K, typename O, typename B, typename C>
void
BinarySearchTree<K,O,B,C>::insert(const K & k, const O & x) {

    if (this->helperInsert(k, x, this->root)) {

        (this->numNodes)++;
    }
}


template <typename K, typename O, typename B, typename C>
bool BinarySearchTree<K,O,B,C>::helperInsert(const K &ky, const O &obj, Node *&r) {

    // returns true if new Node is allocated,
    // that is, heights and subsizes need to be updated,
    // returns false if Key ky already exists.
    // has nothing to do with isBalanced.

    bool returnVal = false;

    if (!r) { // r == NULL, no need to check isBalanced

        r = new Node(ky, obj, NULL, NULL, 0, 1);

        returnVal = true;
        return returnVal;
    }
    else if (this->isLessThan(ky, r->key)) {

        if (this->helperInsert(ky, obj, r->left)) {

            /*int lf = this->height(r->left);
            int rg = this->height(r->right);

            r->height = (lf > rg) ? (lf + 1) : (rg + 1);*/
            r->height = this->max<int>(this->height(r->left), this->height(r->right)) + 1;
            (r->subsize)++;

            returnVal = true;
            // return returnVal;
        }
    }
    else if (this->isLessThan(r->key, ky)) {

        if (this->helperInsert(ky, obj, r->right)) {

            /*int lf = this->height(r->left);
            int rg = this->height(r->right);

            r->height = (lf > rg) ? (lf + 1) : (rg + 1);*/
            r->height = this->max<int>(this->height(r->left), this->height(r->right)) + 1;
            (r->subsize)++;

            returnVal = true;
            // return returnVal;
        }
    }
    else { // ky exists in tree, updating only the obj

        r->data = obj;
        // returnVal = false;
    }

    if (!this->isBalanced(r->height, (size_t) log2(r->subsize))) {

        this->toCompleteBST(r);
    }

    return returnVal;
}


// IMPLEMENT
template <typename K, typename O, typename B, typename C>
void
BinarySearchTree<K,O,B,C>::remove(const K & k) {

    if (this->helperRemove(k, this->root)) {

        (this->numNodes)--;
    }
}


template <typename K, typename O, typename B, typename C>
bool BinarySearchTree<K,O,B,C>::helperRemove(const K & ky, Node *&r) {

    // returns true if a Node is deleted,
    // that is, heights and subsizes need to be updated,
    // returns false if Key ky does not exist.
    // has nothing to do with isBalanced.

    bool returnVal = false;

    if (!r) { // r == NULL, ky does not exist, no need to check isBalanced

        return returnVal;
    }
    else if (this->isLessThan(ky, r->key)) {

        if (this->helperRemove(ky, r->left)) {

            /*int lf = this->height(r->left);
            int rg = this->height(r->right);

            r->height = (lf > rg) ? (lf + 1) : (rg + 1);*/
            r->height = this->max<int>(this->height(r->left), this->height(r->right)) + 1;
            (r->subsize)--;

            returnVal = true;
            // return returnVal;
        }
    }
    else if (this->isLessThan(r->key, ky)) {

        if (this->helperRemove(ky, r->right)) {

            /*int lf = this->height(r->left);
            int rg = this->height(r->right);

            r->height = (lf > rg) ? (lf + 1) : (rg + 1);*/
            r->height = this->max<int>(this->height(r->left), this->height(r->right)) + 1;
            (r->subsize)--;

            returnVal = true;
            // return returnVal;
        }
    }
    else { // we found ky

        if (!r->left && !r->right) { // r is a leaf

            delete r;
            r = NULL;
        }
        else if (!r->left) { // r has right child

            Node *tmp = r->right;

            delete r;
            r = tmp;
        }
        else if (!r->right) { // r has left child

            Node *tmp = r->left;

            delete r;
            r = tmp;
        }
        else { // r has two children

            /*int lf, rg;*/
            Node *tmp = this->helperExtractMin(r->right); // inorder successor of r

            tmp->left = r->left;
            tmp->right = r->right;
            tmp->subsize = r->subsize - 1;
            delete r;
            r = tmp;

            /*lf = this->height(r->left);
            rg = this->height(r->right);
            r->height = (lf > rg) ? (lf + 1) : (rg + 1);*/
            r->height = this->max<int>(this->height(r->left), this->height(r->right)) + 1;
        }

        returnVal = true;
    }

    // buraya bi duzenleme
    if (r && !this->isBalanced(r->height, (size_t) log2(r->subsize))) {

        this->toCompleteBST(r);
    }

    return returnVal;
}


template <typename K, typename O, typename B, typename C>
typename BinarySearchTree<K,O,B,C>::Node *
BinarySearchTree<K,O,B,C>::helperExtractMin(Node *&r) {

    // removes the left most Node from subtree
    // and returns the pointer

    Node *min = NULL;

    if (!r->left) { // r->left == NULL, we hit the left most Node

        min = r;
        r = r->right;
    }
    else { // r->left != NULL

        /*int lf, rg;*/

        min = this->helperExtractMin(r->left);
        /*lf = this->height(r->left);
        rg = this->height(r->right);
        r->height = (lf > rg) ? (lf + 1) : (rg + 1);*/
        r->height = this->max<int>(this->height(r->left), this->height(r->right)) + 1;
        (r->subsize)--;
    }

    return min;
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
typename BinarySearchTree<K,O,B,C>::Node *
BinarySearchTree<K,O,B,C>::find(const K & key) const
{
  return find(key, root);
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
typename BinarySearchTree<K,O,B,C>::Node *
BinarySearchTree<K,O,B,C>::find(const K & key, Node * t) const
{
  if (t == NULL)
  {
    return NULL;
  }
  else if (isLessThan(key, t->key))
  {
    return find(key, t->left);
  }
  else if (isLessThan(t->key, key))
  {
    return find(key, t->right);
  }
  else //found
  {
    return t;
  }
}


// IMPLEMENT
template <typename K, typename O, typename B, typename C>
std::list<typename BinarySearchTree<K,O,B,C>::Node *>
BinarySearchTree<K,O,B,C>::find(const K & lower, const K & upper) const {

    std::list<Node *> ranges;

    this->helperFind(lower, upper, this->root, ranges);

    return ranges;
}


template <typename K, typename O, typename B, typename C>
void BinarySearchTree<K,O,B,C>::helperFind(const K &lower, const K &upper, Node *r, std::list<Node *> &ranges) const {

    if (r) { // r != NULL

        if (this->isLessThan(upper, r->key)) {

            this->helperFind(lower, upper, r->left, ranges);
        }
        else if (this->isLessThan(r->key, lower)) {

            this->helperFind(lower, upper, r->right, ranges);
        }
        else {

            if (this->isLessThan(lower, r->key)) {
                this->helperFind(lower, r->key, r->left, ranges);
            }

            ranges.push_back(r);
            
            if (this->isLessThan(r->key, upper)) {
                this->helperFind(r->key, upper, r->right, ranges);
            }
        }
    }
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
typename BinarySearchTree<K,O,B,C>::Node *
BinarySearchTree<K,O,B,C>::getRoot() const
{
  return root;
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
int
BinarySearchTree<K,O,B,C>::height() const
{
  return height(root);
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
int
BinarySearchTree<K,O,B,C>::height(Node * t) const
{
  return (t == NULL) ? -1 : t->height;
}


// private utility
template <typename K, typename O, typename B, typename C>
int
BinarySearchTree<K,O,B,C>::subsize(Node * t) const
{
  return (t == NULL) ? 0 : t->subsize;
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
size_t
BinarySearchTree<K,O,B,C>::size() const
{
  return numNodes;
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
bool
BinarySearchTree<K,O,B,C>::empty() const
{
  return numNodes == 0;
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
void
BinarySearchTree<K,O,B,C>::makeEmpty(Node * & t)
{
  if (t != NULL)
  {
    makeEmpty(t->left);
    makeEmpty(t->right);
    delete t;

    --numNodes;
  }
  
  t = NULL;
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
BinarySearchTree<K,O,B,C>::~BinarySearchTree()
{
  makeEmpty(root);
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
void
BinarySearchTree<K,O,B,C>::print(std::ostream & out) const
{
  print(root, out);
  out << '\n';
}


// DO NOT CHANGE
template <typename K, typename O, typename B, typename C>
void
BinarySearchTree<K,O,B,C>::print(Node * t, std::ostream & out) const
{
  if (t != NULL && t->left != NULL) 
  {
    out << '[';
    print( t->left, out );
  }
  else if (t != NULL && t->left == NULL && t->right != NULL)
  {
    out << "[";
  }

  if (t != NULL)
  {
    if (t->left == NULL && t->right == NULL)
    {
      out << '(' << (t->key) << ')';
    }
    else if (t->left != NULL || t->right != NULL)
    {
      out << '{' << (t->key) << ",H" << t->height << ",S" << t->subsize << '}';
    }
  }
  
  if (t != NULL && t->right != NULL)
  {
    print( t->right, out );
    out << ']';
  }
  else if (t != NULL && t->left != NULL && t->right == NULL)
  {
    out << "]";
  }
}


// do not change
template <typename K, typename O, typename B, typename C>
template <typename T>
const T &
BinarySearchTree<K,O,B,C>::max(const T & el1, const T & el2)
{
  return el1 > el2 ? el1 : el2;
}

