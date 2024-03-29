#include <gtk/gtk.h>
#include <time.h>
#include <gst/gst.h>
#include <unistd.h>
#include "get_song.h"
#include "get_playlist.h"
#include "globals.h"
#include "connect_db.h"
#include "open_config.h"
#include "winning_page.h"

char title1[256];
char title2[256];
char title3[256];
char title4[256];
int goodAnswer;
int goodAnswerId;
int musicsPassed = 0;
int original_seconds;
int seconds;
int score;
int max_score;

GtkWidget *buttonChoice1;
GtkWidget *buttonChoice2;
GtkWidget *buttonChoice3;
GtkWidget *buttonChoice4;
GtkWidget *timer;
GtkWidget *scoreLabel;

GstElement *pipeline;
GtkWidget *dialog;

Playlist *currentPlaylist;

gboolean paused = FALSE;
guint timer_id = 0;

time_t startTime;
time_t endTime;

gboolean time_handler(GtkWidget *label);
gboolean end_timer_callback(gpointer user_data);

void reset_variables() {
    /*
        Cette fonction permet de remettre à 0 les variables qui en ont besoin.
    */
    seconds = 0;
    score = 0;
    musicsPassed = 0;
}

void initialize_variables() {
    /*
        Cette fonction permet d'initialiser les variables grâce au fichier de configuration.
    */
    original_seconds = config->timer;
    max_score = config->max_score;
}

void save_score(long int score) {
    /*
        Cette fonction permet de sauvegarder le score du joueur dans la base de données.
        Elle enregistre toujours le score en "last_score", et s'il est meilleur que le best_score,
        l'enregistre également en tant que best_score.
    */
    long int best_score;

    char sql[200];
    sprintf(sql, "SELECT best_score, last_score FROM %s WHERE pseudo=?;", config->database_table_name);

    if (connectDb() != 1) {
        fprintf(stderr, "Echec dans la connexion à la base de données");
        return;
    }

    if (sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0) != SQLITE_OK) {
        fprintf(stderr, "Echec dans la transaction");
        closeDb();
        return;
    }

    sqlite3_stmt *query_prepare;
    if (sqlite3_prepare_v2(db, sql, -1, &query_prepare, 0) == SQLITE_OK) {
        sqlite3_bind_text(query_prepare, 1, currentPlayer, -1, SQLITE_STATIC);

        if (sqlite3_step(query_prepare) == SQLITE_ROW) {
            lastBestScore = sqlite3_column_int(query_prepare, 0);
            lastScore = sqlite3_column_int(query_prepare, 1);
        }
        sqlite3_finalize(query_prepare);
    } else {
        fprintf(stderr, "Echec de la requête");
        sqlite3_exec(db, "ROLLBACK", 0, 0, 0);
        closeDb();
        return;
    }

    char sql2[256];
    if (lastBestScore == 0 || lastBestScore > score) {
        sprintf(sql2, "UPDATE %s SET last_score=?, best_score=? WHERE pseudo=?;", config->database_table_name);
    } else {
        sprintf(sql2, "UPDATE %s SET last_score=? WHERE pseudo=?;", config->database_table_name);
    }
    
    sqlite3_stmt *query_prepare_update;
    if (sqlite3_prepare_v2(db, sql2, -1, &query_prepare_update, 0) == SQLITE_OK) {
        sqlite3_bind_int(query_prepare_update, 1, score);
        if (lastBestScore == 0 || lastBestScore > score) {
            sqlite3_bind_int(query_prepare_update, 2, score);
            sqlite3_bind_text(query_prepare_update, 3, currentPlayer, -1, SQLITE_STATIC);
        } else {
            sqlite3_bind_text(query_prepare_update, 2, currentPlayer, -1, SQLITE_STATIC);
        }
        if (sqlite3_step(query_prepare_update) != SQLITE_DONE) {
            fprintf(stderr, "Échec de l'enregistrement");
        }
        sqlite3_finalize(query_prepare_update);
    } else {
        fprintf(stderr, "Échec de la requête");
        sqlite3_exec(db, "ROLLBACK", 0, 0, 0);
    }

    sqlite3_exec(db, "COMMIT", 0, 0, 0);

    closeDb();
}

void update_button_labels() {
    /*
        Cette fonction permet de mettre à jour les labels des boutons avec les titres des nouvelles musiques.
    */
    gtk_button_set_label(GTK_BUTTON(buttonChoice1), title1);
    gtk_button_set_label(GTK_BUTTON(buttonChoice2), title2);
    gtk_button_set_label(GTK_BUTTON(buttonChoice3), title3);
    gtk_button_set_label(GTK_BUTTON(buttonChoice4), title4);
    
    gchar points_label[12];
    g_snprintf(points_label, sizeof(points_label), "%d point(s)", score);
    gtk_label_set_text(GTK_LABEL(scoreLabel), points_label);
}

void update_answers(Playlist *playlist) {
    /*
        Cette fonction permet d'actualiser le blind test à chaque nouvelle musique. 
        Ca met à jour la pipeline qui joue la musique, remet à 0 le compteur de musique si elles sont toutes jouées.
    */
    if (pipeline != NULL) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
        pipeline = NULL;
    }

    gtk_widget_override_color(timer, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){1, 1, 1, 1});

    srand((unsigned int)time(NULL));
    goodAnswer = rand() % 4 + 1;

    if (musicsPassed >= currentPlaylist->num_tracks - 3) {
        musicsPassed = 0;
        shuffle_playlist(currentPlaylist);
    }

    strncpy(title1, playlist->tracklist[musicsPassed].title, sizeof(title1) - 1);
    strncpy(title2, playlist->tracklist[musicsPassed + 1].title, sizeof(title2) - 1);
    strncpy(title3, playlist->tracklist[musicsPassed + 2].title, sizeof(title3) - 1);
    strncpy(title4, playlist->tracklist[musicsPassed + 3].title, sizeof(title4) - 1);
    goodAnswerId = playlist->tracklist[musicsPassed + goodAnswer - 1].id;
    musicsPassed += 4;

    update_button_labels();

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        fprintf(stderr, "getcwd() error");
        return;
    }

    char uri[512];
    snprintf(uri, sizeof(uri), "file://%s/%s%lu/%lu.mp3", cwd, config->songs_path, playlist->id, goodAnswerId);

    char goodUri[512];
    snprintf(goodUri, sizeof(goodUri), "playbin uri=%s", uri);

    GError *error = NULL;

    pipeline = gst_parse_launch(goodUri, &error);
    if (!pipeline) {
        fprintf(stderr, "Erreur lors de la création du pipeline : %s\n", error->message);
        g_error_free(error);
    }
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
}

void on_window_destroy(){
    /*
        Cette fonction permet de réinitialiser le timer, et le remettre en play.
    */
    end_timer_callback((gpointer) timer);
    update_answers(currentPlaylist);
    paused = FALSE;
    gtk_widget_destroy(dialog);
}

void show_info(GtkWidget *widget, gpointer window, char text[]) {
    /*
        Cette fonction permet d'afficher un pop-up avec la bonne réponse.
    */
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
    /*
        Cette fonction permet de vérifier que le bouton sur lequel l'utilisateur a cliqué contient la bonne réponse.
        S'il est arrivé au score maximal, on calcule son temps, on arrête la musique et le timer. On reset les variables s'il veut rejouer.
        Et on libère la mémoire de la playlist, avant d'ouvrir la page de victoire.
    */
    int userAnswer = GPOINTER_TO_INT(user_data);
    char text[256];
    if (userAnswer == goodAnswer) {
        strcpy(text, "Bonne réponse !!");
        score++;

        if(score == max_score){
            time(&endTime);
            elapsedTime = difftime(endTime, startTime);
            save_score(elapsedTime);
            if (pipeline != NULL) {
                gst_element_set_state(pipeline, GST_STATE_NULL);
                gst_object_unref(pipeline);
                pipeline = NULL;
            }
            g_source_remove(timer_id);
            reset_variables();
            gtk_widget_destroy(GTK_WIDGET(gtk_widget_get_toplevel(widget)));
            free_playlist(currentPlaylist);
            winning_page();
            return;
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
    /*
        Cette fonction permet de remettre le timer à sa valeur originale.
    */
    
    seconds = original_seconds;
    
    gchar timer_seconds[6];
    g_snprintf(timer_seconds, sizeof(timer_seconds), "00:%02d", seconds);
    gtk_label_set_text(GTK_LABEL(user_data), timer_seconds);

    return FALSE;
}

gboolean time_handler(GtkWidget *label) {
    /*
        Cette fonction permet de mettre à jour le timer. 
        S'il reste moins de 1/5 du temps, l'affiche en rouge.
    */
    if (!paused) {
        if (seconds >= 0) {
            gchar timer_seconds[6];
            snprintf(timer_seconds, sizeof(timer_seconds), "00:%02d", seconds);
            gtk_label_set_text(GTK_LABEL(label), timer_seconds);
            if (seconds <= config->timer / 5) {
                gtk_widget_override_color(timer, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){1, 0, 0, 1});
            }
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
    return G_SOURCE_CONTINUE;
}

int quiz_page(Playlist *playlist) {
    /*
        Cette fonction permet d'afficher la page de jeu, avec les 4 choix, 
        les points, et le timer.
    */

    initialize_variables();

    seconds = original_seconds;
    currentPlaylist = playlist;

    GtkWidget *window;
    GtkWidget *fixed;
    GtkWidget *label;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "BlindTest");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_widget_set_size_request(window, config->windows_length, config->windows_height);

    GdkRGBA color;
    gdk_rgba_parse(&color, config->windows_color);
    gtk_widget_override_background_color(window, GTK_STATE_FLAG_NORMAL, &color);

    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    label = gtk_label_new("Choisis la bonne réponse !");
    PangoFontDescription *font_desc = pango_font_description_new();
    pango_font_description_set_size(font_desc, 20 * PANGO_SCALE);
    gtk_widget_override_font(label, font_desc);
    gtk_widget_set_size_request(label, 900, 20);
    gtk_fixed_put(GTK_FIXED(fixed), label, (config->windows_length) / 2 - 450, 50);

    int choice1 = 1;
    int choice2 = 2;
    int choice3 = 3;
    int choice4 = 4;

    PangoFontDescription *font_desc2 = pango_font_description_new();
    pango_font_description_set_size(font_desc2, 17 * PANGO_SCALE);

    buttonChoice1 = gtk_button_new_with_label("");
    gtk_fixed_put(GTK_FIXED(fixed), buttonChoice1, (config->windows_length) / 4 - 250, 200);
    gtk_widget_set_size_request(buttonChoice1, 500, 100);

    GdkRGBA text_color0;
    gdk_rgba_parse(&text_color0, "#8202AA");
    gtk_widget_override_color(buttonChoice1, GTK_STATE_FLAG_NORMAL, &text_color0);
    gtk_widget_override_font(buttonChoice1, font_desc2);

    buttonChoice2 = gtk_button_new_with_label("");
    gtk_fixed_put(GTK_FIXED(fixed), buttonChoice2, (config->windows_length) / 4 * 3 - 250, 200);
    gtk_widget_set_size_request(buttonChoice2, 500, 100);

    GdkRGBA text_color1;
    gdk_rgba_parse(&text_color1, "#D7B023");
    gtk_widget_override_color(buttonChoice2, GTK_STATE_FLAG_NORMAL, &text_color1);
    gtk_widget_override_font(buttonChoice2, font_desc2);

    buttonChoice3 = gtk_button_new_with_label("");
    gtk_fixed_put(GTK_FIXED(fixed), buttonChoice3, (config->windows_length) / 4 - 250, 400);
    gtk_widget_set_size_request(buttonChoice3, 500, 100);

    GdkRGBA text_color2;
    gdk_rgba_parse(&text_color2, "#ED80A2");
    gtk_widget_override_color(buttonChoice3, GTK_STATE_FLAG_NORMAL, &text_color2);
    gtk_widget_override_font(buttonChoice3, font_desc2);

    buttonChoice4 = gtk_button_new_with_label("");
    gtk_fixed_put(GTK_FIXED(fixed), buttonChoice4, (config->windows_length) / 4 * 3 - 250, 400);
    gtk_widget_set_size_request(buttonChoice4, 500, 100);

    GdkRGBA text_color3;
    gdk_rgba_parse(&text_color3, "#00B8F8");
    gtk_widget_override_color(buttonChoice4, GTK_STATE_FLAG_NORMAL, &text_color3);
    gtk_widget_override_font(buttonChoice4, font_desc2);

    gchar timer_seconds[6];
    snprintf(timer_seconds, sizeof(timer_seconds), "00:%02d", seconds);

    timer = gtk_label_new(timer_seconds);
    gtk_widget_set_size_request(timer, 200, 20);
    gtk_fixed_put(GTK_FIXED(fixed), timer, (config->windows_length) / 2 - 100, 600);
    gtk_widget_override_font(timer, font_desc);

    gtk_widget_override_color(timer, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){1, 1, 1, 1});

    scoreLabel = gtk_label_new("");
    gtk_fixed_put(GTK_FIXED(fixed), scoreLabel, 20, 20);

    GdkRGBA text_color4;
    gdk_rgba_parse(&text_color4, "#00e700");
    gtk_widget_override_color(scoreLabel, GTK_STATE_FLAG_NORMAL, &text_color4);
    gtk_widget_override_font(scoreLabel, font_desc);

    g_signal_connect(buttonChoice1, "clicked", G_CALLBACK(check_answer), GINT_TO_POINTER(choice1));
    g_signal_connect(buttonChoice2, "clicked", G_CALLBACK(check_answer), GINT_TO_POINTER(choice2));
    g_signal_connect(buttonChoice3, "clicked", G_CALLBACK(check_answer), GINT_TO_POINTER(choice3));
    g_signal_connect(buttonChoice4, "clicked", G_CALLBACK(check_answer), GINT_TO_POINTER(choice4));

    update_answers(playlist);

    time(&startTime);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    timer_id = g_timeout_add_seconds(1, (GSourceFunc) time_handler, (gpointer) timer);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
