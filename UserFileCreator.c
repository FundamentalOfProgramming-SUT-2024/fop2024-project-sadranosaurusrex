#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UserFileCreator.h"
#include "map_plot.h"

#define MAP_HEIGHT 30
#define MAP_WIDTH 80
#define FLOORS 4

void writeUserInfo(user_data user) {
    char fileName[256];  // Buffer for the filename
    snprintf(fileName, sizeof(fileName), "%s.txt", user.username);
    remove(fileName);
    FILE *file = fopen(fileName, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    fprintf(file, "%s\n", user.username);
    fprintf(file, "%s\n", user.password);
    fprintf(file, "%d\n", user.logStatus);
    fprintf(file, "%d\n", user.score);
    fprintf(file, "%d\n", user.gold);
    fprintf(file, "%d\n", user.count_games);
    fprintf(file, "%d\n", user.experience);
    fprintf(file, "---------------------------\n");

    fclose(file);
}

user_data readUserInfo(user_data user) {
    char fileName[256];
    snprintf(fileName, sizeof(fileName), "%s.txt", user.username);

    FILE *file = fopen(fileName, "r");  // Change 'a' to 'r' for reading
    if (file == NULL) {
        writeUserInfo(user);
        return user;
    }

    char line[10][256];
    int index = 0;
    while (fgets(line[index], sizeof(line[index]), file) && index < 7) {
        line[index][strcspn(line[index], "\n")] = 0; // Remove newline character
        index++;
    }
    
    strcpy(user.username, line[0]);
    strcpy(user.password, line[1]);
    user.logStatus = atoi(line[2]);
    user.score = atoi(line[3]);
    user.gold = atoi(line[4]);
    user.count_games = atoi(line[5]);
    user.experience = atoi(line[6]);

    fclose(file);
    return user;
}

void loadDungeon(char* username) {
    char filename[100];
    snprintf(filename, sizeof(filename), "%sdungeon.txt", username);

    FILE* file = fopen(filename, "w+");
    if (file == NULL) {
        perror("Error opening file");
        map_generator();
        return;
    }

    char line[MAP_WIDTH + 2];
    int floor = 0, y = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "Floor", 5) == 0) {
            
            sscanf(line, "Floor %d:", &floor);
            floor--;  
            y = 0;
        } else if (line[0] != '\n' && y < MAP_HEIGHT) {
            
            for (int x = 0; x < MAP_WIDTH && line[x] != '\n' && line[x] != '\0'; x++) {
                dungeon[floor][y][x] = line[x];
            }
            y++;
        }
    }

    fclose(file);
    printw("Dungeon loaded from %s\n", filename);
    return;
}

void saveDungeon(char* username) {
    char filename[100];
    snprintf(filename, sizeof(filename), "%sdungeon.txt", username);

    remove(filename);

    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    for (int f = 0; f < FLOORS; f++) {
        fprintf(file, "Floor %d:\n", f + 1);
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                fprintf(file, "%c", dungeon[f][y][x]);
            }
            fprintf(file, "\n");
        }
        fprintf(file, "\n");
    }

    fclose(file);
    printw("Dungeon saved to %s\n", filename);
}

