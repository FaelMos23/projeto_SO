#include <stdio.h>
#include <stdlib.h>
#include <dirent.h> //lib to manipule directores 

int main(int argc, char *argv[]) {
    const char *directory; //directory of the argument
    if (argc > 1) {
        directory = argv[1];
    } else {
        
        directory = ".";
    }

    DIR *dir;  //pointer for a directory, like type FILE
    struct dirent *entry; //entry of a directory

    dir = opendir(directory); //open directory
//every time than "readdir" is invoqued the pointer "entry" is updated for the next file or dir
    while ((entry = readdir(dir)) != NULL) {
    printf("%s  ", entry->d_name);
    }
    printf("\n");
    closedir(dir); //close directory 
    return 0;
}