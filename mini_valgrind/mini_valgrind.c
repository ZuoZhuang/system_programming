/**
 * Mini Valgrind Lab
 * CS 241 - Spring 2017
 */

#include "mini_valgrind.h"
#include <stdio.h>
#include <string.h>

void *mini_malloc(size_t request_size, const char *filename,
                  void *instruction) {
  // your code here
    if (filename != NULL) {
        meta_data *new_meta = (meta_data *) malloc(sizeof(meta_data)+request_size);
        if (new_meta == NULL) return NULL;

        new_meta->request_size = request_size;
        new_meta->filename = filename;
        new_meta->instruction = instruction;

        if (head == NULL)
            new_meta->next = NULL;
        else
            new_meta->next = head;

        head = new_meta;

        total_memory_requested += request_size;

        return (void *)new_meta+sizeof(meta_data);
    }
    else{
        return NULL;
    }
}

void *mini_calloc(size_t num_elements, size_t element_size,
                  const char *filename, void *instruction) {
  // your code here
    if (filename != NULL) {
        size_t new_size = num_elements*element_size;

        if (new_size == 0) return NULL;

        meta_data *new_meta = (meta_data *) calloc(sizeof(meta_data)+new_size, 1);
        if (new_meta == NULL) return NULL;

        new_meta->request_size = new_size;
        new_meta->filename = filename;
        new_meta->instruction = instruction;

        if (head == NULL)
            new_meta->next = NULL;
        else
            new_meta->next = head;

        head = new_meta;

        total_memory_requested += new_size;

        return (void *)new_meta+sizeof(meta_data);
    }
    else{
        return NULL;
    }
}

void *mini_realloc(void *payload, size_t request_size, const char *filename,
                   void *instruction) {
  // your code here
    if (payload == NULL && request_size == 0) return NULL;
    if (payload == NULL) return mini_malloc(request_size, filename, instruction);
    if (request_size == 0) {
        mini_free(payload);
        return NULL;
    }

    if (head == NULL) {
        invalid_addresses += 1;
        return NULL;
    }

    meta_data *temp = head;
    meta_data *old_meta = (meta_data *) (payload - sizeof(meta_data));

    size_t old_size = old_meta->request_size;
    meta_data *new_meta = realloc(old_meta, sizeof(meta_data)+request_size);
    if (new_meta == NULL) return NULL;

    new_meta->request_size = request_size;
    new_meta->filename = filename;
    new_meta->instruction = instruction;

    if (head == old_meta) {
        head = new_meta;
    }
    else {
        while (temp->next != NULL) {
            if (temp->next == old_meta) break;
            temp = temp->next;
        }

        if (temp->next != old_meta) {
            invalid_addresses += 1;
            return NULL;
        }

        temp->next = new_meta;
    }

    /*while (temp->next != NULL) {*/
        /*if (temp->next == old_meta) break;*/
        /*temp = temp->next;*/
    /*}*/

    /*if (temp->next != old_meta) {*/
        /*invalid_addresses += 1;*/
        /*return NULL;*/
    /*}*/

    /*size_t old_size = old_meta->request_size;*/
    /*meta_data *new_meta = realloc(old_meta, sizeof(meta_data)+request_size);*/
    /*if (new_meta == NULL) return NULL;*/

    /*new_meta->request_size = request_size;*/
    /*new_meta->filename = filename;*/
    /*new_meta->instruction = instruction;*/

    /*temp->next = new_meta;*/

    if (old_size < request_size) total_memory_requested = total_memory_requested + request_size - old_size;
    else total_memory_freed = total_memory_freed + old_size - request_size;

    return (void *) new_meta+sizeof(meta_data);
}

void mini_free(void *payload) {
  // your code here

    if (payload == NULL) return;

    if (head == NULL) {
        invalid_addresses += 1;
        return;
    }

    meta_data *temp = head;
    meta_data *old_meta = (meta_data *) (payload - sizeof(meta_data));

    if (head == old_meta) {
        head = old_meta->next;
    }
    else {
        while (temp->next != NULL) {
            if (temp->next == old_meta) break;
            temp = temp->next;
        }

        if (temp->next != old_meta) {
            invalid_addresses += 1;
            return;
        }

        temp->next = old_meta->next;
    }


    size_t old_size = old_meta->request_size;

    free(old_meta);

    total_memory_freed += old_size;

    return;
}
