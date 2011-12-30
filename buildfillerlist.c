#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>
#define FILLERWORDS 100
#define MAX_ACRO 500
#define TRUE 1
#define FALSE 0

typedef struct sentence_word{
	char *word;
	sentence_word* next;
	short wordhash;
} sentence_word;

typedef struct {
	char *acro;
	sentence_word *wordlist;
} acronym;

typedef struct acrovert {
	struct edge* next;
	acronym* acro;	
} acrovert;

typedef struct edge {
	struct edge* next;
	struct acrovert* y;
	struct acrovert* x;
	int weight;
} edge;

int fillerwords[FILLERWORDS];
struct acrovert *vstack[MAX_ACRO];
int stacktop;
int fillerwordstop;

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
	short wordhash=10;

	for (; sentence[i] != ' ' && sentence[i] != '\0'; i++, j++) {
		a[j] = sentence[i];
		wordhash += a[j];
	}
	a[j] = '\0';

	// Don't add filler words
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

void add_acrovert(char* acro, char* meaning)
{
	acrovert* vert;
	int i=0;

	//init(0, (char**)&vert, INIT_VERTEX);
	vert = (acrovert*)malloc(sizeof(acrovert));
	if (!vert)
		goto cleanup;
	
	vert->acro = NULL;
	vert->acro = (acronym*)malloc(sizeof(acronym));
	if (!(vert->acro))
		goto cleanup;

	vert->acro->acro = NULL;
	vert->next = NULL;

	vert->acro->acro = (char*)malloc(strlen(acro));
	if (vert->acro->acro == NULL)
		goto cleanup;

	strcpy(vert->acro->acro, acro);

	vstack[stacktop] = vert;
	stacktop ++;
	assert(stacktop<= MAX_ACRO);
	
	vert->acro->wordlist = get_next_word(meaning, 0, strlen(meaning)); 

	return; 	
	cleanup:
		assert(0);
}

void add_edge(acrovert* x, acrovert* y, int w, bool bidi)
{
	edge* newedge = (edge*)malloc(sizeof(edge));
	if (!newedge)
		goto cleanup;

	newedge->x = x;
	newedge->y = y;
	newedge->weight = w;
	newedge->next = x->next;
	x->next = newedge;
	
	if (bidi)
		add_edge(y, x, w, FALSE);	
	return;

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

void print_edge_acro(acrovert* x)
{
	edge* acroedge = x->next;
	acrovert* y=NULL;
	printf("original acro=%s",x->acro->acro);
	print_word_list(x);
	while(acroedge) {
		y=acroedge->y;
		printf("%s:",y->acro->acro);
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
	while(w1 != NULL) {
		if (findword(w1->word, w2)) {
			//printf("success~\n");
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

	fp1 = fopen("fillerwords", "r");
	for(nline=0; fgets(string, 100, fp1); nline++)  {
		slen = strlen(string);
		for(j=0; j<slen; j++) {
			fillerwords[nline] += string[j];	
			fillerwordstop ++;
		}
	}
	fclose(fp1);

	qsort(fillerwords, (nline-1), 0);

	fp2 = fopen("internetslang", "r");
	for(nline=0; fgets(acro, 100, fp2); nline++)  {
		if (is_acronymline(acro) == TRUE) {
			fgets(meaning, 100, fp2);
			add_acrovert(acro, meaning);
		}
	}
	fclose(fp2);
	
	for (i=0;i<stacktop;i++) {
		for (j=(i+1);j<stacktop;j++) {
		        distance=documentdistance(vstack[i]->acro->wordlist, vstack[j]->acro->wordlist);
			if (distance) {
				//printf("acro1=%s,acro2=%s, distance=%d\n",vstack[i]->acro->acro, vstack[j]->acro->acro,distance);
				add_edge(vstack[i], vstack[j], distance, TRUE);
			}
		}
	}
	
	for (i=0;i<stacktop; i++)	
		print_edge_acro(vstack[i]);

	return 0;
}
