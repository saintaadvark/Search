#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

struct node {
	char first_ch;
	struct node *child;
	struct node *next;
	char *sfx;
};

enum match_type {
				 exact_match,		// an exact match occured
				 substring_match, 	// tree node has long string with sub match 
				 needs_traversal,	// tree node has short string with sub match
				 needs_modify		// tree node needs to split
				 };

/*
char s1[20] = "deffo";
char s2[20] = "definately";
char s3[20] = "definately";
char s4[20] = "exasper";
char s5[20] = "ex";
char s6[20] = "exasperated";
char s7[20]= "deffo definately";
*/
char dollar[2] = "$";

static enum match_type match_suffix(char **psfx, char *sfxnode);

void find_string(char *sfx, node *pnode)
{
	if (pnode == NULL) {
		printf("string not found\n");
		//assert(0);
		return;
	}

	node *ptemp = NULL;
	enum match_type match = match_suffix(&sfx, pnode->sfx);
	
	if (match == needs_traversal) {
		// find a nnode in this subtree with a matching 1st char, 
		// or confirm that there is none		
		ptemp = pnode->child;
		for (; (ptemp) && (ptemp->first_ch != sfx[0]); ptemp=ptemp->next);

		// add new suffix to child list of parent with matching substring
		find_string(sfx, ptemp);
	} else if (match == exact_match) {
		printf("string found\n");
		if (pnode->child)
			printf("auto complete: %s\n",pnode->child->sfx);
		//while(1);
	}
	return;
}

enum match_type match_suffix(char **psfx, char *sfxnode)
{
	if (!psfx) {
		assert(0);
	}
	
	char *sfx1 = *psfx, *sfx2 = sfxnode;
	int i;
	
	// Only 3 valid cases, either one is a substring of the other or they match
	// exactly. Case 1 will never occur.
	//    sfxnode  		*psfx
	// 1. edeffo 		definately - 1st char has to match to get here
	// 2. deffo 		deffo - will break and return exact_match with &\0 
	// 3. deffonately 	deffo - will break and return substring_match with &\0 
	// 4. deffo 		definately - will break and return needs_traversal with 
	// 								 &inately 
	if (sfx1 && sfx2) {
		for (i=0; sfx1[i] == sfx2[i] && 
			(sfx1[i] != '\0') && (sfx2[i] != '\0'); i++);
		if ((sfx1[i] == '\0') && (sfx2[i] == '\0')) {
			return exact_match;
		} else if (sfx1[i] == '\0') {
			return substring_match;
		} else if (sfx2[i] == '\0') {
			*psfx = &sfx1[i];
			return needs_traversal;				
		}
		*psfx = &sfx1[i];
		return needs_modify;
	}
	
	// if one of the strings doesnt exist go into need_traversal case and add
	// a new node for the one that does exist. This will happen when we compare
	// with root.
	return needs_traversal;
}

node* add_suffix(char *sfx, node *pnode)
{
	node *ptemp = NULL;
	char *a;
	short slen=0, scommon_substring=0, sold_remainder_substring=0;
	enum match_type match = needs_traversal;

	// sfx doesn't exist in this tree yet
	if (!pnode) {
		ptemp = (node*)malloc(sizeof(node));
		if (!ptemp)
			goto cleanup;

		ptemp->child = NULL;
		ptemp->next = NULL;
		ptemp->first_ch = sfx[0];
		ptemp->sfx = (char*)malloc(strlen(sfx));
		if (!ptemp->sfx)
			goto cleanup;
		
		strcpy(ptemp->sfx, sfx);
		return ptemp;
	}

	// at least one character already matches, or it's the root call
	slen = strlen(sfx);
	match = match_suffix(&sfx, pnode->sfx);

	if (match == needs_traversal) {
		// find a node in this subtree with a matching 1st char, 
		// or confirm that there is none
		ptemp = pnode->child;
		for (; (ptemp) && (ptemp->first_ch != sfx[0]); ptemp=ptemp->next);
		
		// add new suffix to child list of parent with matching substring
		ptemp = add_suffix(sfx, ptemp);
	} else if (match == substring_match) {
	
		// tree node string has entire search string embedded in a longer word,  
		// we need to break the node string and insert $. 
		// eg: exasperated and ex, exasperated needs to become ex, asperated, $
		// copy 'exasperated' into buffer
		a = (char*)malloc(strlen(pnode->sfx));
		if (!a)
			goto cleanup;
		strcpy(a, pnode->sfx);  		
		free(pnode->sfx);
		
		// copy 'ex' into existing node
		pnode->sfx = (char*)malloc(strlen(sfx));
		if (!(pnode->sfx))
			goto cleanup;
		strcpy(pnode->sfx, sfx);
		
		// add $ as a child of 'ex', so we know 'ex' also occurs in text
		ptemp = add_suffix(dollar, NULL);
		ptemp->next = pnode->child;
		pnode->child = ptemp;
		
		// further the pointer by 'ex' so we only add 'asperated' as new branch
		slen = strlen(sfx);
		ptemp = add_suffix((a+slen), NULL);
		free(a);
	} else if (match == needs_modify) {
	
		// part of tree nodes substring matches a part of new string. both 
		// strings need truncation. 
		// eg: deffo and definately, deffo needs to become def, fo, inately
		// note: old sfx - new sfx is always +ve
		scommon_substring = slen - strlen(sfx);
		sold_remainder_substring = strlen(pnode->sfx) - scommon_substring;
		
		// put the old remainder substring (fo) into a new node
		a = (char*)malloc(sold_remainder_substring);
		strcpy(a, (pnode->sfx + scommon_substring));
		ptemp = add_suffix(a, NULL);	
		free (a);
		
		// reparent children of deffo to fo. any child of deffo cannot contain
		// 'fo' (save something like deffofo), but must contain all substrings 
		// that followed fo in the original text.
		ptemp->child = pnode->child;		 
		pnode->child = ptemp;
		
		// modify the existing node (deffo -> def)
		a = (char*)malloc((scommon_substring+1));
		strncpy(a, pnode->sfx, scommon_substring);
		a[scommon_substring] = '\0';
		free(pnode->sfx);
		pnode->sfx = (char*)malloc(sizeof(scommon_substring));
		strcpy(pnode->sfx, a);
		free(a);
		
		// put the new remainder substring ('inately') into a new node
		ptemp = add_suffix(sfx, NULL);	 
	} else if (match == exact_match) {
		printf("suffix string exists in the tree\n");
	}
	
	if (ptemp) {
		ptemp->next = pnode->child;
		pnode->child = ptemp;
	}

	return NULL;
	
cleanup:
	assert(0);
}

void bfs(node *pnode)
{
	node *narray[100], *temp, *temp2;
	int qtop = 0, qbottom = 0;

	while(pnode && (qbottom<=qtop)) {
		if (pnode->sfx) {
			printf("%s\n",pnode->sfx);
		}

		temp = pnode->child;			
		while (temp) {
			narray[qtop] = temp;
			qtop++;
			temp = temp->next;
		}
		pnode = narray[qbottom];
		qbottom++; 
	}
}

int main()
{
	node *root;
	FILE *fp, *fp1;
	char acro[100];
	int nline, i=0;
	
	root = (node*)malloc(sizeof(node));
	if (!root)
		goto cleanup;
		
	root->first_ch = 0;
	root->child = NULL;
	root->next = NULL;
	root->sfx = NULL;
	
	fp = fopen("dictionary", "r");
	for(nline = 0; fgets(acro, 100, fp); nline++) {
		acro[(strlen(acro)-1)] = '\0';
		add_suffix(acro, root);
	}
	fclose(fp);
	
	fp1 = fopen("dictionary", "r");
	for(nline = 0; fgets(acro, 100, fp1); nline++) {
		acro[(strlen(acro)-1)] = '\0';
//		find_string(acro, root);
	}
	fclose(fp1);

	return 0;
cleanup:
	assert(0);

}
