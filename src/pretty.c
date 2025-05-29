#include <stdio.h>
#include <string.h>


int main()
{
    char text[256];
    
    fgets(text, 256, stdin);
    
    char *nl = strchr(text, '\n');
    if (nl) *nl = '\0';

    int size = strlen(text);
    int i;

    for(i=0; i<size+4;i++) printf("_");
    printf("\n| %s |\n|", text);
    for(i=0; i<size+2;i++) printf("_");
    printf("|\n");

    return 0;
}