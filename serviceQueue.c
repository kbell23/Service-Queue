#include <stdio.h>
#include <stdlib.h>


typedef struct node {
	int val;
	struct node *next;
	struct node *prev;
	int inQueue;
} NODE;

typedef struct LIST
{
	NODE *front;
	NODE *back;
}LIST;


struct service_queue {
    LIST * the_queue;
    LIST * buzzer_bucket;
	NODE **darr;                     // array to point to each node in the queue
	int length;
	int numOfBuzzers;                // amount of buzzers available to be given
};

LIST *lst_create() {
	LIST *l = malloc(sizeof(LIST));
	l->front = NULL;
	l->back = NULL;
	return l;
}

void lst_free(LIST *l) {
	NODE *p = l->front;
	NODE *pnext;

	while (p != NULL) {
		pnext = p->next;   // keeps us from de-referencing a freed ptr
		free(p);
		p = pnext;
	}
	// now free the LIST 
	free(l);
}

void lst_print(LIST *l) {
NODE *p = l->front;

  printf("[");
  while(p != NULL) {
    printf("%d ",p->val);
    p = p->next;
  }
  printf("]\n");
}

void lst_push_front(LIST *l, int val) {
	NODE *p = malloc(sizeof(NODE));

	p->val = val;
	p->next = l->front;
	p->prev = NULL;

	l->front = p;
	if (l->back == NULL)   // was empty, now one elem
		l->back = p;
}

void lst_push_back(LIST *l, int val) {
	NODE *p;
	if (l->back == NULL)   // list empty - same as push_front
		lst_push_front(l, val);
	else {  // at least one element before push
		p = malloc(sizeof(NODE));
		p->val = val;
		p->next = NULL;
		p->prev = NULL;
		l->back->next = p;
		l->back = p;
	}
}


int lst_is_empty(LIST *l) {
	if (l->front == NULL) { return 1; }
	else return 0;
}


/**
* Function: sq_create()
* Description: creates and intializes an empty service queue.
* 	It is returned as an SQ pointer.
*/

SQ * sq_create() {
SQ *q;

  q = malloc(sizeof(SQ));
  q->the_queue = lst_create();
  q->buzzer_bucket = lst_create();
  q->length = 0;

  int size = 2;                                       // initializes the "amount of buzzers to 2"
  q->numOfBuzzers = size;                             // sets the number of buzzers initially to 2
  q->darr = (NODE**)malloc(size * sizeof(NODE*));     // allocates the array with a size of 2

  int i;
  // populates the "bucket" with buzzers
  for (i = 0; i < size; i++)
  {
	  lst_push_back(q->buzzer_bucket, i);
  }
  return q;
}

/**
* Function: sq_free()
* Description:  see sq.h
*/

void sq_free(SQ *q) {

  lst_free(q->the_queue);
  lst_free(q->buzzer_bucket);
  free(q);
}

/**
* Function: sq_display()
* Description:  see sq.h
*/
void sq_display(SQ *q) {

  printf("current-queue contents:\n    ");
  lst_print(q->the_queue);
  printf("\n");
}

/**
* Function: sq_length()
* Description:  see sq.h
*/
int  sq_length(SQ *q) {
	return q->length;
}

/**
* Function: sq_give_buzzer()
* Description:  see sq.h
*/
int  sq_give_buzzer(SQ *q) {
	int buzzer;
	int i;
	q->length = q->length + 1;
	// if there are no more buzzers available
	if (lst_is_empty(q->buzzer_bucket) == 1)
	{
		// if there are no more buzzers
		if (q->length >= q->numOfBuzzers)
		{
			NODE **temp;
			int size = q->numOfBuzzers * 2;
			q->numOfBuzzers = size;
			temp = (NODE**)malloc(size * sizeof(NODE*));
			for (i = 0; i < q->length; i++){ temp[i] = q->darr[i]; }
			for (i = q->length - 1; i < q->numOfBuzzers; i++) { lst_push_back(q->buzzer_bucket, i); }
			free(q->darr);
			q->darr = temp;
		}

		NODE *p1;
		p1 = malloc(sizeof(NODE));
		buzzer = q->buzzer_bucket->front->val;
		p1 = q->buzzer_bucket->front; 
		p1->prev = q->the_queue->back;
		q->the_queue->back->next = p1;
		q->the_queue->back = p1;
		q->buzzer_bucket->front = p1->next;
		q->darr[buzzer] = p1;
		if (p1->next == NULL) { q->buzzer_bucket->back = NULL; }
		p1->next = NULL;
		p1->inQueue = 1;
	}
	// if the queue is empty
	else if(lst_is_empty(q->the_queue) == 1)
	{ 
		NODE *p2;
		p2 = malloc(sizeof(NODE));
		p2 = q->buzzer_bucket->front;
		buzzer = q->buzzer_bucket->front->val;
		p2->prev = q->the_queue->back;
		q->the_queue->front = p2;
		q->the_queue->back = p2;
		q->buzzer_bucket->front = p2->next;
		p2->next = NULL;
		q->darr[buzzer] = p2;
		p2->inQueue = 1;
	}
	else
	{
		NODE *p3;
		p3 = malloc(sizeof(NODE));
		buzzer = q->buzzer_bucket->front->val;
		p3 = q->buzzer_bucket->front;
		p3->prev = q->the_queue->back;
		q->the_queue->back->next = p3;
		q->the_queue->back = p3;
		q->buzzer_bucket->front = p3->next;
		if (p3->next == NULL) { q->buzzer_bucket->back = NULL; }
		p3->next = NULL;
		p3->inQueue = 1;
		q->darr[buzzer] = p3;
		p3->inQueue = 1;
	}
	return buzzer;
}

/**
* function: sq_seat()
* description:  see sq.h
*/
int sq_seat(SQ *q) 
{
	// if the queue is empty
	if (q->the_queue->front == NULL) { return -1; }

	NODE *p1;
	int buzzer;
	p1 = malloc(sizeof(NODE));
	p1 = q->the_queue->front;
	q->the_queue->front = p1->next;
	// if the buzzer bucket is null, set the buzzer in the bucket by itself
	if (q->buzzer_bucket->front == NULL) 
	{ 
		q->buzzer_bucket->front = p1; q->buzzer_bucket->back = p1;
	}
	q->buzzer_bucket->back->next = p1;
	q->buzzer_bucket->back = p1;
	p1->next = NULL;
	p1->inQueue = 0; // flag indicating the buzzer was removed from the queue
	buzzer = p1->val;
	q->length = q->length - 1;
	return buzzer;
} 

/**
* function: sq_kick_out()
*
* description:  see sq.h
*/
int sq_kick_out(SQ *q, int buzzer) 
{
	NODE* p1 = malloc(sizeof(NODE));
	NODE* p2 = malloc(sizeof(NODE));
	NODE* p3 = malloc(sizeof(NODE));
	if (buzzer >= q->numOfBuzzers) { return 0; }
	if (q->darr[buzzer] == NULL) { return 0; }
	if (q->the_queue->front == NULL) { return 0; }
	p1 = q->darr[buzzer];
	if (p1->inQueue == 0) { return 0; }
	// if the buzzer bucket is empty
	if (q->buzzer_bucket->front == NULL)
	{
		// if the buzzer is the only one in the queue while the buzzer bucket is empty
		if (q->the_queue->front->next == NULL)
		{
			q->buzzer_bucket->front = p1;
			q->buzzer_bucket->back = p1;
			q->the_queue->front = NULL;
			q->the_queue->back = NULL;
			p1->inQueue = 0;                                    // flag indicating the buzzer was removed from the queue
			q->length = q->length - 1;
			return 1;
		}
		// if the buzzer is at the end of the list but there's someone else in front while the buzzer bucket is empty
		else if (p1->next == NULL && p1->prev != NULL)
		{
			p2 = p1->prev;
			q->buzzer_bucket->back = p1;
			q->buzzer_bucket->front = p1;
			p2->next = NULL;
			q->the_queue->back = p2;
			p1->prev = NULL;
			p1->inQueue = 0;                                    // flag indicating the buzzer was removed from the queue
			q->length = q->length - 1;
			return 1;
		}
		// if the buzzer is at the front of the list but there's someone else behind while the buzzer bucket is empty
		else if (p1 == q->the_queue->front)
		{
			p2 = p1->next;
			p2->prev = NULL;
			q->the_queue->front = p2;
			q->buzzer_bucket->back = p1;
			p1->next = NULL;
			q->buzzer_bucket->front = p1;
			p1->inQueue = 0;                                    // flag indicating the buzzer was removed from the queue
			q->length = q->length - 1;
			return 1;
		}
		// if the buzzer is just in the queue, not the first or last while the bucket list is empty
		else
		{
			p2 = p1->prev;
			p3 = p1->next;
			q->buzzer_bucket->back = p1;
			p2->next = p1->next;
			p3->prev = p2;
			p1->prev = NULL;
			p1->next = NULL;
			q->buzzer_bucket->front = p1;
			if (q->the_queue->back == p1) { q->the_queue->back = p2; }
			if (q->the_queue->front == p1) { q->the_queue->front = p3; }
			p1->inQueue = 0;                                    // flag indicating the buzzer was removed from the queue
			q->length = q->length - 1;
			return 1;
		}
	}
	// if the buzzer is the only one in the queue
	if (q->the_queue->front->next == NULL)
	{
		q->buzzer_bucket->back->next = p1;
		q->buzzer_bucket->back = p1;
		q->the_queue->front = NULL;
		q->the_queue->back = NULL;
		p1->inQueue = 0;                                        // flag indicating the buzzer was removed from the queue
		q->length = q->length - 1;
		return 1;
	}
	// if the buzzer is at the end of the list but there's someone else in front
	else if (p1->next == NULL && p1->prev != NULL) 
	{
		p2 = p1->prev; 
		q->buzzer_bucket->back->next = p1;
		p2->next = NULL;
		q->the_queue->back = p2;
		q->buzzer_bucket->back = p1;
		p1->prev = NULL;
		p1->inQueue = 0;                                        // flag indicating the buzzer was removed from the queue
		q->length = q->length - 1;
		return 1;
	}
	// if the buzzer is at the front of the list but there's someone else behind
	else if (p1 == q->the_queue->front) 
	{
		p2 = p1->next;
		p2->prev = NULL;
		q->the_queue->front = p2;
		q->buzzer_bucket->back->next = p1;
		p1->next = NULL;
		q->buzzer_bucket->back = p1;
		p1->inQueue = 0;                                        // flag indicating the buzzer was removed from the queue
		q->length = q->length - 1;
		return 1;
	}
	// if the buzzer is just in the queue, not the first or last
	else 
	{
		p2 = p1->prev;
		p3 = p1->next;
		q->buzzer_bucket->back->next = p1;
		p2->next = p1->next;
		p3->prev = p2;
		p1->prev = NULL;
		p1->next = NULL;
		q->buzzer_bucket->back = p1;
		if (q->the_queue->back == p1) { q->the_queue->back = p2; }
		if (q->the_queue->front == p1) { q->the_queue->front = p3; }
		p1->inQueue = 0;
		q->length = q->length - 1;
		return 1;
	}
}

/**
* function:  sq_take_bribe()
* description:  see sq.h
*/

// find the buzzer in q. then pop it, but push to the front of the queue
int sq_take_bribe(SQ *q, int buzzer) 
{
	NODE *p1;
	NODE *p2;
	NODE *p3;

	if (buzzer >= q->numOfBuzzers) { return 0; }
	if (q->darr[buzzer] == NULL) { return 0; }
	if (q->the_queue->front == NULL) { return 0; }
	p1 = q->darr[buzzer];
	if (p1->inQueue == 0) { return 0; }
	if (p1 == q->the_queue->front) { return 0; }
	// if the bribe comes from the back of the list
	if (p1->next == NULL)
	{
		p2 = p1->prev;
		p2->next = NULL;
		q->the_queue->back = p2;
		p1->next = q->the_queue->front;
		q->the_queue->front->prev = p1;
		q->the_queue->front = p1;
		return 1;
	}
	// if the bribe is somewhere in the middle of the pack
	else 
	{
		p2 = p1->prev;
		p3 = p1->next;
		p2->next = p3;
		p3->prev = p2;

		p1->prev = NULL;
		p1->next = q->the_queue->front;
		q->the_queue->front->prev = p1;
		q->the_queue->front = p1;
		p1->prev = NULL;
		return 1;
	}
}
