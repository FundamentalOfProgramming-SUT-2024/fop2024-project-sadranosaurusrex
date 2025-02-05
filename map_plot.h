#ifndef MAP_PLOT_H
#define MAP_PLOT_H

#define FLOORS 4
#define MAP_HEIGHT 30
#define MAP_WIDTH 80

extern char dungeon[FLOORS][MAP_HEIGHT][MAP_WIDTH];

void displayFloor(int floor);
void map_generator();
void renderGame();
void generateSpell();
void generateTrap();
void generateFood();
void generateGold();
void generateWeapon();

#endif // MAP_PLOT_H
