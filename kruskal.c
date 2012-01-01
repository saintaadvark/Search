#include "buildfillerlist.h"

void add_edge_sorted(edge *newedge)
{
	edge *temp1 = sorted_edge_list, *temp2 = NULL;
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
		sorted_edge_list = newedge;
	}
		
	return;
}

edge* pop_edge()
{
	edge *temp = sorted_edge_list;
	if (sorted_edge_list)
		sorted_edge_list = sorted_edge_list->next;
	return temp;
}

acrovert* find (acrovert *v)
{
	assert(v->set_parent);
	while(v && (v->set_parent->set_parent != v))
		v = v->set_parent->set_parent;
	return v;
}

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
			free(e);
		}
		e = pop_edge();
	}	

}


