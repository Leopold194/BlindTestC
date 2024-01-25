#include <gtk/gtk.h>

int main(int argc, char *argv[]) {

    GtkWidget *window;
    GtkWidget *fixed;

    GtkWidget *label1;
    GtkWidget *label2;
    GtkWidget *label3;

    GtkWidget *entry1;
    GtkWidget *entry2;
    GtkWidget *entry3;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Example");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_widget_set_halign(window, GTK_ALIGN_END);
    gtk_widget_set_valign(window, GTK_ALIGN_START);

    gtk_widget_set_size_request(window, 1800, 900);

    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    label1 = gtk_label_new("Choisis un pseudo :");
    gtk_fixed_put(GTK_FIXED(fixed), entry1, 1000, 100);

    entry1 = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), entry1, 1000, 100);

    gtk_widget_show_all(window);

    g_signal_connect(window, "destroy",
        G_CALLBACK(gtk_main_quit), NULL);

    gtk_main();

    return 0;
}