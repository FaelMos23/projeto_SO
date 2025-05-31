#include <stdio.h>
#include <stdlib.h>



int main(int argc, char *argv[]) {

    char filepath[256];
    const char* cwd = getenv("CWD");
    snprintf(filepath, sizeof(filepath), "%s/%s", cwd, argv[1]);
    FILE *file = fopen(filepath, "r");   //open file
    
    
    char c;
    while ((c = fgetc(file)) != EOF)    //EOF -> END OF FILE
     {
        printf("%c",c); //print character by character
    }

    fclose(file);
    return 0;
}