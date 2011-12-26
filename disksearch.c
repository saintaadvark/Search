#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "graph.h"
#define ALPHABET 27	// 1 extra for z
#define TRUE 1
#define FALSE 0
#define DISKBLOCKSIZE 100
#define ACRONYM_FOUND 1 
#define ACRONYM_RANGE 0

// Things that need fixing: 
// 1. strncmp is sued because of the NULL terminator being included in strcmp
// but not in the string returned by fgets, so we stop comparison with 
// strlen(input) characters. This measn YAR matches YARD if the binary search pans
// out unfavourably (i.e we hit YARD before YAR as a mid point).
//
// 2. special characters like )( appear both in the definitions and acronyms
// so sometimes is_acronymline gets confused and responds 'yes' for a definition.
// this makes our binary search go completely haywire. 
//
// 3. Words that have acronyms following acronyms get messed up because the binary
// search always assumes a sorted file. eg: ZOMG followed by OMG as the definition.
//
// All in all the concept is solid and seems ready for multi threading, though
// functionality is slightly defective because of above constraints.

unsigned int alphabetindex[ALPHABET];
#define GETOFFSETFORCHAR(currchar)(alphabetindex[(currchar-'A')])
#define STOREOFFSETFORCHAR(currchar, size)(alphabetindex[(currchar-'A')] = size)
#define FTELL_ERRORMARGIN 100	// ftell is documented as not being 100% accurate for text files

// Helper1
// Note: Hackyness
bool is_acronymline(char* acro)
{
	if (acro[1] >= 0X21  && acro[1] <= 0x5F &&
	    acro[1] != 0x50 && acro[1] != 0x51 &&
	    acro[0] >= 0x21 && acro[0] <= 0x5F)
		return TRUE;
	else
		return FALSE;
}

// Increments file pointer to the closest acronym
void FindClosestAcronym(int sizestart, FILE* fp, char* acro)
{
	// worst case even if sizestart lands us in the middle
	// of an acronym we grab bytes till end, grab its meaning,
	// and exit at the next acronym. FTELL_ERRORMARGIN ensures
	// that we skip only previous character acronyms if this
	// happens.
	while(fgets(acro, 100, fp)) {
//		printf("closest :%s\n",acro);
		if (is_acronymline(acro)){ 
			return;
		}
	}
}

// On return fp will be at acronyms meaning or at the satrt of a range we need
// to search through linearly.
bool BinaryFileSearch(char* string, int* sizestart, int* sizefin, FILE* fp)
{
	char acro[100];
	short scmp;
	if ((*sizefin - *sizestart) < DISKBLOCKSIZE) {
		//printf("range retur\nn");
		return ACRONYM_RANGE;
	}

	int mid = *sizestart + ((*sizefin - *sizestart)/2);
	fseek(fp, mid, SEEK_SET);
	FindClosestAcronym(mid, fp, acro);	
	scmp = strncmp(acro, string, strlen(string));

	if (scmp > 0) {
		*sizefin = mid;
//		printf("going down %s\n",acro);
		return BinaryFileSearch(string, sizestart, sizefin, fp);
	} else if (scmp < 0) {
		*sizestart = mid;
//		printf("going up %s\n",acro);
		return BinaryFileSearch(string, sizestart, sizefin, fp);	
	} else {
		return ACRONYM_FOUND;
	}
}

// return file segments
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
//				printf("char %c word %s\n",currchar, acro);
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


void SearchFile(char* file, char* input)
{
	FILE *fp;
	char acro[100], meaning[100], currchar, maxchar = 'Z';
	int nline, sizestart, sizefin;
	bool fBinarySearch = ACRONYM_RANGE;

	fp=fopen("internetslangfull", "r");

	printf("search for %s\n",input);
//	while(1) {
		//scanf("%s",input);
		currchar = (input[0]);
		sizestart = (GETOFFSETFORCHAR(currchar) - FTELL_ERRORMARGIN);
		sizestart = (sizestart < 0) ? 0 : sizestart;
		sizefin = (GETOFFSETFORCHAR((currchar+1)) + FTELL_ERRORMARGIN);
		sizefin = (sizefin > GETOFFSETFORCHAR((maxchar+1))) ? GETOFFSETFORCHAR((maxchar+1)) : sizefin;

		fBinarySearch = BinaryFileSearch(input, &sizestart, &sizefin, fp);
		
		if (fBinarySearch == ACRONYM_RANGE) {
			fseek(fp, sizestart, SEEK_SET);
			for(nline=0; fgets(acro, 100, fp) && (ftell(fp) <= (sizefin + FTELL_ERRORMARGIN)); nline++) {
//				printf("goind through %s\n",acro);
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
//	}
	fclose(fp);
}

int o_main()
{
	char file[10];
	FindFileSegments(file);
	SearchFile(file, file);
	return 0;
}
