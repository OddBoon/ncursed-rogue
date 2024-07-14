#include "player.h"

Player* spawnPlayer(int y, int x) {
    Player* newPlayer = malloc(sizeof(Player));
    newPlayer->position.x = x;
    newPlayer->position.y = y;
    newPlayer->health = 100;
    return newPlayer;
}