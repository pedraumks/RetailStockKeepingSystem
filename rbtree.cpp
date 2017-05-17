//  File: rbtree.cpp
//  Author: Pedraum Safarzadeh
//  Date: 2016-06-11
//  Description: Implementation of a RBTree class and template Node class

#include <stdio.h>
#include <iostream>
#include <stdexcept>
#ifdef _RBTREE_H_

using namespace std;

// recursive helper function for deep copy
// creates a new node based on sourcenode's contents, links back to parentnode,
//   and recurses to create left and right children
template <typename T>
Node<T>* RBTree<T>::CopyTree(Node<T>* sourcenode, Node<T>* parentnode) {
    if (sourcenode == NULL)
        return NULL;
    Node<T>* new_node = new Node<T>(sourcenode->data);
    new_node->left = CopyTree(sourcenode->left, new_node);
    new_node->right = CopyTree(sourcenode->right, new_node);
    new_node->p = parentnode;
    new_node->is_black = sourcenode->is_black;
    return new_node;
}

// recursive helper function for tree deletion
// deallocates nodes in post-order
template <typename T>
void RBTree<T>::RemoveAll(Node<T>* node) {
    if (node != NULL) {
        RemoveAll(node->left);
        RemoveAll(node->right);
        delete node;
    }
}

// Tree fix, performed after removal of a black node
// Note that the parameter x may be NULL
template <typename T>
void RBTree<T>::RBRemoveFixUp(Node<T>* x, Node<T>* xparent, bool xisleftchild) {
    while(x != NULL && x != root && x->is_black == true) {
        if (xisleftchild) {
            Node<T>* y = xparent->right;
            if (y->is_black == false) {
                y->is_black = true;
                xparent->is_black = false;
                RotateLeft(xparent);
                y = xparent->right;
            }
            if (y->left->is_black == true && y->right->is_black == true) {
                y->is_black = false;
                x = xparent;
            }
            else {
                if (y->right->is_black == true) {
                    y->left->is_black = true;
                    y->is_black = false;
                    RotateRight(y);
                    y = xparent->right;
                }
                y->is_black = xparent->is_black;
                xparent->is_black = true;
                y->right->is_black = true;
                RotateLeft(xparent);
                x = root;
            }
            
        }
        else {
            Node<T>* y = xparent->left;
            if (y->is_black == false) {
                y->is_black = true;
                xparent->is_black = false;
                RotateRight(xparent);
                y = xparent->left;
            }
            if (y->left->is_black == true && y->right->is_black == true) {
                y->is_black = false;
                x = xparent;
            }
            else {
                if (y->left->is_black == true) {
                    y->right->is_black = true;
                    y->is_black = false;
                    RotateLeft(y);
                    y = xparent->left;
                }
                y->is_black = xparent->is_black;
                xparent->is_black = true;
                y->left->is_black = true;
                RotateRight(xparent);
                x = root;
            }
        }
    }
    if (x != NULL)
        x->is_black = true;
}

// Calculates the height of the tree
// Requires a traversal of the tree, O(n)
template <typename T>
int RBTree<T>::ComputeHeight(Node<T>* node) const {
    if (node == NULL)
        return 0;
    else {
        if (1 + ComputeHeight(node->left) < 1 + ComputeHeight(node->right))
            return 1 + ComputeHeight(node->right);
        else
            return 1 + ComputeHeight(node->left);
    }
}

// default constructor
template <typename T>
RBTree<T>::RBTree() {
    root = NULL;
    size = 0;
}

// copy constructor, performs deep copy of parameter
template <typename T>
RBTree<T>::RBTree(const RBTree<T>& rbtree) {
    root = CopyTree(rbtree.root, NULL);
    size = rbtree.size;
}

// destructor
// Must deallocate memory associated with all nodes in tree
template <typename T>
RBTree<T>::~RBTree() {
    RemoveAll();
}

// overloaded assignment operator
template <typename T>
RBTree<T>& RBTree<T>::operator=(const RBTree<T>& rbtree) {
    if (this != &rbtree) {
        RemoveAll();
        root = CopyTree(rbtree.root, NULL);
        size = rbtree.size;
    }
    return *this;
}

// Accessor functions

// Calls BSTInsert and then performs any necessary tree fixing.
// If item already exists, do not insert and return false.
// Otherwise, insert, increment size, and return true.
template <typename T>
bool RBTree<T>::Insert(T item) {
    if (Contains(item))
        return false;
    Node<T>* node_inserted = BSTInsert(item);
    node_inserted->is_black = false;
    while(node_inserted != NULL && node_inserted != root && !(node_inserted->p->is_black)) {
        if (node_inserted->p == node_inserted->p->p->left) {
            Node<T>* uncle = node_inserted->p->p->right;
            if (uncle != NULL && uncle->is_black == false) {
                node_inserted->p->is_black = true;
                uncle->is_black = true;
                node_inserted->p->p->is_black = false;
                node_inserted = node_inserted->p->p;
            }
            else {
                if (node_inserted == node_inserted->p->right) {
                    node_inserted = node_inserted->p;
                    RotateLeft(node_inserted);
                }
                node_inserted->p->is_black = true;
                node_inserted->p->p->is_black = false;
                RotateRight(node_inserted->p->p);
            }
        }
        else {
            Node<T>* uncle = node_inserted->p->p->left;
            if (uncle != NULL && uncle->is_black == false) {
                node_inserted->p->is_black = true;
                uncle->is_black = true;
                node_inserted->p->p->is_black = false;
                node_inserted = node_inserted->p->p;
            }
            else {
                if (node_inserted == node_inserted->p->left) {
                    node_inserted = node_inserted->p;
                    RotateRight(node_inserted);
                }
                node_inserted->p->is_black = true;
                node_inserted->p->p->is_black = false;
                RotateLeft(node_inserted->p->p);
            }
        }
    }
    root->is_black = true;
    size++;
    return true;
}

// Removal of an item from the tree.
// Must deallocate deleted node after RBDeleteFixUp returns
template <typename T>
bool RBTree<T>::Remove(T item) {
    if (!Contains(item))
        return false;
    Node<T>* x = NULL;
    Node<T>* y = NULL;
    Node<T>* z = Find(item);
    bool xisleftchild = false;
    if (z == NULL)
        return false;
    if (z->left == NULL || z->right == NULL)
        y = z;
    else
        y = Predecessor(z);
    if (y->left != NULL) {
        x = y->left;
        xisleftchild = true;
    }
    else
        x = y->right;
    if (x != NULL)
        x->p = y->p;
    if (y->p == NULL)
        root = x;
    else {
        if (y == y->p->left)
            y->p->left = x;
        else
            y->p->right = x;
    }
    if (y != z)
        z->data = y->data;
    if (y->is_black == true) {
        if (y != NULL)
            RBRemoveFixUp(x, y, xisleftchild);
    }
    delete y;
    size--;
    return true;
}

// deletes all nodes in the tree. Calls recursive helper function.
template <typename T>
void RBTree<T>::RemoveAll() {
    RemoveAll(root);
    root = NULL;
    size = 0;
}

// returns the number of items in the tree
template <typename T>
int RBTree<T>::Size() const {
    return size;
}

// returns the height of the tree, from root to deepest null child. Calls recursive helper function.
// Note that an empty tree should have a height of 0, and a tree with only one node will have a height of 1.
template <typename T>
int RBTree<T>::Height() const {
    return ComputeHeight(root);
}

#endif
