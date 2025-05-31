#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>              /* Definition of O_* constants */
#include <unistd.h>


int main(int argc, char* argv[])
{
    int i, N = argc, sum = 0;
    char output[256];

    for(i=1; i<N; i++)
        sum += atoi(argv[i]);

    
    printf("%d\n", sum);

    return 0;
}