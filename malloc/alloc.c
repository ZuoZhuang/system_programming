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
    struct meta_data *last;
    int free;
    struct meta_data *last_free;
    struct meta_data *next_free;
};

typedef struct meta_data meta_data_t;

#define META_SIZE sizeof(meta_data_t)

typedef struct _head{
    meta_data_t *next;
} head;

void add_to_free_list(meta_data_t *block_ptr);
void remove_from_list(head *list, meta_data_t *block);
void merge_add(meta_data_t *block_ptr);

head *list_head = NULL;
head *free_list = NULL;

meta_data_t *get_free_block(size_t size) {
    meta_data_t *current = free_list->next;

    /*while (current != NULL) {*/
        /*if (current->free == 1 && current->size >= size) return current;*/
        /*current = current->next_free;*/
    /*}*/

    /*current = list_head;*/

    while (current && !(current->free && current->size >= size)) {
        /**last = current;*/
        current = current->next;
    }

    return current;
}

meta_data_t *extend_heap(size_t size) {
    meta_data_t *new_block;

    new_block = sbrk(0);

    void *new_ptr = sbrk(size + META_SIZE);

    assert((void*)new_block == new_ptr);

    if (new_ptr == (void*) -1) {
        return NULL;
    }

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
    new_block->last_free = NULL;
    new_block->last = NULL;

    if (!list_head) {
        head *h = sbrk(sizeof(head));
        list_head = h;
        list_head->next = NULL;
    }
    if (!free_list) {
        head *h = sbrk(sizeof(head));
        free_list = h;
        free_list->next = NULL;
    }

    if (list_head->next) {
        new_block->next = list_head->next;
    }
    else new_block->next = NULL;

    list_head->next = new_block;

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
        new_block = extend_heap(size);

        if (!new_block) {
            return NULL;
        }

        list_head->next = new_block;
     } else {
        /*meta_data_t *last = list_head;*/
        new_block = get_free_block(size);

        if (!new_block) {
            new_block = extend_heap(size);

            if (!new_block) {
                return NULL;
            }
        } else {
            new_block->free = 0;
        }
    }

    if (new_block && (new_block->size - size > sizeof(meta_data_t))) {
        /*perror("\n---\n split \n---\n");*/

    /*fprintf(stderr, "requested: %zu\n", size);*/
        void *ptr = ((void *)new_block) + size + META_SIZE;
        meta_data_t *split_block = (meta_data_t *)ptr;

        split_block->size = new_block->size - size - sizeof(meta_data_t);
        split_block->next = new_block->next;
        split_block->last = new_block;
        split_block->free = 0;
        split_block->last_free = NULL;
        split_block->next_free = NULL;
        /*add_to_free_list(split_block);*/
        /*merge_add(split_block);*/
        free(split_block+1);

        if (new_block->next != NULL) new_block->next->last = split_block;
        new_block->next = split_block;
        new_block->size = size;

        /*split++;*/
        /*fprintf(stderr, "split%d: %zu & %zu\n", split, current->size, split_block->size);*/
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
    /*if (!ptr) {*/
        /*return;*/
    /*}*/

    /*meta_data_t *block_ptr = get_meta_ptr(ptr);*/

    /*assert(block_ptr->free == 0);*/
    /*block_ptr->free = 1;*/
    /*block_ptr->freed = 1;*/

    /*[>meta_data_t *current_free = free_list;<]*/

    /*if (free_list->next == NULL) {*/
        /*free_list->next = block_ptr;*/
        /*block_ptr->next_free = NULL;*/
        /*block_ptr->last_free = NULL;*/
        /*return;*/
    /*}*/
    /*else if (block_ptr->freed == 0){*/
        /*block_ptr->next_free = free_list->next;*/
        /*free_list->next = block_ptr;*/
    /*}*/

    /*meta_data_t *last_free = block_ptr->last_free;*/
    /*meta_data_t *next_free = block_ptr->next_free;*/

    /*if (last_free != NULL) last_free->next_free = next_free;*/
    /*else free_list->next = block_ptr;*/
    /*if (next_free != NULL) next_free->last_free = last_free;*/

    if (!ptr) {
        return;
    }

    meta_data_t *block_ptr = get_meta_ptr(ptr);

    assert(block_ptr->free == 0);

    /*add_to_free_list(block_ptr);*/
    merge_add(block_ptr);
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

void add_to_free_list(meta_data_t *block_ptr) {
    /*meta_data_t *current_free = free_list;*/

    block_ptr->free = 1;

    if (free_list == NULL) {
        free_list->next = block_ptr;
        block_ptr->next_free = NULL;
        block_ptr->last_free = NULL;
        return;
    }
    else {
        block_ptr->next_free = free_list->next;
        free_list->next = block_ptr;
    }
}

void remove_from_list(head *list, meta_data_t *block) {
    if (list == NULL) return;

    if (list == free_list) {
        block->free = 0;

        /*meta_data_t *free_head = free_list;*/
        meta_data_t *last_blk = block->last_free;
        meta_data_t *next_blk = block->next_free;

        if (last_blk != NULL) last_blk->next_free = next_blk;
        else if (free_list->next == block) free_list->next = block->next_free;
        if (next_blk != NULL) next_blk->last_free = last_blk;
    }
    else {
        /*meta_data_t *head = list_head;*/
        meta_data_t *last_blk = block->last;
        meta_data_t *next_blk = block->next;

        if (last_blk != NULL) last_blk->next = next_blk;
        else if (list_head->next == block) list_head->next = block->next;
        if (next_blk != NULL) next_blk->last = last_blk;
    }
}

void merge_add(meta_data_t *block_ptr) {
    if (block_ptr == NULL) return;

    meta_data_t *last_block = block_ptr->last;
    meta_data_t *next_block = block_ptr->next;
    int merged = 0;

    /*fprintf(stderr, "last: %p\n", last_block);*/
    /*if (last_block!= NULL) fprintf(stderr, "left: e: %d, size: %zu\n", last_block->free, last_block->size);*/
    /*fprintf(stderr, "next: %p\n", next_block);*/
    /*if (next_block!=NULL) fprintf(stderr, "right: e: %d, size: %zu\n", next_block->free, next_block->size);*/

    if (next_block != NULL && next_block->free == 1) {

        /*perror("\n~~~\n merge1 \n~~~\n");*/

        /*merge++;*/
        /*fprintf(stderr, "mergeR%d: %zu + %zu", merge, next_block->size, block_ptr->size);*/

        block_ptr->size += (next_block->size + META_SIZE);
        remove_from_list(free_list, next_block);
        remove_from_list(list_head, next_block);

        add_to_free_list(block_ptr);

        /*fprintf(stderr, " = %zu\n", block_ptr->size);*/

        merged = 1;
    }

    if (last_block != NULL && last_block->free == 1) {

        /*merge++;*/
        /*fprintf(stderr, "mergeL%d: %zu + %zu\n", merge, last_block->size, block_ptr->size);*/

        last_block->size += (block_ptr->size + META_SIZE);
        remove_from_list(free_list, block_ptr);
        remove_from_list(list_head, block_ptr);

        /*fprintf(stderr, " = %zu\n", last_block->size);*/

        merged = 1;

    }

    if (merged == 0) {
        add_to_free_list(block_ptr);
    }
}
