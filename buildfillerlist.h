#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>
#include <time.h>

#define FILLERWORDS 100
#define MAX_ACRO 7000
#define TRUE 1
#define FALSE 0
#define KRUSKAL 1
#define BIDI 1
#define DBG 1
#define PRINT_WORD_LISTS 1
#define SKIP_SIZE 4

// data structure for words associated with a given acronym
typedef struct sentence_word{
	char *word;
	sentence_word* next;
	short wordhash;
} sentence_word;

// acronym and its word list
typedef struct {
	char *acro;
	sentence_word *wordlist;
} acronym;

// acronym vertex pushed into adjacency list array
typedef struct acrovert {
	struct edge *next;
	acronym *acro;	
	struct acrovert *parent;
	bool fvisited;
	int arrival_time;
	struct set_union *set_parent;
} acrovert;

// set union to computed links between strongly connected components in kruskal
typedef struct set_union {
	acrovert *set_parent;
	int size;
} set_union;

// a weighted edge between x and y
typedef struct edge {
	struct edge* next;
	struct edge* downedge;
	struct acrovert* y;
	struct acrovert* x;
	int weight;
} edge; 

// define in kruskal.c
extern int dtime;
void add_edge_sorted(edge *newedge, edge *downedge, int skiplist_index);
void build_maximum_spanning_tree();
void add_edge(acrovert* x, acrovert* y, int w, bool bidi, bool fkruskal);
acrovert* dfs(acrovert* v);
extern edge *skiplist_heads[(SKIP_SIZE+1)];

// defined in test_debug.c
void print_edge_acro(acrovert* x, bool fprintwordlist);
void print_kruskal_edge_list();

// defined in buildfillerlist.c
extern int fillerwords[FILLERWORDS];
extern struct acrovert *vstack[MAX_ACRO];
extern int stacktop;
extern int fillerwordstop;
extern edge *sorted_edge_list;

// defined in string_helper.c
extern sentence_word* get_next_word(char* sentence, int i, int len);
extern bool is_acronymline(char* acro);
extern int binarysearch(int* a, int min, int max, int element);
extern void qsort(int *a, int max, int min);
extern int documentdistance(sentence_word* w1, sentence_word* w2);
