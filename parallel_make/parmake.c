/**
 * Parallel Make
 * CS 241 - Spring 2017
 */


#include "format.h"
#include "graph.h"
#include "parmake.h"
#include "parser.h"

#include <unistd.h>
#include <stdio.h>
#include "queue.h"
#include "vector.h"
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>

#define CYCLE -2
#define FAIL -1
#define UNVISIT 0
#define VISITING 1
#define VISITED 2
#define ISFILE 3
#define NOTFILE 4
#define NEWFILE 5

/*#define DEBUG 1*/

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
int make_fail = 0;


// ----- Previous -----

int execute_command(rule_t *rule) {
    while(!vector_empty(rule->commands)) {
        void **command = vector_front(rule->commands);

        int ret = system((char *)(*command));

        if(ret != 0) {
            return FAIL;
        }

        vector_erase(rule->commands, 0);
    }

    return 0;
}

// ----- End of Previous -----

void *worker(void *v_list) {
    vector *execute_list = (vector *)v_list;

    while (1) {
        rule_t *rule = (rule_t *)malloc(sizeof(rule_t));
        pthread_mutex_lock(&m);
        if (make_fail == 1 || vector_empty(execute_list)) {
            pthread_mutex_unlock(&m);

            // Clean up
            free(rule);
            exit(0);
        }

        memcpy(rule, (rule_t *) (*vector_begin(execute_list)), sizeof(rule_t));
        vector_erase(execute_list, 0);

        pthread_mutex_unlock(&m);

        if(execute_command(rule) == FAIL) {
            // TODO stop all thread
            pthread_mutex_lock(&m);
            make_fail = 1;
            vector_destroy(execute_list);
            pthread_mutex_unlock(&m);
            exit(-1);
        }

        // Clean up
        free(rule);
    }

    exit(0);
}

int parallel_execute(int n_threads, vector *execute_list) {
    pthread_t tids[n_threads];

    for (int i = 0; i < n_threads; i++) {
        pthread_create(tids+i, NULL, worker, (void *)execute_list);
    }

    for (int i = 0; i < n_threads; i++) {
        pthread_join(tids[i], NULL);
    }

    return 0;
}

int topological_execute(graph *rule_graph, void *target_name, vector *execute_list) {
    if (target_name == NULL) {
        return 0;
    }

#ifdef DEBUG
        printf("Target_name: %s\n", (char *)target_name);
        printf("Target_n: %ld\n", (long)target_name);
#endif

    rule_t *rule = (rule_t *)graph_get_vertex_value(rule_graph, target_name);

    if (rule->state == VISITING) {
        return CYCLE;
    }

    if (rule->state > VISITING) {
        return 0;
    }

    rule->state = VISITING;

    int should_run = 1;
    vector *neighbors = graph_neighbors(rule_graph, target_name);

    while(!vector_empty(neighbors)) {
        void **next_name = vector_front(neighbors);

        int depend_type = topological_execute(rule_graph, *next_name, execute_list);

        // ----- Previous -----

        /*if (depend_type == FAIL) {*/
            /*// Clean up*/
            /*vector_destroy(neighbors);*/
            /*return FAIL;*/
        /*}*/

        // ----- end of Previous -----

        if (depend_type == CYCLE) {
            vector_destroy(neighbors);
            return CYCLE;
        }

        if (depend_type < 0 || depend_type == ISFILE) {
            should_run = 0;
        }

        vector_erase(neighbors, 0);
    }

    rule->state = VISITED;

    /*int ret = ISFILE;*/
    if (access(target_name, F_OK) == -1) {

        // TODO append to execute_list
        vector_push_back(execute_list, rule);
        vector_destroy(neighbors);
        return NOTFILE;

        // ----- Previous -----

        /*if (execute_command(rule) != -1) {*/
            /*ret = NOTFILE;*/
        /*} else {*/
            /*return FAIL;*/
        /*}*/

        // ----- End of Previous -----
    } else if (should_run == 1) {
        // TODO append to execute_list
        vector_push_back(execute_list, rule);
        vector_destroy(neighbors);
        return NEWFILE;

        // ----- Previous -----

        /*if (execute_command(rule) != -1) {*/
            /*ret = NEWFILE;*/
        /*} else {*/
            /*return FAIL;*/
        /*}*/

        // ----- End of Previous
    }

    // Clean up
    vector_destroy(neighbors);

    return ISFILE;
}

// Treat this as main
int parmake(int argc, char **argv) {
  // good luck!

    // Parse input
    char *make_file = NULL;
    int n_threads = 0;
    int n_opts = 0;

    int flag;
    while ((flag = getopt(argc, argv, ":fj:")) != -1) {
        switch (flag) {
            case 'f':
                make_file = argv[optind];
                n_opts++;
                break;
            case 'j':
                n_threads = atoi(optarg);
                n_opts++;
                break;
            default:
                break;
        }
    }

    if (n_threads < 1) {
        n_threads = 1;
    }

    char **targets = NULL;
    int n_targets = 0;
    if (argc > 1 + 2 * n_opts) {
        targets = argv + 1 + 2 * n_opts;
        n_targets = argc - 1 - 2 * n_opts;
    }

    if (n_targets < 1) {
        n_targets = 1;
    }

    if (make_file == NULL) {
        if (access("./makefile", F_OK) != -1) {
            make_file = "./makefile";
        } else if (access("./Makefile", F_OK) != -1) {
            make_file = "./Makefile";
        } else {
#ifdef DEBUG
            printf("No makefile found!\n");
#endif
            return -1;
        }
    }

    // get rule_graph
    graph *rule_graph = parser_parse_makefile(make_file, targets);
    if (rule_graph == NULL) {
#ifdef DEBUG
        printf("get graph failed\n");
#endif
        return -1;
    }


    vector *target_names = graph_neighbors(rule_graph, "");

    while(!vector_empty(target_names)) {
        make_fail = 0;

        void **target_name = vector_front(target_names);
        vector *execute_list = vector_create(NULL, NULL, NULL);

        if (topological_execute(rule_graph, *target_name, execute_list) == CYCLE) {

            print_cycle_failure((char *)(*target_name));
#ifdef DEBUG
            printf("FAILED\n");
#endif
        }

        // TODO Parallel execute
        int exec_results = parallel_execute(n_threads, execute_list);

        if (exec_results == FAIL || make_fail == 1) {
            /*printf("FAILED\n");*/
        }

        // Clean up
        vector_erase(execute_list, 0);
        vector_erase(target_names, 0);
    }

    // Clean up
    vector_destroy(target_names);
    graph_destroy(rule_graph);

#ifdef DEBUG
    printf("--------\nDONE!\n");
#endif

    return 0;
}
