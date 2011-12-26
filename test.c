#include <string.h>
#include <stdio.h>
#include "graph.h"
// Code needs modification, copied from main function where
// I used it as an inline test
void random_string_multithread_lookup()
{

	graph agraph;
	int i, nline, nacro, c=0;
	vertex** vert;
	char acro[100], meaning[100], chbang;
	FILE *fp;
	thread_pool* tpcb;
	future* qfuture;	
	argstruct *args;
	
	while (1) {
		scanf("%s",acro);
		args = (argstruct*)malloc(sizeof(argstruct));
		args->acronym = (char*)malloc(strlen(acro));
		strcpy(args->acronym, acro); 
		printf("%s\n",args->acronym);
		thread_pool_submit(tpcb, SearchFileWrapper, (void*)args);

		acro[c] ++;
		if (acro[c] > 'Z'){
			acro[c] = 'A';
			c++;
			if (c>2)
				while(1);
		}
	}
}
