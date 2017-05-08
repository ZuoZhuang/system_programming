/**
 * Machine Problem: Malloc
 * CS 241 - Spring 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>

#define MEM_LIMIT 2684354560

struct meta_data {
    size_t size;
    struct meta_data *next;
    int free;
    struct meta_data *last_free;
    struct meta_data *next_free;
    int freed;
};

typedef struct meta_data meta_data_t;

/*struct free_list_struct {*/
    /*meta_data_t *block;*/
    /*meta_data_t *next;*/
/*};*/

/*typedef struct free_list_struct free_list_t;*/

#define META_SIZE sizeof(meta_data_t)

void *list_head = NULL;
void *free_list = NULL;

meta_data_t *get_free_block(meta_data_t **last, size_t size) {
    meta_data_t *current = free_list;

    /*while (current != NULL) {*/
        /*if (current->free == 1 && current->size >= size) return current;*/
        /*current = current->next_free;*/
    /*}*/

    /*current = list_head;*/

    while (current && !(current->free && current->size >= size)) {
        *last = current;
        current = current->next;
    }

    return current;
}

size_t heap_used = 0;

meta_data_t *extend_heap(meta_data_t* last, size_t size) {
    meta_data_t *new_block;

    new_block = sbrk(0);

    // check whether overflow
    if (size+META_SIZE>MEM_LIMIT) {
        return NULL;
    }

    // check whether overflow
    /*if (heap_used+size+META_SIZE>MEM_LIMIT) {*/
        /*return NULL;*/
    /*}*/

    /*assert(heap_used+size+META_SIZE<=MEM_LIMIT);*/

    void *new_ptr = sbrk(size + META_SIZE);

    assert((void*)new_block == new_ptr);

    if (new_ptr == (void*) -1) {
        return NULL;
    }

    heap_used += size+META_SIZE;

    // Original
    //
    /*if (last) {*/
        /*last->next = new_block;*/
    /*}*/

    /*new_block->size = size;*/
    /*new_block->next = NULL;*/
    /*new_block->free = 0;*/

    // New

    new_block->size = size;
    new_block->free = 0;
    new_block->next_free = NULL;
    new_block->freed = 0;

    if (last) {
        new_block->next = last->next;
        last->next = new_block;
    }
    else new_block->next = NULL;


    return new_block;
}

meta_data_t *get_meta_ptr(void *ptr) {
    return (meta_data_t*)ptr - 1;
}

/**
 * Allocate space for array in memory
 *
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 *
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size) {
  // implement calloc!
    size_t req_size = num * size; // TODO: check for overflow.
    void *new_block = malloc(req_size);

    memset(new_block, 0, req_size);

    return new_block;
}

/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
void *malloc(size_t size) {
  // implement malloc!

    meta_data_t *new_block;

    if (size <= 0) {
        return NULL;
    }

    if (!list_head) {
        new_block = extend_heap(NULL, size);

        if (!new_block) {
            return NULL;
        }

        list_head = new_block;
     } else {
        meta_data_t *last = list_head;
        new_block = get_free_block(&last, size);

        if (!new_block) {
            new_block = extend_heap(last, size);

            if (!new_block) {
                return NULL;
            }
        } else {
            new_block->free = 0;

            meta_data_t *last_free = new_block->last_free;
            meta_data_t *next_free = new_block->next_free;

            if (last_free != NULL) last_free->next_free = next_free;
            else free_list = new_block;
            if (next_free != NULL) next_free->last_free = last_free;
        }
    }


    return (new_block+1);
}

/**
 * Deallocate space in memory
 *
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void *ptr) {
  // implement free!
    if (!ptr) {
        return;
    }

    meta_data_t *block_ptr = get_meta_ptr(ptr);

    assert(block_ptr->free == 0);
    block_ptr->free = 1;
    block_ptr->freed++;

    meta_data_t *current_free = free_list;

    if (current_free == NULL) {
        free_list = block_ptr;
        block_ptr->next_free = NULL;
        block_ptr->last_free = NULL;
        return;
    }
    /*else if (block_ptr->freed == 1){*/
    else {
        meta_data_t *next_free = current_free->next_free;

        block_ptr->next_free = next_free;
        if (next_free != NULL) {
            block_ptr->last_free = next_free->last_free;
            next_free->last_free = block_ptr;
        }

        free_list = block_ptr;
    }
    /*else {*/
        /*meta_data_t *last_free = next_free->last_free;*/

        /*if (last_free!=NULL){*/
            /*last_free->next_free = block_ptr;*/
        /*}*/
    /*}*/

    // TODO this should be in malloc or get_free_block

}

/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size) {
  // implement realloc!

    if (!ptr) {
        return malloc(size);
    }

    meta_data_t* old_block = get_meta_ptr(ptr);
    if (old_block->size >= size) {
        return ptr;
    }

    // Then copy old data to new space.
    void *new_block;
    new_block = malloc(size);
    if (!new_block) {
        return NULL;
    }

    memcpy(new_block, ptr, old_block->size);
    free(ptr);

    return new_block;
}
