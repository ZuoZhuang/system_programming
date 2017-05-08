/**
 * Parallel Make
 * CS 241 - Spring 2017
 */

#pragma once
#include "callbacks.h"
#include "compare.h"
#include "vector.h"
#include <stdbool.h>
#include <stdlib.h>

/**
* In computer science, a set is an abstract data type that can store
* certain values, without any particular order, and no repeated values.
* It is a computer implementation of the mathematical concept of a finite set.
* Unlike most other collection types, rather than retrieving a specific element
* from a set, one typically tests a value for membership in a set.
*
* Some set data structures are designed for static or frozen sets that do not
* change after they are constructed. Static sets allow only query operations on
* their elements — such as checking whether a given value is in the set,
* or enumerating the values in some arbitrary order.
* Other variants, called dynamic or mutable sets, allow also the insertion
* and deletion of elements from the set.
*
* https://en.wikipedia.org/wiki/Set_(abstract_data_type)
*/

/* Forward declare set structure. */
typedef struct set set;

/**
 * Allocate and return a pointer to a new set (on the heap).
 *
 * All parameters are optional.
 *
 * If you would like to make 'shallow' copies of the elements of the set,
 * then you may pass in NULL for the parameters
 * (e.g., set_create(hash_function, comp, NULL, NULL)).
 * This means that everytime an element is to be copied or removed from the set
 * the pointer to that element is copied or removed instead of using
 * the user-supplied copy constructor and destructor.
 *
 * If you supply NULL for the hash_function, then all elements will hash to the
 * same index, causing all operations to require a linear scan of the set.
 *
 * If you supply NULL for the compare then elements will be compared by their
 * virtual address.
 */
set *set_create(hash_function_type hash_function, compare comp,
                copy_constructor_type copy_constructor,
                destructor_type destructor);

/**
 * Destroys all container elements by calling on the user provided destructor
 * for every element, and deallocates all the storage capacity
 * allocated by the 'set'.
*/
void set_destroy(set *this);

// Core Set-Theoretical Operations:
//
// Note: For set-theoretical operations it is undefined behavior if
// the two sets do not share the same:
// hash_function, compare function, copy_constructor, and destructor.
//

/**
 * Returns the union of sets 's' and 't'.
 */
set *set_union(set *s, set *t);

/**
 * Returns the intersection of sets 's' and 't'.
 */
set *set_intersection(set *s, set *t);

/**
 * Returns the difference of sets 's' and 't'.
 */
set *set_difference(set *s, set *t);

/**
 * Returns whether 's' is a subset of 't'.
 */
bool set_subset(set *s, set *t);

/**
 * Returns whether 's' equals 't'
 * (i.e. contain all and only the same elements).
 */
bool set_equals(set *s, set *t);

// Static Set Operations:

/**
 * Returns whether the set 'this' contains 'element'.
 */
bool set_contains(set *this, void *element);

/**
 * Returns the element that equals 'element' in 'this',
 * where equality is defined by the compare function.
 *
 * It's undefined behavior if the element can't be found.
 */
void *set_find(set *this, void *element);

/**
 * Returns whether 'this' is empty.
 */
bool set_empty(set *this);

/**
 * Returns the number of elements in 'this'.
 */
size_t set_cardinality(set *this);

/**
 * Returns a flat vector of the set's elements, for iteration.
 *
 * The vector must be freed.
 */
vector *set_elements(set *this);

// Dynamic Set Operations:

/**
 * Adds an 'element' to 'this', if not already present.
 */
void set_add(set *this, void *element);

/**
 * Removes an 'element' from 'this', if present.
 */
void set_remove(set *this, void *element);

/**
 * Removes all elements from 'this'.
 */
void set_clear(set *this);

// The following is code generated:

/**
 * Creates a set meant for char(s).
 */
set *char_set_create();

/**
 * Creates a set meant for double(s).
 */
set *double_set_create();

/**
 * Creates a set meant for float(s).
 */
set *float_set_create();

/**
 * Creates a set meant for int(s).
 */
set *int_set_create();

/**
 * Creates a set meant for long(s).
 */
set *long_set_create();

/**
 * Creates a set meant for short(s).
 */
set *short_set_create();

/**
 * Creates a set meant for unsigned char(s).
 */
set *unsigned_char_set_create();

/**
 * Creates a set meant for unsigned int(s).
 */
set *unsigned_int_set_create();

/**
 * Creates a set meant for unsigned long(s).
 */
set *unsigned_long_set_create();

/**
 * Creates a set meant for unsigned short(s).
 */
set *unsigned_short_set_create();
