#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <stddef.h>

static void render_background(GtkTreeViewColumn *column, GtkCellRenderer *renderer, GtkTreeModel *model, GtkTreeIter *iter, gpointer user_data);
int leaderboard();
int callback(void *data, int argc, char **argv, char **col_names);

#endif
