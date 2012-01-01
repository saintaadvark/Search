#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>

#define FILLERWORDS 100
#define MAX_ACRO 500
#define TRUE 1
#define FALSE 0
#define KRUSKAL 1
#define BIDI 1

typedef struct sentence_word{
	char *word;
	sentence_word* next;
	short wordhash;
} sentence_word;

typedef struct {
	char *acro;
	sentence_word *wordlist;
} acronym;

typedef struct acrovert {
	struct edge *next;
	acronym *acro;	
	struct set_union *set_parent;
	bool fvisited;
} acrovert;

typedef struct set_union {
	acrovert *set_parent;
	int size;
} set_union;

typedef struct edge {
	struct edge* next;
	struct acrovert* y;
	struct acrovert* x;
	int weight;
} edge; 

void add_edge_sorted(edge *newedge);
void build_maximum_spanning_tree();
void add_edge(acrovert* x, acrovert* y, int w, bool bidi, bool fkruskal);

extern int fillerwords[FILLERWORDS];
extern struct acrovert *vstack[MAX_ACRO];
extern int stacktop;
extern int fillerwordstop;
extern edge *sorted_edge_list;
