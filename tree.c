#include "graph.h"
#include<string.h>

// O(n)
void Traverse(vertex *node)
{
	if (node == NULL) {
		return;
	}

	// Simple in order traversal
	Traverse(node->left);
	printf("%s %s \n----\n",node->acro->acro, node->acro->meaning);
	Traverse(node->right);
}

// O(logn)
void Insert(vertex **parentNode, vertex *childNode)
{
	if (!(*parentNode)) {
		*(parentNode) = childNode;
		return;
	}

	int strvalue = strcmp((*parentNode)->acro->acro, childNode->acro->acro);
	if (strvalue < 0) {
		Insert(GET_LEFT_REF(parentNode), childNode);
	} else if (strvalue > 0) { 
		Insert(GET_RIGHT_REF(parentNode), childNode);
	} else {
		printf("===collison\n");
	}
	return;
}

// O(logn)
vertex** Search(vertex **parentNode, char* string)
{
	if(!(*parentNode))
		return NULL;

	int strvalue = strncmp((*parentNode)->acro->acro, string, strlen(string));
//	printf("comparing %s and %s, value %d\n",(*parentNode)->acro->acro, string, strvalue);
	if (strvalue == 0) {
		return parentNode;
	}
	
	if (strvalue < 0)
		return Search((GET_LEFT_REF(parentNode)), string);
	else if (strvalue > 0)
		return Search((GET_RIGHT_REF(parentNode)), string);
}

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
