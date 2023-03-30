//
// Created by tanaka on 22-May-20.
//


#include "RBTree.h"
#include <stdlib.h>

#define FAILURE 0
#define SUCCESS 1

/**
 * The type of child it is
 */
typedef enum childType
{
    LEFT_CHILD,
    RIGHT_CHILD,
    NOT_A_CHILD
} childType;



Color colorOf(Node *node);
void recolor(Node *node1, Node *node2, Node *node3);
childType whichChild(Node *node);

int insertToBST(RBTree *tree, void *data, Node *cur, Node *nodeToInsert);
void fixInaccuraces(RBTree *tree, Node *cur);


void deleteRedNode(RBTree *tree, Node *nodeToDelete);
int oneChildCase(RBTree *tree, Node *nodeToDelete);
int twoNullChildrenCase(RBTree *tree, Node *nodeToDelete);
void deleteNodeWith2NULLChildren(RBTree *tree, Node *unbalancedNodeToDelete);

void deleteCase1(RBTree *tree, Node *unbalancedNode);
void deleteCase2(RBTree *tree, Node *unbalancedNode);
void deleteCase3(RBTree *tree, Node *unbalancedNode);
void deleteCase4(RBTree *tree, Node *unbalancedNode);
void deleteCase5(RBTree *tree, Node *unbalancedNode);
void deleteCase6(RBTree *tree, Node *unbalancedNode);


Node* findInRBTree(const RBTree *tree, const void *data, Node *curNode);
void forEachRBNode(Node *curNode, forEachFunc func, void *args, int *successOrNot);
void freeNode(RBTree *tree, Node *node);


//---------------------------------------HELP FUNCTIONS-----------------------------------------------------------//
/**
 * Returns the Color of the node, including for leaves (NULLs)
 */
Color colorOf(Node *node)
{
    if (node)
    {
        return node->color;
    }
    return BLACK;
}

/**
 * Recolors up to 3 nodes. If less nodes need recoloring, NULL can be supplied
 */
void recolor(Node *node1, Node *node2, Node *node3)
{
    if (node1)
    {
        if (node1->color == RED)
        {
            node1->color = BLACK;
        }
        else
        {
            node1->color = RED;
        }
    }
    if (node2)
    {
        if (node2->color == RED)
        {
            node2->color = BLACK;
        }
        else
        {
            node2->color = RED;
        }
    }
    if (node3)
    {
        if (node3->color == RED)
        {
            node3->color = BLACK;
        }
        else
        {
            node3->color = RED;
        }
    }
}

/**
 * Returns the type of child that the node is. (There's the right child, and then there's... the WRONG child. Or an adult)
 * @param node Assumes it is not NULL.
 */
childType whichChild(Node *node)
{
    if (!node->parent)
    {
        return NOT_A_CHILD; //aka an adult
    }
    if (node->parent->right && node->parent->right == node)
    {
        return RIGHT_CHILD;
    }
    if (node->parent->left && node->parent->left == node)
    {
        return LEFT_CHILD;
    }
    return NOT_A_CHILD; //Note: the algorithm should not get here since we assume node is not NULL.
}

/**
 * Rotates the tree to the right (shocker, I know)
 * Note: This algorithm does not change the pointer to which nodeToRotate was pointing.
 * However, I can't use const since it changes other things about nodeToRotate (like the parent and etc)
 * @param nodeToRotate This is the node that is "on top" and goes down to the left
 */
void rotateToRight(RBTree *tree, Node *nodeToRotate)
{
    if (!nodeToRotate  || !nodeToRotate->left )
    {
        return;
    }
    Node *leftChild = nodeToRotate->left;
    Node *parentNode = nodeToRotate->parent;
    Node *subTree = NULL; //The subtree that needs to be moved over to the other side.
    if (leftChild->right)
    {
        subTree = leftChild->right;
    }
    leftChild->right = nodeToRotate;
    leftChild->parent = parentNode;
    if (parentNode)
    {
        if (whichChild(nodeToRotate) == RIGHT_CHILD)
        {
            parentNode->right = leftChild;
        }
        else if (whichChild(nodeToRotate) == LEFT_CHILD)
        {
            parentNode->left = leftChild;
        }
    }
    nodeToRotate->parent = leftChild;
    nodeToRotate->left = subTree;
    if (subTree)
    {
        subTree->parent = nodeToRotate;
    }
    if (nodeToRotate == tree->root)
    {
        tree->root = nodeToRotate->parent; //Update the root
    }
}

/**
 * Rotates the tree to the left (different side baby!)
 * Note: This algorithm does not change the pointer to which nodeToRotate was pointing.
 * However, I can't use const since it changes other things about nodeToRotate (like the parent and etc)
 * @param nodeToRotate This is the node that is "on top" and goes down to the right
 */
void rotateToLeft(RBTree *tree, Node *nodeToRotate)
{
    if (!nodeToRotate || !nodeToRotate->right)
    {
        return;
    }
    Node *rightChild = nodeToRotate->right;
    Node *parentNode = nodeToRotate->parent;
    Node *subTree = NULL; //the subtree that will be moved over to the other side
    if (rightChild->left)
    {
        subTree = rightChild->left;
    }
    rightChild->left = nodeToRotate;
    rightChild->parent = parentNode;
    if (parentNode)
    {
        if (whichChild(nodeToRotate) == RIGHT_CHILD)
        {
            parentNode->right = rightChild;
        }
        else if (whichChild(nodeToRotate) == LEFT_CHILD)
        {
            parentNode->left = rightChild;
        }
    }
    nodeToRotate->parent = rightChild;
    nodeToRotate->right = subTree;
    if (subTree)
    {
        subTree->parent = nodeToRotate;
    }
    if (nodeToRotate == tree->root)
    {
        tree->root = nodeToRotate->parent; //update the root
    }
}


/**
 * Finds and returns curNode's sibling (maybe sister maybe brother, depends on the time of day)
 */
Node *findSiblingNode(Node *curNode)
{
    if (whichChild(curNode) == NOT_A_CHILD)
    {
        return NULL;
    }
    else if (whichChild(curNode) == LEFT_CHILD)
    {
        if (!curNode->parent->right )
        {
            return NULL;
        }
        return curNode->parent->right;
    }
    else
    {
        if (!curNode->parent->left )
        {
            return NULL;
        }
        return curNode->parent->left;
    }
}


/**
 * Only gets the successor if the successor is deeper in the tree
 * @param curNode Assumes the right child exists
 * @return The inorder successor
 */
Node* getSuccessor(Node *curNode)
{
    curNode = curNode->right;
    while (curNode->left)
    {
        curNode = curNode->left;
    }
    return curNode;
}


/**
 * Doxygen comment in the header
 */
RBTree *newRBTree(CompareFunc compFunc, FreeFunc freeFunc)
{
    if (compFunc == NULL || freeFunc == NULL)
    {
        return NULL;
    }
    RBTree *newTree = (RBTree*) malloc(sizeof(RBTree));
    newTree->root = NULL;
    newTree->compFunc = compFunc;
    newTree->freeFunc = freeFunc;
    newTree->size = 0;
    return newTree;
}

//---------------------------------------INSERT FUNCTIONS-----------------------------------------------------------//


/**
 * Doxygen comment in the header
 */
int insertToRBTree(RBTree *tree, void *data)
{
    if (!tree  || !data )
    {
        return FAILURE;
    }
    Node *newNode = (Node*) malloc(sizeof(Node));
    if (!newNode )
    {
        return FAILURE;
    }
    newNode->data = data;
    newNode->color = RED;
    newNode->left = NULL;
    newNode->right = NULL;
    if (!tree->root)
    {
        newNode->color = BLACK;
        newNode->parent = NULL;
        tree->root = newNode;
        tree->size++;
        return SUCCESS;
    }
    if (insertToBST(tree, data, tree->root, newNode) == FAILURE)
    {
        return FAILURE;
    }
    if (colorOf(newNode->parent) == RED) //This means we have two red nodes in a row which is a problem that we have to fix!
    {
        fixInaccuraces(tree, newNode);
    }
    tree->root->color = BLACK;
    tree->size++;
    return SUCCESS;
}

/**
 * Inserts the node to the tree regularly (as if it was a normal BST tree)
 * @return FAILURE if something goes wrong (like the data doesn't exist in the tree), SUCCESS otherwise.
 */
int insertToBST(RBTree *tree, void *data, Node *cur, Node *nodeToInsert)//helper to insertToRBTree
{
    if (!cur)
    {
        return FAILURE;
    }
    if (tree->compFunc(data, cur->data) == 0)
    {
        return FAILURE;
    }
    if (tree->compFunc(data, cur->data) < 0)
    {
        if (!cur->left )
        {
            cur->left = nodeToInsert;
            nodeToInsert->parent = cur;
            return SUCCESS;
        }
        return insertToBST(tree, data, cur->left, nodeToInsert);
    }
    else
    {
        if (!cur->right )
        {
            cur->right = nodeToInsert;
            nodeToInsert->parent = cur;
            return SUCCESS;
        }
        return insertToBST(tree, data, cur->right, nodeToInsert);
    }
}



/**
 * Fixes the inaccuraces caused by the insertion.
 * This function is called when there are two consecutive RED nodes.
 * @param cur The inaccurate node
 */
void fixInaccuraces(RBTree *tree, Node *cur)
{
    Node *parent = cur->parent;
    Node *grandparent = cur->parent->parent;
    if (!parent || !grandparent )//if parent or grandparent  doesn't exist
    {
        return;
    }
    //Case 1
    if (whichChild(parent) == LEFT_CHILD)
    {
        if (colorOf(grandparent->right) == BLACK) //if cur's uncle is BLACK
        {
            /*This case: (Triangle)
             *         cur.grandparent
             *          /
             *     cur.parent
             *            \
             *            cur
             */
            if (grandparent->left->right && grandparent->left->right == cur)
            {
                rotateToLeft(tree, parent); //check if fucks up
                cur = cur->left; //Pushes the problem down to the line case below
                //Updates the parent and grandparent
                parent = cur->parent;
                grandparent = cur->parent->parent;
            }
            /*This case: (Line)
             *         cur.grandparent
             *        /
             *     cur.parent
             *    /
             * cur
             */
            if (grandparent->left->left && grandparent->left->left == cur)
            {
                rotateToRight(tree, grandparent);
                recolor(cur->parent, cur->parent->right, NULL);
            }
        }
        else if (grandparent->right->color == RED) //if cur's uncle is RED
        {
            recolor(grandparent->right, grandparent, parent);
        }
    }
    //Case 2 (mirror of 1)
    else if (whichChild(parent) == RIGHT_CHILD)
    {
        if (colorOf(grandparent->left) == BLACK)
        {
            /*This case: (Triangle)
             * cur.grandparent
             *          \
             *          cur.parent
             *           /
             *         cur
             */
            if (grandparent->right->left && grandparent->right->left == cur) //If the first one fails the algorithm moves on,
                //doesnt try to check for the second one.
            {
                rotateToRight(tree, parent);
                cur = cur->right;
                //Update the parent and grandparent
                parent = cur->parent;
                grandparent = cur->parent->parent;
            }
            /*This case: (Line)
             * cur.grandparent
             *          \
             *          cur.parent
             *             \
             *             cur
             */
            if (grandparent->right->right && grandparent->right->right == cur)
            {
                rotateToLeft(tree, grandparent);
                recolor(cur->parent, cur->parent->left, NULL);
            }
        }
        else if (grandparent->left->color == RED) //if cur's uncle is RED
        {
            recolor(grandparent->left, grandparent, parent);
        }
    }
    while (cur->parent) //Checking for double RED nodes recursively through the whole tree
    {
        if (colorOf(cur->parent) == RED && colorOf(cur->parent->parent) == RED)
        {
            fixInaccuraces(tree, cur->parent);
        }
        cur = cur->parent;
    }
}



//---------------------------------------DELETE FUNCTIONS-----------------------------------------------------------//

/**
 * Doxygen comment in the header
 */
int deleteFromRBTree(RBTree *tree, void *data)
{
    if (!tree || !data )
    {
        return FAILURE;
    }
    Node *nodeToDelete = findInRBTree(tree, data, tree->root);
    if (!nodeToDelete)
    {
        return FAILURE;
    }
    if (nodeToDelete->left && nodeToDelete->right)
    {
        Node *copyNode = nodeToDelete;
        nodeToDelete = getSuccessor(nodeToDelete);
        void *temp = copyNode->data;
        copyNode->data = nodeToDelete->data;
        nodeToDelete->data = temp; //Switches the data
    }
    if (colorOf(nodeToDelete) == RED)
    {
        deleteRedNode(tree, nodeToDelete);
        tree->size--;
        return SUCCESS;
    }
    else //nodeToDelete is BLACK
    {
        if (oneChildCase(tree, nodeToDelete) == SUCCESS)
        {
            tree->size--;
            return SUCCESS;
        }
        if (twoNullChildrenCase(tree, nodeToDelete) == SUCCESS)
        {
            tree->size--;
            return SUCCESS;
        }
    }
    return FAILURE;
}

/**
 * Deletes a RED node
 * @param nodeToDelete Expects a RED node
 */
void deleteRedNode(RBTree *tree, Node *nodeToDelete)
{
    if (whichChild(nodeToDelete) == RIGHT_CHILD)
    {
        nodeToDelete->parent->right = NULL;
        tree->freeFunc(nodeToDelete->data);
        free(nodeToDelete);
        nodeToDelete = NULL;
    }
    else if (whichChild(nodeToDelete) == LEFT_CHILD)
    {
        nodeToDelete->parent->left = NULL;
        tree->freeFunc(nodeToDelete->data);
        free(nodeToDelete);
        nodeToDelete = NULL;
    }
    else //nodeToDelete is a root
    {
        if (nodeToDelete->right)
        {
            tree->root = nodeToDelete->right;
            tree->root->parent = NULL;
            tree->freeFunc(nodeToDelete->data);
            free(nodeToDelete);
            nodeToDelete = NULL;
        }
        if (nodeToDelete->left)
        {
            tree->root = nodeToDelete->left;
            tree->root->parent = NULL;
            tree->freeFunc(nodeToDelete->data);
            free(nodeToDelete);
            nodeToDelete = NULL;
        }
        else
        {
            tree->root = NULL;
            tree->freeFunc(nodeToDelete->data);
            free(nodeToDelete);
            nodeToDelete = NULL;
        }
    }
}



/**
 * The node is black and it has one red child.
 * @param nodeToDelete
 * @return SUCCESS if the case matches and the node was successfully deleted, FAILURE otherwise.
 */
int oneChildCase(RBTree *tree, Node *nodeToDelete)
{
    if (!nodeToDelete->left  && colorOf(nodeToDelete->right) == RED)
    {
        if (whichChild(nodeToDelete) == RIGHT_CHILD)
        {
            nodeToDelete->parent->right = nodeToDelete->right;
            nodeToDelete->right->color = BLACK; //Recolor
            nodeToDelete->right->parent = nodeToDelete->parent;
        }
        else if (whichChild(nodeToDelete) == LEFT_CHILD)
        {
            nodeToDelete->parent->left = nodeToDelete->right;
            nodeToDelete->right->color = BLACK; //Recolor
            nodeToDelete->right->parent = nodeToDelete->parent;
        }
        else if (whichChild(nodeToDelete) == NOT_A_CHILD)
        {
            tree->root = nodeToDelete->right;
            tree->root->color = BLACK; //Recolor
            tree->root->parent = NULL;
            nodeToDelete->right = NULL; //Do I need this?
        }
        tree->freeFunc(nodeToDelete->data);
        free(nodeToDelete);
        nodeToDelete = NULL;
        return SUCCESS;
    }
    if (!nodeToDelete->right  && colorOf(nodeToDelete->left) == RED)
    {
        if (whichChild(nodeToDelete) == RIGHT_CHILD)
        {
            nodeToDelete->parent->right = nodeToDelete->left;
            nodeToDelete->left->color = BLACK; //Recolor
            nodeToDelete->left->parent = nodeToDelete->parent;
        }
        else if (whichChild(nodeToDelete) == LEFT_CHILD)
        {
            nodeToDelete->parent->left = nodeToDelete->left;
            nodeToDelete->left->color = BLACK; //Recolor
            nodeToDelete->left->parent = nodeToDelete->parent;
        }
        else if (whichChild(nodeToDelete) == NOT_A_CHILD)
        {
            tree->root = nodeToDelete->left;
            tree->root->color = BLACK; //Recolor
            tree->root->parent = NULL;
            nodeToDelete->left = NULL; //Do I need this?
        }
        tree->freeFunc(nodeToDelete->data);
        free(nodeToDelete);
        nodeToDelete = NULL;
        return SUCCESS;
    }
    return FAILURE;
}

/**
 * Node color is black and it has two NULL children
 * @param tree
 * @param nodeToDelete
 * @return
 */
int twoNullChildrenCase(RBTree *tree, Node *nodeToDelete)
{
    if (!nodeToDelete->right  && !nodeToDelete->left )
    {
        Node *cpyPointerToNode = nodeToDelete;
        deleteCase1(tree, nodeToDelete); //This may change the nodeToDelete pointer
        deleteNodeWith2NULLChildren(tree, cpyPointerToNode);
        return SUCCESS;
    }
    return FAILURE;
}

/**
 * Deletes a node with 2 NULL children.
 */
void deleteNodeWith2NULLChildren(RBTree *tree, Node *unbalancedNodeToDelete)
{
    if (!unbalancedNodeToDelete->parent)
    {
        tree->root = NULL;
        tree->freeFunc(unbalancedNodeToDelete->data);
        free(unbalancedNodeToDelete);
        unbalancedNodeToDelete = NULL;
    }
    else
    {
        if (whichChild(unbalancedNodeToDelete) == LEFT_CHILD)
        {
            unbalancedNodeToDelete->parent->left = NULL;
            tree->freeFunc(unbalancedNodeToDelete->data);
            free(unbalancedNodeToDelete);
            unbalancedNodeToDelete = NULL;
        }
        else if (whichChild(unbalancedNodeToDelete) == RIGHT_CHILD)
        {
            unbalancedNodeToDelete->parent->right = NULL;
            tree->freeFunc(unbalancedNodeToDelete->data);
            free(unbalancedNodeToDelete);
            unbalancedNodeToDelete = NULL;
        }
    }

}

/**
 * Case 1: When it is the root
 * Termianl case: the the tree is no longer unbalanced after fix if this case applies
 */
void deleteCase1(RBTree *tree, Node *unbalancedNode) //Terminal case
{
    if (unbalancedNode == tree->root) //node is a root
    {
        unbalancedNode->color = BLACK;
        return;
    }
    deleteCase2(tree, unbalancedNode);
}

/**
 * Case 2: when the parent is black and the sibling is red
 */
void deleteCase2(RBTree *tree, Node *unbalancedNode)
{
    Node *sibling = findSiblingNode(unbalancedNode);
    Node *parent = unbalancedNode->parent;
    if (colorOf(parent) == BLACK && colorOf(sibling) == RED)
    {
        if (whichChild(unbalancedNode) == LEFT_CHILD)
        {
            rotateToLeft(tree, parent);
            Node *grandparent = unbalancedNode->parent->parent;
            recolor(grandparent, parent, NULL); //parent does not change
        }
        else if (whichChild(unbalancedNode) == RIGHT_CHILD)
        {
            rotateToRight(tree, parent);
            Node *grandparent = unbalancedNode->parent->parent;
            recolor(grandparent, parent, NULL); //parent does not change
        }
    }
    deleteCase3(tree, unbalancedNode);
}

/**
 * Case 3: When the parent, sibling, and sibling's children are all black
 * This is a recursive case, it bubbles the problem up to the parent and starts
 * case 1
 */
void deleteCase3(RBTree *tree, Node *unbalancedNode)
{
    Node *sibling = findSiblingNode(unbalancedNode);
    Node *parent = unbalancedNode->parent;
    if (colorOf(parent) == BLACK && colorOf(sibling) == BLACK)
    {
        if (sibling) //needed?
        {
            if (colorOf(sibling->right) == BLACK && colorOf(sibling->left) == BLACK)
            {
                sibling->color = RED;
                deleteCase1(tree, parent); //Bubbled the problem up to the parent
            }
        }
    }
    deleteCase4(tree, unbalancedNode);

}

/**
 * Case 4: When parent is red, and sibling and sibling's children are black.
 * Termianl case: the the tree is no longer unbalanced after fix if this case applies
 */
void deleteCase4(RBTree *tree, Node *unbalancedNode)
{
    Node *sibling = findSiblingNode(unbalancedNode);
    Node *parent = unbalancedNode->parent;
    if (colorOf(parent) == RED && colorOf(sibling) == BLACK)
    {
        if (sibling)
        {
            if (colorOf(sibling->left) == BLACK && colorOf(sibling->right) == BLACK)
            {
                recolor(parent, sibling, NULL);
                return;
            }
        }
    }
    deleteCase5(tree, unbalancedNode);
}

/**
 * Case 5: When sibling is black and one of its children are red and the other black (depends on the case or its mirror)
 */
void deleteCase5(RBTree *tree, Node *unbalancedNode)
{
    Node *sibling = findSiblingNode(unbalancedNode);
    if (colorOf(sibling) == BLACK)
    {
        if (sibling) //needed?
        {
            if (whichChild(unbalancedNode) == LEFT_CHILD)
            {
                if (colorOf(sibling->left) == RED && colorOf(sibling->right) == BLACK)
                {
                    rotateToRight(tree, sibling);
                    sibling->parent->color = BLACK;
                    sibling->color = RED;
                }
            }
            else if (whichChild(unbalancedNode) == RIGHT_CHILD)
            {
                if (colorOf(sibling->right) == RED && colorOf(sibling->left) == BLACK)
                {
                    rotateToLeft(tree, sibling);
                    sibling->parent->color = BLACK;
                    sibling->color = RED;
                }
            }
        }
    }
    deleteCase6(tree, unbalancedNode);
}

/**
 * Case 6: When sibling is black and one of its child is red (depends on the case or its mirror)
 * Termianl case: the the tree is no longer unbalanced after fix if this case applies
 */
void deleteCase6(RBTree *tree, Node *unbalancedNode) //Terminal case
{
    Node *sibling = findSiblingNode(unbalancedNode);
    Node *parent = unbalancedNode->parent;
    if (colorOf(sibling) == BLACK)
    {
        if (sibling) //needed?
        {
            if (whichChild(unbalancedNode) == LEFT_CHILD)
            {
                if (colorOf(sibling->right) == RED)
                {
                    rotateToLeft(tree, parent); //parent does not change
                    Node *grandparent = unbalancedNode->parent->parent;
                    grandparent->color = unbalancedNode->parent->color;
                    parent->color = BLACK;
                    grandparent->right->color = BLACK;
                }
            }
            else if (whichChild(unbalancedNode) == RIGHT_CHILD)
            {
                if (colorOf(sibling->left) == RED)
                {
                    rotateToRight(tree, unbalancedNode->parent); //parent does not change
                    Node *grandparent = unbalancedNode->parent->parent;
                    grandparent->color = unbalancedNode->parent->color;
                    parent->color = BLACK;
                    grandparent->left->color = BLACK;
                }
            }
        }
    }
}



//---------------------------------------FIND FUNCTIONS-----------------------------------------------------------//

/**
 *  Doxygen comment in the header
 */
int RBTreeContains(const RBTree *tree, const void *data)
{
    if (!tree  || !data )
    {
        return FAILURE;
    }
    if (findInRBTree(tree, data, tree->root))
    {
        return SUCCESS;
    }
    return FAILURE;
}

/**
 * Helper to RTBTreeContains. Goes through all of the tree.
 * @param data Data to find
 * @return The node that was found (if it was found), null on a leaf
 */
Node* findInRBTree(const RBTree *tree, const void *data, Node *curNode)
{
    if (!curNode)
    {
        return NULL;
    }
    if (tree->compFunc(data, curNode->data) == 0) //data == curNode->data
    {
        return curNode;
    }
    if (tree->compFunc(data, curNode->data) < 0) //data < curNode->data
    {
        return findInRBTree(tree, data, curNode->left);
    }
    else //data > curNode->data
    {
        return findInRBTree(tree, data, curNode->right);
    }
}



//---------------------------------------OTHER FUNCTIONS-----------------------------------------------------------//

/**
 *  Doxygen comment in the header
 */
int forEachRBTree(const RBTree *tree, forEachFunc func, void *args)
{
    if (!tree)
    {
        return FAILURE;
    }
    if (!tree->root)
    {
        return SUCCESS;
    }
    int successOrNot = 1;
    forEachRBNode(tree->root, func, args, &successOrNot);
    return successOrNot;
}

/**
 * Goes through all of the tree and applies the func on all the nodes, in-order succession.
 * @param successOrNot If one of the func application fails, it is 0. If all the func application succeeds, it is 1.
 */
void forEachRBNode(Node *curNode, forEachFunc func, void *args, int *successOrNot)
{
    if (*successOrNot == 0)
    {
        return;
    }
    if (curNode->left)
    {
        forEachRBNode(curNode->left, func, args, successOrNot);
    }
    *successOrNot = func(curNode->data, args);
    if (curNode->right)
    {
        forEachRBNode(curNode->right, func, args, successOrNot);
    }
}


/**
 *  Doxygen comment in the header
 */
void freeRBTree(RBTree **tree)
{
    if (tree && *tree)
    {
        freeNode(*tree, (*tree)->root);
        free(*tree);
        *tree = NULL;
    }
}

/**
 * Frees the node in all of the tree
 */
void freeNode(RBTree *tree, Node *curNode)
{
    if (curNode)
    {
        freeNode(tree, curNode->left);
        freeNode(tree, curNode->right);
        if (curNode->data)
        {
            tree->freeFunc(curNode->data);
            curNode->data = NULL;
        }
        free(curNode);
        curNode = NULL;
    }
}


