#include "graph.h"
#include <string.h>

vertex* root;

// function: Traverse
// synopsis: Simple in order traversal
// O(n)
// return: void
void Traverse(vertex *node)
{
	if (node == NULL) {
		return;
	}

	Traverse(node->left);
	printf("%s \n----\n",node->acro->acro);
	if (node->parent)
		printf("===parent=%s\n----\n",node->parent->acro->acro);
	Traverse(node->right);
}

// function: Rotate
// synopsis: rigth rotate: 
//	1. left child = successor
//	2. parent = sucessors right child
//	3. sucessors right subtree = parents left subtree
// left rotate is the opposite. Each case handles seperately,
// makes it easier to debug.
// Original tree:	
//          M        
//       /    \
//      G      P
//     / \    / \
//    C   J  N   Q
// return: void
void Rotate(vertex* parent, bool dir)
{
	vertex *M, *G, *J, *P, *N;
	M = parent;
		
	//Right rotated tree:	
	//        G        
	//       / \
	//      C   M
	//	   / \
	//	  J   P
	//
	if (dir == RIGHT_ROTATE && parent->left) {
		G = M->left;
		J = G->right;

		G->right = M;
		G->parent = M->parent;
		updateparent(G, M);
		M->parent = G;
		M->left = J;
		if (J)
			J->parent = M;
	}
 
	//Left rotated tree:	
	//          P        
	//       /    \
	//      M      Q
	//     / \
	//    G   N     
	//
	if (dir == LEFT_ROTATE && parent->right) {
		P = M->right;
		N = P->left;

		P->left = M;
		P->parent = M->parent;
		updateparent(P, M);
		M->parent = P;
		M->right = N;
		if (N)
			N->parent = M;
	}
}

// function: Insert
// synopsis: binary tree insert
// O(logn)
// return: void
void Insert(vertex **parentNode, vertex *childNode, vertex *parentBase)
{
	if (!(*parentNode)) {
		*(parentNode) = childNode;
		childNode->parent = parentBase;
		return;
	}

	int strvalue = strcmp((*parentNode)->acro->acro, childNode->acro->acro);
	if (strvalue > 0) {
		Insert(GET_LEFT_REF(parentNode), childNode, *parentNode);
	} else if (strvalue < 0) { 
		Insert(GET_RIGHT_REF(parentNode), childNode, *parentNode);
	} else {
		printf("===collison\n");
		printf("%s: %s\n",(*parentNode)->acro->acro, childNode->acro->acro);
	}
	return;
}

// function: RedBlackInsert
// synpsis: does a binary insert of a 'red' node and readjusts the
// tree to have o(logn) height. I chose a rb tree because it first
// 'finds' the spot to perform the rotation using colors. Since these
// colors are just bit flips this finding operation doesnt need a lock,
// making it faster than most other trees for multi threading. 
void RBInsert(vertex **parentNode, vertex *childNode, vertex *parentBase)
{
	vertex *x = childNode, *grandx;
	enum nodetype parenttype, childtype;
	Insert(parentNode, childNode, parentBase);

	while (x != root && (x->parent->color == red)) {

		parenttype = classifychild(x->parent);
		childtype = classifychild(x);
		grandx = x->parent->parent;

		//Case1: P   becomes   P.
		//      / \	      / \
		//     N.  Q.        N   Q
		//    /	   	    /
		//   O.	           O.
		// Note: parent-child dir doesn't matter in Case 1
		if (grandx &&
		    grandx->left &&
		    grandx->right &&
		    grandx->color == black &&
		    grandx->left->color == red && 
		    grandx->right->color == red) {
	
			grandx->color = red;
			grandx->left->color = black;		
			grandx->right->color = black;
			x = grandx;
			continue;
		}
	
		if (parenttype == rchild) {
			//Case2: M   becomes	M  ready for Case3
			//	  \		 \
			//	   R.		  R.
			//	  /		   \
			//	 P.		    P.
			if (childtype == lchild) {
				Rotate(x->parent, RIGHT_ROTATE);
				continue;
			}

			//Case 3: M   becomes	R
			//	   \	       / \
			//	    R.	      M.  P.
			//	     \
			//	      P.
			if (childtype == rchild) { 
				x->parent->color = black;
				x->parent->parent->color = red;
				Rotate(x->parent->parent, LEFT_ROTATE);
				continue;
			}
		} else if (parenttype == lchild) {
			if (parenttype == lchild && childtype == rchild) {
				Rotate(x->parent, LEFT_ROTATE);
			} else if (parenttype == lchild && childtype == lchild) {
				x->parent->color = black;
				x->parent->parent->color = red;
				Rotate(x->parent, RIGHT_ROTATE);
			}
		}
	}
	root->color = black;
}

// function: Search
// synopsis: searches tree for given string
// O(logn)
/// return: address of vertex containing string
vertex** Search(vertex **parentNode, char* string)
{
	if(!(*parentNode))
		return NULL;

	int strvalue = strncmp((*parentNode)->acro->acro, string, strlen(string));
	if (strvalue == 0) {
		return parentNode;
	}
	
	if (strvalue > 0)
		return Search((GET_LEFT_REF(parentNode)), string);
	else if (strvalue < 0)
		return Search((GET_RIGHT_REF(parentNode)), string);
}


// Following are the regular tree implementations of delete, min etc.
// There is no current need for them, so Ive put off porting them to
// the Red Black tree. The cases involved in deletion seem daunting, to
// say the least. 
/*
// O(logn)
thread_t **Min(thread_t **parentNode)
{
	if (!(*parentNode))
		return NULL;
	if (GET_LEFT(parentNode) == NULL)
		return parentNode;

	return Min((GET_LEFT_REF(parentNode)));
}

// O(1), O(logn)
void Delete(thread_t **parentNode)
{
	if (!(*parentNode))
		return;
	
	thread_t *leftChild, *rightChild, **leftMostOfRight, *newSuccessor;
	leftChild = (GET_LEFT(parentNode));
	rightChild = (GET_RIGHT(parentNode));

	if (!leftChild && !rightChild)
		*parentNode = (thread_t*)NULL;
	
	if (!leftChild && rightChild)
		*parentNode = rightChild;
	else if (!rightChild && leftChild)
		*parentNode = leftChild;
	else if (rightChild && leftChild) {
		leftMostOfRight = Min(GET_RIGHT_REF(parentNode));	
		if (leftMostOfRight) {	
			newSuccessor = *leftMostOfRight;
			Delete(leftMostOfRight);
			*(GET_LEFT_REF((&newSuccessor))) = GET_LEFT(parentNode);
			*(GET_RIGHT_REF((&newSuccessor))) = GET_RIGHT(parentNode);
			*parentNode = newSuccessor;
		} 
	}
	return;
} 

int main()
{
	thread_t *tcb[10];

	// Fake priorities
	int tprio[10] = {70, 23, 56, 78, 6, 5, 45, 76, 3, 9};
	int i=0, sPrio;
	thread_t **searchTcb;
	
	// O(nlogn), O(n^2) + n*C
	root = NULL;
	for (i=0; i<10; i++) {
		tcb[i] = (thread_t*)malloc(sizeof(thread_t));
		tcb[i]->tid = i;
		tcb[i]->tprio = tprio[i];
		tcb[i]->left = NULL;
		tcb[i]->right = NULL;
		Insert(&root, tcb[i]);
	}

	// O(n)
	Traverse(root);

	while(1) {
		printf("enter priority to delete\n");	
		scanf("%d",&sPrio);
		searchTcb = Search(&root, sPrio);
		if (searchTcb == NULL) {
			printf("priority not found \n");
			break;
		} else {
			printf("-output tid: %d, tprio: %d \n",(*searchTcb)->tid, (*searchTcb)->tprio);
		}
		Delete(searchTcb);
		printf("new tree: \n");
		Traverse(root);
	}
	return 0;
}
*/
