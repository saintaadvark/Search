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
#define DBG 1
#define IS_LEFTCHILD 1
#define IS_RIGHTCHILD 0
#define CREATEGRAPH 1

// Tree traversal macros
// Given a pointer to a vertex in the tree lookup representation 
// get the address of its left and right vertices
#define GET_LEFT_REF(parentNode)((vertex**)(&((*parentNode)->left)))  
#define GET_RIGHT_REF(parentNode)((vertex**)(&(*parentNode)->right)) 

// Caller ensures that parentNode is not NULL
#define GET_LEFT(parentNode)((vertex*)((*parentNode)->left))
#define GET_RIGHT(parentNode)((vertex*)((*parentNode)->right))

enum color {red, black};
enum nodetype {treeroot, lchild, rchild};

// Basic unit of an acronym.
typedef struct acronym {
	char* acro;
	char* wordlist;
} acronym;

// Basic unit of graph. Vertex could
// be stored in a tree or an adjacency list.
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

typedef struct argstruct {
	char* file;
	char* acronym;
} argstruct;

// Defined in tree.c
extern vertex *root;	// root of tree, NULL if hashmap representation
extern void RBInsert(vertex **parentNode, vertex *childNode, vertex *parentBase);
extern void Traverse(vertex *node);
extern vertex** Search(vertex **parentNode, char* string);
extern vertex* root;

// Defined in graph_book.c
extern int fillerlist[FILLER_WORDS];
extern int fillermax;
extern vertex** vstack;		// the hashmap, NULL if tree representation
extern int stacktop;		// top of stack in hashmap representation
extern void init_filler_list();	
extern short is_upper(short ch);
extern void init(int arraysize, char* array[], int bitmask);
extern void push(vertex* vert);

// Defined in tree_book.c
extern enum nodetype classifychild(vertex* child);
extern void updateparent(vertex* newchild, vertex* oldchild);

// Defined in threadPool.c
extern struct thread_pool* thread_pool_new (int nthreads);
extern struct future* thread_pool_submit (thread_pool* tpcb, void*(*func)(void*), void* data);
extern void* future_get(future* qfuture);

// Defined in disksearch.c
extern void FindFileSegments(char* file);
extern void SearchFile(char* file, char* acronym);
extern void* FindFileSegmentsWrapper(void* args);
extern void* SearchFileWrapper(void* args);
