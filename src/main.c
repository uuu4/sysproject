#include "../include/project.h"
#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <sys/mman.h>

// Global shared memory pointer
ShmBuf *g_shm = NULL;

int main(int argc, char **argv) {
    // Shared memory (model) başlatılıyor.
    g_shm = buf_init();
    if (!g_shm) {
        fprintf(stderr, "Shared memory initialization failed.\n");
        exit(EXIT_FAILURE);
    }
    
    // (Opsiyonel) Burada controller modülüne ait ek başlatma işlemleri eklenebilir.
    // Örneğin, kullanıcı girdilerini dinleyen bir event loop veya ayrı bir thread kurulabilir.
    
    // View (GTK arayüzü) başlatılıyor.
    int ret = view_main(argc, argv);
    
    // Uygulama sonlandığında, shared memory temizleniyor.
    shm_unlink(SHARED_FILE_PATH);
    
    return ret;
}