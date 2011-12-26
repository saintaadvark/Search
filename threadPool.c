#include <stdio.h>
#include "threadPool.h"
#include "graph.h"

//future* world_future = NULL;

// TODO: Implement poolKill, incorporate tree

// Synopsis: Enqueue a future at tail of pool queue
// Note: Implemented as a list instead of an array so
// we can reorder elements in worst case 0(n) time.
// The ability to do so becomes crucial when each job
// is a disk access, because we can reorder according to
// spatial locality. 	
// Return: void
void enqueue(future* qfuture, thread_pool* tpcb)
{
	if (!qfuture)
		return;
	
	if (tpcb->future_head == NULL && tpcb->future_tail == NULL) {
		tpcb->future_head = qfuture;
		tpcb->future_tail = qfuture;
		qfuture->next = NULL;
	} else {
		qfuture->next = tpcb->future_tail->next;
		tpcb->future_tail->next = qfuture;
		tpcb->future_tail = qfuture;
	}
}

future* dequeue(thread_pool* tpcb)
{
	if (tpcb->future_head == NULL){
		return tpcb->future_head;
	}

	future* qfuture = tpcb->future_head;
	tpcb->future_head = tpcb->future_head->next;

	if (tpcb->future_head == NULL)
		tpcb->future_tail = NULL;
	
	return qfuture;
}

// Synopsis: All threads part of thread pool
// check the future work queue in the thread pool
// control block and wait conditionally if theres
// no work available.
// Return: void* 
void* thread_pool_start (void* tp_controlblock)
{
	thread_pool* tpcb = (thread_pool*)tp_controlblock;
	future* qfuture = NULL;

	// Note: we have to check if the qfuture actually exists
	// even if we get a cond_signal because the signal is not
	// atomic with the world_mutex drop, so a thread could still
	// creep in leaving a NULL queue. This is the paradise lost
	// scenario.	

	pthread_mutex_lock(&tpcb->mutex_futures);
	for (qfuture = dequeue(tpcb); tpcb->killPool == false; qfuture = dequeue(tpcb)) {
		while (!qfuture) {
			pthread_cond_wait(&tpcb->cvar_futures, &tpcb->mutex_futures);
			qfuture = dequeue(tpcb);
		}
		qfuture->func(qfuture->data);
		sem_post(&(qfuture->sem_Callback));
	}
	pthread_mutex_unlock(&tpcb->mutex_futures);
	return NULL;
}

void* future_get(future* qfuture)
{
	sem_wait(&(qfuture->sem_Callback));
	return qfuture->rval;
}

// Synopsis: submit a future to thread pool and signal
// waiting threads. Cond_var is used on the assumption
// that threads grabbed a mutex, checked the queue, and
// stated to cond_wait on an empty queue.
// Return: NULL
struct future* thread_pool_submit (thread_pool* tpcb, void*(*func)(void*), void* data)
{
	future* newfuture;
	newfuture = (future*)malloc(sizeof(future));
	newfuture->rval = NULL;
	newfuture->next = NULL;
	newfuture->func = func;
	newfuture->data = data;
	sem_init(&newfuture->sem_Callback,0 ,0);

	// Caveat: the cond signal doesnt need 
	// to be within the lock. Eitherway it
	// doesnt guarantee any order of execution.
	pthread_mutex_lock(&tpcb->mutex_futures);
	enqueue(newfuture, tpcb);
	pthread_mutex_unlock(&tpcb->mutex_futures);
	pthread_cond_signal(&tpcb->cvar_futures); 
	return newfuture;
}

// Synopsis: create a thread pool with n threads. Each thread 
// created executes thread_pool_start.
// Return: pointer to thread pool control block
struct thread_pool* thread_pool_new (int nthreads)
{
	thread_pool* tp_controlblock;
	int i=0;

	tp_controlblock = (thread_pool*)malloc(sizeof(thread_pool));
	tp_controlblock->tparray = (pthread_t*)malloc(sizeof(pthread_t) * nthreads);
	pthread_mutex_init(&tp_controlblock->mutex_tp, NULL);
	pthread_cond_init(&tp_controlblock->cvar_futures, NULL);
	pthread_mutex_init(&tp_controlblock->mutex_futures, NULL);
	tp_controlblock->nthreads = nthreads;
	tp_controlblock->killPool = false;
	tp_controlblock->future_head = NULL;
	tp_controlblock->future_tail = NULL;

	for (i=0; i<nthreads; i++) {
		pthread_create((pthread_t*)(&(tp_controlblock->tparray[i])), NULL, thread_pool_start, (void*)tp_controlblock);
	}
	return tp_controlblock;
}

// Empty function for threads in queue
void* thread_execute_function(void* data)
{
	printf("hello world\n");
	return NULL;
}

// Synopsis: Any application will have to create
// a thread pool and start submitting future
// jobs via thread_pool submit. 
int omain()
{
	thread_pool* tpcb;
	future* qfuture;
	int i;
	tpcb = thread_pool_new(10);
	for(i=0; i<10; i++) {
		qfuture = thread_pool_submit(tpcb, thread_execute_function, (void*)NULL);
		printf("q = %p\n",future_get(qfuture));
	}
	return 0;
}
