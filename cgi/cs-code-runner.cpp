#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <ctime>
#include <string.h>
#include <string>

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
        std::string bin = "/usr/bin/cs";
        std::string cs = argv[1];
        const char *args[3] = {bin.c_str(), cs.c_str(), nullptr};
        execve(bin.c_str(), args, nullptr);

    } else {
        waitpid_timeout(pid, timeout);
    }
}
