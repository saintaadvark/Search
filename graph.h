#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>
#define MAX_VERTICES 500
#define MAX_EDGES 1000
#define FILLER_WORDS 500
#define INIT_STACK 0X00000001
#define INIT_QUEUE 0x00000002
#define INIT_GRAPH 0x00000004
#define INIT_VERTEX 0x00000008
#define INIT_ALLOCATE 0x00000010
#define LEFT_ROTATE 1
#define RIGHT_ROTATE 0
#define TRUE 1
#define FALSE 0
#define USE_HASHMAP 0	// Use a tree or hashmap for adjacency list?
#define DBG 0

// Given a pointer to a vertex in the tree lookup representation 
// get the address of its left and right vertices
#define GET_LEFT_REF(parentNode)((vertex**)(&((*parentNode)->left)))  
#define GET_RIGHT_REF(parentNode)((vertex**)(&(*parentNode)->right)) 

// Caller ensures that parentNode is not NULL
#define GET_LEFT(parentNode)((vertex*)((*parentNode)->left))
#define GET_RIGHT(parentNode)((vertex*)((*parentNode)->right))

enum color {red, black};

typedef struct acronym {
	char* acro;
	char* meaning;
	char* wordlist;
} acronym;

typedef struct vertex {
	struct acronym* acro;
	struct vertex* next;	// make this a union with parent
	struct vertex* left;
	struct vertex* right;
	struct vertex* parent;
	enum color color;
} vertex;

typedef struct graph {
	vertex** vstack;	// Pointer to global stack of vertices 
	int* vparents;		// Array of parents of corresponding edges
	int nvertices;		// Size of graph
} graph; 

vertex** vstack;// the hashmap, NULL if tree representation
int stacktop;	// top of stack in hashmap representation
vertex *root;	// root of tree, NULL if hashmap representation
int fillerlist[FILLER_WORDS];
int fillermax;
