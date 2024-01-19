#ifndef GET_PLAYLIST_H
#define GET_PLAYLIST_H

#include <stddef.h>
#include "get_song.h"

typedef struct {
    unsigned long int id;
    size_t num_tracks;
    Track tracklist[]; 
} Playlist;

size_t write_callback_playlist(void *contents, size_t size, size_t nmemb, void *userp);
Playlist* init_playlist(unsigned long int id);
void free_playlist(Playlist *playlist);
void shuffle_playlist(Playlist *playlist);

#endif
