// File: threadwrappers.c
// synopsis: contains wrappers for every function we wish 
// to post via futures to a threadpool.

#include "graph.h"

void* search_file_wrapper(void* args)
{
	search_file(((argstruct*)args)->file, ((argstruct*)args)->acronym);
	return NULL;
}

void* find_file_segments_wrapper(void* args)
{
	find_file_segments(((argstruct*)args)->file);
	return NULL;
}

void* expensive_file_search_wrapper(void* args)
{
	expensive_file_search((thread_pool*)args);
	return NULL;
}
