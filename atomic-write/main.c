#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "log.h"

const static char *file_path = "test.txt";

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void log_lock_func(bool lock, void *udata) {
    if (lock) {
        pthread_mutex_lock(&mutex);
    } else {
        pthread_mutex_unlock(&mutex);
    }
}

void *thread_func(void *arg) {
    log_info("current thread id: %ld, input arg: %ld", pthread_self(),
             *(long int *)arg);

    int fd = open(file_path, O_CREAT | O_APPEND | O_WRONLY | O_SYNC, 0755);
    if (fd == -1) {
        log_error("open file failed: %s", strerror(errno));
        exit(1);
    }

    char buf[100] = {0};
    int cnt = sprintf(buf, "%ld\n", pthread_self());
    cnt = write(fd, buf, cnt);
    if (cnt == -1) {
        log_error("write to test file failed: %s", strerror(errno));
        exit(1);
    }
    log_info("write %d bytes data to %s", cnt, file_path);

    if (close(fd) == -1) {
        log_error("close file failed: %s", strerror(errno));
        exit(1);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    log_set_lock(log_lock_func, NULL);

    const int cpu_core_count = sysconf(_SC_NPROCESSORS_ONLN);
    pthread_t *threads = calloc(sizeof(pthread_t), cpu_core_count);

    for (int i = 0; i < cpu_core_count; i++) {
        if (pthread_create(threads + i, NULL, thread_func, threads + i) != 0) {
            log_error("create thread failed: %s", strerror(errno));
            goto exit_cleanup;
        }
    }

exit_cleanup:
    for (int i = 0; i < 4; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            log_error("wait for thread's termination failed: %s",
                      strerror(errno));
        }
    }

    free(threads);
    threads = NULL;

    return 0;
}
