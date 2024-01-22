#ifndef OPEN_CONFIG_H
#define OPEN_CONFIG_H

#include <stddef.h>

typedef struct {
    int timer;
    int max_score;
    int windows_height;
    int windows_length;
    char img_playlist1[20];
    char img_playlist2[20];
    char image_folder[20];
    char base_url_playlist[200];
    char base_url_track[200];
    char database_table_name[20];
    char songs_path[256];
    char my_ranking_color[8];
    char database_name[35];
} AppConfig;

extern AppConfig *config;

int loadConfig(const char *filename);

#endif
