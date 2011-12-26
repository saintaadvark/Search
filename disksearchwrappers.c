#include "graph.h"

void* SearchFileWrapper(void* args)
{	
	SearchFile((argstruct*)args->file, (argstruct*)args->acronym);
}

void* FindFileSegmentsWrapper(void* args)
{
	FindFileSegments((argstruct*)args->file);
}
