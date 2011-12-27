#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "graph.h"
#define ALPHABET 27		// 1 extra for z
#define TRUE 1
#define FALSE 0
#define DISKBLOCKSIZE 100	// May not be the best name for this #def
#define ACRONYM_FOUND 1 
#define ACRONYM_RANGE 0

// Things that need fixing: 
// Note: A lot of accuracy and cleaner code will become of just inserting
// 27 seperators of the fashion 'alphabet--' before each alphabet range begins.
// I didn't, to get a feel of how one might manage a data base where doing  
// so isn't trivial. 
//
// 1. strncmp is used because of the \0 terminator being included in strcmp
// but not in the string returned by fgets, so we stop comparison with 
// strlen(input) characters. This measn YAR matches YARD if the binary search pans
// out unfavourably (i.e we hit YARD before YAR as a mid point).
//
// 2. special characters like )( appear both in the definitions and acronyms
// so sometimes is_acronymline gets confused and responds 'yes' for a definition.
// this makes our binary search go completely haywire. 
//
// 3. Words that have acronyms following acronyms get messed up because the binary
// search always assumes a sorted file. eg: ZOMG followed by OMG (typed z instead 
// of shift) as the definition.
//
// All in all the concept seems solid and ready for multi threading, though
// functionality is slightly defective because of afore mentioned constraints.

// alpahbetindex: obviously not thread safe, but this array should only
// be written into once during the call to FindFileSegments and remains
// read only (conceptually) thereafter.
unsigned int alphabetindex[ALPHABET];	
#define GETOFFSETFORCHAR(currchar)(alphabetindex[(currchar-'A')])
#define STOREOFFSETFORCHAR(currchar, size)(alphabetindex[(currchar-'A')] = size)
#define FTELL_ERRORMARGIN 100	// ftell is documented as not being 100% accurate for text files

// function: SubstringHash
// synopsis: Performs shift register type hashing of a string. if
// fMasterHash is set a one time, o(n) string hash is performed. 
// As comparison progresses the old hashvalue is remoulded in constant
// time. Equation used in hashing: 
// sumof (largestring[i]*2^i) from 0 -> length of smaller string
// return: new hash value computed for string starting from sinput
int SubstringHash(int oldhash, char *sinput, int ilen, bool fMasterHash)
{
	int newhash=0, i=0;
	char oldch, newch;

	if (fMasterHash) {
		assert(oldhash == 0);

		for(i=0; i<ilen; i++){
			newhash += sinput[i] * pow(2,i);
		} 
	} else {
		// assert something here to prove -1 is valid
		oldch = sinput[(-1)];	  // previous character eg: 'a' in amadnessb
		newch = sinput[(ilen-1)]; // current last character eg: 'b' in amadnessb
		newhash = (oldhash - (oldch * pow(2, 0)))/2;
		newhash += (newch * pow(2, (ilen-1)));	
	}
	
	return newhash;
}

// function: FindEmbeddedSubstring
// synopsis: currently takes each word from dictionary and finds
// all acronym definitions that contain that word in time
// O(no acronyms*(no alphabets/definition)) PER dictionary word.
// It could be a lot slower the conventional way i.e with strcmp
// it would be (mentioned complexity*(avg letters/dictionary word)),
// but this function is far too slow to run on a single thread. 
// return int: nothing useful, but this function should be modified
// to take an input string and search for just that string, in which
// case return values might change.
int FindEmbeddedSubstring()
{
	char sinput[100], scmp[100];
	int ishort, i=0, masterhash=0, oldhash=0, ilong=0, substringstart=0, newhash=0, rand;
	int nline=0;
	bool fMasterHash=1;	
	FILE *fp1, *fp2;
	fp2 = fopen("internetslangfull", "r");
	fp1 = fopen("dictionary", "r");	

	for(nline=0; fgets(scmp, 100, fp1); nline++)  {
		ishort = strlen(scmp);	
		masterhash = SubstringHash(oldhash, scmp, ishort, fMasterHash);

		//For current dictionary word, go through entire acronym list
		// but only check the acronym definitions, not the acronyms.	
		while(fgets(sinput, 100, fp2)) {
			ilong = strlen(sinput);

			// Last character of 'short' string will fall on last character	
			// of 'long' string. Implementation of this loop correlates w/
			// that of SubstringHas, and both need changing if one does. 

			// Note: We Only do the compare if sinput is a 'long string'. This
			// is the easiest way to tell if the long string is an acronym or in 
			// fact a long sentence with an embbedded short string. this probably 
			// isn't the best way to check long-ed-ness, though.
			for (substringstart=0, newhash=0; (substringstart+ishort-1) <= (ilong-1) && (ilong > ishort); substringstart++) {
				newhash = SubstringHash(newhash, &(sinput[substringstart]), ishort, fMasterHash);
				if (masterhash == newhash) {

					// This extra check is crucial for functional correctness.
					// A string of 'AXc' might end up with the same hash as, say,
					// 'AK'.
					if (strncmp(scmp, &sinput[substringstart], ishort) == 0) {
				
						// This check is also crucial. A word like table would
						// otherwise match a check for able, as the strncmp does
						// so from substringstart.
						// Note: this may need more testing for edge cases. eg:
						// are there cases where strings are joined, like lovehatetragedy.
						// This check also needs to take into account the end space or \n case.
						if (!substringstart || (substringstart && sinput[substringstart-1] == ' ')) {
							printf("string %s found in %s\n", scmp, sinput);
						}
					}
				}
				// recompute master hash for each new acronym definition
				fMasterHash = 0;
			}
			fMasterHash = 1;
		}
		// seek back to start of acronym file
		fseek(fp2, 0, SEEK_SET);
	}
	fclose(fp1);
	fclose(fp2);
	return 0;
}

// function: is_acronymline
// synopsis: checks if current line is an acronym or definition. 
// the easiest way to do so is by checking for caps/special 
// characters and making sure it isn't a sentence. The db of
// acronyms needs patching up, and with that change this function
// will receive fine tuning too.
// return: true if line is acronym
bool is_acronymline(char* acro)
{
	if (acro[1] >= 0X21  && acro[1] <= 0x5F &&
	    acro[1] != 0x50 && acro[1] != 0x51 &&
	    acro[0] >= 0x21 && acro[0] <= 0x5F)
		return TRUE;
	else
		return FALSE;
}

// function: FindClosestAcronym
// synopsis: Increments file pointer to the closest acronym.
// we need to do this because the binary search scans bytes 
// in a file, so 'jumps' may land us anywhere. FTELL_ERRORMARGIN
// may need tweaking to account for cases where this function's 
// error is too high, but on average this function should skip
// at most 1 definition since acronym/def pairs are consecutive.
void FindClosestAcronym(int sizestart, FILE* fp, char* acro)
{
	// worst case even if sizestart lands us in the middle
	// of an acronym we grab bytes till end, grab its meaning,
	// and exit at the next acronym. FTELL_ERRORMARGIN ensures
	// that we skip only previous character acronyms if this
	// happens.
	while(fgets(acro, 100, fp)){
		if (is_acronymline(acro)){ 
			return;
		}
	}
}

// function: BinarySearchFile
// synopsis: Performs a binary search for a given acronym. We will eiter 
// find the acronym itself or find a range within which the acronym lies. The
// range ~100 bytes; if fin and start are 100 bytes apart then regardless of 
// which recursion we came through, top half or bottom, we would have touched 
// and hence cached these DISKBLOCKSIZE bytes in fast memory (one hopes). 
// return: ACRONYM_FOUND if binary search mid element is acronym, ACRONYM_RANGE
// if range of linear search has been narrowed down to DISKBLOCKSIZE bytes.
bool BinaryFileSearch(char* string, int* sizestart, int* sizefin, FILE* fp)
{
	char acro[100];
	short scmp;
	if ((*sizefin - *sizestart) < DISKBLOCKSIZE) {
		return ACRONYM_RANGE;
	}

	int mid = *sizestart + ((*sizefin - *sizestart)/2);
	fseek(fp, mid, SEEK_SET);
	FindClosestAcronym(mid, fp, acro);	
	scmp = strncmp(acro, string, strlen(string));

	if (scmp > 0) {
		*sizefin = mid;
		return BinaryFileSearch(string, sizestart, sizefin, fp);
	} else if (scmp < 0) {
		*sizestart = mid;
		return BinaryFileSearch(string, sizestart, sizefin, fp);	
	} else {
		return ACRONYM_FOUND;
	}
}

// function: FindFileSEgments
// synopsis: populates alphabetindex[] with the byte offset, from 
// start of file, where that acronym begins. To find an acronym we 
// check it's first letter and perform a binary search within 
// alphabetindex[alphabet] and alphabetindex[alphabet+1].
// return: void
void FindFileSegments(char* file)
{
	FILE *fp;
	char acro[100], meaning[100], currchar = 'A';
	unsigned int nline, size;

	for (nline=0; nline< ALPHABET; nline++)
		alphabetindex[nline] = 0;

	fp=fopen("internetslangfull", "r");
	for(nline=0; fgets(acro, 100, fp); nline++) {
		if (is_acronymline(acro) == TRUE) {
			if (acro[0] == (currchar+1)) {
				size = ftell(fp);
				currchar = currchar+1;
				STOREOFFSETFORCHAR(currchar, size);
			}
		}
	}	

	// Store Z's end offset as EOF
	// Another option is to seek back from EOF and 
	// make it ftell.	
	size = ftell(fp);
	STOREOFFSETFORCHAR((currchar+1), size);
	fclose(fp);
	printf("file segments done!\n");
}

// function: SearchFile
// synopsis: searches file for string. String should be acronym,
// first perfroms a binary search on range of first alphabet and 
// then a linear search on 100 bytes returned from binary search.
// return: void
void SearchFile(char* file, char* input)
{
	FILE *fp;
	char acro[100], meaning[100], currchar, maxchar = 'Z';
	int nline, sizestart, sizefin;
	bool fBinarySearch = ACRONYM_RANGE;

	fp=fopen("internetslangfull", "r");
	printf("search for %s\n",input);

	// Get range for binary search
	currchar = (input[0]);
	sizestart = (GETOFFSETFORCHAR(currchar) - FTELL_ERRORMARGIN);
	sizestart = (sizestart < 0) ? 0 : sizestart;
	sizefin = (GETOFFSETFORCHAR((currchar+1)) + FTELL_ERRORMARGIN);
	sizefin = (sizefin > GETOFFSETFORCHAR((maxchar+1))) ? GETOFFSETFORCHAR((maxchar+1)) : sizefin;

	// To perfrom a purely linear search comment out this line
	fBinarySearch = BinaryFileSearch(input, &sizestart, &sizefin, fp);
	
	if (fBinarySearch == ACRONYM_RANGE) {
		fseek(fp, sizestart, SEEK_SET);
		for(nline=0; fgets(acro, 100, fp) && (ftell(fp) <= (sizefin + FTELL_ERRORMARGIN)); nline++) {
			if ((is_acronymline(acro) == TRUE) && !(strncmp(acro, input, strlen(input)))) {
				fgets(meaning, 100, fp);
				printf("%s\n",meaning);
				break;
			}
		}
	} else {
		fgets(meaning, 100, fp);
		printf("%s\n",meaning);
  	}
	fclose(fp);
}

// function: o_main
// synopsis: example usage
int o_main()
{
	char file[10];
	FindFileSegments(file);
	SearchFile(file, file);
	return 0;
}
