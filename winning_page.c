#include <gtk/gtk.h>
#include <sqlite3.h>
#include <stdio.h>
#include "globals.h"
#include "connect_db.h"
#include "open_config.h"
#include "menu.h"

int callback_win(void *data, int argc, char **argv, char **col_names);

long int best_score;
long int last_score;

void replay(GtkWidget *widget) {
    gtk_widget_destroy(GTK_WIDGET(gtk_widget_get_toplevel(widget)));
    menu();
    return;
}

int winning_page() {

    GtkWidget *window;
    GtkWidget *fixed;
    GtkWidget *label;
    GtkWidget *summary;
    GtkWidget *summary2;
    GtkWidget *replay_button;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Victoire");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_widget_set_size_request(window, config->windows_length, config->windows_height);

    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    gchar winning_message[256];
    snprintf(winning_message, sizeof(winning_message), "Bravo %s ! Tu as réussi à avoir %d points en %ld secondes", currentPlayer, config->max_score, elapsedTime);

    label = gtk_label_new(winning_message);
    gtk_fixed_put(GTK_FIXED(fixed), label, 750, 50);

    gchar summary_message[256];
    if(lastBestScore < elapsedTime){
        snprintf(summary_message, sizeof(summary_message), "Ton meilleur score est %lu secondes ! Tu dois encore gagner %lu secondes pour le dépasser !", lastBestScore, elapsedTime-lastBestScore);
    }else{
        snprintf(summary_message, sizeof(summary_message), "Incroyable, tu as battu ton meilleur score qui était de %lu secondes, tu as gagné %lu secondes !", lastBestScore, lastBestScore-elapsedTime);   
    }

    summary = gtk_label_new(summary_message);
    gtk_fixed_put(GTK_FIXED(fixed), summary, 620, 150);

    gchar summary_message2[256];
    snprintf(summary_message2, sizeof(summary_message2), "Ton précédent score était %lu secondes", lastScore);

    summary2 = gtk_label_new(summary_message2);
    gtk_fixed_put(GTK_FIXED(fixed), summary2, 790, 220);
        
    replay_button = gtk_button_new_with_label("Rejouer");
    gtk_fixed_put(GTK_FIXED(fixed), replay_button, 750, 500);
    gtk_widget_set_size_request(replay_button, 300, 50);

    g_signal_connect(replay_button, "clicked", G_CALLBACK(replay), NULL);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}