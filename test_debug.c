#include "buildfillerlist.h"

// Prints a list of words associated with this acronym. Doesnt include fillers.
void print_word_list(acrovert* x)
{
	sentence_word *w = x->acro->wordlist;
	printf("(");
	while(w) {
		printf("%s ",w->word);
		w=w->next;
	}
	printf(")");
}

// Prints each acronym of each edge in the kruskal edge sorted list.
void print_kruskal_edge_list()
{
	edge *e = sorted_edge_list;
	while(e) {
		printf("\n");
		assert(e->x && e->y);
		printf("acronyms %s ", e->x->acro->acro);
		print_word_list(e->x);
		printf("and %s", e->y->acro->acro);
		print_word_list(e->y);
		printf("have weight %d",e->weight);											
		e=e->next;
		printf("\n");
	}
}

// prints given vertex, acronym, word list and repeats for all edges of vertex.
void print_edge_acro(acrovert* x, bool fprintwordlists)
{
	edge* acroedge = x->next;
	acrovert* y=NULL;
	printf("[%s ",x->acro->acro);
	
	if (fprintwordlists) 
		print_word_list(x);

	printf("]:");
	printf("\b");

	while(acroedge) {
		y=acroedge->y;
		printf("-> %s",y->acro->acro);
		if (y->fvisited)
			printf("|");
		if (fprintwordlists)
			print_word_list(y);
		acroedge = acroedge->next;
	}
	printf("\n");
}
