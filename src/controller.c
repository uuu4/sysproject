#include "../include/project.h"
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

extern ShmBuf *g_shm;

void controller_parse_input(const char *input) {
    if (!input)
        return;

    if (strncmp(input, "@msg", 4) == 0) {
        const char *message = input + 4;
        while (*message == ' ') {
            message++;
        }
    
        if (model_send_message(g_shm, message) == 0) {
            view_display_message(message);
        } else {
            fprintf(stderr, "Error while sending message\n");
        }
    } else {
        // Komut girilmişse, controller_handle_command() çağrılıyor.
        controller_handle_command(input);
    }
}

void controller_handle_command(const char *command) {
    if (!command) {
        fprintf(stderr, "Need a valid command\n");
        return;
    }
    int status = execute_command(command);
    char resultMsg[256];
    snprintf(resultMsg, sizeof(resultMsg), "Running command '%s' with %d status code", command, status);
    view_update_terminal(resultMsg);
}
void controller_handle_message(const char *message){
    if(!message){
        fprintf(stderr,"Write a valid message\n");
        return;
    }
    if(model_send_message(g_shm,message)==0){
        view_display_message(message);
    }else{
        fprintf(stderr,"Error while sending the message");
    }
}

void controller_refresh_messages(void) {
    char buffer[BUF_SIZE];
    
    // Buffer'ı temizle
    memset(buffer, 0, sizeof(buffer));
    
    // Shared memory'den mesaj okuma
    // model_read_messages() fonksiyonu, okunan bayt sayısını döndürebilir;
    // 0 ise okunacak mesaj yok demektir, negatif değer hata durumunu belirtir.
    int bytesRead = model_read_messages(g_shm, buffer, sizeof(buffer));
    
    if (bytesRead > 0) {
        // Yeni mesaj bulundu, view'i güncelle
        view_display_message(buffer);
    } else if (bytesRead < 0) {
        fprintf(stderr, "Mesaj okuma hatası meydana geldi.\n");
    }
    // bytesRead == 0 ise, okunacak yeni mesaj yok demektir, bu durumda herhangi bir işlem yapılmaz.
}


