#include "buildfillerlist.h"

#define MIN(a,b)(a<b?a:b)
int dtime;

void add_edge_sorted(edge *newedge, edge* downedge, int skiplist_index);
static edge* pop_edge();
edge *skiplist_heads[(SKIP_SIZE+1)];
	
// dfs needs to record back edges so it can identify articulation vertices. 
// These denote words that might have poisoned the recommendation list because
// they're the only ones that connect 2 sub components of otherwise independent
// strongly connected acronyms. On return frome the dfs call we'll get the 
// address of the earliest starting vertex in our subtree, if this time is
// earlier than ours we have a backedge, otherwise it will be the time of our
// immediate child.
acrovert* dfs(acrovert* v)
{
	edge *e = NULL;
	acrovert *vback = v;
	int dfstime = 0;

	assert(v);
	v->fvisited = TRUE;
	
	v->arrival_time = dtime++;
	for(e = v->next; e != NULL; e = e->next) {
	
		// If we're going from an unvisited edge to a visited one which has 
		// entry time less than ours its a back edge.
		if (!(e->y->fvisited)) {
		
			e->y->parent = v;
			print_edge_acro(e->x, FALSE);	
			printf("\n  recursing into %s ", e->y->acro->acro);
			
			// if vback returned is an ancestor it'll have an earlier entry time 
			vback = dfs(e->y);
			dfstime = MIN(v->arrival_time, vback->arrival_time);
			
			printf("\nreturning from %s, to ",e->y->acro->acro);
			print_edge_acro(e->x, FALSE);	
			
			// If no back edges were found dfstime will be our own entry time, 
			// we dont want to break for a perfectly sane tree edge return.
			if (vback != v && (dfstime < v->arrival_time)) {
				//do backedge stuff
			}
		} else if (e->y->arrival_time < v->arrival_time) {
				
				// each vertex returns itself unless it has a back edge, in 
				// which case it returns the ancestor it has that edge with. 
				vback = e->y;
				if (vback != v->parent)
					printf("=====backedge acro %s to parent %s\n",v->acro->acro, vback->acro->acro);
		}
	}
	return vback;
}

// find 'root' of a given vertex, 'root' being the first vertex in a connected
// subcomponent
acrovert* find (acrovert *v)
{
	assert(v->set_parent);
	while(v && (v->set_parent->set_parent != v))
		v = v->set_parent->set_parent;
	return v;
}

// union 2 connected subcomponents of a graph, parenting smaller to larger
void component_union(acrovert *v1, acrovert *v2)
{
	assert(v1 && v2);
	int net_size = v1->set_parent->size + v2->set_parent->size;
	if (v1->set_parent->size >= v2->set_parent->size) {
		v2->set_parent->set_parent = v1;
	} else {
		v1->set_parent->set_parent = v2;
	}
	v1->set_parent->size = net_size;
	v2->set_parent->size = net_size;
	return;
}

// go through list of sorted edges and add those that dont create cycles in the 
// same component. Since this isnt a connected graph we cant check for number 
// of edges, so we keep adding until there are no more edges to add. If an edge
// does create a cycle, dont add it to the graph, but dequeue it. The graph was 
// empty when we started out but now contains one edge between an element and
// it's most strongly connected component. Some vertices may not be a part of  
// this tree, i.e it isnt a spannign tree, because acronyms may be alone in  
// their esoteric definitions.
void build_maximum_spanning_tree()
{
	edge *e = pop_edge(); 
	acrovert *v1,*v2;	

	while(e) {
		v1 = find(e->x);
		v2 = find(e->y);
		if (v1 != v2) {
			add_edge(e->x, e->y, e->weight, BIDI, !KRUSKAL);
			component_union(v1, v2);
		} else {
			//we only want to free 
			free(e);
		}
		e = pop_edge();
	}	

}

// add edge to sorted linked list
void add_edge_sorted(edge *newedge, edge *downedge, int skiplist_head_index)
{
	if (skiplist_head_index >= SKIP_SIZE) {
		return;
	}

	edge *temp1 = skiplist_heads[skiplist_head_index], *temp2 = NULL, *skiplist_edge = NULL;
	int randvar = 0;
	while(temp1 && (temp1->weight > newedge->weight)) {
		temp2 = temp1;
		temp1 = temp1->next;
	}

	// 1. temp2 isn't NULL: add newnode to end of list or insert in the middle
	// 2. temp1 valid but temp2 NULL: add newnode as head
	// 3. both temp2 and temp1 NULL: list is empty	
	if (temp2) {
		newedge->next = temp2->next;
		temp2->next = newedge;
	} else if (!temp2 && temp1) {
		newedge->next = temp1;
		sorted_edge_list = newedge;	
	} else if (!temp2 && !temp1) {
		skiplist_heads[skiplist_head_index] = newedge;
	}
	
	// add the edge and assign it's down pointer. for 1st list this is NULL but
	// for subsequent lists this is the newedge we just added. subsequent lists
	// also need a malloc dupe of newedge to add to their own lists.
	newedge->downedge = downedge;
	
	// flip a coin
	srand(time(NULL));
	randvar = (rand()%10 + 1);
	
	// onyl if element makes it to the first list will it make it to the others,
	// with half probability at each level.
	if (randvar >= 5) {
		skiplist_edge = (edge*)malloc(sizeof(edge));

		skiplist_edge->weight = newedge->weight;
		skiplist_edge->x = newedge->x;
		skiplist_edge->y = newedge->y;
		skiplist_edge->next = NULL; 
		add_edge_sorted(skiplist_edge, newedge, ++skiplist_head_index);
	}
		
	return;
}

// delete head of linked list and return it
edge* pop_edge()
{
	edge *temp = sorted_edge_list;
	if (sorted_edge_list)
		sorted_edge_list = sorted_edge_list->next;
	return temp;
}

