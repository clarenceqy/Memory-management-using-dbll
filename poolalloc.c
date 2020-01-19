#include "dbll.h"
#include <stdlib.h>
#include "poolalloc.h"
#include <stdio.h>
#include <stdbool.h>

/*
   a pool-based allocator that uses doubly-linked lists to track
   allocated and free blocks
 */

/* create and initialize a memory pool of the required size */
/* use malloc() or calloc() to obtain this initial pool of memory from the system */
struct memory_pool *mpool_create(size_t size)
{
  struct memory_pool* pool = (struct memory_pool*)malloc(sizeof(struct memory_pool));

  /* set start to memory obtained from malloc */
  /* set size to size */

  char *memblock = (char*)malloc(size);
  pool->start = memblock;
  pool->size = size;

  /* create a doubly-linked list to track allocations */
  /* create a doubly-linked list to track free blocks */

  struct dbll *alloc_list = dbll_create();   
  struct dbll *free_list = dbll_create(); 
  pool -> alloc_list = alloc_list;
  pool -> free_list = free_list;

  /* create a free block of memory for the entire pool and place it on the free_list */

  struct alloc_info *ai = (struct alloc_info *) malloc(sizeof(struct alloc_info));
  //struct alloc_info *ai = pool->start;
  ai->offset = 0;

  ai->size = size;
  ai->request_size = 0;

  dbll_append(pool ->free_list,ai);

  /* return memory pool object */

  return pool;
}

void put_inorder(struct memory_pool *p, struct alloc_info *ai){
  struct llnode* curr_node = (struct llnode* )malloc(sizeof(struct llnode));
  curr_node = p->alloc_list->first;
  if(curr_node == NULL){
    dbll_append(p->alloc_list,ai);
    return;
  }
  while(curr_node != NULL){
    struct alloc_info *curr_ai = curr_node->user_data;
    if(curr_node->next != NULL){
      struct alloc_info *next_ai = curr_node->next->user_data;
      if(ai->offset > (curr_ai->offset + curr_ai->size - 1) && (ai->offset +ai->size -1 ) < next_ai->offset){
        dbll_insert_after(p->alloc_list,curr_node,ai);
        return;
      }
    }
    else{
      if(ai->offset > (curr_ai->offset + curr_ai->size - 1)){
        dbll_append(p->alloc_list,ai);
      }
      else{
        dbll_insert_before(p->alloc_list,curr_node,ai);
      }
      return;
    }
    curr_node = curr_node->next;
  }
}

void put_free_inorder(struct memory_pool *p, struct alloc_info *ai){
  struct llnode* curr_node = (struct llnode* )malloc(sizeof(struct llnode));
  
  curr_node = p->free_list->first;
  
  if(curr_node == NULL){
    dbll_append(p->free_list,ai);
    return;
  }
  while(curr_node != NULL){
    struct alloc_info *curr_ai = curr_node->user_data;
    if(curr_node->next != NULL){
      struct alloc_info *next_ai = curr_node->next->user_data;
      if(ai->offset > (curr_ai->offset + curr_ai->size - 1) && (ai->offset +ai->size -1 ) < next_ai->offset){
        dbll_insert_after(p->free_list,curr_node,ai);
        return;
      }
    }
    else{
      if(ai->offset > (curr_ai->offset + curr_ai->size - 1)){
        dbll_append(p->free_list,ai);
      }
      else{
        if(ai->offset == 0){
          dbll_insert_before(p->free_list,NULL,ai);
          return;
        }
        dbll_insert_before(p->free_list,curr_node,ai);
      }
      return;
    }
    curr_node = curr_node->next;
  }
  
}

/* ``destroy'' the memory pool by freeing it and all associated data structures */
/* this includes the alloc_list and the free_list as well */
void mpool_destroy(struct memory_pool *p)
{
  /* make sure the allocated list is empty (i.e. everything has been freed) */
  /* free the alloc_list dbll */
  /* free the free_list dbll  */
  /* free the memory pool structure */
  dbll_free(p->free_list);
  //dbll_free(p->alloc_list);
  free(p);
}


/* allocate a chunk of memory out of the free pool */

/* Return NULL if there is not enough memory in the free pool */

/* The address you return must be aligned to 1 (for size=1), 2 (for
   size=2), 4 (for size=3,4), 8 (for size=5,6,7,8). For all other
   sizes, align to 16.
*/

void *mpool_alloc(struct memory_pool *p, size_t size)
{
  /* check if there is enough memory for allocation of `size` (taking
	 alignment into account) by checking the list of free blocks */
   
   
   int aligned_size;
   bool hassuitiable_mem = false;
   switch(size){
     case 1: aligned_size = 1;break;
     case 2: aligned_size = 2;break;
     case 3: aligned_size = 4;break;
     case 4: aligned_size = 4;break;
     case 5: aligned_size = 8;break;
     case 6: aligned_size = 8;break;
     case 7: aligned_size = 8;break;
     case 8: aligned_size = 8;break;
     default:aligned_size = 16;break;
   }
  /* there are many strategies you can use: first fit (the first block that fits),
	 best fit (the smallest block that fits), etc. */
   /* if no suitable block can be found, return NULL */
   /* if found, create an alloc_info node, store start of new region
	 into offset, set size to allocation size (take alignment into
	 account!), set free to null */

  /* add the new alloc_info node to the memory pool's allocated
	 list */

  /* search the free list for a suitable block */

  if(p->free_list->first == NULL){
    return NULL;
  }

  while(aligned_size < size){
    aligned_size = aligned_size + 16;
  }

  struct llnode* curr_node = (struct llnode* )malloc(sizeof(struct llnode));
  curr_node = p->free_list->first;
  
  while(curr_node != NULL){
    
    struct alloc_info *curr_free_ai =curr_node -> user_data;
    
    //if there is a block has enough size
    if(curr_free_ai->size >= aligned_size){
      int origin_size = curr_free_ai->size;
      
      
      /*checking alignment
        if you can put allocate on first of the the block*/
      if((curr_free_ai->offset) % aligned_size == 0){
        struct alloc_info *ai = (struct alloc_info *) malloc(sizeof(struct alloc_info));
        
        //put alloc_into into alloc_list in order
        ai->size = aligned_size;
        ai->request_size = size;
        ai->offset = curr_free_ai->offset;
        put_inorder(p,ai);
        
        //free free_list, split it if necessary
        if(origin_size > aligned_size){
          dbll_remove(p->free_list,curr_node);
          struct alloc_info *new_ai = (struct alloc_info *) malloc(sizeof(struct alloc_info));
          
          new_ai ->size = origin_size - aligned_size;
          new_ai -> request_size = 0;
          new_ai -> offset = (ai->offset)+(ai->size);
          //dbll_append(p->free_list,new_ai);
          put_free_inorder(p,new_ai);
        }
        else{
          dbll_remove(p->free_list,curr_node);
        }
        char* ptr = (p->start+ai->offset);
        return ptr;
      }
      //check if there is a place you can put in this block
      int index = curr_free_ai->offset;

      while(index+aligned_size-1 <= curr_free_ai->offset+curr_free_ai->size-1){
        if(index % aligned_size == 0){
          struct alloc_info *ai = (struct alloc_info *) malloc(sizeof(struct alloc_info));
          ai = p->start + curr_free_ai->offset + index;
          ai->size = aligned_size;
          ai->request_size = size;
          ai->offset = index;
          put_inorder(p,ai);
          dbll_remove(p->free_list,curr_node);

          //also free and split free_list
          if(curr_free_ai->offset < ai->offset){
            struct alloc_info *new_ai = (struct alloc_info *) malloc(sizeof(struct alloc_info));
            new_ai -> size = ai->offset - curr_free_ai->offset;
            new_ai -> request_size = 0;
            new_ai -> offset = curr_free_ai->offset;
            //dbll_append(p->free_list,new_ai);
            put_free_inorder(p,new_ai);
            
          }
          
          if((ai->offset + aligned_size - 1) < (curr_free_ai->offset + curr_free_ai->size -1)){
            struct alloc_info *new_ai = (struct alloc_info *) malloc(sizeof(struct alloc_info));
            new_ai -> size = (curr_free_ai->offset + curr_free_ai->size -1) - (ai->offset + aligned_size - 1);
            new_ai -> request_size = 0;
            new_ai -> offset = ai->offset + aligned_size;
            //dbll_append(p->free_list,new_ai);
            put_free_inorder(p,new_ai);
          }
          
          return (p->start+ai->offset);
        }
        index++;
      }
    }
    curr_node = curr_node ->next;
  }

  /* return pointer to allocated region*/

  return NULL;

}

void print_list(struct memory_pool *p){
struct llnode* a_node = (struct llnode* )malloc(sizeof(struct llnode));
  a_node = p->free_list->first;
  while(a_node != NULL){
      struct alloc_info *a_ai = a_node->user_data;
      printf("%d",a_ai->offset);
      printf(":");
      printf("%d",a_ai->size);
      printf("  ");
      a_node = a_node->next;
  }
  printf("\n");
}

/* Free a chunk of memory out of the pool */
/* This moves the chunk of memory to the free list. */
/* You may want to coalesce free blocks [i.e. combine two free blocks
   that are are next to each other in the pool into one larger free
   block. Note this requires that you keep the list of free blocks in order */
void mpool_free(struct memory_pool *p, void *addr)
{
  

  
  /* search the alloc_list for the block */
  struct llnode* curr_node = (struct llnode* )malloc(sizeof(struct llnode));
  curr_node = p->alloc_list->first;
  while(curr_node != NULL){
    
    struct alloc_info *ai = (struct alloc_info *)curr_node->user_data;

    if(ai->offset + p->start == addr){
      //dbll_append(p->free_list,ai);
        
      struct alloc_info *new_ai =(struct alloc_info *) malloc(sizeof(struct alloc_info));
      //printf("%d\n",curr_ai->offset);
      new_ai->size = ai->size;
      new_ai->request_size = ai->request_size;
      new_ai->offset = ai->offset;
      //printf("%d\n",new_ai->size);

      dbll_remove(p->alloc_list,curr_node);
      put_free_inorder(p,new_ai);
      
      struct llnode* free_node = (struct llnode* )malloc(sizeof(struct llnode));
      free_node = p->free_list->first;
      while(free_node != NULL){
        if(free_node->next == NULL){
          print_list(p);
          return;
        }
        else{
          struct alloc_info *curr_ai = free_node->user_data;
          struct llnode* next_node = free_node->next;
          struct alloc_info *next_ai = next_node->user_data;
          if(curr_ai->offset + curr_ai->size == next_ai->offset){
            curr_ai->size = curr_ai->size + next_ai->size;
            dbll_remove(p->free_list,free_node->next);
            free_node = free_node;
          }
          else{
            free_node = curr_node->next;
          }
        }
      }
      //print_list(p);
      //printf("%x\n",addr);
      return;
    }
    curr_node = curr_node->next;
  }
  /* move it to the free_list */
  /* coalesce the free_list */
}

