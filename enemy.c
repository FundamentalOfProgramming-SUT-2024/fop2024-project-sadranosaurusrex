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

#define FLOORS 4
#define MAP_HEIGHT 30
#define MAP_WIDTH 80

enemy enemies[10];

void snakeGenerator()
{
    srand(time(NULL));
    for (int i = 0; i < 2; i++)
    {
        while (1)
        {
            int randomX = MAP_WIDTH / 2 + rand() % (MAP_WIDTH / 2);
            int randomY = rand() % (MAP_HEIGHT);
            int randomF = rand() % (FLOORS);

            if (dungeon[randomF][randomY][randomX] == '.')
            {
                enemies[i].floor = randomF;
                enemies[i].x = randomX;
                enemies[i].y = randomY;
                dungeon[randomF][randomY][randomX] = 'S';
                break;
            }
        }

        enemies[i].duration = -1;
        enemies[i].health = 20;
        enemies[i].status = -1;
        enemies[i].symbol = 'S';
        enemies[i].followingSince = -1;
    }
}

void monsterGenerator()
{
    srand(time(NULL));
    for (int i = 2; i < 4; i++)
    {
        while (1)
        {
            int randomX = MAP_WIDTH / 2 + rand() % (MAP_WIDTH / 2);
            int randomY = rand() % (MAP_HEIGHT);
            int randomF = rand() % (FLOORS);

            if (dungeon[randomF][randomY][randomX] == '.')
            {
                enemies[i].floor = randomF;
                enemies[i].x = randomX;
                enemies[i].y = randomY;
                dungeon[randomF][randomY][randomX] = 'F';
                break;
            }
        }

        enemies[i].duration = 45;
        enemies[i].health = 10;
        enemies[i].status = -1;
        enemies[i].symbol = 'F';
        enemies[i].followingSince = -1;
    }
}

void demonGenerator()
{
    srand(time(NULL));
    for (int i = 4; i < 6; i++)
    {
        while (1)
        {
            int randomX = MAP_WIDTH / 2 + rand() % (MAP_WIDTH / 2);
            int randomY = rand() % (MAP_HEIGHT);
            int randomF = rand() % (FLOORS);

            if (dungeon[randomF][randomY][randomX] == '.')
            {
                enemies[i].floor = randomF;
                enemies[i].x = randomX;
                enemies[i].y = randomY;
                dungeon[randomF][randomY][randomX] = 'd';
                break;
            }
        }

        enemies[i].duration = 45;
        enemies[i].health = 5;
        enemies[i].status = -1;
        enemies[i].symbol = 'd';
        enemies[i].followingSince = -1;
    }
}

void giantGenerator()
{
    srand(time(NULL));
    for (int i = 6; i < 8; i++)
    {
        while (1)
        {
            int randomX = MAP_WIDTH / 2 + rand() % (MAP_WIDTH / 2);
            int randomY = rand() % (MAP_HEIGHT);
            int randomF = rand() % (FLOORS);

            if (dungeon[randomF][randomY][randomX] == '.')
            {
                enemies[i].floor = randomF;
                enemies[i].x = randomX;
                enemies[i].y = randomY;
                dungeon[randomF][randomY][randomX] = 'G';
                break;
            }
        }

        enemies[i].duration = 90;
        enemies[i].health = 15;
        enemies[i].status = -1;
        enemies[i].symbol = 'G';
        enemies[i].followingSince = -1;
    }
}

void undeedGenerator()
{
    srand(time(NULL));
    for (int i = 8; i < 10; i++)
    {
        while (1)
        {
            int randomX = MAP_WIDTH / 2 + rand() % (MAP_WIDTH / 2);
            int randomY = rand() % (MAP_HEIGHT);
            int randomF = rand() % (FLOORS - 1) + 1;

            if (dungeon[randomF][randomY][randomX] == '.')
            {
                enemies[i].floor = randomF;
                enemies[i].x = randomX;
                enemies[i].y = randomY;
                dungeon[randomF][randomY][randomX] = 'u';
                break;
            }
        }

        enemies[i].duration = 90;
        enemies[i].health = 30;
        enemies[i].status = -1;
        enemies[i].symbol = 'u';
        enemies[i].followingSince = -1;
    }
}

void enemyMovement() {
    srand(time(NULL));
    for (int i = 0; i < 10; i++) {
        dungeon[enemies[i].floor][enemies[i].y][enemies[i].x] = '.';

        if (enemies[i].status == 1) {
            if (enemies[i].x < myhero.x) {
                enemies[i].x++;
                if (dungeon[enemies[i].floor][enemies[i].y][enemies[i].x] != '.') enemies[i].x--;
            }
            else if (enemies[i].x > myhero.x) {
                enemies[i].x--;
                if (dungeon[enemies[i].floor][enemies[i].y][enemies[i].x] != '.') enemies[i].x++;
            }
            if (enemies[i].y < myhero.y) {
                enemies[i].y++;
                if (dungeon[enemies[i].floor][enemies[i].y][enemies[i].x] != '.') enemies[i].y--;
            }
            else if (enemies[i].y > myhero.y ) {
                enemies[i].y--;
                if (dungeon[enemies[i].floor][enemies[i].y][enemies[i].x] != '.') enemies[i].y++;
            }
        }
        else {
            int randX = rand()%3 -1;
            int randY = rand()%3 -1;
            enemies[i].x += randX;
            if (dungeon[enemies[i].floor][enemies[i].y][enemies[i].x] != '.') enemies[i].x -= randX;
            enemies[i].y += randY;
            if (dungeon[enemies[i].floor][enemies[i].y][enemies[i].x] != '.') enemies[i].y -= randY;
        }

        dungeon[enemies[i].floor][enemies[i].y][enemies[i].x] = enemies[i].symbol;
    }
}

void followStatus() {
    for (int i = 0; i < 2; i++) { //for snakes
        if ((myhero.x -enemies[i].x < 21 && myhero.x -enemies[i].x > -21) 
        || (myhero.y -enemies[i].y < 21 && myhero.y -enemies[i].y > -21)) {
            enemies[i].status = 1;
        }
        if (myhero.floor != enemies[i].floor || enemies[i].health < 1) {
            enemies[i].status = -1;
        }
    }
    for (int i = 2; i < 10; i++) { //for the rest of enemies
        if ((myhero.x -enemies[i].x < 21 && myhero.x -enemies[i].x > -21) 
        || (myhero.y -enemies[i].y < 21 && myhero.y -enemies[i].y > -21)) {
            enemies[i].status = 1;
            if (enemies[i].followingSince <= 0) {
                enemies[i].followingSince = (int)time(NULL);
            }
        }
        if ((int)time(NULL) -enemies[i].followingSince > enemies[i].duration
        || enemies[i].health < 1 || myhero.floor != enemies[i].floor) {
            enemies[i].status = -1;
            enemies[i].followingSince = -1;
        }
    }
}
