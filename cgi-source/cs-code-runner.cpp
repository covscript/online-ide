//
// Created by kiva on 2019/11/16.
//
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <cerrno>
#include <ctime>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "cs-online-ide-config.h"

int timed_wait(const sigset_t *__restrict set,
               siginfo_t *__restrict info,
               const struct timespec *__restrict timeout) {
#if HAVE_SIGTIMEDWAIT
    return sigtimedwait(set, info, timeout);
#else
    return 0;
#endif
}

int waitpid_timeout(pid_t pid, int ms) {
    sigset_t mask, orig_mask;
    sigemptyset(&mask);

    // When pid exits, the parent receives a SIGCHLD signal
    sigaddset(&mask, SIGCHLD);

    if (sigprocmask(SIG_BLOCK, &mask, &orig_mask) < 0) {
        perror("sigprocmask");
        return 1;

    } else {
        struct timespec timeout{};
        timeout.tv_sec = ms / 1000;
        timeout.tv_nsec = (ms % 1000) * 1000000;

        do {
            if (timed_wait(&mask, nullptr, &timeout) < 0) {
                if (errno == EINTR) {
                    // timed_wait() was interrupted by a signal
                    continue;

                } else if (errno == EAGAIN) {
                    printf("Time limit exceeded: %d (ms)\n", ms);
                    kill(pid, SIGKILL);

                } else {
                    perror("timed_wait");
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
    if (argc != 3) {
        fprintf(stderr, "The Covariant Script Code Runner v1.0\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "Usage: cs-code-runner <timeout> <file>\n");
        fprintf(stderr, "  where\n");
        fprintf(stderr, "    timeout       The max execution time in milliseconds\n");
        fprintf(stderr, "    file          The script file to run\n");
        fprintf(stderr, "\n");
        return 1;
    }

    int timeout = atoi(argv[1]);
    const char *file = argv[2];

    if (timeout <= 0) {
        fprintf(stderr, "cs-code-runner: invalid timeout: %d\n", timeout);
        return 1;
    }

    if (file == nullptr) {
        fprintf(stderr, "cs-code-runner: invalid file path: null\n");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("cs-code-runner: failed to fork()");

    } else if (pid == 0) {
        // This code executes in the child process
        char *bin = strdup("/usr/bin/cs");
        char *cs = strdup(file);

        if (bin == nullptr || cs == nullptr) {
            fprintf(stderr, "cs-code-runner(child): out of memory\n");
            exit(1);
        }

        char *const args[3] = {bin, cs, nullptr};
        execve(bin, args, nullptr);

        // if execve() returns, the call failed
        free(bin);
        free(cs);
        perror("cs-code-runner(child): execve()");
        exit(1);

    } else {
        // Parent process
        waitpid_timeout(pid, timeout);
    }

    return 0;
}
