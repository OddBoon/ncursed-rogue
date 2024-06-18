#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

struct Connection;

enum Orientation {
    TOP,
    RIGHT,
    BOTTOM,
    LEFT
};

typedef struct Point {
    int x;
    int y;
}Point;

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

typedef struct Player {
    Point position;
    int health;
}Player;

int screenSetUp();
Map createTestMap();

int printRoom(Room* room);
Room* buildRoom(int y, int x, int height, int width);
int connectRooms(Room* head, Room* tail, enum Orientation orientHead, enum Orientation orientTail);

Point checkCollision(Point newPosition, Player* player);
int movePlayer(Point newPosition, Player* player);
int handleInput(int ch, Player* player);


Player* spawnPlayer(int y, int x);


int main () 
{
    srand(time(NULL));
    Player* user;
    int ch;
    screenSetUp();

    Map map = createTestMap();

    user = spawnPlayer(14, 14);

    while((ch = getch()) != 'q')
    {
        handleInput(ch, user);
    }
    endwin();
    free(user);
    for(int i=0; i<map.numRooms; i++)
    {
        free(map.rooms[i]);
        printf("Free room #%d", i);
    }
    return 0;
}

Player* spawnPlayer(int y, int x) {
    Player* newPlayer = malloc(sizeof(Player));
    newPlayer->position.x = x;
    newPlayer->position.y = y;
    newPlayer->health = 100;

    mvprintw(newPlayer->position.y, newPlayer->position.x, "@");
    move(newPlayer->position.y, newPlayer->position.x);
    return newPlayer;
}

int screenSetUp() {
    initscr();
    keypad(stdscr, true);
    printw("Welcome!");
    noecho();
    refresh();

    return 0;
}

int handleInput(int ch, Player* player) {
    bool isShift = false;
    if(ch <= 'Z' && ch >= 'A')
    {
        isShift = true;
        ch |= 32;
    }
    Point position = player->position;
    switch(ch)
    {
        case 'w':
        case KEY_UP:
            position.y -= 1;
            break;
        case 's':
        case KEY_DOWN:
            position.y += 1;
            break;
        case 'a':
        case KEY_LEFT:
            position.x -= 1;
            break;
        case 'd':
        case KEY_RIGHT:
            position.x += 1;
            break;
        default:
            break; 
    }
    position = checkCollision(position, player);
    movePlayer(position, player);
}

Point checkCollision(Point newPosition, Player* player){
    //collision testing
    switch(mvinch(newPosition.y, newPosition.x))
    {
        case '|':
        case '-':
            return player->position;
        case '.':
            return newPosition;
        case '+':
            return player->position; //TODO: connect doors
        default:
            return player->position;
    }
}

int movePlayer(Point newPosition, Player* player) {
    Point prevPosition = player->position;
    if(prevPosition.x == newPosition.x && prevPosition.y == newPosition.y) { return 0; }
    mvprintw(prevPosition.y, prevPosition.x, ".");
    mvprintw(newPosition.y, newPosition.x, "@");
    move(newPosition.y, newPosition.x);
    player->position = newPosition;
}

Room* buildRoom(int y, int x, int height, int width) {
    if(width <= 0 || height <= 0)
    {
        //TODO:
        //error handling done here
        //return 1;
    }
    Room* newRoom = malloc(sizeof(Room));
    newRoom->position.x = x;
    newRoom->position.y = y;
    newRoom->width = width;
    newRoom->height = height;
    return newRoom;
}

int printRoom(Room* room) {
    int x = room->position.x, y = room->position.y, width = room->width, height = room->height;
    if(width <= 0 || height <= 0)
    {
        //TODO:
        //error handling done here
        return 1;
    }
    char* wall=malloc(sizeof(char)*(width + 1));
    char* interior = malloc(sizeof(char)*(width+1));
    wall[0] = '(';
    interior[0] = '|';
    for(int i=1; i<width; i++)
    {
        wall[i] = '-';
        interior[i] = '.';
    }
    wall[width-1] = ')';
    interior[width-1] = '|';
    wall[width] = '\0';
    interior[width] = '\0';
    mvprintw(y, x, "%s", wall);
    for(int i=y+1; i<y+height-1; i++)
    {
        mvprintw(i, x, "%s", interior);
    }
    mvprintw(y+height-1, x, "%s", wall);
    free(wall);
    free(interior);
    //Done with x/y variables as origin of room
    for(int i=0; i<4; i++)
    {   
        x=room->doors[i].position.x;
        y=room->doors[i].position.y;
        if(x == 0 && y == 0){
            continue;
        }
        mvprintw(y, x, "D");
    }
    return 0;
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
    if(placementRoll < 1)
    {
        //ERROR: size of room can't work with less than 3 width/height
        return -1;
    } else if(placementRoll == 1)
    {
        pos.x += 1; //or end result of (rand() % (room->width-2) + 1) where width has to be 3;
    } else {
        placementRoll--;
        pos.x += (rand() % placementRoll + rand() % placementRoll);
    }
    if(orient == BOTTOM || orient == RIGHT)
    {
        pos.y += face;
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
    if(head->doors[orientHead].outside != NULL) {
        //Error: Room is already connected
        //TODO:error handling
        return -1;
    }
    if(tail->doors[orientTail].outside != NULL) {
        //Error: Room is already connected
        //TODO:error handling
        return -1;
    }
    Door* tailDoor = &tail->doors[orientTail];
    //if(head->)
    Connection* newConnection = malloc(sizeof(Connection));
    
    
    
    return 0;
}

//Map generateMap() {

//}

Map createTestMap() {
    Room** rooms = malloc(sizeof(Room*)*3);
    rooms[0] = buildRoom(13, 13, 6, 12);
    rooms[1] = buildRoom(3, 13, 6, 12);
    rooms[2] = buildRoom(3, 40, 6, 12);

    //connectRooms(rooms[0], rooms[1], Orientation.TOP, Orientation.BOTTOM);
    //connectRooms(rooms[1], rooms[2], Orientation.RIGHT, Orientation.LEFT);
    for(int i=0; i<4; i++){
        buildDoor(rooms[0], i);
    }

    for(int i=0; i<3; i++ )
    {
        printRoom(rooms[i]);
    }
    Map map;
    map.rooms = rooms;
    map.numRooms = 3;
    map.currentRoom = rooms[0];
    return map;
}