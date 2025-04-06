// test_view.c

#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "../include/project.h"

// Bu test dosyasında, view.c’de tanımlı global widget değişkenlerine erişebilmek için,
// view.c'de aşağıdaki değişkenlerin static olmadan tanımlandığından emin olun:
//    GtkWidget *terminal_text_view;
//    GtkWidget *message_text_view;
extern GtkWidget *terminal_text_view;
extern GtkWidget *message_text_view;

void test_view_update_terminal(void) {
    // Test için terminal text view'u oluştur
    terminal_text_view = gtk_text_view_new();
    // Text view'un buffer'ını sıfırla
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(terminal_text_view));
    gtk_text_buffer_set_text(buffer, "", -1);

    // Fonksiyonu test et
    const char *testOutput = "Test Terminal Output";
    view_update_terminal(testOutput);

    // Buffer içeriğini oku
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    if (strstr(text, testOutput) != NULL) {
        printf("VIEW TEST: view_update_terminal PASSED\n");
    } else {
        printf("VIEW TEST: view_update_terminal FAILED: got '%s'\n", text);
    }
    g_free(text);
}

void test_view_display_message(void) {
    // Test için message text view'u oluştur
    message_text_view = gtk_text_view_new();
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(message_text_view));
    gtk_text_buffer_set_text(buffer, "", -1);

    // Fonksiyonu test et
    const char *testMsg = "Test Message Display";
    view_display_message(testMsg);

    // Buffer içeriğini oku
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    if (strstr(text, testMsg) != NULL) {
        printf("VIEW TEST: view_display_message PASSED\n");
    } else {
        printf("VIEW TEST: view_display_message FAILED: got '%s'\n", text);
    }
    g_free(text);
}

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);

    printf("=== View Module Unit Test Start ===\n");
    test_view_update_terminal();
    test_view_display_message();
    printf("=== View Module Unit Test End ===\n");

    return 0;
}