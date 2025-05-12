#ifndef MAIN_FUNCS_H
#define MAIN_FUNCS_H


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define BUFFER_SIZE 255


typedef struct Read_args
{
    int* can_read;
    int* can_run;
    char* buffer; // don't need to define size, as long as I use fgets to get an appropriate size
}read_args;


static inline void* reading(void* a)
{
    // permits the main thread to cancel this one, because when exit was typed, this thread was still mid loop, in fgets
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype (PTHREAD_CANCEL_DEFERRED, NULL);

    read_args* args = (read_args*) a;
    while(*args->can_run){
        
        while(!(*args->can_read))
            sched_yield();   // red light

        // green light
        if (fgets(args->buffer, BUFFER_SIZE, stdin))    // buffer is being used as a pointer, no need for *
            *args->can_read = 0;                                            // returns to red light, only if a value was read
    }

    return NULL;
}


#endif