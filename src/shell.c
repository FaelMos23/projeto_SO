#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <fcntl.h>              /* Definition of O_* constants */
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// functions
#include "../lib/main_funcs.h"  // functions for use in this file

// this is the character that will show up before every command
const char stchar = '>';
const char* RESET = "\033[0m";
const char* GREEN = "\033[32m";
const char* BLUE =  "\033[34m";


int main()
{
    system("clear");
    printf("Shell Initiated.\n\n");

    int can_run = 1;
    char read_buffer[BUFFER_SIZE];

    // input thread definition
    int can_read = 1;   // traffic light
    read_args a;
    a.can_run = &can_run;
    a.can_read = &can_read;
    a.buffer = read_buffer;

    pthread_t reader;
    pthread_create(&reader, NULL, reading, (void*) &a);

    while(can_run)
    {
        printf("\n%sUser@machine: %s/path%s\n%c ", GREEN, BLUE, RESET, stchar);
        while(can_read)
            sched_yield();    // red light

        // green light

        // exit?
        if(!strcmp(read_buffer, "exit\n")){

            can_run = 0;
            pthread_cancel(reader); // stops the thread, even if it is at fgets

        }
        
        // now that we know we won't exit, we will do the fork and run the command
        else
        {
        
            // TEST, DELETE IT OR COMMENT IT WHEN NOT USING
            printf("\nDid you just say:\t%s", read_buffer);

        }
        can_read = 1;    // returns to red light
    }

    pthread_join(reader, NULL); // waits for thread to finish
    printf("\nShell Terminated.\n");
    return 0;
}
