#include "buildfillerlist.h"

// not the ideal way to do this, need to port suffix tree.
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

// takes each word from sentence 1 and looks for it in sentence 2.
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

// Splits 'sentence' into a list of words, eviscerates filler words and returns 
// list in sentence_word structure. There is no real need for this to be 
// recursive. i is the index at which the sentence 'begins', i.e words before i
// have been added to the list. len is the length of the entire original string.
sentence_word* get_next_word(char* sentence, int i, int len)
{	
	if (i>=len || sentence[i] == '\0')	
		return NULL;

	char a[100], *word;
	sentence_word *s_word=NULL;
	int j=0, searchpos=0;
	short wordhash=0; 

	// compute a hash of the current word then search for it in fillerlist
	for (; sentence[i] != ' ' && sentence[i] != '\0'; i++, j++) {
		a[j] = sentence[i];
		wordhash += a[j];
	}
	a[j] = '\0';
	searchpos = binarysearch(fillerwords, 0, (fillerwordstop-1), wordhash);
			
	// If it isn't found in the filler list create a node for it, inset into 
	// word list and move on to the next word.		
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

// Check if currently read line is an acronym line of a definition. The need for 
// this hackyness will disappear when i have time to cleanup the input data.
bool is_acronymline(char* acro)
{
	if (acro[1] >= 0X21  && acro[1] <= 0x5F &&
	    acro[1] != 0x50 && acro[1] != 0x51 &&
	    acro[0] >= 0x21 && acro[0] <= 0x5F)
		return TRUE;
	else
		return FALSE;
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
