#include "graph.h"
#include <stdio.h>
#include <stdlib.h>

// Processes all vertices as follows: 
// 1. Pop current vertex from FIFO queue
// 2. Add all its unvisited edge vertices to queue
// 3. Mark as visited
// 4. Goto 1. or return if theres nothing left in queue
void bfs(graph* agraph)
{
	edge* current;
	edge* temp = NULL;
	int i=0;
	
	for (current = dequeue(); current != NULL; current = dequeue()) {
		printf("\n dequeued %d: enqueueing",current->eid);
		temp = current->next;
		while(temp != NULL) {
			if (!(visited[temp->eid])) {
				visited[temp->eid] = 1;
				enqueue(&agraph->edges[temp->eid]);
				printf("%d,",temp->eid);
			}
			temp = temp->next;
		}
		visited[current->eid] = 1;
	}
}

// Recursively visits children until all
// children of first node have been visited.
// 1. Visit child recursively
// 2. get next child of last visited node
// 3. Do 1.
// 3. if no children or child already visited, return
void dfs(graph* agraph, edge* node)
{
	edge* temp;
	if (node->fvisited || (node->next == NULL))
		return;

	node->fvisited = true;
	temp = node->next;
	while(temp != NULL) {
		dfs(agraph, &(agraph->edges[temp->eid]));	
		temp = temp->next;
	}
}

// Each vertex has a list of all other vertices it is
// connected to. Since graph is bidirectional each 
// vertex is duplicated for every edge it has through 
// a recursive call.
void add_edge(int sedge, int dedge, graph* agraph, bool bidi)
{
	edge* destedge = (edge*)malloc(sizeof(edge));
	destedge->eid = dedge;
	destedge->fvisited = false;
	destedge->next = NULL;	
	
	agraph->edges[sedge].eid = sedge;
	destedge->next = agraph->edges[sedge].next;
	agraph->edges[sedge].next = destedge;
	
	if (bidi)
		add_edge(dedge, sedge, agraph, false);
	
	return;	
}

// Initialize an array of vertices. For
// each member of array add edges.
graph* build_graph(int* nodes, int* edges)
{
	graph* agraph = (graph*)malloc(sizeof(graph));
	int i=0, a, b;
	for (i=1; i<=MAX_NODES; i++) {
		agraph->edges[i].eid = i;
		agraph->edges[i].fvisited = false;
		agraph->edges[i].next = NULL;
	}

	for (i=0; i<MAX_EDGES; i = i+2) {
		a = edges[i];
		b = edges[i+1];
		add_edge(a, b, agraph, true);
	}
	return agraph;
}

int main()
{
	int nodes[MAX_NODES] = {1,2,3,4,5,6};
	int edges[MAX_EDGES] = {1,6,1,5,1,2,2,3,2,5,5,4,3,4};
	graph *agraph;
	edge *temp;
	int i=0;
	agraph = build_graph(nodes, edges);

	for (i=1; i<=MAX_NODES; i++) {
		printf("\n %d:", agraph->edges[i].eid);
		temp = agraph->edges[i].next;
		while(temp != NULL) {
			printf("%d,",temp->eid);
			temp = temp->next;
		}
			
 	}

	init_queue();
	enqueue(&(agraph->edges[1]));	
	//bfs(agraph);
	dfs(agraph, &(agraph->edges[1]));
	return 0;
}
