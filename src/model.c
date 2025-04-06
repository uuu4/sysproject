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
#define SHARED_FILE_PATH "/mymsgbuf"


ShmBuf* buf_init(void) {
    shm_unlink(SHARED_FILE_PATH);
    ShmBuf *shmp;
    int fd = shm_open(SHARED_FILE_PATH, O_CREAT | O_RDWR, 0600);
    if (fd < 0) {
        errExit("could not open shared file");
    }

    // Önce shared memory dosya boyutunu ayarla.
    if (ftruncate(fd, BUF_SIZE) == -1) {
        errExit("ftruncate error");
    }

    // Daha sonra mmap işlemi yap.
    shmp = mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED) {
        errExit("mmap error");
    }

    // Artık shared memory alanı doğru şekilde haritalandı.
    shmp->fd = fd;
    sem_init(&shmp->sem, 1, 1);

    return shmp;
}


int model_send_message(ShmBuf *shmp, const char *msg) {
    if (shmp == NULL || msg == NULL) return -1;

    sem_wait(&shmp->sem); // Kritik bölgeye giriş

    /* Mesajı kopyala.
     * BUF_SIZE, tüm shared memory boyutunu belirler; 
     * burada yapının esnek alanına yazdığımız için, basitçe BUF_SIZE kadar alanı kullanıyoruz.
     */
    strncpy(shmp->msgbuf, msg, BUF_SIZE - sizeof(ShmBuf));
    shmp->msgbuf[BUF_SIZE - sizeof(ShmBuf) - 1] = '\0';  // Null-terminator ekle
    shmp->cnt = strlen(shmp->msgbuf); // Yazılan bayt sayısını güncelle

    sem_post(&shmp->sem); // Kritik bölgeden çıkış
    return 0;
}


int model_read_messages(ShmBuf *shmp, char *buffer, size_t bufsize) {
    if (shmp == NULL || buffer == NULL) return -1;

    sem_wait(&shmp->sem); // Kritik bölgeye giriş

    size_t count = shmp->cnt;
    if (count >= bufsize) {
        count = bufsize - 1;
    }
    strncpy(buffer, shmp->msgbuf, count);
    buffer[count] = '\0'; // Null-terminator ekle

    sem_post(&shmp->sem); // Kritik bölgeden çıkış
    return count;
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

