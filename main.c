#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
#include <signal.h>

int main(int argc, char **argv) {
    char buff[1024];
    printf("shell# ");
    fgets(buff,1024,stdin);
    // switch (/* expression */) {
    //     case /* value */:
    // }
    return 0;
}
