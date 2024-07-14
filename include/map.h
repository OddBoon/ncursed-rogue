#ifndef MAP_H
#define MAP_H

#ifndef PRE_H_H
#define PRE_H_H
#include <stdlib.h>
#include <stdbool.h>
#endif

#include "point.h"


struct Connection;

enum Orientation {
    TOP,
    RIGHT,
    BOTTOM,
    LEFT
};

typedef struct Door {
    Point position;
    struct Connection *outside;
    bool isBlocked;
}Door;

typedef struct Room {
    Point position;
    int width;
    int height;
    Door doors[4]; //implicit Orientation (top, then clockwise)
}Room;

enum ConnectionMode {
    PIPE,
    STITCH,
    EMBEDDED
};

typedef struct Connection {
    enum ConnectionMode mode;
    enum Orientation doorSideHead;
    enum Orientation doorSideTail;
    Room* roomHead;
    Room* roomTail;
}Connection;

typedef struct Map {
    Room** rooms;
    Room* currentRoom;
    int numRooms;
}Map;

Room* buildRoom(int y, int x, int height, int width);
int connectRooms(Room* head, Room* tail, enum Orientation orientHead, enum Orientation orientTail);
int buildDoor(Room* room, enum Orientation orient);
int freeRoom(Room* room);
int clearMap(Map* map);

#endif