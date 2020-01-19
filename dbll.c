#include <stdlib.h>
#include "dbll.h"
#include <stdio.h>

/* Routines to create and manipulate a doubly-linked list */

/* create a doubly-linked list */
/* returns an empty list or NULL if memory allocation failed */
struct dbll *dbll_create()
{
  struct dbll* new_list = (struct dbll* )malloc(sizeof(struct dbll));
  if(new_list != NULL){
    return new_list;
  }
  return NULL;
}

/* frees all memory associated with a doubly-linked list */
/* this must also free all memory associated with the linked list nodes */
/* assumes user data has already been freed */
void dbll_free(struct dbll *list)
{

  if(list ->first == NULL && list->last == NULL){
    free(list->first);
    free(list->last);
    free(list);
    list = NULL;
  }

  else{
    // printf("HHHHHHHH\n");
    struct llnode* curr_node = list->first;
    while(curr_node != NULL){
      struct llnode* next_node = curr_node ->next;
      curr_node -> prev= NULL;
      curr_node -> next= NULL;
      free(curr_node -> prev);
      free(curr_node -> next);
      curr_node = NULL;
      free(curr_node);
      curr_node = next_node;
      }
      list->first = NULL;
      list->last = NULL;
      free(list->first);
      free(list->last);
      free(list);
      list = NULL;
  }
  
}

/* iterate over items stored in a doubly-linked list */

/* begin iteration from node start, end iteration at node stop (include stop in iteration) */
/* the list is traversed using the next pointer of each node */

/* if start is NULL, then start is the first node */
/* if end is NULL, then end is the last node */

/* at each node, call the user function f with a pointer to the list,
   a pointer to the current node, and the value of ctx */

/* if f returns 0, stop iteration and return 1 */

/* return 0 if you reached the end of the list without encountering end */
/* return 1 on successful iteration */

int dbll_iterate(struct dbll *list,
				 struct llnode *start,
				 struct llnode *end,
				 void *ctx,
				 int (*f)(struct dbll *, struct llnode *, void *))
{
  if(list ->first == NULL && list->last == NULL){
    return 0;
  }
  struct llnode* curr_node = (struct llnode* )malloc(sizeof(struct llnode));
  struct llnode* stop_node = (struct llnode* )malloc(sizeof(struct llnode));
  curr_node = start;
  if(start == NULL){
    curr_node = list -> first;
  }
  stop_node = end;
  if(end == NULL){
    stop_node = list -> last;
  }


  while(curr_node != NULL){

    if(f (list,curr_node,ctx) == 0){
      return 1;
    }

    if(curr_node == stop_node){
      break;
    }
    if(curr_node == list->last && curr_node != stop_node){
      return 0;
    }

    curr_node = curr_node->next;
  }

  return 1;
}

/* similar to dbll_iterate, except that the list is traversed using
   the prev pointer of each node (i.e. in the reverse direction).

   Adjust accordingly. For example if start is NULL, then start is the
   last node in the list.  If end is NULL, then end is the first node
   in the list.

*/

int dbll_iterate_reverse(struct dbll *list,
						 struct llnode *start,
						 struct llnode *end,
						 void *ctx,
						 int (*f)(struct dbll *, struct llnode *, void *)
						 )
{
  if(list ->first == NULL && list->last == NULL){
    return 0;
  }
  struct llnode* curr_node = (struct llnode* )malloc(sizeof(struct llnode));
  struct llnode* stop_node = (struct llnode* )malloc(sizeof(struct llnode));
  curr_node = end;
  if(start == NULL){
    curr_node = list -> last;
  }
  stop_node = start;
  if(end == NULL){
    stop_node = list -> first;
  }


  while(curr_node != NULL){

    if(f (list,curr_node,ctx) == 0){
      return 1;
    }

    if(curr_node == stop_node){
      break;
    }
    if(curr_node == list->first && curr_node != stop_node){
      return 1;
    }

    curr_node = curr_node->prev;
  }

  return 1;
}


/* Remove `llnode` from `list` */
/* Memory associated with `node` must be freed */
/* You can assume user_data will be freed by somebody else (or has already been freed) */
void dbll_remove(struct dbll *list, struct llnode *node)
{
  if(list -> first == node ){
    if(node -> next == NULL){
      list -> first = NULL;
      list -> last = NULL;
      node->prev = NULL;
      free(node->prev);
      node->prev = NULL;
      node->next = NULL;
      free(node->next);
      node->next = NULL;
      free(node);
      node = NULL;
      return;
    }
    else{
      node -> next -> prev = NULL;
      list -> first = node -> next;
      node->prev = NULL;
      free(node->prev);
      node->prev = NULL;
      node->next = NULL;
      free(node->next);
      node->next = NULL;
      free(node);
      node = NULL;
      return;
    }
  }

  if(list -> last == node){
    if(node -> prev == NULL){
      list -> first = NULL;
      list -> last = NULL;
      node->prev = NULL;
      free(node->prev);
      node->prev = NULL;
      node->next = NULL;
      free(node->next);
      node->next = NULL;
      free(node);
      node = NULL;
      return;
    }
    else{
      node -> prev -> next = NULL;
      list -> last = node -> prev;
      node->prev = NULL;
      free(node->prev);
      node->prev = NULL;
      node->next = NULL;
      free(node->next);
      node->next = NULL;
      free(node);
      node = NULL;
      return;
    }
  }
  
  else{
    
    node -> prev -> next = node -> next;
    node -> next -> prev = node -> prev;

    node->prev = NULL;
    free(node->prev);
    node->prev = NULL;
    node->next = NULL;
    free(node->next);
    node->next = NULL;
    free(node);
    node = NULL;
    
    return;
  }
}

/* Create and return a new node containing `user_data` */
/* The new node must be inserted after `node` */
/* if node is NULL, then insert the node at the end of the list */
/* return NULL if memory could not be allocated */
struct llnode *dbll_insert_after(struct dbll *list, struct llnode *node, void *user_data)
{
  struct llnode* new_node = (struct llnode* )malloc(sizeof(struct llnode));
  if(new_node != NULL){
    new_node -> user_data = user_data;
    new_node -> next = NULL;

    //if list is empty
    if(list -> first == NULL || list ->last == NULL){
      new_node -> prev = NULL;
      list -> first = new_node;
      list -> last = new_node;
      return new_node;
    }

    //if node is null
    if(node == NULL){
      struct llnode *old_last_node = list->last;
      new_node -> prev = old_last_node;
      old_last_node -> next = new_node;
      list-> last = new_node;
      return new_node;
    }

    else{
      struct llnode *old_next_node = node -> next;
      node -> next = new_node;
      new_node -> prev = node;
      new_node -> next = old_next_node;
      if(old_next_node != NULL){
        old_next_node -> prev = new_node;
      }
      return new_node;
    }
  }
  return NULL;
}

/* Create and return a new node containing `user_data` */
/* The new node must be inserted before `node` */
/* if node is NULL, then insert the new node at the beginning of the list */
/* return NULL if memory could not be allocated */
struct llnode *dbll_insert_before(struct dbll *list, struct llnode *node, void *user_data)
{
  struct llnode* new_node = (struct llnode* )malloc(sizeof(struct llnode));
  if(new_node != NULL){
    new_node -> user_data = user_data;

    //if list is empty
    if(list -> first == NULL){
      new_node -> prev = NULL;
      new_node -> next = NULL;
      list -> first = new_node;
      list -> last = new_node;
      return new_node;
    }

    //if node is null
    if(node == NULL){
      struct llnode *old_first_node = list->first;
      new_node -> prev = NULL;
      new_node -> next = old_first_node;
      old_first_node -> prev = new_node;
      list-> first = new_node;
      return new_node;
    }

    else{
      struct llnode *old_prev_node = node -> prev;
      node -> prev = new_node;
      new_node -> prev = old_prev_node;
      new_node -> next = node;
      if(old_prev_node != NULL){
        old_prev_node -> next = new_node;
      }
      return new_node;
    }
  }
  return NULL;
}

/* create and return an `llnode` that stores `user_data` */
/* the `llnode` is added to the end of list */
/* return NULL if memory could not be allocated */
/* this function is a convenience function and can use the dbll_insert_after function */
struct llnode *dbll_append(struct dbll *list, void *user_data)
{
  return dbll_insert_after(list,NULL, user_data);
}
