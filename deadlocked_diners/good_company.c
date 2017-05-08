/**
 * Deadlocked Diners Lab
 * CS 241 - Spring 2017
 */
#include "company.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *work_interns(void *p) {
    Company *company = (Company *)p;

    pthread_mutex_t *m1;
    pthread_mutex_t *m2;

    while (running) {
        m1 = Company_get_left_intern(company);
        m2 = Company_get_right_intern(company);

        if (m1 == m2) {
            return NULL;
        }

        pthread_mutex_lock(m1);

        while (1) {
            int two_locked = pthread_mutex_trylock(m2);
            if (two_locked == 0) {
                break;
            }
            else {
                pthread_mutex_unlock(m1);
                usleep(10000);
                Company_have_board_meeting(company);
                pthread_mutex_lock(m1);
            }
        }
        Company_hire_interns(company);
        pthread_mutex_unlock(m1);
        pthread_mutex_unlock(m2);
        usleep(300000);
    }

    return NULL;
}
