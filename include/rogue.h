#ifndef ROGUE_H
#define ROGUE_H

#ifndef PRE_H_H
#define PRE_H_H

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#endif

#include "point.h"
#include "player.h"
#include "map.h"

int screenSetUp();

Map createTestMap();

int printRoom(Room* room);
int printConnection(Connection* con);
int printPlayer(Player* player);

int movePlayer(Point newPosition, Player* player);
Point checkCollision(Point newPosition, Player* player);
int handleInput(int ch, Player* player);

#endif