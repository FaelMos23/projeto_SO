#ifndef MAIN_FUNCS_H
#define MAIN_FUNCS_H


#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#define BUFFER_SHELL_SIZE 256

// FLAGS MASKS
#define OUT_FILE 0b100
#define IN_PROC  0b010
#define OUT_PROC 0b001


typedef struct Process_info{
    char* command;
    int argc;
    char** args;
    char* outputFilePath;
    int flags;
}proc_info;
/*
    FLAGS EXPLANATION
    3 BITS

    --- 0b100 means OUTPUT PIPED TOWARDS FILE "target_comm arg1 arg2 > file.extension"

    --- 0b010 means INPUT PIPED FROM ANOTHER PROCESS "other_comm arg1 | target_comm arg1 arg2"

    --- 0b001 means OUTPUT PIPED TOWARDS ANOTHER FILE "target_comm arg1 arg2 | other_comm arg1"

    --- Possible iterations
        000 - NORMAL
        001 - OUT PROC
        010 - IN PROC
        100 - OUT FILE
        011 - IN PROC, OUT PROC
        110 - IN PROC, OUT FILE
*/

// boolean for testing if line is an executable
static inline int isExec(char* line)
{
    int i;

    for(i=0; line[i]==' ';i++){}

    if(line[i] == '.' || line[i] == '/')
        return 1;

    return 0;
}

// returns number of processes found
static inline int getProcesses(proc_info** processArray, char* text, int* num_pipes)
{
    int i, num_procs = 1;

    for(i=0; i<BUFFER_SHELL_SIZE; i++)
    {
        if(text[i] == '\n')
            break;

        if(text[i] == '&' || text[i] == '|')
        {
            num_procs++;
            if(text[i] == '|')
                (*num_pipes)++;
        }
    }

    
    int proc_loop, arg_loop;
    proc_info* procArray = (proc_info*) malloc(num_procs*sizeof(proc_info));
    for(proc_loop = 0; proc_loop<num_procs; proc_loop++)
    {
        // initializing with NULL
        procArray[proc_loop].command = NULL;
        procArray[proc_loop].args = NULL;
        procArray[proc_loop].outputFilePath = NULL;
    }

    // start separating text into tokens
    char *tmp = strdup(text);
    char *tmp2 = strdup(text);
    
    int pipeIn = 0;
    char* storeTok;
    char* storeTokCount;

    char *tok_count = strtok_r(tmp, " \n",&storeTokCount); // starts the token analyzing
    char *tok = strtok_r(tmp2, " \n", &storeTok); // starts the token analyzing


    for(proc_loop = 0; proc_loop<num_procs; proc_loop++)
    {
        // SET FLAGS AND COUNT ARGUMENTS
        procArray[proc_loop].flags = pipeIn; // sets to 0 if this process doesn't receive from another process
        pipeIn = 0; // resets the pipeIn for the next command

        int count = 0;
        for (; tok_count; tok_count = strtok_r(NULL, " \n", &storeTokCount))
        {
            if(tok_count[0] == '|')
            {
                procArray[proc_loop].flags = procArray[proc_loop].flags | OUT_PROC;
                pipeIn = IN_PROC; // for next process
                
                break;
            }
            if(tok_count[0] == '>')
            {
                procArray[proc_loop].flags = procArray[proc_loop].flags | OUT_FILE;
                tok_count = strtok_r(NULL, " \n", &storeTokCount);
                procArray[proc_loop].outputFilePath = strdup(tok_count); // must be freed
                tok_count = strtok_r(NULL, " \n", &storeTokCount); // prepares next process (only skips '&', others don't make sense)

                break;
            }
            if(tok_count[0] == '&') // another process
            {
                break;
            }
            count++;
        }
        tok_count = strtok_r(NULL, " \n", &storeTokCount); // prepares next process

        procArray[proc_loop].argc = count;

        // solves error with comm1 arg1 > file.txt & comm2 arg2
        // for when there is a process after a file redirection
        // EDIT: this actually broke executable running, so I added '!isExec(text) &&'
        if(!isExec(text) && (tok[0] == '.' || tok[0] == '/'))
        {
            tok = strtok_r(NULL, " \n", &storeTok);
            tok = strtok_r(NULL, " \n", &storeTok);
        }

        // command is first token
        procArray[proc_loop].command = strdup(tok); // must be freed

        // sets array of args size
        procArray[proc_loop].args = (char**) malloc((count+1)*sizeof(char*));
        for (arg_loop=0; arg_loop<count; arg_loop++)
        {
            procArray[proc_loop].args[arg_loop] = strdup(tok); // must be freed

            tok = strtok_r(NULL, " \n", &storeTok);
        }
        tok = strtok_r(NULL, " \n", &storeTok); // prepares next process

        // NULL terminated string
        procArray[proc_loop].args[arg_loop] = NULL;
    }


    // result of array of processes
    *processArray = procArray;
    
    free(tmp);
    free(tmp2);

    return num_procs;
}

static inline void freeProcArr(proc_info* procArray, int num_procs)
{
    int clean;
    for(clean=0; clean<num_procs; clean++)
    {
        if(procArray[clean].command != NULL)
            free(procArray[clean].command);

        if(procArray[clean].args != NULL)
        {
            for(int i=0; i<procArray[clean].argc; i++)
            {
                if(procArray[clean].args[i] != NULL)
                    free(procArray[clean].args[i]);
            }

            free(procArray[clean].args);
        }

        if(procArray[clean].outputFilePath != NULL)
            free(procArray[clean].outputFilePath);
    }

    free(procArray);
    return;
}

// separates the path into an Array
static inline int getPath(char* allPaths, char*** pathArray)
{
    int numPaths = 1;

    int i;
    int allPathsSize = strlen(allPaths);
    for(i=0; i<allPathsSize; i++)
        if(allPaths[i] == ':') numPaths++;

    char* tmp;
    tmp = strdup(allPaths); // must be freed
    
    char* tok = strtok(tmp, ":");

    char** pA = (char**) malloc(numPaths*sizeof(char*));

    for(i=0; i<numPaths; i++)
    {
        pA[i] = tok;
        tok = strtok(NULL, ":");
    }

    *pathArray = pA;

    return numPaths;
}

static inline char* getEnv(char* eVar)
{
    char* currChar = eVar;
    while(*(currChar++) != '=');

    return currChar;
}
static inline int skip(char* line)
{
    int i;

    for(i=0; line[i]==' ';i++){}

    if(line[i] != '\n')
        return 1;

    return 0;
}

static inline int checkDir(char* dir){
    DIR* d = opendir(dir);
    if (d) {
        closedir(d);  // memory leak if not closed
        return 1;
    }
    return 0;
}

static inline int dirInPath(char* dir, char* PATH)
{
    char* tmp = strdup(PATH);
    char* currPath = strtok(tmp, "=");
    int res = 1;

    for(currPath = strtok(NULL, ":"); currPath != NULL; currPath = strtok(NULL, ":"))
    {
        if(!strcmp(currPath, dir))
            break;

        res++;
    }

    free(tmp);

    if(currPath == NULL)
        return 0;
    return res;
}

#endif