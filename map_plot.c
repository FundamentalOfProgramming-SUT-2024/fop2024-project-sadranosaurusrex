#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include "map_plot.h"
#include "UserFileCreator.h"
#include "main.h"

#define FLOORS 4
#define MAP_HEIGHT 30
#define MAP_WIDTH 80
#define ROOMS 6

typedef struct
{
    int x, y, width, height;
} Room;

char dungeon[FLOORS][MAP_HEIGHT][MAP_WIDTH];

void initializeDungeon()
{
    for (int f = 0; f < FLOORS; f++)
    {
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                dungeon[f][y][x] = ' ';
            }
        }
    }
}

void generateRooms()
{
    srand(time(NULL));

    for (int f = 0; f < FLOORS; f++)
    {
        int placedRooms = 0;
        Room rooms[6];

        while (placedRooms < ROOMS)
        {
            Room room;
            room.width = 4 + rand() % 8;
            room.height = 4 + rand() % 8;

            int regionX = (placedRooms % 3) * (MAP_WIDTH / 3);
            int regionY = (placedRooms / 3) * (MAP_HEIGHT / 2);

            room.x = regionX + 1 + rand() % ((MAP_WIDTH / 3) - room.width - 2);
            room.y = regionY + 1 + rand() % ((MAP_HEIGHT / 2) - room.height - 2);

            if (room.x + room.width < MAP_WIDTH && room.y + room.height < MAP_HEIGHT)
            {
                // Draw room with borders
                for (int y = room.y; y < room.y + room.height; y++)
                {
                    for (int x = room.x; x < room.x + room.width; x++)
                    {
                        dungeon[f][y][x] = '.'; // Room interior
                    }
                }
                rooms[placedRooms] = room;
                placedRooms++;
            }
        }

        // Draw corridors between rooms
        for (int i = 0; i < ROOMS - 1; i++)
        {
            Room room1 = rooms[i];
            Room room2 = rooms[i + 1];

            if (i == 2)
                room2 = rooms[5];

            int centerX1 = room1.x + room1.width / 2;
            int centerY1 = room1.y + room1.height / 2;
            int centerX2 = room2.x + room2.width / 2;
            int centerY2 = room2.y + room2.height / 2;

            // Horizontal corridor
            if (centerX1 < centerX2)
            {
                for (int x = centerX1; x <= centerX2; x++)
                {
                    dungeon[f][centerY1][x] = '.'; // Corridor path
                }
            }
            else
            {
                for (int x = centerX1; x >= centerX2; x--)
                {
                    dungeon[f][centerY1][x] = '.'; // Corridor path
                }
            }

            // Vertical corridor
            if (centerY1 < centerY2)
            {
                for (int y = centerY1; y <= centerY2; y++)
                {
                    dungeon[f][y][centerX2] = '.'; // Corridor path
                }
            }
            else
            {
                for (int y = centerY1; y >= centerY2; y--)
                {
                    dungeon[f][y][centerX2] = '.'; // Corridor path
                }
            }
        }

        // Add borders around all '.' characters
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                if (dungeon[f][y][x] == '.')
                {
                    // Check if it's on the edge of the map or adjacent to another space
                    if (y > 0 && dungeon[f][y - 1][x] != '.')
                    {
                        dungeon[f][y - 1][x] = '_'; // Top border
                    }
                    if (y < MAP_HEIGHT - 1 && dungeon[f][y + 1][x] != '.')
                    {
                        dungeon[f][y + 1][x] = '_'; // Bottom border
                    }
                    if (x > 0 && dungeon[f][y][x - 1] != '.')
                    {
                        dungeon[f][y][x - 1] = '|'; // Left border
                    }
                    if (x < MAP_WIDTH - 1 && dungeon[f][y][x + 1] != '.')
                    {
                        dungeon[f][y][x + 1] = '|'; // Right border
                    }
                }
            }
        }

        srand(time(NULL));
        while (f != FLOORS -1) {
            int randomX = rand() % (MAP_WIDTH / 3);
            int randomY = MAP_HEIGHT -rand() % (MAP_HEIGHT / 2);

            if (dungeon[f][randomY][randomX] == '|' || dungeon[f][randomY][randomX] == '_')
            {
                dungeon[f][randomY][randomX] = 'U';
                break;
            }
        }

        while (f != 0) {
            int randomX = rand() % (MAP_WIDTH / 3);
            int randomY = rand() % (MAP_HEIGHT / 2);

            if (dungeon[f][randomY][randomX] == '|' || dungeon[f][randomY][randomX] == '_')
            {
                dungeon[f][randomY][randomX] = 'D';

                break;
            }
        }
        
        while (f == FLOORS -1) {
            int randomX = rand() % (MAP_WIDTH / 3);
            int randomY = MAP_HEIGHT -rand() % (MAP_HEIGHT / 2);

            if (dungeon[f][randomY][randomX] == '|' || dungeon[f][randomY][randomX] == '_')
            {
                dungeon[f][randomY][randomX] = 'W';
                break;
            }
        }
    }
}

void displayFloor(int floor)
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
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            if (dungeon[floor][y][x] == '.') attron(COLOR_PAIR(7));
            mvaddch(y, x, dungeon[floor][y][x]);
            if (dungeon[floor][y][x] == '.') attroff(COLOR_PAIR(7));
        }
    }
    refresh();
}

void map_generator()
{
    initializeDungeon();
    generateRooms();
}

void generateTrap() {
    //for the whole map
    srand(time(NULL));
    for (int i = 0; i < 5; i++) {
        while (1) {
            int randomX = rand() % (MAP_WIDTH);
            int randomY = rand() % (MAP_HEIGHT);
            int randomF = rand() % (FLOORS);

            if (dungeon[randomF][randomY][randomX] == '.') {
                mygame.traps[i].floor = randomF;
                mygame.traps[i].x = randomX;
                mygame.traps[i].y = randomY;
                //dungeon[randomF][randomY][randomX]= '^';
                break;
            }
        }
    }

    //for treasure room: 
    for (int i = 5; i < 10; i++) {
        while (1) {
            int randomX = rand() % (MAP_WIDTH/3);
            int randomY = MAP_HEIGHT/2 +rand() % (MAP_HEIGHT/2);
            int randomF = 3;

            if (dungeon[randomF][randomY][randomX] == '.') {
                mygame.traps[i].floor = randomF;
                mygame.traps[i].x = randomX;
                mygame.traps[i].y = randomY;
                //dungeon[randomF][randomY][randomX] = '^';
                break;
            }
        }
    }
}

void generateSpell() {
    //for the whole map
    srand(time(NULL));
    for (int i = 0; i < 5; i++) {
        while (1) {
            int randomX = rand() % (MAP_WIDTH);
            int randomY = rand() % (MAP_HEIGHT);
            int randomF = rand() % (FLOORS);
            int randomType = rand() % 3;

            if (dungeon[randomF][randomY][randomX] == '.') {
                mygame.spell[i].floor = randomF;
                mygame.spell[i].x = randomX;
                mygame.spell[i].y = randomY;
                mygame.spell[i].visiblity = -1;
                mygame.spell[i].type = randomType;
                dungeon[randomF][randomY][randomX] = 'e';
                break;
            }
        }
    }

    //for spell room: 
    for (int i = 5; i < 10; i++) {
        while (1) {
            int randomX = rand() % (MAP_WIDTH);
            int randomY = rand() % (MAP_HEIGHT);
            int randomF = 2;
            int randomType = rand() % 3;

            if (dungeon[randomF][randomY][randomX] == '.') {
                mygame.spell[i].floor = randomF;
                mygame.spell[i].x = randomX;
                mygame.spell[i].y = randomY;
                mygame.spell[i].type = randomType;
                mygame.spell[i].visiblity = -1;
                dungeon[randomF][randomY][randomX] = 'e';
                break;
            }
        }
    }
}

void generateFood() {
    //for the whole map
    srand(time(NULL));
    for (int i = 0; i < 5; i++) {
        while (1) {
            int randomX = rand() % (MAP_WIDTH);
            int randomY = rand() % (MAP_HEIGHT);
            int randomF = rand() % (FLOORS);
            int randomType = -1 +rand() % 4;

            if (dungeon[randomF][randomY][randomX] == '.') {
                mygame.food[i].floor = randomF;
                mygame.food[i].x = randomX;
                mygame.food[i].y = randomY;
                mygame.food[i].visiblity = -1;
                mygame.food[i].type = randomType;
                dungeon[randomF][randomY][randomX] = 'f';
                break;
            }
        }
    }

    //for the second floor: 
    for (int i = 5; i < 10; i++) {
        while (1) {
            int randomX = rand() % (MAP_WIDTH);
            int randomY = rand() % (MAP_HEIGHT);
            int randomF = 1;
            int randomType = -1 +rand() % 4;

            if (dungeon[randomF][randomY][randomX] == '.') {
                mygame.food[i].floor = randomF;
                mygame.food[i].x = randomX;
                mygame.food[i].y = randomY;
                mygame.food[i].type = randomType;
                mygame.food[i].visiblity = -1;
                dungeon[randomF][randomY][randomX] = 'f';
                break;
            }
        }
    }
}

void generateGold() {
    //for the whole map
    srand(time(NULL));
    for (int i = 0; i < 5; i++) {
        while (1) {
            int randomX = rand() % (MAP_WIDTH);
            int randomY = rand() % (MAP_HEIGHT);
            int randomF = rand() % (FLOORS);
            int randomType = rand() % 2;

            if (dungeon[randomF][randomY][randomX] == '.') {
                mygame.gold[i].floor = randomF;
                mygame.gold[i].x = randomX;
                mygame.gold[i].y = randomY;
                mygame.gold[i].visiblity = -1;
                mygame.gold[i].type = randomType;
                dungeon[randomF][randomY][randomX] = 'g';
                break;
            }
        }
    }

    //for the fourth floor: 
    for (int i = 5; i < 10; i++) {
        while (1) {
            int randomX = rand() % (MAP_WIDTH);
            int randomY = rand() % (MAP_HEIGHT);
            int randomF = 1;
            int randomType = rand() % 2;

            if (dungeon[randomF][randomY][randomX] == '.') {
                mygame.gold[i].floor = randomF;
                mygame.gold[i].x = randomX;
                mygame.gold[i].y = randomY;
                mygame.gold[i].type = randomType;
                mygame.gold[i].visiblity = -1;
                dungeon[randomF][randomY][randomX] = 'g';
                break;
            }
        }
    }
}

void generateWeapon() {
    //for the whole map
    srand(time(NULL));
    for (int i = 0; i < 5; i++) {
        while (1) {
            int randomX = rand() % (MAP_WIDTH);
            int randomY = rand() % (MAP_HEIGHT);
            int randomF = rand() % (FLOORS);
            int randomType = rand() % 5;

            if (dungeon[randomF][randomY][randomX] == '.') {
                mygame.weapon[i].floor = randomF;
                mygame.weapon[i].x = randomX;
                mygame.weapon[i].y = randomY;
                mygame.weapon[i].visiblity = -1;
                mygame.weapon[i].type = randomType;
                dungeon[randomF][randomY][randomX] = '!';
                break;
            }
        }
    }

    //for the third floor: 
    for (int i = 5; i < 10; i++) {
        while (1) {
            int randomX = rand() % (MAP_WIDTH);
            int randomY = rand() % (MAP_HEIGHT);
            int randomF = 2;
            int randomType = rand() % 2;

            if (dungeon[randomF][randomY][randomX] == '.') {
                mygame.weapon[i].floor = randomF;
                mygame.weapon[i].x = randomX;
                mygame.weapon[i].y = randomY;
                mygame.weapon[i].type = randomType;
                mygame.weapon[i].visiblity = -1;
                dungeon[randomF][randomY][randomX] = '!';
                break;
            }
        }
    }
}