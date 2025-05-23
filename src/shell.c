#include <fcntl.h>              /* Definition of O_* constants */
#include <unistd.h>
#include <sys/wait.h>
#include <libgen.h>             // to get path to executable shell file
#include <pwd.h>                // environmental variables (frompasswd)

// functions
#include "../lib/main_funcs.h"  // functions for use in this file
// BUFFER_SHELL_SIZE 256

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

    char env_var[5][BUFFER_SHELL_SIZE] = {"USER=", 
                                          "MACHINE=", 
                                          "HOME=",
                                          "CWD=", 
                                          "PATH="};

    // 0-> USER, 1-> MACHINE, 2-> HOME, 3-> CWD, 4-> PATH
    char * const envp[6] = {env_var[0], 
                            env_var[1],
                            env_var[2],
                            env_var[3],
                            env_var[4],
                            NULL};

    strcat(env_var[0], pw->pw_name);
    char temp_MACHINE[BUFFER_SHELL_SIZE];
    gethostname(temp_MACHINE, sizeof(temp_MACHINE));
    strcat(env_var[1], temp_MACHINE);
    strcat(env_var[2], pw->pw_dir);
    strcat(env_var[3], getEnv(env_var[2]));          // CWD starts at HOME

    // get path to this executable and concatenate with the comm directory
    char temp_PATH[BUFFER_SHELL_SIZE];
    int len = readlink("/proc/self/exe", temp_PATH, sizeof(temp_PATH)-1);
    temp_PATH[len] = '\0';
    strcat(env_var[4], dirname(temp_PATH));
    strcat(env_var[4], "/comm");
    
    // defining values
    int can_run = 1;
    char read_buffer[BUFFER_SHELL_SIZE];
    char child_buffer[BUFFER_SHELL_SIZE];
    char* command;
    char** args;

    // input thread definition
    int can_read = 0;   // traffic light
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

        printf("\n%s%s@%s: %s%s%s\n> ", GREEN, getEnv(env_var[0]), getEnv(env_var[1]), BLUE, getEnv(env_var[3]), RESET);
        can_read = 1;
        while(can_read)
            sched_yield();    // red light

        // green light

        // check for lineskip
        if(read_buffer[0] != '\n')
        {

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



                    // MODIFY TO APPLY SCALABILITY
                    // pipe treatment
                    if(!strcmp(command, "test"))
                    {
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
                    //dup2(pp2c[0], STDIN_FILENO);
                    //dup2(pc2p[1], STDOUT_FILENO);
                    
                    close(pc2p[1]);
                    close(pp2c[0]);
                
                    char filePath[BUFFER_SHELL_SIZE];
                    strcpy(filePath, getEnv(envp[4]));
                    strcat(filePath, "/");
                    strcat(filePath, command);

                    execve(filePath, args, envp);

                    // error handling
                    perror("Fail to run command: ");
                    _exit(2);
                }

            }
            can_read = 1;    // returns to red light
        }

        }

    pthread_join(reader, NULL); // waits for thread to finish
    printf("\nShell Terminated.\n");
    return 0;
}
