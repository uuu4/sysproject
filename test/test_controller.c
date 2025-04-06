// test_controller.c

#include "../include/project.h"
#include <stdio.h>
#include <string.h>

// NOT: Controller modülünde kullanılan fonksiyon isimlerinin proje dosyalarınızda
// tanımlanmış olanlarla uyumlu olması gerekir.
// Burada, controller_refresh_messages() fonksiyonunun tanımlandığını varsayıyoruz.

// Global dummy değişkenler (çıktıları yakalamak için)
char dummy_model_message[256] = {0};
char dummy_view_message[256] = {0};
char dummy_terminal_output[256] = {0};
int dummy_exec_status = 0; // execute_command'un döndüreceği durum

// --- Dummy Model Fonksiyonları ---
// Bu dummy implementasyonlar, controller fonksiyonlarının model çağrılarını test etmek için kullanılır.

int execute_command(const char *command) {
    printf("[Dummy execute_command] Received command: %s\n", command);
    // Örneğin, her zaman 0 (başarılı) döndürsün.
    return dummy_exec_status;
}

int model_send_message(ShmBuf *shmp, const char *msg) {
    (void) shmp; // Dummy olarak kullanılmıyor
    printf("[Dummy model_send_message] Message: %s\n", msg);
    strncpy(dummy_model_message, msg, sizeof(dummy_model_message)-1);
    dummy_model_message[sizeof(dummy_model_message)-1] = '\0';
    return 0;
}

int model_read_messages(ShmBuf *shmp, char *buffer, size_t bufsize) {
    (void) shmp;
    // Dummy olarak, dummy_model_message içeriğini döndürsün.
    strncpy(buffer, dummy_model_message, bufsize-1);
    buffer[bufsize-1] = '\0';
    return (int)strlen(buffer);
}

// --- Dummy View Fonksiyonları ---
// Bu dummy implementasyonlar, controller fonksiyonlarının view çağrılarını test etmek için kullanılır.

void view_update_terminal(const char *output) {
    printf("[Dummy view_update_terminal] Output: %s\n", output);
    strncpy(dummy_terminal_output, output, sizeof(dummy_terminal_output)-1);
    dummy_terminal_output[sizeof(dummy_terminal_output)-1] = '\0';
}

void view_display_message(const char *msg) {
    printf("[Dummy view_display_message] Display message: %s\n", msg);
    strncpy(dummy_view_message, msg, sizeof(dummy_view_message)-1);
    dummy_view_message[sizeof(dummy_view_message)-1] = '\0';
}

int view_main(int argc, char **argv) {
    (void)argc; (void)argv;
    return 0;
}

// Dummy shared memory pointer (controller fonksiyonları bu pointer'ı kullanıyor)
ShmBuf *g_shm = NULL; // Testler sırasında kullanılmayacak

// --- Test Fonksiyonları ---

// Test: controller_parse_input() fonksiyonunun "@msg" ile başlayan girdi durumunu kontrol eder.
void test_controller_parse_input_message(void) {
    // Dummy çıktıların temizlenmesi
    memset(dummy_model_message, 0, sizeof(dummy_model_message));
    memset(dummy_view_message, 0, sizeof(dummy_view_message));

    const char *input = "@msg Test Controller Message";
    printf("\n--- Running test_controller_parse_input_message ---\n");
    controller_parse_input(input);

    // Beklenen: model_send_message ve view_display_message fonksiyonları,
    // "@msg" kısmı atıldıktan sonraki metni (yani "Test Controller Message") almalı.
    if (strcmp(dummy_model_message, "Test Controller Message") == 0 &&
        strcmp(dummy_view_message, "Test Controller Message") == 0) {
        printf("TEST: controller_parse_input_message PASSED\n");
    } else {
        printf("TEST: controller_parse_input_message FAILED\n");
        printf("dummy_model_message: '%s'\n", dummy_model_message);
        printf("dummy_view_message: '%s'\n", dummy_view_message);
    }
}

// Test: controller_handle_command() fonksiyonunun komut yürütme akışını test eder.
void test_controller_handle_command(void) {
    // Dummy terminal çıktısını temizle
    memset(dummy_terminal_output, 0, sizeof(dummy_terminal_output));

    const char *command = "echo Hello";
    dummy_exec_status = 0; // execute_command her zaman 0 döndürsün.
    printf("\n--- Running test_controller_handle_command ---\n");
    controller_handle_command(command);

    // Beklenen: view_update_terminal fonksiyonu, komutun çalıştırıldığına dair bir çıktı üretmeli.
    // Örneğin, çıktı "Running command 'echo Hello' with 0 status code" içermeli.
    if (strstr(dummy_terminal_output, "echo Hello") != NULL) {
        printf("TEST: controller_handle_command PASSED\n");
    } else {
        printf("TEST: controller_handle_command FAILED\n");
        printf("dummy_terminal_output: '%s'\n", dummy_terminal_output);
    }
}

// Test: controller_handle_message() fonksiyonunun doğrudan mesaj gönderimini test eder.
void test_controller_handle_message(void) {
    // Dummy çıktıların temizlenmesi
    memset(dummy_model_message, 0, sizeof(dummy_model_message));
    memset(dummy_view_message, 0, sizeof(dummy_view_message));

    const char *message = "Direct Test Message";
    printf("\n--- Running test_controller_handle_message ---\n");
    controller_handle_message(message);

    if (strcmp(dummy_model_message, "Direct Test Message") == 0 &&
        strcmp(dummy_view_message, "Direct Test Message") == 0) {
        printf("TEST: controller_handle_message PASSED\n");
    } else {
        printf("TEST: controller_handle_message FAILED\n");
        printf("dummy_model_message: '%s'\n", dummy_model_message);
        printf("dummy_view_message: '%s'\n", dummy_view_message);
    }
}

// Test: controller_refresh_messages() fonksiyonunun shared memory'den mesaj okuyup view'i güncellemesini test eder.
void test_controller_refresh_messages(void) {
    // Dummy olarak, dummy_model_message içine bir mesaj yerleştiriyoruz.
    strncpy(dummy_model_message, "Refresh Test Message", sizeof(dummy_model_message)-1);
    dummy_model_message[sizeof(dummy_model_message)-1] = '\0';
    memset(dummy_view_message, 0, sizeof(dummy_view_message));

    printf("\n--- Running test_controller_refresh_messages ---\n");
    controller_refresh_messages();

    if (strcmp(dummy_view_message, "Refresh Test Message") == 0) {
        printf("TEST: controller_refresh_messages PASSED\n");
    } else {
        printf("TEST: controller_refresh_messages FAILED\n");
        printf("dummy_view_message: '%s'\n", dummy_view_message);
    }
}

int main(void) {
    printf("=== Controller Module Unit Test Start ===\n");

    test_controller_parse_input_message();
    test_controller_handle_command();
    test_controller_handle_message();
    test_controller_refresh_messages();

    printf("\n=== Controller Module Unit Test End ===\n");
    return 0;
}