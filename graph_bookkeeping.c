/**
 * @file: disksearch.c 
 * @brief: 
 * @author:
 * @bug:
 * 
 */
#include "graph.h"
#include <string.h>
#include <assert.h>

int stacktop;		// top of stack in hashmap representation

int fillerwords[FILLERWORDS];
struct acrovert *vstack[MAX_ACRO];
int fillerwordstop;

// All helper functions need a new home
// Helper 1
void init_filler_list()
{

	return;	
}

// Helper2 
// Note: Hackyness
short is_upper(short ch)
{
	if (ch >= 0x41  && ch <= 0x5a)
		return TRUE;
	else
		return FALSE;
}

// Helper 3
// function: Init 
//	Init an array, also used to allocate memory and
//	init other structures according to the bitmask.
// return: void
void init(int arraysize, char* array[], int bitmask)
{
	int i=0, nelem;
	
	// Currently only allocats array of bytes
	if ((bitmask & INIT_ALLOCATE) != 0) {
		*(char**)array = (char*)malloc(arraysize*sizeof(char));	
		if (!array)
			goto cleanup;

		for(i=0; i<arraysize; i++)
			array[i] = NULL;
	}

	if ((bitmask & INIT_STACK) != 0) {
		stacktop = 0;	
		for (i=0; i<MAX_VERTICES; i++)
			vstack[i] = NULL;
	}


	if ((bitmask & INIT_GRAPH) != 0) {
		((graph*)(array))->vstack = vstack;
		((graph*)(array))->vparents = (int*)malloc(sizeof(int)*MAX_VERTICES);	
		((graph*)(array))->nvertices = MAX_VERTICES;
	}
	if ((bitmask & INIT_VERTEX) != 0) {
		(*(vertex**)(array)) = (vertex*)malloc(sizeof(vertex));
		if (!*(array))
			goto cleanup;
		
		((vertex*)(*array))->acro = NULL;
		((vertex*)(*array))->acro = (acronym*)malloc(sizeof(acronym));
		if (!(((vertex*)(*array))->acro))
			goto cleanup;

		((vertex*)(*array))->acro->acro = NULL;
		((vertex*)(*array))->left = NULL;
		((vertex*)(*array))->right = NULL;
		((vertex*)(*array))->next = NULL;
		((vertex*)(*array))->parent = NULL;
		((vertex*)(*array))->color = red;
	}
	
	return;
	
	cleanup:
		assert(0);
}

// Helper 4
// function: Push
//	Push a vertex pointer onto adjacency
//	list stack. 
// return: void
void push(acrovert* vert)
{
	vstack[stacktop] = vert;
	stacktop++;
}

