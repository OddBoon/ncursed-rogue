#ifndef PLAYER_H
#define PLAYER_H

#ifndef PRE_H_H
#define PRE_H_H
#include <stdlib.h>
#include <stdbool.h>
#endif

#include "point.h"

typedef struct Player {
    Point position;
    int health;
}Player;

Player* spawnPlayer(int y, int x);
#endif
