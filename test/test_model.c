// test_model.c

#include <stdio.h>
#include <string.h>
#include "../include/project.h"
#include <sys/mman.h>

int main(void) {
    // Shared memory bölgesini başlat
    ShmBuf *shmp = buf_init();
    if (shmp == NULL) {
        fprintf(stderr, "buf_init() başarısız.\n");
        return 1;
    }
    
    // Test mesajı
    const char *test_message = "Test Model Message";
    
    // Mesaj gönderimi
    if (model_send_message(shmp, test_message) != 0) {
        fprintf(stderr, "model_send_message() başarısız.\n");
        return 1;
    }
    
    // Mesajı okuma
    char buffer[BUF_SIZE];
    int bytesRead = model_read_messages(shmp, buffer, sizeof(buffer));
    if (bytesRead < 0) {
        fprintf(stderr, "model_read_messages() başarısız.\n");
        return 1;
    }
    
    // Sonuçları karşılaştırma
    if (strcmp(test_message, buffer) == 0) {
        printf("TEST_MODEL: Mesaj başarıyla gönderildi ve okundu: '%s'\n", buffer);
    } else {
        printf("TEST_MODEL: Beklenen '%s', ancak okunan '%s'\n", test_message, buffer);
    }
    
    // Shared memory temizleme
    shm_unlink(SHARED_FILE_PATH);
    return 0;
}