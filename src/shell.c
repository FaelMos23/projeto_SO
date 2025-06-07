#include <fcntl.h>              /* Definition of O_* constants */
#include <unistd.h>
#include <dirent.h>             // to check a directory
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
const char* RED = "\033[31m";


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

    strcat(env_var[4], ":");                // maybe delete this
    strcat(env_var[4], temp_PATH);          // second path for
    strcat(env_var[4], "/extras\0");        // final presentation

    // number of paths
    //int path_dirs = 1; // to usando isso?

    // defining values
    int can_run = 1;
    char read_buffer[BUFFER_SHELL_SIZE];

    // pipes initialization
    //int verify; delete?

    // script interaction
    FILE* file = NULL;
    int readScript = 0;


    while(can_run)
    {
        printf("\n%s%s@%s: %s%s%s\n> ", GREEN, getEnv(env_var[0]), getEnv(env_var[1]), BLUE, getEnv(env_var[3]), RESET);

        // reads from script
        if(readScript)
        {
            if (readScript)
            {
                if (fgets(read_buffer, BUFFER_SHELL_SIZE, file) == NULL) {
                    readScript = 0;
                    fclose(file);

                    // continue as normal
                    fgets(read_buffer, BUFFER_SHELL_SIZE, stdin);
                }
                else
                {
                    read_buffer[strcspn(read_buffer, "\r\n")] = '\0';

                    if (!strcmp(read_buffer, "")) strcpy(read_buffer, "\n");

                    printf("%s\n", read_buffer);
                }

            } else {
                fgets(read_buffer, BUFFER_SHELL_SIZE, stdin);
            }
        }
        else  // reads from terminal
            fgets(read_buffer, BUFFER_SHELL_SIZE, stdin);

        // check for lineskip
        if(read_buffer[0] != '\n')
        {
            // array that contains the information for each flag
            proc_info* procArray;
            int num_pipes = 0;
            int currPipe = 0;


            int num_procs = getProcesses(&procArray, read_buffer, &num_pipes);

            // prepare pipes
            /*
                p[0] -> receive
                p[1] -> send
            */
            int i;
            int pipes[num_pipes][2];
            for(i=0; i<num_pipes; i++)
            {
                if (pipe(pipes[i]) < 0)
                    exit(1);
            }
            // used to know which pipe is being set up

            // setting path array
            char** pathArray;
            pathArray = NULL;

            // setting pipes
            pid_t pid[num_procs];

            int proc_loop;
            for(proc_loop=0; proc_loop<num_procs; proc_loop++)
            {

                if(!strcmp(procArray[proc_loop].command, "script"))
                {
                    char filePath_script[BUFFER_SHELL_SIZE];
                    if(procArray[proc_loop].args[1][0] == '/')
                    {
                        strcpy(filePath_script, procArray[proc_loop].args[1]);
                    }
                    else
                    {
                        // CWD
                        strcpy(filePath_script, getEnv(env_var[3]));
                        strcat(filePath_script, "/");
                        strcat(filePath_script, procArray[proc_loop].args[1]);
                    }

                    file = fopen(filePath_script, "r");

                    if (!file) {
                        perror("error opening file");
                        continue;
                    }

                    readScript = 1;
                }
                else
                {
                    // exit?
                    if(!strcmp(procArray[proc_loop].command, "exit"))
                        can_run = 0;
                    else
                    {
                        // cd?
                        if(!strcmp(procArray[proc_loop].command, "cd"))
                        {
                            // STEP 1:
                            // first: make a verification if "/" is absolute (cd /pasta) or relative (cd pasta || cd ./pasta)
                                // if relative, add in CWD the argument (can to be:
                                //                                          my path is: home/aluno/projeto_SO/bin
                                //                                          I need to concat with the argument pasta or /pasta or ./pasta
                                //                                          so the final path is: home/aluno/projeto_SO/bin/pasta or home/aluno/projeto_SO/bin/./pasta)

                            // new string to hold the file path
                            char temp[BUFFER_SHELL_SIZE] = "";
                            char finalCWD[BUFFER_SHELL_SIZE] = "";
                            char newCWD[BUFFER_SHELL_SIZE] = "";

                            if(procArray[proc_loop].argc > 1){
                                // verification of the interaction with cd (ignore the first "/" or "./" the string)
                                // printf("args[0]: %s, args[1]: %s\n", procArray[proc_loop].args[0], procArray[proc_loop].args[1]);
                                if(procArray[proc_loop].args[1][0] == '/')
                                {
                                    // absolute path
                                    strcpy(temp, procArray[proc_loop].args[1]);
                                }
                                else
                                { // relative path
                                    strcpy(temp, getEnv(env_var[3]));
                                    strcat(temp, "/");
                                    strcat(temp, procArray[proc_loop].args[1]);
                                }
                                // printf("Variable temp: %s\n", temp);

                                // STEP 2:
                                // normalize the path
                                // verific if there are ".." to go back to last directory
                                // or "." (ignore)
                                // or just a directory name (add to variable finalPath)
                                // in the final step, we needs to verify if the finalPath exist
                                char* token = strtok(temp, "/");

                                while(token != NULL)
                                {
                                    if(strcmp(token, "..") == 0)
                                    {
                                        // remove the last directory from CWD
                                        char* last_slash = strrchr(finalCWD, '/');
                                        if (last_slash != NULL && last_slash != temp) {
                                            *last_slash = '\0'; // remove the last directory
                                        }
                                    }
                                    else if(strcmp(token, ".") == 0)
                                    {
                                        // ignore
                                    }
                                    else
                                    {
                                        // add the directory to CWD                                        if(strcmp(newPath, "/") != 0)
                                        if (strlen(finalCWD) > 0) {
                                            strcat(finalCWD, "/");
                                        }
                                        strcat(finalCWD, token);
                                    }
                                    token = strtok(NULL, "/");
                                }
                                // printf("Variable finalPath: %s\n", finalPath);
                                // update newPath
                                strcpy(newCWD, "/");
                                strcat(newCWD, finalCWD);
                                // printf("Variable newPath: %s\n", newPath);

                                // verific if exist this path
                                DIR* dir = opendir(newCWD);
                                if (dir) {
                                    // The dir open, so it exist
                                    closedir(dir);

                                    env_var[3][0+4] = '\0'; // clear CWD
                                    strcat(env_var[3], newCWD); // Update CWD

                                    // printf("%s\n", env_var[3]);
                                }else{
                                    printf("%scd: Cannot find the path '%s' because it does not exist.%s\n", RED, newCWD, RESET);
                                }
                            }else{
                                // cd NULL return at HOME
                                env_var[3][0+4] = '\0'; // clear CWD
                                strcat(env_var[3], getEnv(env_var[2])); // copy HOME
                            }
                        }
                        else
                        {
                            // path?
                            if(!strcmp(procArray[proc_loop].command, "path"))
                            {
                                char temp[BUFFER_SHELL_SIZE] = "";

                                if (procArray[proc_loop].argc > 1){
                                    int countPath = 0;

                                    strcpy(temp, env_var[4]); // Copy PATH

                                    while (countPath != procArray[proc_loop].argc) {
                                        if(strcmp(procArray[proc_loop].args[countPath], procArray[proc_loop].command) != 0){
                                            if (checkDir(procArray[proc_loop].args[countPath]) == 1) {
                                                strcat(temp, ":"); // Add ":" in string
                                                strcat(temp, procArray[proc_loop].args[countPath]);
                                            }else{
                                                printf("%spath: Cannot find the path '%s' because it does not exist.%s\n", RED, procArray[proc_loop].args[countPath], RESET);
                                            }
                                        }                               
                                        countPath++;
                                    }

                                    env_var[4][0] = '\0'; // clear PATH
                                    strcpy(env_var[4], temp); // copy new path
                                    // printf("PATH:\n%s\n", env_var[4]);

                                }else{

                                    strcpy(temp, env_var[4]);

                                    char* token = strtok(temp, "=");
                                    printf("\nCurrent PATHS:");
                                    while(token != NULL){
                                        if (strcmp(token, "PATH") == 0){
                                            // Ignore token

                                        }else{
                                            printf("%s\n", token);
                                        }

                                        // Next token
                                        token = strtok(NULL, ":");
                                    }
                                }
                            }
                            else
                            {
                                // executable?
                                if(procArray[proc_loop].command[0] == '.' || procArray[proc_loop].command[0] == '/')
                                {
                                    pid[proc_loop] = fork();
                                    if(pid[proc_loop] < 0)
                                        return 1;

                                    // child
                                    if(pid[proc_loop] == 0)
                                    {
                                        // close unused pipes for this process
                                        int pipe_loop;
                                        for(pipe_loop=0; pipe_loop<num_pipes; pipe_loop++)
                                        {
                                            if(pipe_loop != currPipe && pipe_loop+1 != currPipe)
                                            {
                                                close(pipes[pipe_loop][0]);
                                                close(pipes[pipe_loop][1]);
                                            }
                                        }

                                        if(procArray[proc_loop].flags & IN_PROC)
                                        {
                                            close(pipes[currPipe][1]); // no sending
                                            dup2(pipes[currPipe][0], STDIN_FILENO);
                                            close(pipes[currPipe][0]); // end of transaction
                                            currPipe++; // prepares a new pipe if needed
                                        }

                                        if(procArray[proc_loop].flags & OUT_PROC)
                                        {
                                            close(pipes[currPipe][0]); // no receiving
                                            dup2(pipes[currPipe][1], STDOUT_FILENO);
                                            close(pipes[currPipe][1]); // end of transaction
                                        }

                                        if(procArray[proc_loop].flags & OUT_FILE)
                                        {
                                            // send information to
                                            // procArray[proc_loop].outputFilePath
                                            // may be relative or absolute path


                                            char filePath_output[BUFFER_SHELL_SIZE];
                                            if(procArray[proc_loop].outputFilePath[0] == '/')
                                            {
                                                strcpy(filePath_output, procArray[proc_loop].command);
                                            }
                                            else
                                            {
                                                // CWD
                                                strcpy(filePath_output, getEnv(env_var[3]));
                                                strcat(filePath_output, "/");
                                                strcat(filePath_output, procArray[proc_loop].command);
                                            }


                                            int fd = open(filePath_output,
                                                            O_WRONLY | O_CREAT | O_TRUNC,
                                                            0666 ); // rw-rw-rw-
                                            if (fd < 0) {
                                                perror("open for redirection");
                                                _exit(2);
                                            }
                                            // redirect file as output
                                            if (dup2(fd, STDOUT_FILENO) < 0) {
                                                perror("dup2 for redirection");
                                                _exit(2);
                                            }
                                            close(fd);
                                        }

                                        // RUN THE EXECUTABLE AFTER REDIRECTING INPUT/OUTPUT
                                        char filePath_file[BUFFER_SHELL_SIZE];
                                        if(procArray[proc_loop].command[0] == '/')
                                        {
                                            strcpy(filePath_file, procArray[proc_loop].command);
                                        }
                                        else
                                        {
                                            // CWD
                                            strcpy(filePath_file, getEnv(env_var[3]));
                                            strcat(filePath_file, "/");
                                            strcat(filePath_file, procArray[proc_loop].command);
                                        }

                                        execv(filePath_file, procArray[proc_loop].args);

                                        // error handling
                                        char error_text[BUFFER_SHELL_SIZE];
                                        strcpy(error_text, "Fail to run executable ");
                                        strcat(error_text, procArray[proc_loop].command);
                                        perror(error_text);
                                        _exit(2);
                                    }
                                }
                                else
                                {
                                    // all built-in commands and executables are taken care of
                                    // run commands

                                    pid[proc_loop] = fork();
                                    if(pid[proc_loop] < 0)
                                        return 1;

                                    // child
                                    if(pid[proc_loop] == 0)
                                    {
                                        // close unused pipes for this process
                                        int pipe_loop;
                                        for(pipe_loop=0; pipe_loop<num_pipes; pipe_loop++)
                                        {
                                            if(pipe_loop != currPipe && pipe_loop+1 != currPipe)
                                            {
                                                close(pipes[pipe_loop][0]);
                                                close(pipes[pipe_loop][1]);
                                            }
                                        }


                                        if(procArray[proc_loop].flags & IN_PROC)
                                        {
                                            close(pipes[currPipe][1]); // no sending
                                            dup2(pipes[currPipe][0], STDIN_FILENO);
                                            close(pipes[currPipe][0]); // end of transaction
                                            currPipe++; // prepares a new pipe if needed
                                        }

                                        if(procArray[proc_loop].flags & OUT_PROC)
                                        {
                                            close(pipes[currPipe][0]); // no receiving
                                            dup2(pipes[currPipe][1], STDOUT_FILENO);
                                            close(pipes[currPipe][1]); // end of transaction
                                        }

                                        if(procArray[proc_loop].flags & OUT_FILE)
                                        {
                                            // send information to
                                            // procArray[proc_loop].outputFilePath
                                            // may be relative or absolute path


                                            char filePath_output[BUFFER_SHELL_SIZE];
                                            if(procArray[proc_loop].outputFilePath[0] == '/')
                                            {
                                                strcpy(filePath_output, procArray[proc_loop].outputFilePath);
                                            }
                                            else
                                            {
                                                // CWD
                                                strcpy(filePath_output, getEnv(env_var[3]));
                                                strcat(filePath_output, "/");
                                                strcat(filePath_output, procArray[proc_loop].outputFilePath);
                                            }


                                            int fd = open(filePath_output,
                                                            O_WRONLY | O_CREAT | O_TRUNC,
                                                            0666 ); // rw-rw-rw-
                                            if (fd < 0) {
                                                perror("open for redirection");
                                                _exit(2);
                                            }
                                            // redirect file as output
                                            if (dup2(fd, STDOUT_FILENO) < 0) {
                                                perror("dup2 for redirection");
                                                _exit(2);
                                            }
                                            close(fd);
                                        }


                                        // MUTIPLE PATH HANDLING
                                        int numPaths = getPath(getEnv(env_var[4]), &pathArray);

                                        // tries all paths
                                        int path_loop;
                                        for(path_loop=0; path_loop<numPaths; path_loop++)
                                        {
                                            // RUN THE COMMAND AFTER REDIRECTING INPUT/OUTPUT
                                            char filePath[BUFFER_SHELL_SIZE];
                                            strcpy(filePath, pathArray[path_loop]);
                                            strcat(filePath, "/");
                                            strcat(filePath, procArray[proc_loop].command);


                                            execve(filePath, procArray[proc_loop].args, envp);
                                        }
                                        // error handling, none of the paths worked
                                        char error_text[BUFFER_SHELL_SIZE];
                                        strcpy(error_text, "Fail to run command ");
                                        strcat(error_text, procArray[proc_loop].command);
                                        perror(error_text);
                                        _exit(2);

                                    }

                                }

                            }

                        }

                    }

                }

            }
            // only the parent will get here
            // wait for all forks
            for(proc_loop=0; proc_loop<num_procs; proc_loop++)
                waitpid(pid[proc_loop], NULL, 0);

            if(pathArray != NULL)
                free(pathArray);
            freeProcArr(procArray, num_procs);

        }

    }

    printf("\nShell Terminated.\n");
    return 0;
}