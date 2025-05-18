#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    FILE *file = fopen(argv[1], "r");   //open file
    
    
    char c;
    while ((c = fgetc(file)) != EOF)    //EOF -> END OF FILE
     {
        printf("%c",c); //print character by character
    }

    fclose(file);
    return 0;
}