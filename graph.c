#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "graph.h"

// Todo free malloc arg structs
// make threads wait on findfilesegmentswrapper

// Not that trivial, just return for today.
// This function needs to take a sentence and
// eviscerate it of all 'filler words', 
// returning a data structure with only 
// the words that matter stored sequentially.
void build_word_list(vertex* vert)
{
	return;
}

// function add_vertex: 
//	The adjacency list has 2 parts:
//	1. An array of pointers to each vertex
//	2. A list of edges within each vertex
// 	This function adds a vertex to the first.
// return: void	    	                      
void add_vertex(char* acro)
{
	vertex* vert;
	int i=0;

	init(0, (char**)&vert, INIT_VERTEX);
	vert->acro->acro = (char*)malloc(sizeof(acro));
	if (vert->acro->acro == NULL)
		assert(0);
	strcpy(vert->acro->acro, acro);
	build_word_list(vert);

	if (USE_HASHMAP)  
		push(vert);
	else
		RBInsert(&root, vert, NULL);
}

// Function: main 
// Synopsis: grabs user input and searches.
// return: void 
int main()
{
	graph agraph;
	int i, nline, nacro, c=0;
	vertex** vert;
	char acro[100];
	FILE *fp;
	thread_pool* tpcb;
	future* qfuture;

	// Fork off a thread to handle acronym searche the 
	// expensive way while while we fuel up our graph.
	tpcb = thread_pool_new(10);
	qfuture = thread_pool_submit(tpcb, expensive_file_search_wrapper, (void*)tpcb);

	init(0, (char**)(&agraph), INIT_GRAPH);
	init(MAX_VERTICES, (char**)vstack, INIT_STACK);
	init_filler_list();
	root = NULL;

	fp=fopen("dictionary", "r");
	for(nline=0; fgets(acro, 100, fp) && CREATETREE; nline++) {
		add_vertex(acro);
	}
	fclose(fp);

	// really need to implement threadpool kill
	future_get(qfuture);	

	return 0;
}

