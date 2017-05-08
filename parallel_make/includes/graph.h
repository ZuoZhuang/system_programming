/**
 * Parallel Make
 * CS 241 - Spring 2017
 */

#pragma once
#include "callbacks.h"
#include "compare.h"
#include "dictionary.h"
#include "vector.h"
#include <stdbool.h>
#include <stdlib.h>

/**
 * A graph is a set of vertices and a set of edges.
 *
 * This library implements a graph by storing a dictionary of vertices and
 * dictionary of edges.
 *
 * Every node in the graph needs to have a key for the dictionay.
 * Every edge is directed and uses a (source, dest) pair as the key.
 */

/* Forward declare graph structure. */
typedef struct graph graph;

// Member Functions:

/**
 * Allocate and return a pointer to a new graph (on the heap).
 *
 * If you would like to make 'shallow' copies of the elements of the 'graph',
 * then you may pass in NULL for the parameters
 * (ex. graph(hash_function, compare, NULL, NULL, NULL, NULL, NULL, NULL)).
 * This means that everytime an element is to be copied or removed from the
 * 'graph'. the pointer to that element is copied or removed instead of using
 * the user supplied copy constructor and destructor.
 *
 * If you supply NULL for the hash_function then all elements will hash to the
 * same index causing all operations to require a linear scan of the graph.
 *
 * If you supply NULL for the compare then elements will be compared by their
 * virtual address.
 */
graph *graph_create(hash_function_type vertex_hash_function,
                    compare vertex_comp,

                    copy_constructor_type vertex_key_copy_constructor,
                    destructor_type vertex_key_destructor,
                    copy_constructor_type vertex_value_copy_constructor,
                    destructor_type vertex_value_destructor,

                    copy_constructor_type edge_value_copy_constructor,
                    destructor_type edge_value_destructor);

/**
 * Destroys all container elements by calling on the user provided destructor
 * for every element, and deallocates all the storage capacity allocated by the
 * graph.
 */
void graph_destroy(graph *this);

// Graph Connectivity Functions:

/**
 * Returns whether there is a directed edge between 's' and 't' in 'this'.
 *
 * Note that 's' and 't' must represent valid vertices, or this method will
 * result in undefined behavior.
 */
bool graph_adjacent(graph *this, void *s, void *t);

/**
 * Returns the keys of all vertices 't' in 'this' such that 's' is adjacent to
 * 't'.
 *
 * 's' must represent a valid vertex, or this method will result in undefined
 * behavior.
 */
vector *graph_neighbors(graph *this, void *s);

/**
 * Returns the keys of all vertices 's' in 'this' such that 's' is adjacent to
 * 't'.
 */
vector *graph_antineighbors(graph *this, void *s);

/**
 * Returns the number of neighbors of a graph vertex with the given 'key'.
 *
 * 's' must represent a valid vertex, or this method will result in undefined
 * behavior.
 */
size_t graph_vertex_degree(graph *this, void *s);

// Vertex Functions:

/**
 * Returns a vector containing all vertices in this graph.
 */
vector *graph_vertices(graph *this);

/**
 * Returns the number of vertices in the graph.
 */
size_t graph_vertex_count(graph *this);

/**
 * Returns whether there exists a vertex with key 'key' in 'this'.
 */
bool graph_contains_vertex(graph *this, void *key);

/**
 * Adds a vertex with 'key' to 'this' with a NULL associated value.
 *
 * Note: if the vertex, 'key', already exists in 'this', then nothing happens.
 *
 * WARNING: The vertex value copy constructor and destructor must handle NULL
 * if this method is to be used.
 */
void graph_add_vertex(graph *this, void *key);

/**
 * Removes a vertex with 'key' from 'this' and its associated value.
 *
 * All ingoing and outgoing edges will be destroyed.
 *
 * 'key' must represent a valid vertex in 'this', or this method will result
 * in undefined behavior.
 */
void graph_remove_vertex(graph *this, void *key);

/**
 * Gets the value associated with the vertex 'key' in 'this'.
 *
 * 'key' must represent a valid vertex in 'this', or this method will result
 * in undefined behavior.
 */
void *graph_get_vertex_value(graph *this, void *key);

/**
 * Sets the value associated with the vertex 'key' in 'this' to 'value'.
 *
 * If the vertex, 'key', already has a value in 'this', then the old value is
 * replaced with 'value'.
 */
void graph_set_vertex_value(graph *this, void *key, void *value);

// Edge Functions:

/**
 * Returns the number of edges in this graph.
 */
size_t graph_edge_count(graph *this);

/**
 * Adds a directed edge from 's' to 't' without a NULL associated value.
 *
 * Note: if a directed edge from 's' to 't' already exists in 'this', then
 * nothing happens.
 *
 * WARNING: The edge value copy constructor and destructor must handle NULL
 * values if this method is to be used.
 */
void graph_add_edge(graph *this, void *s, void *t);

/**
 * Removes the directed edge from 's' to 't' and it's associated value.
 *
 * 's' and 't' must represent valid vertices in 'this', and the edge ('s','t')
 * must exist, or this method will result in undefined behavior.
 */
void graph_remove_edge(graph *this, void *s, void *t);

/**
 * Gets the value associated with the directed edge from 's' to 't' in 'this'.
 *
 * 's' and 't' must represent valid vertices in 'this', and the edge ('s','t')
 * must exist, or this method will result in undefined behavior.
 */
void *graph_get_edge_value(graph *this, void *s, void *t);

/**
 * Sets the value associated with the the directed edge from 's' to 't' in
 * 'this' to 'value'.
 *
 * Note: if the directed edge from 's' to 't' already has a value in 'this',
 * then the old value is replaced with 'value'.
 *
 * 's' and 't' must represent valid vertices in 'this', and the edge ('s','t')
 * must exist, or this method will result in undefined behavior.
 */
void graph_set_edge_value(graph *this, void *s, void *t, void *value);
