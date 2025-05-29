#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>  // lib to rescue file infos
#include <string.h>    // strcmp to verify flags
#include <pwd.h>       // verify owner
#include <grp.h>       // verify group
#include <time.h>      // format time
#include <dirent.h> //lib to manipule directores 
#include "../lib/ls_funcs.h"

int main(int argc, char *argv[]) {
    const char *directory = "."; // default directory
    int show_all = 0; //flag -a
    int long_list = 0; //flag -l

//verify directory and flags -a -l 
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0)
            show_all = 1;
        else if (strcmp(argv[i], "-l") == 0)
            long_list = 1;
        else
            directory = argv[i];
    }

    DIR *dir;  //pointer for a directory, like type FILE
    struct dirent *entry; //entry of a directory

    dir = opendir(directory); //open directory
//every time than "readdir" is invoqued the pointer "entry" is updated for the next file or dir
    while ((entry = readdir(dir)) != NULL) {
        if (!show_all && entry->d_name[0] == '.') // if not flag -a not show ocult files or directory
            continue;

        if (long_list) { //if flag -l show details 
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name); //format full path of file

            struct stat info; //informations abount file or directory
            stat(path, &info); //get informations
            print_permissions(info.st_mode); //function to format permissions
            printf("%ld ", info.st_nlink);

            struct passwd *pw = getpwuid(info.st_uid);//get owner
            struct group  *gr = getgrgid(info.st_gid);//get group
            printf("%s %s ", pw->pw_name, gr->gr_name);
            printf("%5ld ", info.st_size);
            //format time
            char timebuf[80];
            strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&info.st_mtime));
            printf("%s ", timebuf);   
        }

        printf("%s\n", entry->d_name);//file name
    }

    closedir(dir);
    return 0;

}