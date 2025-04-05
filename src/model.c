#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../include/project.h"

#define BUF_SIZE 4096
#define MAX_ARGS 64
#define SHARED_FILE_PATH "mymsgbuf"


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
/**
 * execute_command - Girilen komut satırını çalıştırır ve çıktısını yakalar.
 * @command: Çalıştırılacak komut (örn: "ls -l /home")
 *
 * Return: Çocuk sürecin çıkış durumunu döndürür.
 */
int execute_command(const char *command) {
    int pipefd[2];
    char buffer[BUF_SIZE];
    pid_t pid;

    // Pipe oluşturma
    if (pipe(pipefd) == -1) {
        perror("pipe error");
        return -1;
    }

    pid = fork();
    if (pid < 0) {
        perror("fork error");
        return -1;
    }

    if (pid == 0) {
        // Çocuk süreç: stdout ve stderr'i pipe'a yönlendir
        close(pipefd[0]); // Okuma ucunu kapat
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);

        // Komutu tokenize etme
        char *args[MAX_ARGS];
        char *cmd_copy = strdup(command);
        if (!cmd_copy) {
            perror("strdup error");
            exit(EXIT_FAILURE);
        }
        int i = 0;
        char *token = strtok(cmd_copy, " ");
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL; // Argüman listesini sonlandır

        // Komutu çalıştır (execvp komut dizisinin ilk elemanına göre çalışır)
        execvp(args[0], args);
        // Eğer execvp başarısız olursa hata mesajı yazdır ve çık
        perror("execvp failed");
        free(cmd_copy);
        exit(EXIT_FAILURE);
    } else {
        // Ebeveyn süreç: yazma ucunu kapatıp, çocuk sürecin çıktısını oku
        close(pipefd[1]);

        int nbytes = read(pipefd[0], buffer, sizeof(buffer) - 1);
        if (nbytes < 0) {
            perror("read error");
        } else {
            buffer[nbytes] = '\0';
            printf("Komut çıktısı:\n%s\n", buffer);
        }
        close(pipefd[0]);

        // Çocuk süreci bekle ve çıkış durumunu döndür
        int status;
        waitpid(pid, &status, 0);
        return status;
    }
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

