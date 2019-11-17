//
// Created by kiva on 2019/11/17.
//
#include <csignal>
#include <cerrno>
#include <iostream>
#include <fstream>
#include <string>
#include <json/json.h>
#include <cppcodec/base64_rfc4648.hpp>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using Base64 = cppcodec::base64_rfc4648;
using CodeSession = std::pair<std::string, std::string>;

CodeSession parseSessionStream(std::istream &stream) {
    Json::Value root;

    Json::CharReaderBuilder builder;
    std::string errs;
    if (!parseFromStream(builder, std::cin, &root, &errs)) {
        throw std::runtime_error("failed to parse code session: " + errs);
    }

    std::string encodedCode = root["code"].asString();
    std::string encodedInput = root["stdin"].asString();

    std::string code = Base64::decode<std::string>(encodedCode);
    std::string input = Base64::decode<std::string>(encodedInput);

    return std::make_pair(code, input);
}

std::string createTempFile(const char *nameTemplate_, const std::string &content) {
    char *nameTemplate = strdup(nameTemplate_);
    if (nameTemplate == nullptr) {
        throw std::runtime_error("cs-online: out of memory");
    }

    char *tempFileName = mktemp(nameTemplate);
    if (tempFileName == nullptr) {
        free(nameTemplate);
        throw std::runtime_error("cs-online: no available temporary file");
    }

    std::string fileName = nameTemplate;
    free(nameTemplate);

    std::ofstream out(fileName, std::ios::out);
    if (!out.good()) {
        throw std::runtime_error("cs-online: failed to open temporary file");
    }

    out << content;
    out.flush();
    out.close();

    return std::move(fileName);
}

int main(int argc, const char **argv) {
    CodeSession session = parseSessionStream(std::cin);
    std::string csFile = createTempFile("/tmp/csonline.XXXXXXXXXX", session.first);

    int fds[2] = {0};
    if (pipe(fds) != 0) {
        perror("cs-online: pipe()");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("cs-online: fork()");

    } else if (pid == 0) {
        // receive stdin from parent process
        close(fds[1]);
        if (dup2(fds[0], STDIN_FILENO) != STDIN_FILENO) {
            perror("cs-online(child): stdin");
            exit(1);
        }

        char *bin = strdup("/usr/bin/cs-code-runner");
        char *timeout = strdup("10");
        char *file = strdup(csFile.c_str());

        if (bin == nullptr || file == nullptr || timeout == nullptr) {
            fprintf(stderr, "cs-online(child): out of memory\n");
            exit(1);
        }

        char *const args[4] = {bin, timeout, file, nullptr};
        execve(bin, args, nullptr);

        // if execve() returns, the call failed
        free(bin);
        free(timeout);
        free(file);
        perror("cs-online(child): execve()");
        exit(1);

    } else {
        close(fds[0]);
        write(fds[1], session.second.c_str(), session.second.size());
        close(fds[1]);

        int status = 0;
        if (waitpid(pid, &status, 0) < 0) {
            perror("cs-online: waitpid()");
        }
        unlink(csFile.c_str());
    }

    return 0;
}
