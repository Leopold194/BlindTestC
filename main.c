#include <gtk/gtk.h>
#include <gst/gst.h>
#include <sqlite3.h>
#include "globals.h"
#include "menu.h"
#include "connect_db.h"
#include "open_config.h"
#include "create_database.h"

GtkWidget *signin_entry_login;
GtkWidget *signin_entry_pwd;
GtkWidget *signout_entry_login;
GtkWidget *signout_entry_pwd;

GtkWidget *errorLabel;

gchar *password = NULL;
gboolean userExists = FALSE;
gboolean checkUserExist = FALSE;

void connection(GtkWidget *widget, gpointer data) {
    /*
        Cette fonction permet à un utilisateur du jeu de se connecter à son compte. 
        Pour ce faire on récupère les valeurs entrées dans les entry.
        Puis on recherche le mot de passe en base de données pour vérifier la correspondance,
        et si on ne trouve pas de lignes correspondant au pseudo, on le fait savoir.
        On utilise des requêtes préparées.
    */
    const gchar *login_text = gtk_entry_get_text(GTK_ENTRY(signin_entry_login));
    const gchar *pwd_text = gtk_entry_get_text(GTK_ENTRY(signin_entry_pwd));

    char sql[200];
    sprintf(sql, "SELECT password FROM %s WHERE pseudo=?;", config->database_table_name);

    if (connectDb() != 1) {
        fprintf(stderr, "Echec de la connexion à la base de données");
        return;
    }

    if (sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0) != SQLITE_OK) {
        fprintf(stderr, "Echec de la transaction");
        closeDb();
        return;
    }

    sqlite3_stmt *query_prepare;
    if (sqlite3_prepare_v2(db, sql, -1, &query_prepare, 0) == SQLITE_OK) {
        sqlite3_bind_text(query_prepare, 1, login_text, -1, SQLITE_STATIC);

        if (sqlite3_step(query_prepare) == SQLITE_ROW) {
            g_free(password);
            password = g_strdup((const char*)sqlite3_column_text(query_prepare, 0));

            if (strcmp(pwd_text, password) == 0) {
                gtk_label_set_text(GTK_LABEL(errorLabel), "Connexion réussie");
                strcpy(currentPlayer, login_text);

                sqlite3_exec(db, "COMMIT", 0, 0, 0);
                sqlite3_finalize(query_prepare);
                closeDb();

                gtk_widget_destroy(GTK_WIDGET(gtk_widget_get_toplevel(widget)));
                menu();
                return;
            } else {
                gtk_label_set_text(GTK_LABEL(errorLabel), "Pseudo ou Mot de passe incorrect");
            }
        } else {
            gtk_label_set_text(GTK_LABEL(errorLabel), "Pseudo non trouvé");
        }

        sqlite3_finalize(query_prepare);
    } else {
        fprintf(stderr, "Echec de la requête");
    }

    sqlite3_exec(db, "ROLLBACK", 0, 0, 0);

    closeDb();
}


void registration(GtkWidget *widget, gpointer data) {
    /*
        Cette fonction permet à un nouvel utilisateur du jeu de s'inscrire. 
        Pour ce faire on récupère les valeurs entrées dans les entry.
        Puis on vérifie que le pseudo n'est pas déjà pris, dans le cas contraire,
        on enregistre pseudo et mot de passe dans la base de données.
        On utilise également des requêtes préparées.
    */
    const gchar *login_text = gtk_entry_get_text(GTK_ENTRY(signout_entry_login));
    const gchar *pwd_text = gtk_entry_get_text(GTK_ENTRY(signout_entry_pwd));

    char sql[200];
    sprintf(sql, "SELECT * FROM %s WHERE pseudo=?;", config->database_table_name);

    if (connectDb() != 1) {
        fprintf(stderr, "Echec de la connexion à la base de données");
        return;
    }

    if (sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0) != SQLITE_OK) {
        fprintf(stderr, "Echec de la transaction");
        closeDb();
        return;
    }

    sqlite3_stmt *query_prepare;
    if (sqlite3_prepare_v2(db, sql, -1, &query_prepare, 0) == SQLITE_OK) {
        sqlite3_bind_text(query_prepare, 1, login_text, -1, SQLITE_STATIC);

        if (sqlite3_step(query_prepare) == SQLITE_ROW) {
            gtk_label_set_text(GTK_LABEL(errorLabel), "Pseudo existant");
            sqlite3_finalize(query_prepare);
            sqlite3_exec(db, "ROLLBACK", 0, 0, 0);
            closeDb();
            return;
        } else {
            sqlite3_finalize(query_prepare);
            
            char sql2[200];
            sprintf(sql2, "INSERT INTO %s(pseudo, password) VALUES (?, ?);", config->database_table_name);
            
            sqlite3_stmt *query_prepare_insert;
            if (sqlite3_prepare_v2(db, sql2, -1, &query_prepare_insert, 0) == SQLITE_OK) {
                sqlite3_bind_text(query_prepare_insert, 1, login_text, -1, SQLITE_STATIC);
                sqlite3_bind_text(query_prepare_insert, 2, pwd_text, -1, SQLITE_STATIC);

                if (sqlite3_step(query_prepare_insert) == SQLITE_DONE) {
                    gtk_label_set_text(GTK_LABEL(errorLabel), "Inscription réussie");
                    strcpy(currentPlayer, login_text);
                    sqlite3_finalize(query_prepare_insert);
                    
                    sqlite3_exec(db, "COMMIT", 0, 0, 0);
                    closeDb();
                    
                    gtk_widget_destroy(GTK_WIDGET(gtk_widget_get_toplevel(widget)));
                    menu();
                    return;
                } else {
                    gtk_label_set_text(GTK_LABEL(errorLabel), "Échec de l'inscription");
                }
                sqlite3_finalize(query_prepare_insert);
            } else {
                fprintf(stderr, "Échec de préparation de la requête d'insertion");
            }
        }
    } else {
        fprintf(stderr, "Echec de la requête");
    }

    sqlite3_exec(db, "ROLLBACK", 0, 0, 0);

    closeDb();
}


int main(int argc, char *argv[]) {
    /*
        Cette fonction affiche la fenetre de connexion. Elle commence par créer la base de données
        si elle n'existe pas, puis elle charge le fichier de configuration.
        Puis on crée les différents labels/boutons/entry, avec des couleurs et des font size différentes.
        On affiche les erreurs de connexion possibles dans un label erreur.
    */
    loadConfig("config.txt");
    create_database();

    GtkWidget *window;
    GtkWidget *fixed;

    GtkWidget *signin_label;
    GtkWidget *signin_label_login;
    GtkWidget *signin_label_pwd;
    GtkWidget *signout_label;
    GtkWidget *signout_label_login;
    GtkWidget *signout_label_pwd;

    GtkWidget *signin_button;
    GtkWidget *signout_button;

    gtk_init(&argc, &argv);
    gst_init(&argc, &argv);
    PangoFontDescription *font_desc1 = pango_font_description_new();
    pango_font_description_set_size(font_desc1, 20 * PANGO_SCALE);
    PangoFontDescription *font_desc = pango_font_description_new();
    pango_font_description_set_size(font_desc, 30 * PANGO_SCALE);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Connexion");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_widget_set_size_request(window, config->windows_length, config->windows_height);

    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    GtkWidget *image = gtk_image_new_from_file("uploads/background.jpg");
    gtk_widget_set_size_request(image, 400, 300);
    gtk_fixed_put(GTK_FIXED(fixed), image, 0, 0);

    //Signin

    signin_label = gtk_label_new("Connexion :");
    gtk_widget_override_font(signin_label, font_desc);
    gtk_fixed_put(GTK_FIXED(fixed), signin_label, (config->windows_length)/4  - 100, 80);
    gtk_widget_set_size_request(signin_label, 200, 30);

    signin_label_login = gtk_label_new("Pseudo :");
    gtk_widget_override_font(signin_label_login, font_desc1);
    gtk_fixed_put(GTK_FIXED(fixed), signin_label_login, (config->windows_length)/4  - 100, 150);
    gtk_widget_set_size_request(signin_label_login, 200, 30);

    signin_entry_login = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), signin_entry_login, (config->windows_length)/4 - 100, 200);
    gtk_widget_set_size_request(signin_entry_login, 200, 30);

    signin_label_pwd = gtk_label_new("Mot de passe :");
    gtk_widget_override_font(signin_label_pwd, font_desc1);
    gtk_fixed_put(GTK_FIXED(fixed), signin_label_pwd, (config->windows_length)/4  - 100, 260);
    gtk_widget_set_size_request(signin_label_pwd, 200, 30);

    signin_entry_pwd = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), signin_entry_pwd, (config->windows_length)/4 - 100, 310);
    gtk_widget_set_size_request(signin_entry_pwd, 200, 30);

    signin_button = gtk_button_new_with_label("Connexion");
    gtk_fixed_put(GTK_FIXED(fixed), signin_button, (config->windows_length)/4 - 100, 380);
    gtk_widget_set_size_request(signin_button, 200, 30);

    //Signout

    signout_label = gtk_label_new("Inscription :");
    gtk_widget_override_font(signout_label, font_desc);
    gtk_fixed_put(GTK_FIXED(fixed), signout_label, (config->windows_length)/4 *3 - 100, 80);
    gtk_widget_set_size_request(signout_label, 200, 30);

    signout_label_login = gtk_label_new("Pseudo :");
    gtk_widget_override_font(signout_label_login, font_desc1);
    gtk_fixed_put(GTK_FIXED(fixed), signout_label_login, (config->windows_length)/4 *3 - 100, 150);
    gtk_widget_set_size_request(signout_label_login, 200, 30);

    signout_entry_login = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), signout_entry_login, (config->windows_length)/4 *3 - 100, 200);
    gtk_widget_set_size_request(signout_entry_login, 200, 30);

    signout_label_pwd = gtk_label_new("Mot de passe :");
    gtk_widget_override_font(signout_label_pwd, font_desc1);
    gtk_fixed_put(GTK_FIXED(fixed), signout_label_pwd, (config->windows_length)/4 *3 - 100, 260);
    gtk_widget_set_size_request(signout_label_pwd, 200, 30);

    signout_entry_pwd = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), signout_entry_pwd, (config->windows_length)/4 *3 - 100, 310);
    gtk_widget_set_size_request(signout_entry_pwd, 200, 30);
    

    signout_button = gtk_button_new_with_label("Inscription");
    gtk_fixed_put(GTK_FIXED(fixed), signout_button, (config->windows_length)/4 *3 - 100, 380);
    gtk_widget_set_size_request(signout_button, 200, 30);

    //Error

    errorLabel = gtk_label_new("");
    gtk_widget_set_size_request(errorLabel, 300, 20);
    gtk_fixed_put(GTK_FIXED(fixed), errorLabel, ((config->windows_length/2) - 150), (config->windows_height /4 - 50 ));
    PangoFontDescription *font_desc3 = pango_font_description_new();
    pango_font_description_set_size(font_desc3, 30 * PANGO_SCALE);
    gtk_widget_override_font(errorLabel, font_desc3);

    gtk_widget_override_color(signin_label, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){1, 1, 1, 1});
    gtk_widget_override_color(signin_label_login, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){1, 1, 1, 1});
    gtk_widget_override_color(signin_label_pwd, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){1, 1, 1, 1});
    gtk_widget_override_color(signout_label, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){1, 1, 1, 1});
    gtk_widget_override_color(signout_label_login, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){1, 1, 1, 1});
    gtk_widget_override_color(signout_label_pwd, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){1, 1, 1, 1});
    gtk_widget_override_color(errorLabel, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){1, 1, 1, 1});

    gtk_widget_show_all(window);

    g_signal_connect(signin_button, "clicked", G_CALLBACK(connection), NULL);
    g_signal_connect(signout_button, "clicked", G_CALLBACK(registration), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_main();

    return 0;
}
