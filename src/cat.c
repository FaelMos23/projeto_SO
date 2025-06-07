#include <stdio.h>
#include <stdlib.h>




int main(int argc, char *argv[]) {

const char* RESET = "\033[0m";
const char* RED = "\033[31m";

    char filepath[256];
    const char* cwd = getenv("CWD");
    snprintf(filepath, sizeof(filepath), "%s/%s", cwd, argv[1]);
    FILE *file = fopen(filepath, "r");   //open file
     if (file == NULL) {
        printf("%sError:Cannot find file:%s %s\n", RED, filepath, RESET);
        return 1;
    }
    
    
    char c;
    while ((c = fgetc(file)) != EOF)    //EOF -> END OF FILE
     {
        printf("%c",c); //print character by character
    }

    fclose(file);
    return 0;
}