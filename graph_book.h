
void init_filler_list()
{
	FILE* fp;
	char filler[100];
	int i, sum=0;

	fp=fopen("fillerwords", "rw");
	for(fillermax=0; fgets(filler, 100, fp) && (fillermax < FILLER_WORDS); fillermax++) {
		for(i=0; i<strlen(filler); i++)
			sum += (short)filler[i];
		fillerlist[fillermax] = sum;
		sum=0;
	}
	fillermax -= 1;
	fclose(fp);

	return;	
}

// Hackyness
short is_upper(short ch)
{
	if (ch >= 0x41  && ch <= 0x5a)
		return TRUE;
	else
		return FALSE;
}

// function: Init 
//	Init an array, also used to allocate memory and
//	init other structures according to the bitmask.
// return: void
void init(int arraysize, char* array[], int bitmask)
{
	int i=0, nelem;
	
	// Currently only allocats array of bytes
	if ((bitmask & INIT_ALLOCATE) != 0) {
		*(char**)array = (char*)malloc(arraysize*sizeof(char));	
		if (!array)
			goto cleanup;

		for(i=0; i<arraysize; i++)
			array[i] = NULL;
	}

	if ((bitmask & INIT_STACK) != 0) {
		vstack = (vertex**)malloc(MAX_VERTICES*sizeof(void*));
		if (!vstack)
			goto cleanup;

		stacktop = 0;	
		for (i=0; i<MAX_VERTICES; i++)
			vstack[i] = NULL;
	}


	if ((bitmask & INIT_GRAPH) != 0) {
		((graph*)(array))->vstack = vstack;
		((graph*)(array))->vparents = (int*)malloc(sizeof(int)*MAX_VERTICES);	
		((graph*)(array))->nvertices = MAX_VERTICES;
	}
	if ((bitmask & INIT_VERTEX) != 0) {
		(*(vertex**)(array)) = (vertex*)malloc(sizeof(vertex));
		if (!*(array))
			goto cleanup;
		
		((vertex*)(*array))->acro = NULL;
		((vertex*)(*array))->acro = (acronym*)malloc(sizeof(acronym));
		if (!(((vertex*)(*array))->acro))
			goto cleanup;

		((vertex*)(*array))->acro->acro = NULL;
		((vertex*)(*array))->acro->meaning = NULL;
		((vertex*)(*array))->left = NULL;
		((vertex*)(*array))->right = NULL;
		((vertex*)(*array))->next = NULL;
		((vertex*)(*array))->parent = NULL;
		((vertex*)(*array))->color = red;
	}
	
	return;
	
	cleanup:
		assert(0);
}

// function: Push
//	Push a vertex pointer onto adjacency
//	list stack. 
// return: void
void push(vertex* vert)
{
	vstack[stacktop] = vert;
	stacktop++;
}

