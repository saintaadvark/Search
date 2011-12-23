#include <stdio.h>
#define MAX_NODES 6
#define MAX_EDGES 14

typedef struct edge {
	bool fvisited;	// useless flag for bfs
	int eid;	
	edge* next;	
} edge;

typedef struct graph {
	edge edges[(MAX_NODES+1)];
	int parents[(MAX_NODES+1)];
	int nnodes;
} graph; 

edge* queue[MAX_EDGES];		// Unprocessed bfs nodes
int visited[MAX_NODES+1];	// If node has been visited dont add to queue
int qstart, qend;		

void init_queue()
{
	int i=0;
	for(i=0; i<MAX_EDGES; i++)
		queue[i] = NULL;
	for (i=0; i<MAX_EDGES+1;i++) {
		visited[i] = 0;
 	}

	qstart = 0;
	qend = 0;
}

// Dequeue FIFO, only time start = end is when
// nothing has been enqueued yet.
edge* dequeue()
{
	edge* edge;
	if (qstart == qend) {
		return NULL;	
	} else {
		if (qend > qstart) {
			edge = queue[qstart];
			queue[qstart] = NULL;
			qstart++;
			return edge;
		} else {
			printf("underflow!\n");
		}
	}
}

void enqueue(edge* edge)
{
	queue[qend] = edge;
	qend++;
}
