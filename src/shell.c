#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <fcntl.h>              /* Definition of O_* constants */
#include <unistd.h>
#include <sys/wait.h>
#include <libgen.h>             // to get path to executable shell file
#include <pwd.h>                // environmental variables (frompasswd)

// functions
#include "../lib/main_funcs.h"  // functions for use in this file

// this is the character that will show up before every command
const char* RESET = "\033[0m";
const char* GREEN = "\033[32m";
const char* BLUE =  "\033[34m";


int main()
{
    system("clear");
    printf("Shell Initiated.\n\n");

    // initializes enviroment variables
    struct passwd* pw = getpwuid(geteuid());

    if (pw == NULL) {
        perror("getpwuid");
        return 1;
    }

    char USER[BUFFER_SIZE];
    char MACHINE[BUFFER_SIZE];
    char HOME[BUFFER_SIZE];
    char CWD[1024];
    char PATH[1024];

    strcpy(USER, pw->pw_name);
    gethostname(MACHINE, sizeof(MACHINE));
    strcpy(HOME, pw->pw_dir);
    strcpy(CWD, HOME);          // CWD starts at HOME

    // get path to this executable and concatenate with the comm directory
    int len = readlink("/proc/self/exe", PATH, sizeof(PATH)-1);
    PATH[len] = '\0';
    strcpy(PATH, dirname(PATH));
    strcat(PATH, "/comm");


    // defining values
    int can_run = 1;
    char read_buffer[BUFFER_SIZE];
    char child_buffer[BUFFER_SIZE];
    char* command;
    char** args;

    // input thread definition
    int can_read = 1;   // traffic light
    read_args a;
    a.can_run = &can_run;
    a.can_read = &can_read;
    a.buffer = read_buffer;

    pthread_t reader;
    pthread_create(&reader, NULL, reading, (void*) &a);

    // pipes initialization
    int verify;
    int pp2c[2];   // pipe parent to child
    int pc2p[2];   // pipe child to parent
    /*
        p[0] -> receive
        p[1] -> send
    */


    while(can_run)
    {

        if (pipe(pp2c) < 0)
            exit(1);
        if (pipe(pc2p) < 0)
            exit(1);

        printf("\n%s%s@%s: %s%s%s\n> ", GREEN, USER, MACHINE, BLUE, CWD, RESET);
        while(can_read)
            sched_yield();    // red light

        // green light

        getCommandArgs(&command, &args, read_buffer);

        // exit?
        if(!strcmp(command, "exit")){

            can_run = 0;
            pthread_cancel(reader); // stops the thread, even if it is at fgets

        }

        // now that we know we won't exit, we will do the fork and run the command
        else
        {

            pid_t pid = fork();
            if(pid < 0)
                return 1;

            // parent
            if(pid > 0)
            {
                close(pc2p[1]); // no sending
                close(pp2c[0]); // no receiving

                // pipe treatment
                if(!strcmp(command, "test"))
                {
                    verify = write(pp2c[1], USER, BUFFER_SIZE);
                    if(!verify)
                        exit(1);

                    verify = read(pc2p[0], child_buffer, BUFFER_SIZE);
                    if(!verify)
                        exit(1);

                    child_buffer[verify] = '\0';
                    printf("%s", child_buffer);
                }

                // pipe treatment for command cd
                if (!strcmp(command, "cd")) {
                    // send HOME to child (cd.exe)
                    verify = write(pp2c[1], HOME, BUFFER_SIZE);
                    if (!verify)
                        exit(1);

                    // send CWD to child (cd.exe)
                    verify = write(pp2c[1], CWD, BUFFER_SIZE);
                    if (!verify)
                        exit(1);

                    // parent receives new path
                    verify = read(pc2p[0], child_buffer, BUFFER_SIZE);
                    if (!verify)
                        exit(1);

                    child_buffer[verify] = '\0';

                    // prints the new path
                    // printf("%s", child_buffer);
                    // update CWD
                    strncpy(CWD, child_buffer, sizeof(CWD));
                }

                close(pc2p[0]);
                close(pp2c[1]);
                waitpid(pid, NULL, 0);
            }
            // child
            else
            {
                close(pc2p[0]); // no receiving
                close(pp2c[1]); // no sending

                // points one pipe to child and the other to parent
                dup2(pp2c[0], STDIN_FILENO);
                dup2(pc2p[1], STDOUT_FILENO);

                close(pc2p[1]);
                close(pp2c[0]);

                if(!strcmp(command, "test"))
                {
                    char fullPath[1024];
                    strcpy(fullPath, PATH);
                    strcat(fullPath, "/test.exe");

                    execv(fullPath, args);
                }

                if(!strcmp(command, "cd"))
                {
                    char fullPath[1024];
                    strcpy(fullPath, PATH);
                    strcat(fullPath, "/cd.exe");

                    execv(fullPath, args);
                }
                // execv error handling
            }

        }
        can_read = 1;    // returns to red light
    }

    pthread_join(reader, NULL); // waits for thread to finish
    printf("\nShell Terminated.\n");
    return 0;
}