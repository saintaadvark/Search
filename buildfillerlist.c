#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>
#include "buildfillerlist.h"

int fillerwords[FILLERWORDS];
struct acrovert *vstack[MAX_ACRO];
int stacktop;
int fillerwordstop;
edge *sorted_edge_list;

// allocate an acronym vertex and add it to the adjacency list array. This array
// will never suffice for the entire list, needs add, find, delete methods that
// are compatible with the RBT. Thsi function also breaks 'meaning' into a 
// linked list of words.
void add_acrovert(char* acro, char* meaning)
{
	acrovert* vert;
	int i=0;

	vert = (acrovert*)malloc(sizeof(acrovert));
	if (!vert)
		goto cleanup;
	vert->fvisited = FALSE;
	vert->arrival_time = 0;
	vert->parent = NULL;
	
	vert->acro = NULL;
	vert->acro = (acronym*)malloc(sizeof(acronym));
	if (!(vert->acro))
		goto cleanup;
	vert->acro->acro = NULL;
	vert->next = NULL;
	
	vert->set_parent = (set_union*)malloc(sizeof(set_union));
	if (!(vert->set_parent))
		goto cleanup;
	vert->set_parent->set_parent = vert;
	vert->set_parent->size = 1;

	vert->acro->acro = (char*)malloc(strlen(acro));
	if (vert->acro->acro == NULL)
		goto cleanup;
	strcpy(vert->acro->acro, acro);

	// insert acronym into adjacency list
	vstack[stacktop] = vert;
	stacktop ++;
	assert(stacktop <= MAX_ACRO);

	// split the meaning of the acro into a linked list of words after removing
	// filler words.	
	vert->acro->wordlist = get_next_word(meaning, 0, strlen(meaning)); 

	return; 	
	cleanup:
		assert(0);
}

// adds an edge to the adjacency list the normal way if !fkruskal. normal way is
// by pushin it into the array and adding edge weights to all other vertices
// prooportional to the common words. The 'kruskal way' is by adding this edge 
// to a sorted linked list which is then processed on the call to 
// build_aximum_spanning_tree. note: also recursively adds a bidirectional edge.
void add_edge(acrovert* x, acrovert* y, int w, bool bidi, bool fkruskal)
{
	edge* newedge = (edge*)malloc(sizeof(edge));
	if (!newedge)
		goto cleanup;
		
	newedge->x = x;
	newedge->y = y;
	newedge->weight = w;
	newedge->next = NULL;
	
	if (!fkruskal) {
		newedge->next = x->next;
		x->next = newedge;
		if (bidi)
			add_edge(y, x, w, FALSE, FALSE);	
	} else {
		// bidi is completely ignored if fkruskal. 
		add_edge_sorted(newedge, NULL, 0);
	}
	return;
	
	cleanup:
		assert(0);
}

void build_graph(bool fkruskal, bool fbidi)
{
	int j=0, i=0, distance=0;
	
	if (fkruskal) {
		for (i = 0; i < SKIP_SIZE; i++)
			skiplist_heads[i] = NULL;
	}
	
	// for each acronym in the array compare the words in it's definition with 
	// the words in the definitions of all other acronyms. add an edge if the 
	// common words > 0, with weight ~proportional to length of common word. 	
	for (i = 0; i < stacktop; i++) {
		for (j = (i+1); j < stacktop; j++) {
		        distance = documentdistance(vstack[i]->acro->wordlist, vstack[j]->acro->wordlist);
			if (distance) {
				add_edge(vstack[i], vstack[j], distance, fbidi, fkruskal);
			}
		}
	}
    
    printf("dfs stack :\n");
    if (fkruskal) {
    	if (DBG)
    		print_kruskal_edge_list();
    	build_maximum_spanning_tree();
		for (i=0;i<stacktop; i++) {
	//			dfs(vstack[i]);	
		}
	} else {
		dfs(vstack[0]);
	}
    printf("\n adjacency list\n");
    for (i = 0; i < stacktop; i++) {
    	print_edge_acro(vstack[i], FALSE);
	}
	printf("\n expanded adjacency list\n");
	for (i = 0; i < stacktop && PRINT_WORD_LISTS; i++) {
    	print_edge_acro(vstack[i], PRINT_WORD_LISTS);
	}
}

int main()
{
	FILE *fp1, *fp2;
	char string[100], acro[100], meaning[100];
	int nline, j, slen=0, search=0, searchpos=0;
	sorted_edge_list = NULL;

	// create a table of the sum of ascii characters of all filler words, then 
	// sort it so we can quickly search within for a given hash string. This 
	// functionality will be replaced by a suffix tree in due time.
	fp1 = fopen("fillerwords", "r");
	for(fillerwordstop = 0; fgets(string, 100, fp1); fillerwordstop++)  {
		slen = strlen(string);
		for(j = 0; j<(slen-1); j++) {
			fillerwords[fillerwordstop] += string[j];	
		}
	}
	fclose(fp1);
	qsort(fillerwords, (fillerwordstop - 1), 0);

	// create array of all acronyms+meanings for the adjacency list of graph. 
	fp2 = fopen("internetslang3", "r");
	for(nline = 0; fgets(acro, 100, fp2); nline++)  {
		if (is_acronymline(acro) == TRUE) {
		
			// fgets includes line feed =(
			slen = strlen(acro);
			acro[slen-1] = '\0';
			
			fgets(meaning, 100, fp2);
			slen = strlen(meaning);
			meaning[(slen-1)] = '\0';
			
			add_acrovert(acro, meaning);
		}
	}
	fclose(fp2);

	// for each acronym in the array compare the words in it's definition with 
	// the words in the definitions of all other acronyms. add an edge if the 
	// common words > 0, with weight ~proportional to length of common word. 
	build_graph(KRUSKAL, BIDI);
	
	return 0;
}
