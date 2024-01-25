#include <gtk/gtk.h>
#include <time.h>
#include <gst/gst.h>
#include <unistd.h>
#include "get_song.h"
#include "get_playlist.h"

char title1[256];
char title2[256];
char title3[256];
char title4[256];
int goodAnswer;
int goodAnswerId;
int musicsPassed = 0;
int original_seconds = 30;
int seconds;
int score;
int max_score = 2;

GtkWidget *buttonChoice1;
GtkWidget *buttonChoice2;
GtkWidget *buttonChoice3;
GtkWidget *buttonChoice4;
GtkWidget *timer;
GtkWidget *scoreLabel;
gdk_rgba_parse(&color, "blue");
gtk_widget_override_background_color(buttonChoice1, GTK_STATE_FLAG_NORMAL, &color);
gtk_widget_override_background_color(buttonChoice2, GTK_STATE_FLAG_NORMAL, &color);
gtk_widget_override_background_color(buttonChoice3, GTK_STATE_FLAG_NORMAL, &color);
gtk_widget_override_background_color(buttonChoice4, GTK_STATE_FLAG_NORMAL, &color);
gdk_rgba_parse(&color, "red");
gtk_widget_override_background_color(timer, GTK_STATE_FLAG_NORMAL, &color);
gdk_rgba_parse(&color, "light green");
gtk_widget_override_background_color(scoreLabel, GTK_STATE_FLAG_NORMAL, &color);
GstElement *pipeline;
GtkWidget *dialog;

Playlist *currentPlaylist;

gboolean paused = FALSE;

time_t startTime;
time_t endTime;

gboolean time_handler(GtkWidget *label);
gboolean end_timer_callback(gpointer user_data);

void update_button_labels() {
    gtk_button_set_label(GTK_BUTTON(buttonChoice1), title1);
    gtk_button_set_label(GTK_BUTTON(buttonChoice2), title2);
    gtk_button_set_label(GTK_BUTTON(buttonChoice3), title3);
    gtk_button_set_label(GTK_BUTTON(buttonChoice4), title4);
    
    gchar points_label[12];
    g_snprintf(points_label, sizeof(points_label), "%d point(s)", score);
    gtk_label_set_text(GTK_LABEL(scoreLabel), points_label);
}

void update_answers(Playlist *playlist) {
    if (pipeline != NULL) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
    }

    srand((unsigned int)time(NULL));
    goodAnswer = rand() % 4 + 1;

    strncpy(title1, playlist->tracklist[musicsPassed].title, sizeof(title1) - 1);
    strncpy(title2, playlist->tracklist[musicsPassed + 1].title, sizeof(title2) - 1);
    strncpy(title3, playlist->tracklist[musicsPassed + 2].title, sizeof(title3) - 1);
    strncpy(title4, playlist->tracklist[musicsPassed + 3].title, sizeof(title4) - 1);
    goodAnswerId = playlist->tracklist[musicsPassed + goodAnswer - 1].id;
    musicsPassed += 4;

    update_button_labels();

    char uri[512];
    snprintf(uri, sizeof(uri), "file:///home/leo/Documents/PROJET_C/Songs/%lu/%lu.mp3", playlist->id, goodAnswerId);
    char goodUri[512] = "playbin uri=";
    strcat(goodUri, uri);
    pipeline = gst_parse_launch(goodUri, NULL);
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
}

void on_window_destroy(){
    end_timer_callback((gpointer) timer);
    update_answers(currentPlaylist);
    paused = FALSE;
    gtk_widget_destroy(dialog);
}

void show_info(GtkWidget *widget, gpointer window, char text[]) {
    dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_INFO,
                GTK_BUTTONS_OK,
                text);
    gtk_window_set_title(GTK_WINDOW(dialog), "Réponse");
    
    g_signal_connect(dialog, "response", G_CALLBACK(on_window_destroy), NULL);
    gtk_dialog_run(GTK_DIALOG(dialog));
}

void check_answer(GtkWidget *widget, gpointer user_data) {
    int userAnswer = GPOINTER_TO_INT(user_data);
    char text[256];
    if (userAnswer == goodAnswer) {
        strcpy(text, "Bonne réponse !!");
        score++;

        if(score == max_score){
            time(&endTime);
            long int elapsedTime = difftime(endTime, startTime);
            g_print("Temps écoulé : %ld secondes\n", elapsedTime);
        }
    } else {
        strcpy(text, "Raté ! La bonne réponse était : ");
        switch(goodAnswer){
            case 1:
                strcat(text, title1);
                break;
            case 2:
                strcat(text, title2);
                break;
            case 3:
                strcat(text, title3);
                break;
            default:
                strcat(text, title4);
        }
    }
    paused = TRUE;
    show_info(NULL, NULL, text);
}

gboolean end_timer_callback(gpointer user_data) {
    
    seconds = original_seconds;
    
    gchar timer_seconds[6];
    g_snprintf(timer_seconds, sizeof(timer_seconds), "00:%02d", seconds);
    gtk_label_set_text(GTK_LABEL(user_data), timer_seconds);

    return FALSE;
}

gboolean time_handler(GtkWidget *label) {
    if (!paused) {
        if (seconds >= 0) {
            gchar timer_seconds[6];
            snprintf(timer_seconds, sizeof(timer_seconds), "00:%02d", seconds);
            gtk_label_set_text(GTK_LABEL(label), timer_seconds);

            seconds--;
            return TRUE;
        } else {
            gtk_label_set_text(GTK_LABEL(label), "Temps écoulé !");
            
            char text[256];
            strcpy(text, "Raté ! Le temps est écoulé ! La bonne réponse était : ");
            switch(goodAnswer){
                case 1:
                    strcat(text, title1);
                    break;
                case 2:
                    strcat(text, title2);
                    break;
                case 3:
                    strcat(text, title3);
                    break;
                default:
                    strcat(text, title4);
            }
            paused = TRUE;
            show_info(NULL, NULL, text);
        }
    }
    return TRUE;
}

int quiz_page(Playlist *playlist) {
    seconds = original_seconds;
    currentPlaylist = playlist;

    GtkWidget *window;
    GtkWidget *fixed;
    GtkWidget *label;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Example");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_widget_set_size_request(window, 1800, 900);

    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    label = gtk_label_new("Choisis la bonne réponse !");
    gtk_fixed_put(GTK_FIXED(fixed), label, 800, 50);

    int choice1 = 1;
    int choice2 = 2;
    int choice3 = 3;
    int choice4 = 4;

    buttonChoice1 = gtk_button_new_with_label("");
    gtk_fixed_put(GTK_FIXED(fixed), buttonChoice1, 300, 200);
    gtk_widget_set_size_request(buttonChoice1, 300, 100);

    buttonChoice2 = gtk_button_new_with_label("");
    gtk_fixed_put(GTK_FIXED(fixed), buttonChoice2, 1200, 200);
    gtk_widget_set_size_request(buttonChoice2, 300, 100);

    buttonChoice3 = gtk_button_new_with_label("");
    gtk_fixed_put(GTK_FIXED(fixed), buttonChoice3, 300, 400);
    gtk_widget_set_size_request(buttonChoice3, 300, 100);

    buttonChoice4 = gtk_button_new_with_label("");
    gtk_fixed_put(GTK_FIXED(fixed), buttonChoice4, 1200, 400);
    gtk_widget_set_size_request(buttonChoice4, 300, 100);

    gchar timer_seconds[6];
    snprintf(timer_seconds, sizeof(timer_seconds), "00:%02d", seconds);

    timer = gtk_label_new(timer_seconds);
    gtk_fixed_put(GTK_FIXED(fixed), timer, 870, 600);

    scoreLabel = gtk_label_new("");
    gtk_fixed_put(GTK_FIXED(fixed), scoreLabel, 20, 20);

    g_signal_connect(buttonChoice1, "clicked", G_CALLBACK(check_answer), GINT_TO_POINTER(choice1));
    g_signal_connect(buttonChoice2, "clicked", G_CALLBACK(check_answer), GINT_TO_POINTER(choice2));
    g_signal_connect(buttonChoice3, "clicked", G_CALLBACK(check_answer), GINT_TO_POINTER(choice3));
    g_signal_connect(buttonChoice4, "clicked", G_CALLBACK(check_answer), GINT_TO_POINTER(choice4));

    update_answers(playlist);

    time(&startTime);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    g_timeout_add_seconds(1, (GSourceFunc) time_handler, (gpointer) timer);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
