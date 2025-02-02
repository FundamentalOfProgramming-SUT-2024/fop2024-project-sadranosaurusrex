#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include "map_plot.h"
#include "menu.h"
#include "UserFileCreator.h"

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
        else if (ch == 'w' && currentFloor > 0) currentFloor--;
        else if (ch == 's' && currentFloor < FLOORS - 1) currentFloor++;
        else if (ch == 'a') displayStatus *= -1;
        else if (ch == 'z') ;
    }

    endwin();
}

void movementHandler(int j, int i) {
    bunny();
    int desx = myhero.x +i;
    int desy = myhero.y +j;
    if (desx < 1 || desy < 1 || desx > MAP_WIDTH || desy > MAP_HEIGHT || dungeon[myhero.floor][desy][desx] != '.') return;
    myhero.x += i;
    myhero.y += j;
    mvprintw(myhero.y, myhero.x, "%c", myhero.heroSymbol);
    return;
}

int main() {
    user_data player = setupLogin();
    if (player.logStatus == -1) {
        endwin();
        return 0;
    }

    renderGame();

    player.logStatus = -1;
    writeUserInfo(player);
    saveDungeon(player.username);

    printf("Press any key to exit. \n");
    getchar();
    return 0;
}
