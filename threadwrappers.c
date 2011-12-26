#include "graph.h"

void* SearchFileWrapper(void* args)
{
	SearchFile(((argstruct*)args)->file, ((argstruct*)args)->acronym);
	return NULL;
}

void* FindFileSegmentsWrapper(void* args)
{
	FindFileSegments(((argstruct*)args)->file);
	return NULL;
}
