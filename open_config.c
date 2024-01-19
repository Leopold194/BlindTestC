#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure pour stocker la configuration
typedef struct {
    int timer;
    int max_score;
    int windows_height;
    int windows_length;
    char playlist1[13];
    char playlist2 [16];
    int buttonPlaylist1_height;
    int buttonPlaylist1_lenght;
    int buttonPlaylist2_height;
    int buttonPlaylist2_lenght;
    char image_folder [7];
} AppConfig;

// Fonction pour charger la configuration à partir d'un fichier
int loadConfig(const char *filename, AppConfig *config) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier de configuration");
        return -1;
    }

    fscanf(file, "timer: %d\n", config->timer);
    fscanf(file, "max_score: %d\n", &config->max_score);
    fscanf(file, "windows_height: %d\n", config->windows_height);
    fscanf(file, "windows_length: %d\n", config->windows_length);
    fscanf(file, "img_playlist1: %12s\n", config->playlist1);
    fscanf(file, "img_playlist2: %15s\n", config->playlist2);
    fscanf(file, "buttonPlaylist1_height: %d\n", config->buttonPlaylist1_height);
    fscanf(file, "buttonPlaylist1_lenght: %d\n", config->buttonPlaylist1_lenght);
    fscanf(file, "buttonPlaylist2_height: %d\n", config->buttonPlaylist2_height);
    fscanf(file, "buttonPlaylist2_lenght: %d\n", config->buttonPlaylist2_lenght);
    fscanf(file, "image_folder: %6s\n", config->image_folder);

    fclose(file);
    return 0;
}

int main() {
    AppConfig config;

    // Remplacez "config.txt" par le chemin complet de votre fichier de configuration
    if (loadConfig("config.txt", &config) == 0) {
        // Affiche les valeurs de configuration
        printf("timer: %s\n", config.timer);
        printf("max_score: %d\n", config.max_score);
        printf("windows_height: %d\n", config.windows_height);
        printf("windows_length: %d\n", config.windows_length);
        printf("img_playlist1: %12s\n", config.playlist1);
        printf("img_playlist2: %15s\n", config.playlist2);
        printf("buttonPlaylist1_height: %d\n", config.buttonPlaylist1_height);
        printf("buttonPlaylist1_lenght: %d\n", config.buttonPlaylist1_lenght);
        printf("buttonPlaylist2_height: %d\n", config.buttonPlaylist2_height);
        printf("buttonPlaylist2_lenght: %d\n", config.buttonPlaylist2_lenght);
        printf("image_folder: %6s\n", config.image_folder);
    }

    return 0;
}
