#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "open_config.h"

AppConfig *config;

int loadConfig(const char *filename) {
    config = malloc(sizeof(AppConfig));
    
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier de configuration");
        return -1;
    }

    char line[256];
    char *key;
    char *value;

    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\r\n")] = '\0';

        key = strtok(line, "=");
        value = strtok(NULL, "=");

        if (key != NULL && value != NULL) {

            if (strcmp(key, "timer") == 0) {
                config->timer = atoi(value);
            } else if (strcmp(key, "max_score") == 0) {
                config->max_score = atoi(value);
            } else if (strcmp(key, "windows_height") == 0) {
                config->windows_height = atoi(value);
            } else if (strcmp(key, "windows_length") == 0) {
                config->windows_length = atoi(value);
            } else if (strcmp(key, "img_playlist1") == 0) {
                strncpy(config->img_playlist1, value, sizeof(config->img_playlist1));
            } else if (strcmp(key, "img_playlist2") == 0) {
                strncpy(config->img_playlist2, value, sizeof(config->img_playlist2));
            } else if (strcmp(key, "playlist_id1") == 0) {
                config->playlist_id1 = strtoul(value, NULL, 10);
            } else if (strcmp(key, "playlist_id2") == 0) {
                config->playlist_id2 = strtoul(value, NULL, 10);
            } else if (strcmp(key, "image_folder") == 0) {
                strncpy(config->image_folder, value, sizeof(config->image_folder));
            } else if (strcmp(key, "base_url_playlist") == 0) {
                strncpy(config->base_url_playlist, value, sizeof(config->base_url_playlist));
            } else if (strcmp(key, "base_url_track") == 0) {
                strncpy(config->base_url_track, value, sizeof(config->base_url_track));
            } else if (strcmp(key, "database_table_name") == 0) {
                strncpy(config->database_table_name, value, sizeof(config->database_table_name));
            } else if (strcmp(key, "songs_path") == 0) {
                strncpy(config->songs_path, value, sizeof(config->songs_path));
            } else if (strcmp(key, "my_ranking_color") == 0) {
                strncpy(config->my_ranking_color, value, sizeof(config->my_ranking_color));
            } else if (strcmp(key, "database_name") == 0) {
                strncpy(config->database_name, value, sizeof(config->database_name));
            } else if (strcmp(key, "windows_color") == 0) {
                strncpy(config->windows_color, value, sizeof(config->windows_color));
            } 
        }
    }

    fclose(file);
    return 0;
}