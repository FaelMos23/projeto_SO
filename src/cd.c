#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define BUFFER_SIZE 255

int main(int argc, char *argv[]) {
    char HOME[BUFFER_SIZE];
    char CWD[BUFFER_SIZE];

    // read HOME from buffer
    fgets(HOME, BUFFER_SIZE, stdin);
    HOME[strcspn(HOME, "\n")] = '\0'; // remove \n

    strncpy(CWD, HOME, BUFFER_SIZE);

    // read CWD from buffer
    // fgets(CWD, BUFFER_SIZE, stdin);
    // CWD[strcspn(CWD, "\n")] = '\0'; // remove \n

    // check if exists more 1 argument
    if (argc > 1) {
        if (strcmp(argv[1], "..") == 0) {
            char path[BUFFER_SIZE];

            // copy CWD to path
            strncpy(path, CWD, BUFFER_SIZE);

            // remove the last directory from CWD
            char *last_slash = strrchr(path, '/');
            if (last_slash != NULL && last_slash != path) {
                *last_slash = '\0'; // remove the last directory
                 strcpy(CWD, path); // update CWD
            } else {
                // path raiz
                //printf("Caminho: %s\n", path);
                strcpy(CWD, "/");
            }

        }else {
            char path[BUFFER_SIZE];

            // check the absolute path
            if (argv[1][0] == '/') {
                strncpy(path, argv[1], BUFFER_SIZE);
            }else{
                // argument + CWD
                snprintf(path, BUFFER_SIZE, "%s/%s", CWD, argv[1]);
            }

            // check if the directory exist
            struct stat st;
            if (stat(path, &st)== 0 && S_ISDIR(st.st_mode)){
                strncpy(CWD, path, BUFFER_SIZE); //update CWD
            }else{
                // exit(1); // make tratament in shell.c
                //printf("Directory don't exist.\n");
            }

        }

    }else {
        // no argument, return to HOME

        strncpy(CWD, HOME, BUFFER_SIZE);
    }

    // clear buffer stdin
    // int ch;
    // while ((ch = getchar()) != '\n' && ch != EOF);

    // show the diretory update
    printf("%s", CWD);

    return 0;
}