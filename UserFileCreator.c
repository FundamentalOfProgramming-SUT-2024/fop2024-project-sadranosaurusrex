#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UserFileCreator.h"

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
        perror("Error opening file");
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


// int main() {
//     user_data user;

//     // Getting user input
//     printf("Enter username: ");
//     fgets(user.username, sizeof(user.username), stdin);
//     user.username[strcspn(user.username, "\n")] = 0; // Remove newline character

//     printf("Enter password: ");
//     fgets(user.password, sizeof(user.password), stdin);
//     user.password[strcspn(user.password, "\n")] = 0; // Remove newline character

//     printf("Enter log status (0 for logged out, 1 for logged in): ");
//     scanf("%d", &user.logStatus);

//     printf("Enter score: ");
//     scanf("%d", &user.score);

//     printf("Enter gold: ");
//     scanf("%d", &user.gold);

//     printf("Enter number of games played: ");
//     scanf("%d", &user.count_games);

//     printf("Enter experience points: ");
//     scanf("%d", &user.experience);

//     writeUserInfo(user);

//     printf("User information saved successfully.\n");

//     return 0;
// }
