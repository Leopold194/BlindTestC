#include <gtk/gtk.h>
#include "get_song.h"
#include "get_playlist.h"
#include "quiz_page.h"
#include "globals.h"
#include "leaderboard.h"

GtkWidget *label;

void launch_game(GtkWidget *widget, gpointer user_data){
    unsigned long int playlistId = GPOINTER_TO_SIZE(user_data);
    Playlist *playlist = init_playlist(playlistId);

    gtk_widget_destroy(GTK_WIDGET(gtk_widget_get_toplevel(widget)));
    quiz_page(playlist);
}

int menu() {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *fixed;

    GtkWidget *buttonPlaylist1;
    GtkWidget *imagePlaylist1;
    GtkWidget *buttonPlaylist2;
    GtkWidget *imagePlaylist2;

    GtkWidget *leaderboardBtn;
    
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Example");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_widget_set_size_request(window, 1800, 900);

    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    gchar welcome_message[256];
    snprintf(welcome_message, sizeof(welcome_message), "Salut %s ! Choisis la playlist avec laquelle tu veux jouer", currentPlayer);

    label = gtk_label_new(welcome_message);
    gtk_fixed_put(GTK_FIXED(fixed), label, 750, 50);

    imagePlaylist1 = gtk_image_new_from_file("uploads/annees_80.jpg");
    buttonPlaylist1 = gtk_button_new();
    gtk_button_set_image(GTK_BUTTON(buttonPlaylist1), imagePlaylist1);
    //GtkWidget *button2 = gtk_button_new_with_label ("test");
    //gtk_button_set_always_show_image (GTK_BUTTON(button2), TRUE);
    gtk_fixed_put(GTK_FIXED(fixed), buttonPlaylist1, 500, 100);
    gtk_widget_set_size_request(buttonPlaylist1, 300, 300);

    imagePlaylist2 = gtk_image_new_from_file("uploads/annees_2010.jpg");
    buttonPlaylist2 = gtk_button_new();
    gtk_button_set_image(GTK_BUTTON(buttonPlaylist2), imagePlaylist2);
    gtk_fixed_put(GTK_FIXED(fixed), buttonPlaylist2, 1000, 100);
    gtk_widget_set_size_request(buttonPlaylist2, 300, 300);
    
    leaderboardBtn = gtk_button_new_with_label("Classement");
    gtk_fixed_put(GTK_FIXED(fixed), leaderboardBtn, 750, 500);
    gtk_widget_set_size_request(leaderboardBtn, 300, 50);

    unsigned long int playlistId1 = 6312324264;
    g_signal_connect(buttonPlaylist1, "clicked", G_CALLBACK(launch_game), GSIZE_TO_POINTER(playlistId1));

    unsigned long int playlistId2 = 6312310484;
    g_signal_connect(buttonPlaylist2, "clicked", G_CALLBACK(launch_game), GINT_TO_POINTER(playlistId2));

    g_signal_connect(leaderboardBtn, "clicked", G_CALLBACK(leaderboard), NULL);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
