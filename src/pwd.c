#include <stdio.h>
#include <stdlib.h>

int main() {
    const char* cwd = getenv("CWD"); //rescue env CWD
    printf("%s\n", cwd);
    return 0;
}