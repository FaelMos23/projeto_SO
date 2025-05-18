#include <stdio.h>
#define BUFFER_SIZE 255


int main(int argc, char* argv[])
{

    char user[BUFFER_SIZE];
    fgets(user, sizeof(user), stdin);

    printf("%s disse %s\n", user, argv[1]);

    return 0;
}