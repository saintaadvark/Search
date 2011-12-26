#include <pthread.h>
#include <semaphore.h>
#include <malloc.h>

// Data structure in queue
typedef struct future {
	void* (*func) (void* data);	// future execution
	void* data;			// function arg
	void* rval;			// function return value retrieved by future_get
	sem_t sem_Callback;		// sem used by future_get
	future* next;
} future;


// TCB needed for user level scheduling 
typedef struct thread_control_block {
	pthread_t tid;			// pthread identifier
} tcb; 


typedef struct thread_pool {
	int nthreads;		
	bool killPool;	
	pthread_t* tparray;
	pthread_mutex_t mutex_tp;

	future* future_head;		// queue of futures
	future* future_tail;
	pthread_mutex_t mutex_futures; 	// mutex and cvar for
	pthread_cond_t cvar_futures;	// future queue.
} thread_pool;

