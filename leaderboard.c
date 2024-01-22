#include <gtk/gtk.h>
#include <sqlite3.h>
#include <stdio.h>
#include "globals.h"
#include "connect_db.h"

GtkWidget *tree_view;
int callback(void *data, int argc, char **argv, char **col_names);

static void render_background(GtkTreeViewColumn *column, GtkCellRenderer *renderer, GtkTreeModel *model, GtkTreeIter *iter, gpointer user_data) {
    gchar *pseudo;
    gtk_tree_model_get(model, iter, 0, &pseudo, -1);
    if (strcmp(pseudo, currentPlayer) == 0) {
        GdkRGBA color;
        gdk_rgba_parse(&color, "#FAD150"); 
        g_object_set(renderer, "cell-background-rgba", &color, NULL);
    } else {
        g_object_set(renderer, "cell-background-rgba", NULL, NULL);
    }
    free(pseudo);
}

int leaderboard() {
    GtkWidget *window;
    GtkWidget *fixed;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Example");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_widget_set_size_request(window, 1800, 900);

    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_fixed_put(GTK_FIXED(fixed), scrolled_window, 50, 50);
    gtk_widget_set_size_request(scrolled_window, 1620, 700);

    tree_view = gtk_tree_view_new();
    gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);

    GtkListStore *list_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);

    GtkTreeViewColumn *column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(column, "Pseudo");
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 0);
    gtk_tree_view_column_set_cell_data_func(column, renderer, render_background, NULL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(column, "Meilleur Score");
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 1);
    gtk_tree_view_column_set_cell_data_func(column, renderer, render_background, NULL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    if (connectDb() != 1) {
        return 1;
    }

    char *err_msg = 0;
    char *sql = "SELECT pseudo, best_score FROM User ORDER BY best_score ASC;";

    if (sqlite3_exec(db, sql, callback, list_store, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        closeDb();
        return 1;
    }

    closeDb();

    gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(list_store));
    g_object_unref(list_store);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

int callback(void *data, int argc, char **argv, char **col_names) {
    GtkListStore *list_store = GTK_LIST_STORE(data);
    GtkTreeIter iter;

    int best_score = atoi(argv[1]);

    gtk_list_store_append(list_store, &iter);
    gtk_list_store_set(list_store, &iter, 0, argv[0], 1, best_score, -1);

    return 0;
}
