#include <stdio.h>
#include <unistd.h>

int main() {
    char cwd[1024]; //store actual path string

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
        return 0;
    } else {
        printf("ERROR");
        return 1;
    }
}