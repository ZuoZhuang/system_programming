/**
 * Ideal Indirection Lab
 * CS 241 - Spring 2017
 */
#pragma once
#include <stdlib.h>
// Max number of items that the TLB can hold
#define MAX_NODES 16

// The TLB struct is just a node of a linked list.
// We are using a linked list to implement an LRU cache.
typedef struct TLB {
  // The key is the virtual address with the offset removed.
  // This is because all virtual addresses with the same 3 virtual page numbers
  // will map to the same frame in memory.
  void *key;
  // The value is the beginning of the frame that corresponds to the key
  // provided.
  void *value;
  struct TLB *next;
} TLB;

/**
 * Allocate and return a new TLB structure.
 */
TLB *TLB_create();

/**
  Checks to see if 'tlb' knows the value of 'key'.

  If so, then this will return the physical_address.
  If not, then this will return NULL.

  Notice that this function takes a pointer to a tlb pointer.
  The double pointer is so that this function can modify the pointer the user
  passes in (updating their head pointer).
*/
void *TLB_get_physical_address(TLB **head, void *key);

/**
  Adds what the corresponding 'key': 'value'.

  If the TLB is at its capacity, then it will evict the least recently used
  (LRU) item.

  Notice that this function takes a pointer to a tlb pointer.
  The double pointer is so that this function can modify the pointer the user
  passes in (updating their head pointer).
 */
void TLB_add_physical_address(TLB **head, void *key, void *value);

/**
 * Clears the TLB's data
 */
void TLB_flush(TLB **tlb);

/**
 * Frees the TLB
 */
void TLB_delete(TLB *tlb);
