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

static bool is_acronymline(char* acro);
static void print_kruskal_edge_list();
static void print_word_list(acrovert* x);
static int binarysearch(int* a, int min, int max, int element);
static sentence_word* get_next_word(char* sentence, int i, int len);
static void qsort(int *a, int max, int min);

void add_acrovert(char* acro, char* meaning)
{
	acrovert* vert;
	int i=0;

	vert = (acrovert*)malloc(sizeof(acrovert));
	if (!vert)
		goto cleanup;
	vert->fvisited = FALSE;
	vert->arrival_time = 0;
	
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
		add_edge_sorted(newedge);
	}
	return;
	
	cleanup:
		assert(0);
}

bool findword(char* word, sentence_word* w2)
{
	while(w2) {
		if (!strcmp(word, w2->word)) {
			//printf("%s, %s\n",word, w2->word);
			return TRUE;
		}
		w2 = w2->next;
	}
	return FALSE;
}

int documentdistance(sentence_word* w1, sentence_word* w2)
{
	int distance=0;
	
	// take every word in w1 and look for it in w2. if found add sum of common
	// words ascii to distance/edge weight.
	while(w1 != NULL) {
		if (findword(w1->word, w2)) {
			distance += w1->wordhash;
		}
		w1 = w1->next;
	}
	return distance;
}

int main()
{
	FILE *fp1, *fp2;
	char string[100], acro[100], meaning[100];
	int nline, j, slen=0, search=0, searchpos=0, i=0, distance=0;
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
	for (i = 0; i < stacktop; i++) {
		for (j = (i+1); j < stacktop; j++) {
		        distance = documentdistance(vstack[i]->acro->wordlist, vstack[j]->acro->wordlist);
			if (distance) {
				printf("adding an edge between %s and %s\n",vstack[i]->acro->acro, vstack[j]->acro->acro);
				add_edge(vstack[i], vstack[j], distance, BIDI, !KRUSKAL);
				print_edge_acro(vstack[i]);
			}
		}
	}
	
	//print_kruskal_edge_list();
    //build_maximum_spanning_tree();
    
	//for (i=0;i<stacktop; i++) {
			dfs(vstack[0]);	
	//}
	//	print_edge_acro(vstack[i]);
    
    //for (i = 0; i < stacktop; i++) {
    	//printf("=========dfs vertex %s",vstack[i]->acro->acro);
    	//dfs(vstack[i]);
    	//print_edge_acro(vstack[i]);
	//}
	
	return 0;
}

inline void swap(int *a, int pos1, int pos2)
{
	int temp=0;
	assert(pos1>=0);
	assert(pos2>=0);
	
	temp=a[pos1];
	a[pos1]=a[pos2];
	a[pos2]=temp; 
	
	return;
}

// function: qsort
// synopsis: none necessary, but one provided. because i like the algorithm. 
// As we check the array from min to max we know that every element from swappos
// to i is < pivot. Upon reaching an element > pivot, throw it to the left end of the 
// array by swapping it with swappos. The element that was at swappos has already 
// been checked, and we know that it's less than pivot. So by performing the swap we
// throw all > elements to the left and all < elements to the right. 
// Note: max needs to be max addressable element of the array, not that element+1.
void qsort(int *a, int max, int min)
{
	if (max<min)
		return;
	assert(min>=0);	
	int pivot = a[max], i=0, swappos=min;

	for (i=min; i<max; i++) {
		if (a[i] > pivot) {
			swap(a, swappos, i);
			swappos++;
		}
	}
	
	// Final resting place for pivot
	swap(a, max, swappos);
	qsort(a, (swappos-1), min);
	qsort(a, max, (swappos+1));
}

// function: binarysearch
// synopsis: none necessary, but one provided. because it has some caveats that 
// made me feel foolish.
int binarysearch(int* a, int min, int max, int element)
{
	// min = max is valid and will occur when searching for
	// last/first element. penultimate call is with args (max-1, max)
	// leading to a mid of max-1. since a[max] in a increasing
	// array > a[max-1] it goes into the 'upper half' search,
	// from mid+1(=max) to max.  
	if (min > max)
		return -1;

	// Note to self: Offset from min, duh
	int mid = min+((max-min)/2);
	if (a[mid]==element)
		return mid;
	
	// we need to check mid+1 to max, and mid-1 to min.
	// including mid is not overkill, it's wrong. 
	if (element<a[mid])
		return binarysearch(a, (mid+1), max, element);
	else
		return binarysearch(a, min, (mid-1), element);
}

sentence_word* get_next_word(char* sentence, int i, int len)
{	
	if (i>=len || sentence[i] == '\0')	
		return NULL;

	char a[100], *word;
	sentence_word *s_word=NULL;
	int j=0, searchpos=0;
	short wordhash=0; // TODO: manually insert \0 instead of this 

	// compute a hash of the current word then search for it in fillerlist
	for (; sentence[i] != ' ' && sentence[i] != '\0'; i++, j++) {
		a[j] = sentence[i];
		wordhash += a[j];
	}
	a[j] = '\0';
	searchpos = binarysearch(fillerwords, 0, (fillerwordstop-1), wordhash);
			
	if (fillerwords[searchpos] != wordhash) {

		s_word = (sentence_word*)malloc(sizeof(sentence_word));
		if (!s_word)
			goto cleanup;
 
		s_word->word = (char*)malloc(j+1);
		if (!s_word->word)
			goto cleanup;

		strcpy(s_word->word, a);
		s_word->wordhash = wordhash;
		s_word->next = get_next_word(sentence, (i+1), len); 
		return s_word;
	} else {
		return get_next_word(sentence, (i+1), len);
	}
	
	cleanup:
		assert(0);
}

void print_word_list(acrovert* x)
{
	sentence_word *w = x->acro->wordlist;
	while(w) {
		printf("%s ",w->word);
		w=w->next;
	}
	printf("\n");
}

void print_kruskal_edge_list()
{
	edge *e = sorted_edge_list;
	while(e) {
		printf("\n");
		assert(e->x && e->y);
		printf("acronyms %s ", e->x->acro->acro);
		print_word_list(e->x);
		printf("and %s", e->y->acro->acro);
		print_word_list(e->y);
		printf("have weight %d",e->weight);											
		e=e->next;
		printf("\n");
	}
}

void print_edge_acro(acrovert* x)
{
	edge* acroedge = x->next;
	acrovert* y=NULL;
	printf("original acro=%s\n",x->acro->acro);
	print_word_list(x);
	while(acroedge) {
		y=acroedge->y;
		printf("%s:\n",y->acro->acro);
		print_word_list(y);
		acroedge = acroedge->next;
	}
}

bool is_acronymline(char* acro)
{
	if (acro[1] >= 0X21  && acro[1] <= 0x5F &&
	    acro[1] != 0x50 && acro[1] != 0x51 &&
	    acro[0] >= 0x21 && acro[0] <= 0x5F)
		return TRUE;
	else
		return FALSE;
}
