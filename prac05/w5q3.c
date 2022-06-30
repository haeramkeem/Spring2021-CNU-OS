#include <stdio.h>
#include <unistd.h>

int main(void) {
        char *path = "/bin/touch";
        char *argv[] = {"/bin/touch", "201702004", NULL};
        char *envp[] = {NULL};

        execve(path, argv, envp);
        return 0;
}