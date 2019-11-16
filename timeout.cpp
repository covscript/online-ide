#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <string.h>

int waitpid_timeout(pid_t pid, int ms) {
    sigset_t mask, orig_mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);

    if (sigprocmask(SIG_BLOCK, &mask, &orig_mask) < 0) {
        perror("sigprocmask");
        return 1;
        
    } else {
        struct timespec timeout;
        timeout.tv_sec = ms / 1000;
        timeout.tv_nsec = (ms % 1000) * 1000000;

        do {
            if (sigtimedwait(&mask, nullptr, &timeout) < 0) {
                if (errno == EINTR) {
                    continue;
                } else if (errno == EAGAIN) {
                    printf("Time limie exceeded: %d (ms)\n", ms);
                    kill(pid, SIGKILL);
                } else {
                    perror("sigtimedwait");
                    return 1;
                }
            }
            break;
        } while (true);

        if (waitpid(pid, nullptr, 0) < 0) {
            perror("waitpid");
            return 1;
        }
        return 0;
    }
}

int main(int argc, const char **argv) {
    const int timeout = 1 * 1000;

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");

    } else if (pid == 0) {
        // execve
        char *args[3]{strdup("/usr/bin/cs"), strdup(argv[1]), nullptr};
        execve("/usr/bin/cs", args, nullptr);

    } else {
        waitpid_timeout(pid, timeout);
    }
}
