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
	char acro[100], meaning[100], chbang;
	FILE *fp;
	thread_pool* tpcb;
	future* qfuture;	
	argstruct *args;

	init(0, (char**)(&agraph), INIT_GRAPH);
	init(MAX_VERTICES, (char**)vstack, INIT_STACK);
	init_filler_list();
	root = NULL;

	if(CREATEGRAPH) {
		fp=fopen("dictionary", "r");
		for(nline=0; fgets(acro, 100, fp); nline++) {
			add_vertex(acro);
		}
		fclose(fp);
	}
	
	tpcb = thread_pool_new(10);
	args = (argstruct*)malloc(sizeof(argstruct));
	args->file = NULL;
	args->acronym = NULL;
	qfuture = thread_pool_submit(tpcb, FindFileSegmentsWrapper, (void*)args);

	if (!DBG) {
		while (1) {
			scanf("%s",acro);
			args = (argstruct*)malloc(sizeof(argstruct));
			args->acronym = (char*)malloc(strlen(acro));
			strcpy(args->acronym, acro); 
			printf("%s\n",args->acronym);
			thread_pool_submit(tpcb, SearchFileWrapper, (void*)args);
			/*
			vert = Search(&root, acro);
			if (vert)
				printf("======%s\n",((vertex*)(*vert))->acro->meaning);
			*/
		}
	} else {
		Traverse(root);
	}
	
	return 0;
}

