#ifndef ENEMY_H
#define ENEMY_H

typedef struct {
    char symbol;
    int health;
    int x;
    int y;
    int floor;
    int duration;
    int status;
    int followingSince;
} enemy;

extern enemy enemies[20];

void undeedGenerator();
void demonGenerator();
void monsterGenerator();
void snakeGenerator();
void giantGenerator();
void enemyMovement();
void followStatus();

#endif // ENEMY_H