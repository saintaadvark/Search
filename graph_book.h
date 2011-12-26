// File: graph_book.h
// Synopsis: Definitions specific to the main 
// graph file that cannot get included in
// graph.h because of tree.c dependencies.


// Forward declarations of helper functions in graph.c
void init_filler_list();	
short is_upper(short ch);
void init(int arraysize, char* array[], int bitmask);
void push(vertex* vert);
