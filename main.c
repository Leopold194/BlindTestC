#include <gtk/gtk.h>
#include <gst/gst.h>
#include <sqlite3.h>
#include "globals.h"
#include "menu.h"
#include "connect_db.h"

GtkWidget *signin_entry_login;
GtkWidget *signin_entry_pwd;
GtkWidget *signout_entry_login;
GtkWidget *signout_entry_pwd;

GtkWidget *errorLabel;

gchar *password = NULL;
gboolean userExists = FALSE;
gboolean checkUserExist = FALSE;

void connection(GtkWidget *widget, gpointer data) {
    const gchar *login_text = gtk_entry_get_text(GTK_ENTRY(signin_entry_login));
    const gchar *pwd_text = gtk_entry_get_text(GTK_ENTRY(signin_entry_pwd));

    char sql[200];
    sprintf(sql, "SELECT password FROM User WHERE pseudo=?;");

    if (connectDb() != 1) {
        gtk_label_set_text(GTK_LABEL(errorLabel), "Database connection failed");
        return;
    }

    if (sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0) != SQLITE_OK) {
        gtk_label_set_text(GTK_LABEL(errorLabel), "Failed to begin transaction");
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
        gtk_label_set_text(GTK_LABEL(errorLabel), "Failed to execute query");
    }

    sqlite3_exec(db, "ROLLBACK", 0, 0, 0);

    closeDb();
}


void registration(GtkWidget *widget, gpointer data) {
    const gchar *login_text = gtk_entry_get_text(GTK_ENTRY(signout_entry_login));
    const gchar *pwd_text = gtk_entry_get_text(GTK_ENTRY(signout_entry_pwd));

    char sql[200];
    sprintf(sql, "SELECT * FROM User WHERE pseudo=?;");

    if (connectDb() != 1) {
        gtk_label_set_text(GTK_LABEL(errorLabel), "Database connection failed");
        return;
    }

    if (sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0) != SQLITE_OK) {
        gtk_label_set_text(GTK_LABEL(errorLabel), "Failed to begin transaction");
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
            sprintf(sql2, "INSERT INTO User(pseudo, password) VALUES (?, ?);");
            
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
                    gtk_label_set_text(GTK_LABEL(errorLabel), "Échec de l'insertion");
                }
                sqlite3_finalize(query_prepare_insert);
            } else {
                gtk_label_set_text(GTK_LABEL(errorLabel), "Échec de préparation de la requête d'insertion");
            }
        }
    } else {
        gtk_label_set_text(GTK_LABEL(errorLabel), "Failed to execute query");
    }

    sqlite3_exec(db, "ROLLBACK", 0, 0, 0);

    closeDb();
}


int main(int argc, char *argv[]) {

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

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Example");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_widget_set_size_request(window, 1800, 900);

    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    //Signin

    signin_label = gtk_label_new("Connexion :");
    gtk_fixed_put(GTK_FIXED(fixed), signin_label, 400, 100);

    signin_label_login = gtk_label_new("Pseudo :");
    gtk_fixed_put(GTK_FIXED(fixed), signin_label_login, 410, 150);

    signin_entry_login = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), signin_entry_login, 410, 200);
    gtk_widget_set_size_request(signin_entry_login, 200, 30);

    signin_label_pwd = gtk_label_new("Mot de passe :");
    gtk_fixed_put(GTK_FIXED(fixed), signin_label_pwd, 390, 260);

    signin_entry_pwd = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), signin_entry_pwd, 410, 310);
    gtk_widget_set_size_request(signin_entry_pwd, 200, 30);

    signin_button = gtk_button_new_with_label("Connexion");
    gtk_fixed_put(GTK_FIXED(fixed), signin_button, 300, 360);
    gtk_widget_set_size_request(signin_button, 300, 50);

    //Signout

    signout_label = gtk_label_new("Inscription :");
    gtk_fixed_put(GTK_FIXED(fixed), signout_label, 1300, 100);

    signout_label_login = gtk_label_new("Pseudo :");
    gtk_fixed_put(GTK_FIXED(fixed), signout_label_login, 1290, 150);

    signout_entry_login = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), signout_entry_login, 1290, 200);
    gtk_widget_set_size_request(signout_entry_login, 200, 30);

    signout_label_pwd = gtk_label_new("Mot de passe :");
    gtk_fixed_put(GTK_FIXED(fixed), signout_label_pwd, 1310, 260);

    signout_entry_pwd = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), signout_entry_pwd, 1290, 310);
    gtk_widget_set_size_request(signout_entry_pwd, 200, 30);

    signout_button = gtk_button_new_with_label("Inscription");
    gtk_fixed_put(GTK_FIXED(fixed), signout_button, 1100, 360);
    gtk_widget_set_size_request(signout_button, 300, 50);

    //Error

    errorLabel = gtk_label_new("");
    gtk_fixed_put(GTK_FIXED(fixed), errorLabel, 770, 500);

    gtk_widget_show_all(window);

    g_signal_connect(signin_button, "clicked", G_CALLBACK(connection), NULL);
    g_signal_connect(signout_button, "clicked", G_CALLBACK(registration), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_main();

    return 0;
}