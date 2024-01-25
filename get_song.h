#ifndef GET_SONG_H
#define GET_SONG_H

#include <stddef.h>

typedef struct {
    unsigned long int id;
    char title[256];
    char artist[256];
} Track;

size_t write_callback_track(void *contents, size_t size, size_t nmemb, void *userp);
Track* init_track(unsigned long int id);

#endif
