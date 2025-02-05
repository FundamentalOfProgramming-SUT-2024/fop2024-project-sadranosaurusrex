#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include "map_plot.h"
#include "menu.h"
#include "UserFileCreator.h"
#include "main.h"
#include <locale.h>
#include <unistd.h>

#define MAP_HEIGHT 30
#define MAP_WIDTH 80
#define MAX_HEALTH 100

int heroColor = 6;

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

    newHero.weaponIndex = 0;
    newHero.spellIndex = 0;
    newHero.currentSpell = -1;
    newHero.currentWeapon = 20;
    newHero.weapon[20] = 0;

    return newHero;
}

user_data userFinder(char* username) {
    user_data user;
    strcpy(user.username, username);
    user = readUserInfo(user);
    return user;
} 

hero myhero;
gameInfo mygame;
int messageIndex = -1;

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
            newuser = readUserInfo(newuser);
            writeUserInfo(newuser);
            newuser.logStatus = 1;
            loadDungeon(newuser.username);
            myhero = createHero();
        }
        else if (choice == 1) {
            //userfile will be deleted and created again
            newuser = newPlayerCreation(usernameext, passwordext);
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

void newGame() {
    mygame.DifficultyLevel = 0;
    mygame.heroColor = 3;
    generateSpell();
    generateTrap();
    generateFood();
    generateGold();
    generateWeapon();
}

void onlySomeFeet (int feet, int floor) {
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_BLUE, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_CYAN, COLOR_BLACK);
    
    clear();
    for (int i = myhero.x -feet; i <= myhero.x +feet; i++) {
        for (int j = myhero.y -feet; j <= myhero.y +feet; j++) {
            if (i > 0 && i <= MAP_HEIGHT *3 && j > 0 && j <= MAP_WIDTH*2) {
                if (dungeon[floor][j][i] == '.') attron(COLOR_PAIR(7));
                mvprintw(j, i, "%c", dungeon[floor][j][i]);
                if (dungeon[floor][j][i] == '.') attroff(COLOR_PAIR(7));
            }
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
    
    strcpy(mygame.messages[++messageIndex], "You've found the door to the next floor!\n");
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

    strcpy(mygame.messages[++messageIndex], "You've found the door to the previous floor!\n");
    return myhero.floor;
}

void rerank() {    
    char users[256][50];
    int scores[256];
    int i = 0;
    int existance = 0;
    
    while (board[i][0] != '\0') {
        char* token1 = strtok(board[i], ".");
        char* token2 = strtok(NULL, ":");
        int token3 = atoi(strtok(NULL, "\0"));

        if (!strcmp(myhero.user.username, token2)) {
            token3 = myhero.user.score;
            existance = 1;
            myhero.user.rank = atoi(token1);
        }

        strcpy(users[i], token2);
        scores[i] = token3;
        i++;
    }

    if (!existance) {
        snprintf(board[i], sizeof(board[i]), "%d.%s:%d", i +1, myhero.user.username, myhero.user.score);
        strcpy(users[i], myhero.user.username);
        scores[i] = myhero.user.score;
        i++;
        myhero.user.rank = i;
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

                if (myhero.user.rank == j +1) myhero.user.rank = k +1;
                else if(myhero.user.rank == k +1) myhero.user.rank = j +1;
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

void supplements() {

}


void options(int choice) {
    loadBoard();
    switch (choice)
    {
    case 0:
        supplements();
        return;
    
    case 1: {
        int weapon = weaponDisplayer();
        if (weapon = -1) break;
        myhero.currentWeapon = weapon;
        break;
    }

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
    case 4: {
        int difficulty = Difficulty();
        break;
    }

    case 5:
        myhero.heroSymbol = heroCharacters();
        break;

    case 6:
        heroColor = 1 +HeroColors();
        break;

    case 7: {
        int spell;
    }

    default:
        break;
    }
}

void displayMessages() {
    screen_setup();

    while (1) {
        clear();
        bunny();

        for (int i = messageIndex, j = 0; i > -1 && messageIndex -i < MAP_HEIGHT; i--, j++) {
            mvprintw(j, 0, "%d. %s", i +1, mygame.messages[i]);
        }

        char c = getch();
        refresh();
        if (c == 'q' || c == 'm') {
            break;
        }
    }

    endwin();
}

int spellDetector() {
    for (int i = 0; i < 20; i++) {
        if (mygame.spell[i].x == myhero.x && mygame.spell[i].y == myhero.y && mygame.spell[i].floor == myhero.floor) {
            return i;
        }
    }
    return -1;
}

int trapDetector() {
    for (int i = 0; i < 20; i++) {
        if (mygame.traps[i].x == myhero.x && mygame.traps[i].y == myhero.y && mygame.traps[i].floor == myhero.floor) {
            return i;
        }
    }
    return -1;
}

int foodDetector() {
    for (int i = 0; i < 20; i++) {
        if (mygame.food[i].x == myhero.x && mygame.food[i].y == myhero.y && mygame.food[i].floor == myhero.floor) {
            return i;
        }
    }
    return -1;
}

int goldDetector() {
    for (int i = 0; i < 20; i++) {
        if (mygame.gold[i].x == myhero.x && mygame.gold[i].y == myhero.y && mygame.gold[i].floor == myhero.floor) {
            return i;
        }
    }
    return -1;
}

int weaponDetector() {
    for (int i = 0; i < 20; i++) {
        if (mygame.weapon[i].x == myhero.x && mygame.weapon[i].y == myhero.y && mygame.weapon[i].floor == myhero.floor) {
            return i;
        }
    }
    return -1;
}

void renderGame() {
    initscr();
    noecho();
    curs_set(FALSE);

    int currentFloor = myhero.floor;
    int displayStatus = -1;
    int speed = -1;

    while (1) {
        if (displayStatus == 1) displayFloor(currentFloor);
        else onlySomeFeet(5, currentFloor);
        bunny();
        if (myhero.floor == currentFloor) movementHandler(0, 0);
        else mvprintw(myhero.y, myhero.x, "%c", dungeon[currentFloor][myhero.y][myhero.x]);
        
        if (dungeon[myhero.floor][myhero.y][myhero.x] == 'U') {
            currentFloor = U();
        }
        else if (dungeon[myhero.floor][myhero.y][myhero.x] == 'D') {
            currentFloor = D();
        }
        else if (dungeon[myhero.floor][myhero.y][myhero.x] == 'W') {
            strcpy(mygame.messages[++messageIndex], "You won!\n");
            break;
        }

        int index = spellDetector();
        if (index != -1) {
            int c = getch();
            if (c == 'g') {
                mygame.spell[index].visiblity = 1;
                dungeon[myhero.floor][myhero.y][myhero.x] = '.';
                strcpy(mygame.messages[++messageIndex], "You've grabbed an enchant!\n");
                myhero.spell[myhero.spellIndex++] = index;
            }
        }
        index = trapDetector();
        if (index != -1) {
            if (1) {
                //mygame.traps[index].visiblity = 1;
                dungeon[myhero.floor][myhero.y][myhero.x] = '^';
                strcpy(mygame.messages[++messageIndex], "You've fallen in a trap!\n");
            }
        }
        index = foodDetector();
        if (index != -1) {
            int c = getch();
            if (c == 'g') {
                mygame.food[index].visiblity = 1;
                dungeon[myhero.floor][myhero.y][myhero.x] = '.';
                strcpy(mygame.messages[++messageIndex], "You consumed food!\n");
            }
        }
        index = goldDetector();
        if (index != -1) {
            int c = getch();
            if (c == 'g') {
                mygame.food[index].visiblity = 1;
                dungeon[myhero.floor][myhero.y][myhero.x] = '.';
                myhero.user.gold += 10*mygame.gold[index].type +10;
                myhero.user.score += 10*mygame.gold[index].type +10;
                strcpy(mygame.messages[++messageIndex], "You've grabbed a gold!\n");
                writeUserInfo(myhero.user);
            }
        }
        index = weaponDetector();
        if (index != -1) {
            int c = getch();
            if (c == 'g') {
                mygame.food[index].visiblity = 1;
                dungeon[myhero.floor][myhero.y][myhero.x] = '.';
                strcpy(mygame.messages[++messageIndex], "You've grabbed a weapon!\n");
                myhero.weapon[myhero.weaponIndex++] = index;
            }
        }

        int ch = getch();
        if (ch == 'v') speed *= -1;
        else if (ch == 'y' && speed == 1) movementHandler(-2, -2); else if (ch == 'y') movementHandler(-1, -1);
        else if (ch == 'u' && speed == 1) movementHandler(-2, 2); else if (ch == 'u') movementHandler(-1, 1);
        else if (ch == 'h' && speed == 1) movementHandler(0, -2); else if (ch == 'h') movementHandler(0, -1);
        else if (ch == 'j' && speed == 1) movementHandler(-2, 0); else if (ch == 'j') movementHandler(-1, 0);
        else if (ch == 'k' && speed == 1) movementHandler(2, 0); else if (ch == 'k') movementHandler(1, 0);
        else if (ch == 'l' && speed == 1) movementHandler(0, 2); else if (ch == 'l') movementHandler(0, 1);
        else if (ch == 'b' && speed == 1) movementHandler(2, -2); else if (ch == 'b') movementHandler(1, -1);
        else if (ch == 'n' && speed == 1) movementHandler(2, 2); else if (ch == 'n') movementHandler(1, 1);
        else if (ch == 'q') break;
        else if (ch == 's' && currentFloor > 0) currentFloor--;
        else if (ch == 'w' && currentFloor < FLOORS - 1) currentFloor++;
        else if (ch == 'a') displayStatus *= -1;
        else if (ch == 'p') {
            options(settingMenu());
        } else if (ch == 'm') {
            displayMessages();
        } else if (ch == 'e') {}
    }

    endwin();
}

void movementHandler(int j, int i) {
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_BLUE, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_CYAN, COLOR_BLACK);

    bunny();
    int desx = myhero.x +i;
    int desy = myhero.y +j;
    if (desx < 1 || desy < 1 || desx > MAP_WIDTH || desy > MAP_HEIGHT 
        || dungeon[myhero.floor][desy][desx] == '|' || dungeon[myhero.floor][desy][desx] == '_'
        || dungeon[myhero.floor][desy][desx] == ' ') return;
    else {
        myhero.x += i;
        myhero.y += j;
        attron(COLOR_PAIR(heroColor));
        attron(A_REVERSE);
        mvprintw(myhero.y, myhero.x, "%c", myhero.heroSymbol);
        attroff(COLOR_PAIR(heroColor));
        attroff(A_REVERSE);
    }
    return;
}

int main() {
    setlocale(LC_ALL, "");
    start_color(); 

    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_BLUE, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_CYAN, COLOR_BLACK);

    user_data player = setupLogin();
    myhero.user = player;
    if (myhero.user.logStatus == -1) {
        endwin();
        return 0;
    }

    time_t start_time = time(NULL);

    loadBoard();
    newGame();
    renderGame();

    time_t end_time = time(NULL);
    
    rerank();
    boardSaver();
    myhero.user.logStatus = -1;
    myhero.user.count_games++;
    myhero.user.experience += (int)difftime(end_time, start_time);
    writeUserInfo(myhero.user);
    saveDungeon(myhero.user.username);

    printf("Press Enter to exit. \n");
    getchar();
    return 0;
}
