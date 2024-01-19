#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure pour stocker la configuration
typedef struct {
    int timer;
    int max_score;
    int windows_height;
    int windows_length;
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
        printf("windows_height: %s\n", config.windows_height);
        printf("windows_length: %s\n", config.windows_length);
    }

    return 0;
}
