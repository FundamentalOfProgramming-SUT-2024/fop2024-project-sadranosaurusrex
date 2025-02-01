#ifndef UserFileCreator_H
#define UserFileCreator_H

typedef struct {
    char username[50];
    char password[50];
    int logStatus;
    int score;
    int gold;
    int count_games;
    int experience;
} user_data;

typedef struct {
    int health;
    int floor;
    int x;
    int y;
    char heroSymbol;
} hero;

void writeUserInfo(user_data user);
user_data readUserInfo(user_data user);
void loadDungeon(char* username);
void saveDungeon(char* username);

#endif // UserFileCreator_H