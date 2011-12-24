#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "graph.h"
#include "graph_book.h"


// Not that trivial, just return for today
void build_word_list(vertex* vert)
{
	return;
/*
	char* string = vert->acro->meaning;
	int slen = strlen(string),curr=0,start=0;
	while(curr<slen) {
		if (string[curr] == ' ') {
			string[curr] = '\0';
			if (!fillercheck(sum)) {
				
			}
			sum = 0;
		}
		sum += (short) string[curr];
	}
	
	return;
*/
}

// function add_vertex: 
//	The adjacency list has 2 parts:
//	1. An array of pointers to each vertex
//	2. A list of edges within each vertex
// 	This function adds a vertex to the first.
// return: void	    	                      
void add_vertex(char* acro, char* meaning)
{
	vertex* vert;
	int i=0;
	init(0, (char**)&vert, INIT_VERTEX);
	vert->acro->acro = (char*)malloc(sizeof(acro));
	vert->acro->meaning = (char*)malloc(sizeof(meaning));
	strcpy(vert->acro->acro, acro);
	strcpy(vert->acro->meaning, meaning);
	
	build_word_list(vert);

	if (USE_HASHMAP)  
		push(vert);
	else
		Insert(&root, vert);

}

int main()
{
	graph agraph;
	int i, nline, nacro;
	vertex** vert;
	char acro[100], meaning[100], chbang;
	FILE *fp;
	
	init(0, (char**)(&agraph), INIT_GRAPH);
	init(MAX_VERTICES, (char**)vstack, INIT_STACK);
	init_filler_list();
	root = NULL;

	fp=fopen("internetslang2", "rw");
	for(nline=0; fgets(acro, 100, fp); nline++) {
		if (is_upper(acro[1]) == TRUE) {
			fgets(meaning, 100, fp);
			add_vertex(acro, meaning);
		}
	}
	fclose(fp);

	while (1) {
		scanf("%s",acro);
		printf("%s\n",acro);
		vert = Search(&root, acro);
		if (vert)
			printf("======%s\n",((vertex*)(*vert))->acro->meaning);
	}
	
	if (DBG)
		Traverse(root);
	
	return 0;
}
