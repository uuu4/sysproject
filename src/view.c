#include <gtk/gtk.h>
#include "../include/project.h"

// Global GTK widget'ları
static GtkWidget *window;
static GtkWidget *terminal_text_view;
static GtkWidget *message_text_view;

/**
 * append_text_to_view - Belirtilen GtkTextView widget'ına metin ekler.
 * @text_view: Metnin ekleneceği text view.
 * @text: Eklenecek metin.
 */
static void append_text_to_view(GtkWidget *text_view, const char *text) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    GtkTextIter end_iter;
    gtk_text_buffer_get_end_iter(buffer, &end_iter);
    gtk_text_buffer_insert(buffer, &end_iter, text, -1);
    gtk_text_buffer_insert(buffer, &end_iter, "\n", -1);
}
static void on_entry_activate(GtkEntry *entry, gpointer user_data) {
    const char *input = gtk_entry_get_text(entry);
    // Controller'a göndererek komut veya mesaj ayrıştırması yapılıyor.
    controller_parse_input(input);
    // Giriş alanını temizle
    gtk_entry_set_text(entry, "");
}

static void activate(GtkApplication *app, gpointer user_data) {
    // Ana pencere oluşturuluyor
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Multi-User Communicating Shells");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    // Ana konteyner: dikey kutu (vbox)
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Terminal alanı başlığı
    GtkWidget *terminal_label = gtk_label_new("Terminal Output:");
    gtk_box_pack_start(GTK_BOX(vbox), terminal_label, FALSE, FALSE, 0);

    // Terminal text view (çıktı alanı)
    terminal_text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(terminal_text_view), FALSE);
    GtkWidget *terminal_scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(terminal_scrolled), terminal_text_view);
    gtk_box_pack_start(GTK_BOX(vbox), terminal_scrolled, TRUE, TRUE, 0);

    // Mesaj alanı başlığı
    GtkWidget *message_label = gtk_label_new("Messages:");
    gtk_box_pack_start(GTK_BOX(vbox), message_label, FALSE, FALSE, 0);

    // Mesaj text view (mesajların gösterileceği alan)
    message_text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(message_text_view), FALSE);
    GtkWidget *message_scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(message_scrolled), message_text_view);
    gtk_box_pack_start(GTK_BOX(vbox), message_scrolled, TRUE, TRUE, 0);

    // Giriş alanı: Kullanıcının komut girebileceği GtkEntry widget'ı
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Komut veya @msg ile mesaj girin...");
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);
    // Enter'a basıldığında on_entry_activate callback'i çağrılır.
    g_signal_connect(entry, "activate", G_CALLBACK(on_entry_activate), NULL);

    gtk_widget_show_all(window);
}

/**
 * view_update_terminal - Terminal çıktısını günceller.
 * @output: Terminale yazdırılacak çıktı metni.
 */
void view_update_terminal(const char *output) {
    if (terminal_text_view && output) {
        append_text_to_view(terminal_text_view, output);
    }
}

/**
 * view_display_message - Mesaj panelini günceller.
 * @msg: Ekrana eklenecek mesaj.
 */
void view_display_message(const char *msg) {
    if (message_text_view && msg) {
        append_text_to_view(message_text_view, msg);
    }
}

/**
 * view_main - GTK uygulamasını başlatır.
 * @argc: Komut satırı argüman sayısı.
 * @argv: Komut satırı argümanları.
 *
 * Return: GTK uygulamasından dönen durum kodu.
 */
int view_main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.example.MultiUserShell", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}