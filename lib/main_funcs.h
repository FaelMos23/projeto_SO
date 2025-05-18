#ifndef MAIN_FUNCS_H
#define MAIN_FUNCS_H


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#define BUFFER_SIZE 256


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

static inline void getCommandArgs(char **outComm, char ***outArgs, char *line)
{
    // count comm+args
    int count = 0;
    char *tmp = strdup(line);
    // while there still are arguments, keep going
    for (char *p = strtok(tmp, " \n"); p; p = strtok(NULL, " \n"))
    {
        count++;
    }
    free(tmp);

    // allocate argv array (NULL‑terminated)
    *outArgs = (char**) malloc((count + 1) * sizeof(char*));

    // add command and args to argv
    int i = 0;
    char *tok = strtok(line, " \n");
    *outComm = tok;
    (*outArgs)[i++] = tok;
    while ((tok = strtok(NULL, " \n")))
    {
        (*outArgs)[i++] = tok;
    }
    (*outArgs)[i] = NULL;
}


#endif