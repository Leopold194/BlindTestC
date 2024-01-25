#include <gtk/gtk.h>
#include <gst/gst.h>
#include "get_song.h"
#include "get_playlist.h"
#include "quiz_page.h"

GtkWidget *label;

void launch_game(GtkWidget *widget, gpointer user_data){
    int playlistId = GPOINTER_TO_INT(user_data);
    Playlist *playlist = init_playlist(playlistId);

    gtk_widget_destroy(GTK_WIDGET(gtk_widget_get_toplevel(widget)));
    quiz_page(playlist);
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *fixed;

    GtkWidget *buttonPlaylist1;
    GtkWidget *imagePlaylist1;
    GtkWidget *buttonPlaylist2;
    GtkWidget *imagePlaylist2;

    gtk_init(&argc, &argv);
    gst_init(&argc, &argv);
    
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Example");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_widget_set_size_request(window, 1800, 900);
    GdkRGBA color;
    gdk_rgba_parse(&color, "white");
    gtk_widget_override_background_color(buttonPlaylist1, GTK_STATE_FLAG_NORMAL, &color);
    gtk_widget_override_background_color(buttonPlaylist2, GTK_STATE_FLAG_NORMAL, &color);

    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    label = gtk_label_new("Choisis la playlist avec laquelle tu veux jouer");
    gtk_fixed_put(GTK_FIXED(fixed), label, 750, 50);

    imagePlaylist1 = gtk_image_new_from_file("uploads/top_fr.jpg");
    buttonPlaylist1 = gtk_button_new();
    gtk_button_set_image(GTK_BUTTON(buttonPlaylist1), imagePlaylist1);
    //GtkWidget *button2 = gtk_button_new_with_label ("test");
    //gtk_button_set_always_show_image (GTK_BUTTON(button2), TRUE);
    gtk_widget_set_size_request(buttonPlaylist1, 300, 300);
    gtk_widget_set_halign(buttonPlaylist1, GTK_ALIGN_START);
    gtk_widget_set_valign(buttonPlaylist1, GTK_ALIGN_END);

    imagePlaylist2 = gtk_image_new_from_file("uploads/top_world.jpg");
    buttonPlaylist2 = gtk_button_new();
    gtk_button_set_image(GTK_BUTTON(buttonPlaylist2), imagePlaylist2);
    gtk_widget_set_halign(buttonPlaylist2, GTK_ALIGN_END);
    gtk_widget_set_valign(buttonPlaylist2, GTK_ALIGN_START);
    gtk_fixed_put(GTK_FIXED(fixed), buttonPlaylist2, 1000, 100);
    gtk_widget_set_size_request(buttonPlaylist2, 300, 300);
    
    int myParam = 1109890291;
    g_signal_connect(buttonPlaylist1, "clicked", G_CALLBACK(launch_game), GINT_TO_POINTER(myParam));

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
