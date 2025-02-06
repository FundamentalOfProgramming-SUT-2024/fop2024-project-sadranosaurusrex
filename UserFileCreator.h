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
    int rank;
} user_data;

typedef struct {
    int health;
    int floor;
    int x;
    int y;
    char heroSymbol;
    user_data user;
    int weapon[21];
    int weaponIndex;
    int currentWeapon;
    int spell[20];
    int spellIndex;
    int currentSpell;
} hero;

typedef struct {
    int floor;
    int x;
    int y;
    int visiblity;
    int type;
} trap;

typedef struct {
    int DifficultyLevel;
    int heroColor;
    char messages[256][256];
    trap traps[20];
    trap gold[20];
    trap food[20];
    trap spell[20];
    trap weapon[20]; 
} gameInfo;

extern char board[256][50];

void writeUserInfo(user_data user);
user_data readUserInfo(user_data user);
void loadDungeon(char* username);
void saveDungeon(char* username);
void loadBoard();
void boardSaver();

#endif // UserFileCreator_H