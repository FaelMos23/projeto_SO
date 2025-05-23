#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define BUFFER_COMM_SIZE 256


char* getEnv(char* eVar)
{
    char* currChar = eVar;
    while(*currChar != '=')
        currChar++;

    return ++currChar;
}


// extern char **environ; //alternative way to access


int main(int argc, char* argv[], char* envp[])
{

    printf("\n%s disse %s\n", getEnv(envp[0]), argv[1]);

    printf("\n%s esta em %s\n", getEnv(envp[0]), getEnv(envp[3]));
    
    
    // alternative way to print all
    //for (char **e = environ; *e; ++e)
    //    printf("%s\n", *e);

    return 0;
}