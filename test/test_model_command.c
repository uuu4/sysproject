// test_model_command.c

#include <stdio.h>
#include "../include/project.h"
#include <sys/mman.h>

int main(void) {
    // Basit bir komut çalıştırılıyor.
    const char *command = "echo Hello from execute_command";
    int status = execute_command(command);
    
    // execute_command, komut çıktısını yazdırır ve çıkış durumunu döndürür.
    if (status == 0) {
        printf("TEST_MODEL_COMMAND: Komut başarıyla çalıştı (çıkış durumu: %d).\n", status);
    } else {
        printf("TEST_MODEL_COMMAND: Komut çalıştırma hatası (çıkış durumu: %d).\n", status);
    }
    
    return 0;
}