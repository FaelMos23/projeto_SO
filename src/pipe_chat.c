#include <stdio.h>
#include <stdlib.h>
#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <fcntl.h>              /* Definition of O_* constants */
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
const int MSGSIZE = 21;      // set to the highest space occupied by constant messages

// constant messages of system
const char* entry_msg = "\nProgram Initiated.";
const char* exit_msg =  "\nProgram Terminated.";


int main()
{
    char in_buffer[MSGSIZE];
    int pid, verify;
    int running = 1;
    int p1[2], p2[2];   // p[0] -> read
                        // p[1] -> write

    /*
        p1 ==> father <-- child
        p2 ==> father --> child
    */

    // creates pipes
    if (pipe(p1) < 0)
        exit(1);
    if (pipe(p2) < 0)
        exit(1);

    pid = fork();
    if(pid < 0)
        return 1;

    // father process
    if(pid > 0)
    {
        close(p1[1]);   // no writing on p1
        close(p2[0]);   // no reading on p2


        // reads the message on pipe
        verify = read(p1[0], in_buffer, MSGSIZE);
        if(!verify)
            exit(1);

        printf("\n%s\n", in_buffer);

        close(p1[0]);

        // loop
        while(running)
        {
            // gets info (no bigger than MSGSIZE)
            fgets(in_buffer, MSGSIZE, stdin);
            in_buffer[strcspn(in_buffer, "\n")] = 0;    // removes '\n'

            // sends to child
            verify = write(p2[1], in_buffer, MSGSIZE);
            if(!verify)
                exit(1);

            if(!strcmp(in_buffer, "exit"))
                running = 0;
        }


        close(p2[1]);
        wait(NULL); // this makes sure there won't be a zombie
    }
    // child process
    else {
        close(p1[0]);   // no reading on p1
        close(p2[1]);   // no writing on p2


        // writes the message on pipe
        verify = write(p1[1], entry_msg, MSGSIZE);
        if(!verify)
            exit(1);

        close(p1[1]);

        while(running)
        {
            // receives input from father
            verify = read(p2[0], in_buffer, MSGSIZE);
            if(!verify)
                exit(1);


            if(!strcmp(in_buffer, "exit"))
                running = 0;
            else
                printf("\n-->%s<--\n", in_buffer);
        }


        close(p2[0]);
    }

    return 0;
}
