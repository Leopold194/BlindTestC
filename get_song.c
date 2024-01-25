#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include "get_song.h"
#include "open_config.h"

size_t write_callback_track(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    json_error_t error;

    json_t *root = json_loadb(contents, realsize, 0, &error);
    if (!root) {
        fprintf(stderr, "Error parsing JSON: %s\n", error.text);
        return 0;
    }

    // Create variable to retrieve a data
    json_t *title = json_object_get(root, "title");
    json_t *artist = json_object_get(root, "artist");

    // If we want to retrieve a data which is not in array
    if (json_is_string(title)) {
        strcpy(((Track *)userp)->title, json_string_value(title));
    }

    // If we want to retrieve a data which is in array
    if (json_is_object(artist)) {
        json_t *artist_name = json_object_get(artist, "name");
        if (json_is_string(artist_name)) {
            strcpy(((Track *)userp)->artist, json_string_value(artist_name));
        }
    }

    json_decref(root);
    return realsize;
}


Track* init_track(unsigned long int id) {

    Track *track = malloc(sizeof(Track));
    track->id = id;

    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        char url[256];
        snprintf(url, sizeof(url), "%s%d", config->base_url_track, track->id);

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_track);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)track);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        curl_easy_cleanup(curl);
    }

    return track;
}