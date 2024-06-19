#include "map.h"

Room* buildRoom(int y, int x, int height, int width) {
    if(width <= 2 || height <= 2)
    {
        //TODO:
        //error handling done here
        //return 1;
    }
    Room* newRoom = malloc(sizeof(Room));
    newRoom->position.x = x;
    newRoom->position.y = y;
    Point startPos = newRoom->position;
    newRoom->width = width;
    newRoom->height = height;
    for(enum Orientation i=TOP; i<=LEFT; i++)
    {
        newRoom->doors[i].position = startPos;
    }
    return newRoom;
}

int buildDoor(Room* room, enum Orientation orient) {
    
    Point pos = room->position;
    int placement = room->width;
    int face = room->height;
    switch(orient) {
        case LEFT:
        case RIGHT: //y is movable -- flip axis manipulation
            //flip x/y in position
            pos.x=pos.x^pos.y;
            pos.y=pos.x^pos.y;
            pos.x=pos.x^pos.y;
            //flip placement/face
            placement=placement^face;
            face=placement^face;
            placement=placement^face;
        default:
    }
    int placementRoll = placement >> 1;
    if(placement <= 2)
    {
        //ERROR: size of room can't work with less than 2 width/height
        return -1;
    } else if(placement == 3)
    {
        pos.x += 1; //or end result of (rand() % (room->width-2) + 1) where width has to be 3;
    } else {
        int placementRoll = placement >> 1;
        placementRoll--;
        pos.x += rand() % placementRoll + rand() % placementRoll + 1;
    }
    if(orient == BOTTOM || orient == RIGHT)
    {
        pos.y += face-1;
    }
    //flip back to x/y if y/x
    switch(orient) {
        case LEFT:
        case RIGHT:
            //flip x/y in position
            pos.x=pos.x^pos.y;
            pos.y=pos.x^pos.y;
            pos.x=pos.x^pos.y;
            //flip placement/face
            placement=placement^face;
            face=placement^face;
            placement=placement^face;
        default:
    }
    room->doors[orient].position = pos;
    return 0;
}

int connectRooms(Room* head, Room* tail, enum Orientation orientHead, enum Orientation orientTail) {
    int err;
    //TODO:check for invalid head/tail as well
    if(head->doors[orientHead].outside != NULL) {
        //Error: Room is already connected
        //TODO:error handling
        return -1;
    }else if(ptAtRelOrigin(head->position, head->doors[orientHead].position)){
        err = buildDoor(head, orientHead);
        if(err != 0)
        {
            return err;
        }
    }
    if(tail->doors[orientTail].outside != NULL) {
        //Error: Room is already connected
        //TODO:error handling
        return -1;
    }else if(ptAtRelOrigin(tail->position, tail->doors[orientTail].position)){
        err = buildDoor(tail, orientTail);
        if(err != 0)
        {
            return err;
        }
    }
    //Copy doors to local variables
    Door tailDoor = tail->doors[orientTail];
    Door headDoor = head->doors[orientHead];

    if(tailDoor.position.x-headDoor.position.x == 0 || tailDoor.position.y-headDoor.position.y == 0)
    //Rooms are touching
    {
        //TODO: Dont know what to do yet
    }

    //Make Connection
    Connection* newConnection = malloc(sizeof(Connection));
    newConnection->doorSideTail = orientTail;
    newConnection->doorSideHead = orientHead;
    newConnection->roomTail = tail;
    newConnection->roomHead = head;
    newConnection->mode = PIPE;

    //Tap Doors to connection
    tailDoor.outside = newConnection;
    headDoor.outside = newConnection;

    //unblock doors (TODO:make overridable later)
    tailDoor.isBlocked = false;
    headDoor.isBlocked = false;

    //Push new doors to rooms
    tail->doors[orientTail] = tailDoor;
    head->doors[orientHead] = headDoor;
    return 0;
}

//Map generateMap() {

//}
int clearMap(Map* map){
    //free each room
    for(int i=0; i<map->numRooms; i++)
    {
        freeRoom(map->rooms[i]);
        free(map->rooms[i]);
    }
    free(map->rooms);
    map->currentRoom = NULL;
    return 0;
}


//Frees all connections associated with given room
int freeRoom(Room* room){
    if(room == NULL) { return 0; }
    Door port;
    Connection* removedConnection;
    Room* connectedRoom;
    enum Orientation roomSide;
    for(enum Orientation i=TOP; i<=LEFT; i++)
    {
        port = room->doors[i];
        if(port.outside == NULL) { continue; }
        removedConnection = port.outside;
        connectedRoom = removedConnection->roomHead;
        roomSide = removedConnection->doorSideHead;
        if(connectedRoom == room) { 
            connectedRoom = removedConnection->roomTail; 
            roomSide = removedConnection->doorSideTail;
        }
        connectedRoom->doors[roomSide].outside = NULL;
        free(removedConnection);
        room->doors[i].outside = NULL;
    }
    return 0;
}