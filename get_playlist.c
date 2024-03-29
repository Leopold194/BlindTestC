#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include <time.h>
#include "get_song.h"
#include "get_playlist.h"
#include "open_config.h"

/*
    Structure pour stocker les données téléchargées par le curl.
*/
struct MemoryStruct {
    char *memory;
    size_t size;
};

void shuffle_playlist(Playlist *playlist) {
    /*
        Cette fonction permet de mélanger aléatoirement les track de la playlist.
    */
    size_t num_tracks = playlist->num_tracks;

    srand((unsigned int)time(NULL));

    for (size_t i = num_tracks - 1; i > 0; --i) {
        size_t j = rand() % (i + 1);

        Track temp = playlist->tracklist[i];
        playlist->tracklist[i] = playlist->tracklist[j];
        playlist->tracklist[j] = temp;
    }
}

size_t write_callback_playlist(void *contents, size_t size, size_t nmemb, void *userp) {
    /*
        Cette fonction permet de lire/garder en mémoire la réponse du curl.
    */
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    // Réallouer la mémoire pour le tampon
    mem->memory = realloc(mem->memory, mem->size + realsize + 1);

    if (mem->memory == NULL) {
        fprintf(stderr, "Erreur pendant le realloc\n");
        return 0;
    }

    // Copier les données dans le tampon
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;

    // Ajouter le caractère nul à la fin
    mem->memory[mem->size] = '\0';

    return realsize;
}

Playlist* init_playlist(unsigned long int id) {
    /*
        Cette fonction permet d'initialiser une playlist, grâce à son id.
        Elle récupère tout d'abord le lien, execute un curl, puis récupère les données json, 
        afin de les attribuer à chaque track de la playlist.
    */

    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Erreur dans l'initialisation du curl\n");
        return NULL;
    }

    struct MemoryStruct chunk;
    chunk.memory = NULL;
    chunk.size = 0;

    char url[256];
    snprintf(url, sizeof(url), "%s%lu", config->base_url_playlist, id);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_playlist);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "Ereur dans le curl_easy_perform() : %s\n", curl_easy_strerror(res));
        free(chunk.memory);
        curl_easy_cleanup(curl);
        return NULL;
    }

    json_error_t error;
    json_t *root = json_loads(chunk.memory, 0, &error);

    if (!root) {
        fprintf(stderr, "Erreur avec le JSON: %s\n", error.text);
        free(chunk.memory);
        curl_easy_cleanup(curl);
        return NULL;
    }

    json_t *tracks = json_object_get(root, "tracks");
    json_t *data = json_object_get(tracks, "data");
    if (!json_is_array(data)) {
        fprintf(stderr, "Erreur JSON, data n'est pas un array\n");
        free(chunk.memory);
        json_decref(root);
        curl_easy_cleanup(curl);
        return NULL;
    }

    unsigned long int num_tracks = json_array_size(data);

    Playlist *playlist = malloc(sizeof(Playlist) + num_tracks * sizeof(Track));

    playlist->id = json_integer_value(json_object_get(root, "id"));
    playlist->num_tracks = num_tracks;

    for (unsigned long int i = 0; i < num_tracks; ++i) {
        json_t *track = json_array_get(data, i);

        json_t *title_json = json_object_get(track, "title");
        json_t *artist_json = json_object_get(track, "artist");
        json_t *artist_name = json_object_get(artist_json, "name");

        if (!json_is_string(title_json) || !json_is_string(artist_name)) {
            fprintf(stderr, "Erreur JSON, artist et name ne sont pas des string\n");
            free(chunk.memory);
            free(playlist);
            json_decref(root);
            curl_easy_cleanup(curl);
            return NULL;
        }

        playlist->tracklist[i].id = json_integer_value(json_object_get(track, "id"));
        strncpy(playlist->tracklist[i].title, json_string_value(title_json), sizeof(playlist->tracklist[i].title) - 1);
        strncpy(playlist->tracklist[i].artist, json_string_value(artist_name), sizeof(playlist->tracklist[i].artist) - 1);
    }

    free(chunk.memory);
    json_decref(root);
    curl_easy_cleanup(curl);

    shuffle_playlist(playlist);

    return playlist;
}

void free_playlist(Playlist *playlist) {
    /*
        Cette fonction permet de libérer la mémoire allouée pour la playlist.
    */
    free(playlist);
}
