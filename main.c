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
#include "enemy.h"

#define MAP_HEIGHT 30
#define MAP_WIDTH 80
#define MAX_HEALTH 150
#define DELAY 10000
#define ENEMY_TURN 45

int heroColor = 6;
int inTrap = 1;
int speed = -1;
int damageBoost = 1;

hero createHero()
{
    hero newHero;
    newHero.floor = 0;
    newHero.health = 2 * MAX_HEALTH / 3;
    newHero.heroSymbol = '0';

    srand(time(NULL));

    while (1)
    {
        int randomX = rand() % MAP_WIDTH / 3;
        int randomY = rand() % MAP_HEIGHT / 2;

        if (dungeon[newHero.floor][randomY][randomX] == '.')
        {
            newHero.x = randomX;
            newHero.y = randomY;
            break;
        }
    }

    newHero.weaponIndex = 0;
    newHero.spellIndex = 0;
    newHero.currentSpell = -1;
    newHero.currentWeapon = -1;

    return newHero;
}

user_data userFinder(char *username)
{
    user_data user;
    strcpy(user.username, username);
    user = readUserInfo(user);
    return user;
}

hero myhero;
gameInfo mygame;
int messageIndex = -1;

user_data newPlayerCreation(char username[], char password[])
{
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

user_data setupLogin()
{
    user_data newuser;
    int menu_value = menu();
    while (menu_value == -1)
        menu_value = menu();
    if (menu_value == 3)
    {
        newuser.logStatus = -1;
        return newuser;
    }
    else if (menu_value == 0)
    {
        int choice = NewGameChoices();
        if (choice == 0)
        {
            // userfile will be read
            newuser = newPlayerCreation(usernameext, passwordext);
            newuser = readUserInfo(newuser);
            writeUserInfo(newuser);
            newuser.logStatus = 1;
            loadDungeon(newuser.username);
            myhero = createHero();
        }
        else if (choice == 1)
        {
            // userfile will be deleted and created again
            newuser = newPlayerCreation(usernameext, passwordext);
            newuser.logStatus = 1;
            map_generator();
            myhero = createHero();
        }
    }
    else if (menu_value == 1)
    {
        newuser = newPlayerCreation(usernameext, passwordext);
    }
    else if (menu_value == 2)
    {
        newuser = newPlayerCreation("Guest", "Guest");
        map_generator();
        myhero = createHero();
    }
    return newuser;
}

void newGame()
{
    mygame.DifficultyLevel = 2;
    mygame.heroColor = 3;
    generateSpell();
    generateTrap();
    generateFood();
    generateGold();
    generateWeapon();
}

void onlySomeFeet(int feet, int floor)
{
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_BLUE, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_CYAN, COLOR_BLACK);

    clear();
    for (int i = myhero.x - feet; i <= myhero.x + feet; i++)
    {
        for (int j = myhero.y - feet; j <= myhero.y + feet; j++)
        {
            if (i > 0 && i <= MAP_HEIGHT * 3 && j > 0 && j <= MAP_WIDTH * 2)
            {
                if (dungeon[floor][j][i] == '.')
                    attron(COLOR_PAIR(7));
                mvprintw(j, i, "%c", dungeon[floor][j][i]);
                if (dungeon[floor][j][i] == '.')
                    attroff(COLOR_PAIR(7));
            }
        }
    }
    movementHandler(0, 0);
}

int U()
{
    myhero.floor++;
    srand(time(NULL));

    while (1)
    {
        int randomX = rand() % MAP_WIDTH / 3;
        int randomY = rand() % MAP_HEIGHT / 2;

        if (dungeon[myhero.floor][randomY][randomX] == '.')
        {
            myhero.x = randomX;
            myhero.y = randomY;
            break;
        }
    }

    strcpy(mygame.messages[++messageIndex], "You've found the door to the next floor!\n");
    return myhero.floor;
}

int D()
{
    myhero.floor--;
    srand(time(NULL));

    while (1)
    {
        int randomX = rand() % MAP_WIDTH / 3;
        int randomY = MAP_HEIGHT - rand() % MAP_HEIGHT / 2;

        if (dungeon[myhero.floor][randomY][randomX] == '.')
        {
            myhero.x = randomX;
            myhero.y = randomY;
            break;
        }
    }

    strcpy(mygame.messages[++messageIndex], "You've found the door to the previous floor!\n");
    return myhero.floor;
}

void rerank()
{
    char users[256][50];
    int scores[256];
    int i = 0;
    int existance = 0;

    while (board[i][0] != '\0')
    {
        char *token1 = strtok(board[i], ".");
        char *token2 = strtok(NULL, ":");
        int token3 = atoi(strtok(NULL, "\0"));

        if (!strcmp(myhero.user.username, token2))
        {
            token3 = myhero.user.score;
            existance = 1;
            myhero.user.rank = atoi(token1);
        }

        strcpy(users[i], token2);
        scores[i] = token3;
        i++;
    }

    if (!existance)
    {
        snprintf(board[i], sizeof(board[i]), "%d.%s:%d", i + 1, myhero.user.username, myhero.user.score);
        strcpy(users[i], myhero.user.username);
        scores[i] = myhero.user.score;
        i++;
        myhero.user.rank = i;
    }

    for (int j = 0; j < i; j++)
    {
        for (int k = j + 1; k < i; k++)
        {
            if (scores[j] < scores[k])
            {
                int temp = scores[j];
                scores[j] = scores[k];
                scores[k] = temp;

                char tempstr[50];
                strcpy(tempstr, users[j]);
                strcpy(users[j], users[k]);
                strcpy(users[k], tempstr);

                if (myhero.user.rank == j + 1)
                    myhero.user.rank = k + 1;
                else if (myhero.user.rank == k + 1)
                    myhero.user.rank = j + 1;
            }
        }
    }

    for (int j = 0; j < i; j++)
    {
        snprintf(board[j], sizeof(board[j]), "%d.%s:%d", j + 1, users[j], scores[j]);
    }

    return board;
}

int damages(int weaponType)
{
    if (weaponType == 0)
        return 5;
    else if (weaponType == 1)
        return 12;
    else if (weaponType == 2)
        return 15;
    else if (weaponType == 3)
        return 5;
    else if (weaponType == 4)
        return 10;
    else
        return 0;
}

int rangeClaculator(int weaponType)
{
    if (weaponType == 1)
        return 5;
    else if (weaponType == 2)
        return 10;
    else if (weaponType == 3)
        return 5;
}

void hitDamage(int i, int j, int k, int damage)
{
    if (enemies[k].x == i && enemies[k].y == j && enemies[k].status != 0) {
        enemies[k].health -= damage; 
    }
    if (enemies[k].status != 0 && enemies[k].health <= 0)
    {
        enemies[k].status = 0;
        dungeon[myhero.floor][j][i] = '.';
        strcpy(mygame.messages[++messageIndex], "You have eliminated an enemy!\n");
        myhero.user.score += 50;
        writeUserInfo(myhero.user);
        rerank();
    }
}

void hit()
{
    int type;
    if (myhero.currentWeapon == -1)
        type = 0;
    else
        type = mygame.weapon[myhero.weapon[myhero.currentWeapon]].type;
    int damage = damages(type) * damageBoost;
    damageBoost = 1;

    if (type == 0 || type == 4)
    {
        for (int i = myhero.x - 1; i < myhero.x + 2; i++)
        {   
            for (int j = myhero.y - 1; j < myhero.y + 2; j++)
            {
                if (i < 0 || j < 0)
                    continue;
                for (int k = 0; k < 10; k++)
                {   
                    hitDamage(i, j, k, damage);
                }
            }
        }
    }

    else
    {
        int c = getch();
        // while (getch())
        // {   
            int exitloop = 0;
            if (c == 'y' || c == '7')
            {
                for (int i = myhero.x; i >= myhero.x -rangeClaculator(type); i--)
                {
                    for (int j = myhero.y; j >= myhero.y -rangeClaculator(type); j--)
                    {   
                        if (dungeon[myhero.floor][j][i] == '_' || dungeon[myhero.floor][j][i] == '|' 
                        || i < 0 || j < 0) {
                            exitloop = 1;
                            break;
                        }
                        for (int k = 0; k < 10; k++) hitDamage(i, j, k, damage);
                    }
                    if (exitloop) break;
                }
            }
            if (c == 'u' || c == '9')
            {
                for (int i = myhero.x; i <= myhero.x +rangeClaculator(type); i++)
                {
                    for (int j = myhero.y; j >= myhero.y -rangeClaculator(type); j--)
                    {   
                        if (dungeon[myhero.floor][j][i] == '_' || dungeon[myhero.floor][j][i] == '|') {
                            exitloop = 1;
                            break;
                        }
                        for (int k = 0; k < 10; k++) hitDamage(i, j, k, damage);
                    }
                    if (exitloop) break;
                }
            }
            if (c == 'h' || c == '4')
            {
                for (int i = myhero.x; i >= myhero.x -rangeClaculator(type); i--)
                {
                    for (int j = myhero.y; j == myhero.y; j++)
                    {   
                        if (dungeon[myhero.floor][j][i] == '_' || dungeon[myhero.floor][j][i] == '|') {
                            exitloop = 1;
                            break;
                        }
                        for (int k = 0; k < 10; k++) hitDamage(i, j, k, damage);
                    }
                    if (exitloop) break;
                }
            }
            if (c == 'k' || c == '2')
            {
                for (int i = myhero.x; i == myhero.x; i--)
                {
                    for (int j = myhero.y; j <= myhero.y +rangeClaculator(type); j++)
                    {   
                        if (dungeon[myhero.floor][j][i] == '_' || dungeon[myhero.floor][j][i] == '|') {
                            exitloop = 1;
                            break;
                        }
                        for (int k = 0; k < 10; k++) hitDamage(i, j, k, damage);
                    }
                    if (exitloop) break;
                }
            }
            if (c == 'j' || c == '8')
            {
                for (int i = myhero.x; i == myhero.x; i--)
                {
                    for (int j = myhero.y; j >= myhero.y -rangeClaculator(type); j--)
                    {   
                        if (dungeon[myhero.floor][j][i] == '_' || dungeon[myhero.floor][j][i] == '|') {
                            exitloop = 1;
                            break;
                        }
                        for (int k = 0; k < 10; k++) hitDamage(i, j, k, damage);
                    }
                    if (exitloop) break;
                }
            }
            if (c == 'l' || c == '6')
            {
                for (int i = myhero.x; i <= myhero.x +rangeClaculator(type); i++)
                {
                    for (int j = myhero.y; j >= myhero.y; j--)
                    {   
                        if (dungeon[myhero.floor][j][i] == '_' || dungeon[myhero.floor][j][i] == '|') {
                            exitloop = 1;
                            break;
                        }
                        for (int k = 0; k < 10; k++) hitDamage(i, j, k, damage);
                    }
                    if (exitloop) break;
                }
            }
            if (c == 'b' || c == '1')
            {
                for (int i = myhero.x; i >= myhero.x -rangeClaculator(type); i--)
                {
                    for (int j = myhero.y; j <= myhero.y + rangeClaculator(type); j++)
                    {   
                        if (dungeon[myhero.floor][j][i] == '_' || dungeon[myhero.floor][j][i] == '|') {
                            exitloop = 1;
                            break;
                        }
                        for (int k = 0; k < 10; k++) hitDamage(i, j, k, damage);
                    }
                    if (exitloop) break;
                }
            }
            if (c == 'n' || c == '3')
            {
                for (int i = myhero.x; i <= myhero.x +rangeClaculator(type); i++)
                {
                    for (int j = myhero.y; j <= myhero.y + rangeClaculator(type); j++)
                    {   
                        if (dungeon[myhero.floor][j][i] == '_' || dungeon[myhero.floor][j][i] == '|') {
                            exitloop = 1;
                            break;
                        }
                        for (int k = 0; k < 10; k++) hitDamage(i, j, k, damage);
                    }
                    if (exitloop) break;
                }
            }
        // }
    }
}

void options(int choice)
{
    loadBoard();
    switch (choice)
    {
    case 0:
        return;

    case 1:
    {
        int weapon = weaponDisplayer();
        if (weapon = -1)
            break;
        myhero.currentWeapon = weapon;
        break;
    }

    case 2:
    {
        rerank();
        boardDisplayer();
        boardSaver();
        break;
    }
    case 3:
    {
        rerank();
        boardSaver();
        break;
    }
    case 4:
    {
        int difficulty = Difficulty();
        mygame.DifficultyLevel = difficulty;
        break;
    }

    case 5:
        myhero.heroSymbol = heroCharacters();
        break;

    case 6:
        heroColor = 1 + HeroColors();
        break;

    case 7:
    {
        int spell = spellDisplayer();
        if (spell == -1)
            break;
        myhero.currentSpell = spell;
        if (mygame.spell[myhero.spell[spell]].type == 1)
            speed = 1;
        else if (mygame.spell[myhero.spell[spell]].type == 2)
            damageBoost = 10;
        break;
    }

    default:
        break;
    }
}

void displayMessages()
{
    screen_setup();

    while (1)
    {
        clear();
        bunny();

        for (int i = messageIndex, j = 0; i > -1 && messageIndex - i < MAP_HEIGHT; i--, j++)
        {
            mvprintw(j, 0, "%d. %s", i + 1, mygame.messages[i]);
        }

        char c = getch();
        refresh();
        if (c == 'q' || c == 'm')
        {
            break;
        }
    }

    endwin();
}

int spellDetector()
{
    for (int i = 0; i < 20; i++)
    {
        if (mygame.spell[i].x == myhero.x && mygame.spell[i].y == myhero.y && mygame.spell[i].floor == myhero.floor && mygame.spell[i].visiblity == -1)
        {
            return i;
        }
    }
    return -1;
}

int trapDetector()
{
    for (int i = 0; i < 20; i++)
    {
        if (mygame.traps[i].x == myhero.x && mygame.traps[i].y == myhero.y && mygame.traps[i].floor == myhero.floor)
        {
            return i;
        }
    }
    return -1;
}

int foodDetector()
{
    for (int i = 0; i < 20; i++)
    {
        if (mygame.food[i].x == myhero.x && mygame.food[i].y == myhero.y && mygame.food[i].floor == myhero.floor && mygame.food[i].visiblity == -1)
        {
            return i;
        }
    }
    return -1;
}

int goldDetector()
{
    for (int i = 0; i < 20; i++)
    {
        if (mygame.gold[i].x == myhero.x && mygame.gold[i].y == myhero.y && mygame.gold[i].floor == myhero.floor && mygame.gold[i].visiblity == -1)
        {
            return i;
        }
    }
    return -1;
}

int weaponDetector()
{
    for (int i = 0; i < 20; i++)
    {
        if (mygame.weapon[i].x == myhero.x && mygame.weapon[i].y == myhero.y && mygame.weapon[i].floor == myhero.floor && mygame.weapon[i].visiblity < 0)
        {
            return i;
        }
    }
    return -1;
}

void renderGame()
{
    initscr();
    noecho();
    curs_set(FALSE);
    nodelay(stdscr, TRUE);

    int currentFloor = myhero.floor;
    int displayStatus = 1;
    int enemyTurn = 0;

    while (1)
    {
        followStatus();
        enemyTurn = (enemyTurn + 1) % (ENEMY_TURN / (3 - mygame.DifficultyLevel));
        if (!enemyTurn)
            enemyMovement();

        if (displayStatus == 1)
            displayFloor(currentFloor);
        else
            onlySomeFeet(5, currentFloor);
        bunny();
        if (myhero.floor == currentFloor)
            movementHandler(0, 0);
        else
            mvprintw(myhero.y, myhero.x, "%c", dungeon[currentFloor][myhero.y][myhero.x]);

        if (dungeon[myhero.floor][myhero.y][myhero.x] == 'U')
        {
            currentFloor = U();
        }
        else if (dungeon[myhero.floor][myhero.y][myhero.x] == 'D')
        {
            currentFloor = D();
        }
        else if (dungeon[myhero.floor][myhero.y][myhero.x] == 'W')
        {
            strcpy(mygame.messages[++messageIndex], "You won!\n");
            printf("You won soldier!\n");
            myhero.user.score += 100;
            break;
        }

        int index = spellDetector();
        if (index != -1)
        {
            // int c = getch();
            if (1)
            {
                mygame.spell[index].visiblity = 1;
                dungeon[myhero.floor][myhero.y][myhero.x] = '.';
                strcpy(mygame.messages[++messageIndex], "You've grabbed an enchant!\n");
                myhero.spell[myhero.spellIndex++] = index;
            }
            else
                continue;
        }
        index = trapDetector();
        if (index != -1 && inTrap == 1)
        {
            inTrap = -1;
            dungeon[myhero.floor][myhero.y][myhero.x] = '^';
            strcpy(mygame.messages[++messageIndex], "You've fallen in a trap!\n");
        }
        index = foodDetector();
        if (index != -1)
        {
            // int c = getch();
            if (1)
            {
                mygame.food[index].visiblity = 1;
                dungeon[myhero.floor][myhero.y][myhero.x] = '.';
                int rate = (mygame.food[index].type * 10 + 5);
                if (myhero.currentSpell != -1 && mygame.spell[myhero.spell[myhero.currentSpell]].type == 0 && mygame.spell[myhero.spell[myhero.currentSpell]].visiblity != 0)
                {
                    rate *= 2;
                    mygame.spell[myhero.spell[myhero.currentSpell]].visiblity = 0;
                }
                myhero.health += rate;
                if (myhero.health > MAX_HEALTH)
                    myhero.health = MAX_HEALTH;
                char tempmessage[256];
                snprintf(tempmessage, sizeof(tempmessage),
                         "You consumed food! Now your health is %d/%d\n", myhero.health, MAX_HEALTH);
                strcpy(mygame.messages[++messageIndex], tempmessage);
            }
            else
                continue;
        }
        index = goldDetector();
        if (index != -1)
        {
            // int c = getch();
            if (1)
            {
                mygame.gold[index].visiblity = 1;
                dungeon[myhero.floor][myhero.y][myhero.x] = '.';
                myhero.user.gold += 10 * mygame.gold[index].type + 10;
                myhero.user.score += 10 * mygame.gold[index].type + 10;
                char tempmessage[256];
                snprintf(tempmessage, sizeof(tempmessage), "You've grabbed a gold! (Gold supply: %d)\n", myhero.user.gold);
                strcpy(mygame.messages[++messageIndex], tempmessage);
                writeUserInfo(myhero.user);
            }
            else
                continue;
        }
        index = weaponDetector();
        if (index != -1)
        {
            // int c = getch();
            if (1)
            {
                mygame.weapon[index].visiblity *= -1;
                dungeon[myhero.floor][myhero.y][myhero.x] = '.';
                strcpy(mygame.messages[++messageIndex], "You've grabbed a weapon!\n");
                myhero.weapon[myhero.weaponIndex++] = index;
            }
            else
                continue;
        }

        int ch = getch();
        if (ch == 'v')
            speed *= -1;
        else if (ch == 'y' && speed == 1)
            movementHandler(-2, -2);
        else if (ch == 'y')
            movementHandler(-1, -1);
        else if (ch == 'u' && speed == 1)
            movementHandler(-2, 2);
        else if (ch == 'u')
            movementHandler(-1, 1);
        else if (ch == 'h' && speed == 1)
            movementHandler(0, -2);
        else if (ch == 'h')
            movementHandler(0, -1);
        else if (ch == 'j' && speed == 1)
            movementHandler(-2, 0);
        else if (ch == 'j')
            movementHandler(-1, 0);
        else if (ch == 'k' && speed == 1)
            movementHandler(2, 0);
        else if (ch == 'k')
            movementHandler(1, 0);
        else if (ch == 'l' && speed == 1)
            movementHandler(0, 2);
        else if (ch == 'l')
            movementHandler(0, 1);
        else if (ch == 'b' && speed == 1)
            movementHandler(2, -2);
        else if (ch == 'b')
            movementHandler(1, -1);
        else if (ch == 'n' && speed == 1)
            movementHandler(2, 2);
        else if (ch == 'n')
            movementHandler(1, 1);
        else if (ch == 'q')
            break;
        else if (ch == 's' && currentFloor > 0)
            currentFloor--;
        else if (ch == 'w' && currentFloor < FLOORS - 1)
            currentFloor++;
        else if (ch == 'a')
            displayStatus *= -1;
        else if (ch == 'p')
        {
            nodelay(stdscr, FALSE);
            options(settingMenu());
            nodelay(stdscr, TRUE);
        }
        else if (ch == 'm')
        {
            nodelay(stdscr, FALSE);
            displayMessages();
            nodelay(stdscr, TRUE);
        }
        else if (ch == ' ')
        {
            nodelay(stdscr, FALSE);
            //printf("hit\n");
            hit();
            nodelay(stdscr, TRUE);
        }

        usleep(DELAY);
    }

    endwin();
}

void movementHandler(int j, int i)
{
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_BLUE, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_CYAN, COLOR_BLACK);

    if (inTrap == -1 && (i != 0 || j != 0))
        inTrap = 1;
    bunny();

    int desx = myhero.x + i;
    int desy = myhero.y + j;
    if (desx < 1 || desy < 1 || desx > MAP_WIDTH || desy > MAP_HEIGHT || dungeon[myhero.floor][desy][desx] == '|' || dungeon[myhero.floor][desy][desx] == '_' || dungeon[myhero.floor][desy][desx] == ' ')
        return;
    else
    {
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

int main()
{
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
    if (myhero.user.logStatus == -1)
    {
        endwin();
        return 0;
    }

    time_t start_time = time(NULL);

    undeedGenerator();
    demonGenerator();
    monsterGenerator();
    snakeGenerator();
    giantGenerator();

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
