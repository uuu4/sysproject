#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUF_SIZE 4096
#define SHARED_FILE_PATH "mymsgbuf"

#define errExit(msg)        \
    do {                    \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

/* YOU CAN CHANGE THIS */
typedef struct shmbuf {
    sem_t sem;     /** controls read/write */
    size_t cnt;    /** number of bytes used in 'msgbuf' */
    int fd;        /** file descriptor*/
    char msgbuf[]; /* data being transferred */
} ShmBuf;

struct shmbuf *buf_init() {
    struct shmbuf *shmp;

    /*contents of model_init() function*/
    int fd = shm_open(SHARED_FILE_PATH, O_CREAT | O_RDWR, 0600);
    if (fd < 0) {
        errExit("could not open shared file");
    }

    /* Map the object into the caller's address space. */
    shmp =
        mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (shmp != NULL) {
        ftruncate(fd, BUF_SIZE);
        shmp->fd = fd;
        sem_init(&shmp->sem, 1, 1);
    } else {
        errExit("mmap error");
    }

    return shmp;
}

int main() {
    struct shmbuf *shmp = buf_init();
    pid_t pid = fork();

    if (pid == 0) {
        /* child */
        char msg[100] = "hi from child";

        sem_wait(&shmp->sem); /* for synchronization */
        strncpy(shmp->msgbuf, msg, sizeof(msg));
        sem_post(&shmp->sem);

        errExit("msg sent:");
    } else if (pid > 0) {
        /* parent */
        wait(NULL);

        sem_wait(&shmp->sem); /* for synchronization */
        printf("parent received: %d-%.100s\n", shmp->fd,
               shmp->msgbuf);
        sem_post(&shmp->sem);
    }
    shm_unlink(SHARED_FILE_PATH);
    return 0;
}