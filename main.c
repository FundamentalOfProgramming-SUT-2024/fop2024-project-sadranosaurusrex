#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include "map_plot.h"
#include "menu.h"
#include "UserFileCreator.h"
#include "main.h"

#define MAP_HEIGHT 30
#define MAP_WIDTH 80
#define MAX_HEALTH 100

hero createHero() {
    hero newHero;
    newHero.floor = 0;
    newHero.health = MAX_HEALTH;
    newHero.heroSymbol = '0';

    srand(time(NULL));

    while (1) {
        int randomX = rand() % MAP_WIDTH/3;
        int randomY = rand() % MAP_HEIGHT/2;

        if (dungeon[newHero.floor][randomY][randomX] == '.') {
            newHero.x = randomX;
            newHero.y = randomY;
            break;
        }
    }

    return newHero;
}

user_data userFinder(char* username) {
    user_data user;
    strcpy(user.username, username);
    user = readUserInfo(user);
    return user;
} 

hero myhero;

user_data newPlayerCreation (char username[], char password[]) {
    user_data newplayer;
    strcpy(newplayer.username, username);
    strcpy(newplayer.password, password);
    newplayer.logStatus = 1;
    newplayer.score = 0;
    newplayer.gold = 0;
    newplayer.count_games = 0;
    newplayer.experience = 0;
    return newplayer;
}

user_data setupLogin() {
    user_data newuser;
    int menu_value = menu();
    while (menu_value == -1) menu_value = menu();
    if (menu_value == 3) {
        newuser.logStatus = -1;
        return newuser;
    }
    else if (menu_value == 0) {
        int choice = NewGameChoices();
        if (choice == 0) {
            // userfile will be read
            newuser = newPlayerCreation(usernameext, passwordext);
            writeUserInfo(newuser);
            newuser.logStatus = 1;
            loadDungeon(newuser.username);
        }
        else if (choice == 1) {
            //userfile will be deleted and created again
            newuser = readUserInfo(newuser);
            newuser.logStatus = 1;
            map_generator();
            myhero = createHero();
        }
    }
    else if (menu_value == 1) {
        newuser = newPlayerCreation(usernameext, passwordext);
    }
    else if (menu_value == 2) {
        newuser = newPlayerCreation("Guest", "Guest");
        map_generator();
        myhero = createHero();
    }
    return newuser;
}

void onlySomeFeet (int feet, int floor) {
    clear();
    for (int i = myhero.x -feet; i <= myhero.x +feet; i++) {
        for (int j = myhero.y -feet; j <= myhero.y +feet; j++) {
            if (i > 0 && i <= MAP_HEIGHT *3 && j > 0 && j <= MAP_WIDTH*2) 
                mvprintw(j, i, "%c", dungeon[floor][j][i]);
        }
    }
    movementHandler(0, 0);
}

int U() {
    myhero.floor++;
    srand(time(NULL));

    while (1) {
        int randomX = rand() % MAP_WIDTH/3;
        int randomY = rand() % MAP_HEIGHT/2;

        if (dungeon[myhero.floor][randomY][randomX] == '.') {
            myhero.x = randomX;
            myhero.y = randomY;
            break;
        }
    }
    return myhero.floor;
}

int D() {
    myhero.floor--;
    srand(time(NULL));

    while (1) {
        int randomX = rand() % MAP_WIDTH/3;
        int randomY = MAP_HEIGHT -rand() % MAP_HEIGHT/2;

        if (dungeon[myhero.floor][randomY][randomX] == '.') {
            myhero.x = randomX;
            myhero.y = randomY;
            break;
        }
    }
    return myhero.floor;
}

void rerank() {
    printf("first on board: %s", board[0]);
    getchar();
    
    char users[256][50];
    int scores[256];
    int i = 0;
    int existance = 0;
    while (board[i][0] != '\0') {
        char* token2 = strtok(board[i] +2, ":");
        int token3 = atoi(strtok(NULL, "\0"));
        if (!strcmp(myhero.user.username, token2)) {
            token3 = myhero.user.score;
            existance = 1;
        }
        strcpy(users[i], token2);
        scores[i] = token3;
        i++;
    }

    if (!existance) {
        i++;
        snprintf(board[i], sizeof(board[i]), "%d.%s:%d", i +1, myhero.user.username, myhero.user.score);
        strcpy(users[i], myhero.user.username);
        scores[i] = myhero.user.score;
    }

    for (int j = 0; j < i; j++) {
        for (int k = j +1; k < i; k++) {
            if (scores[j] < scores[k]) {
                int temp = scores[j];
                scores[j] = scores[k];
                scores[k] = temp;

                char tempstr[50]; 
                strcpy(tempstr, users[j]);
                strcpy(users[j], users[k]);
                strcpy(users[k], tempstr);
            }
        }
    }
    
    for (int j = 0; j < i; j++) {
        snprintf(board[j], sizeof(board[j]), "%d.%s:%d", j +1, users[j], scores[j]);
    }

    return board;
}

void weapons() {

}


void options(int choice) {
    switch (choice)
    {
    case 0:
        return;
    
    case 1:
        weapons();
        break;

    case 2: {
        rerank();
        boardDisplayer();
        boardSaver();
        break;
    }
    case 3: {
        rerank();
        boardSaver();
        break;
    }

    default:
        break;
    }
}

void renderGame() {
    initscr();
    noecho();
    curs_set(FALSE);

    int currentFloor = myhero.floor;
    int displayStatus = 1;

    while (1) {
        if (displayStatus == 1) displayFloor(currentFloor);
        else onlySomeFeet(5, currentFloor);
        bunny();
        if (myhero.floor == currentFloor) movementHandler(0, 0);
        else mvprintw(myhero.y, myhero.x, "%c", dungeon[currentFloor][myhero.y][myhero.x]);
        
        if (dungeon[currentFloor][myhero.y][myhero.x] == 'U') {
            currentFloor = U();
        }
        else if (dungeon[currentFloor][myhero.y][myhero.x] == 'D') {
            currentFloor = D();
        }
        
        int ch = getch();
        if (ch == 'y') movementHandler(-1, -1);
        else if (ch == 'u') movementHandler(-1, 1);
        else if (ch == 'h') movementHandler(0, -1);
        else if (ch == 'j') movementHandler(-1, 0);
        else if (ch == 'k') movementHandler(1, 0);
        else if (ch == 'l') movementHandler(0, 1);
        else if (ch == 'b') movementHandler(1, -1);
        else if (ch == 'n') movementHandler(1, 1);
        else if (ch == 'q') break;
        else if (ch == 's' && currentFloor > 0) currentFloor--;
        else if (ch == 'w' && currentFloor < FLOORS - 1) currentFloor++;
        else if (ch == 'a') displayStatus *= -1;
        else if (ch == 'p') {
            options(settingMenu());
        }
    }

    endwin();
}

void movementHandler(int j, int i) {
    bunny();
    int desx = myhero.x +i;
    int desy = myhero.y +j;
    if (desx < 1 || desy < 1 || desx > MAP_WIDTH || desy > MAP_HEIGHT 
        || dungeon[myhero.floor][desy][desx] == '|' || dungeon[myhero.floor][desy][desx] == '_') return;
    else 
    myhero.x += i;
    myhero.y += j;
    attron(A_REVERSE); mvprintw(myhero.y, myhero.x, "%c", myhero.heroSymbol); attroff(A_REVERSE);
    return;
}

int main() {
    user_data player = setupLogin();
    myhero.user = player;
    if (player.logStatus == -1) {
        endwin();
        return 0;
    }

    renderGame();

    player.logStatus = -1;
    writeUserInfo(player);
    saveDungeon(player.username);

    printf("Press Enter to exit. \n");
    getchar();
    return 0;
}
