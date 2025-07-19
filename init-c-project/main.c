#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("please input project name!\n");
        exit(1);
    }

    char *project_name = argv[1];
    struct stat stat_;
    int ret;

    ret = stat(project_name, &stat_);
    if (ret == 0) {
        printf("file or directory named `%s` already exists!\n", project_name);
        exit(1);
    }

    ret = mkdir(project_name, 0755);
    if (ret == -1) {
        perror("create project directory failed");
        exit(1);
    }

    // create CMakeLists.txt
    char path_buf[PATH_MAX] = {0};
    ret = sprintf(path_buf, "%s/CMakeLists.txt", project_name);
    if (ret < 0) {
        perror("initialize path buffer for CMakeLists.txt failed");
        exit(1);
    }

    int fd = open(path_buf, O_RDWR | O_CREAT | O_SYNC, 0644);
    if (fd == -1) {
        perror("create CMakeLists.txt failed");
        exit(1);
    }

    char file_content[200] = {0};
    ret = sprintf(file_content,
                  "cmake_minimum_required(VERSION 3.10)\n"
                  "project(%s)\nset(CMAKE_EXPORT_COMPILE_COMMANDS "
                  "ON)\nadd_executable(%s main.c log.c)",
                  project_name, project_name);
    if (ret < 0) {
        perror("initialize file content buffer failed");
        exit(1);
    }

    ret = write(fd, file_content, strlen(file_content));
    if (ret == -1) {
        perror("create CMakeLists.txt failed");
        exit(1);
    }

    ret = close(fd);
    if (ret == -1) {
        perror("close CMakeLists.txt failed");
        exit(1);
    }

    // create main.c
    ret = sprintf(path_buf, "%s/main.c", project_name);
    if (ret < 0) {
        perror("initialize path buffer for main.c failed");
        exit(1);
    }

    fd = open(path_buf, O_RDONLY | O_CREAT, 0644);
    if (fd == -1) {
        perror("create main.c failed");
        exit(1);
    }

    ret = close(fd);
    if (ret == -1) {
        perror("close main.c failed");
        exit(1);
    }

    // add third party log library
    ret = chdir(project_name);
    if (ret == -1) {
        perror("change process's working directory failed");
        exit(1);
    }

    ret = system(
        "wget https://github.com/rxi/log.c/raw/refs/heads/master/src/log.c");
    if (ret == -1) {
        perror("download log.c failed");
        exit(1);
    }

    ret = system(
        "wget https://github.com/rxi/log.c/raw/refs/heads/master/src/log.h");
    if (ret == -1) {
        perror("download log.h failed");
        exit(1);
    }

    ret = chdir("..");
    if (ret == -1) {
        perror("recover process's working directory failed");
        exit(1);
    }

    return 0;
}
