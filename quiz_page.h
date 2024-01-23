#ifndef QUIZ_PAGE_H
#define QUIZ_PAGE_H

#include <stddef.h>

void initialize_variables();
void update_button_labels();
void update_answers(Playlist *playlist);
int quiz_page(Playlist *playlist);
void check_answer(GtkWidget *widget, gpointer user_data);
void on_window_destroy();
void show_info(GtkWidget *widget, gpointer window, char text[]);

#endif
