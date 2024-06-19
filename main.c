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

static inline bool ptAtRelOrigin(Point parentPosition, Point childPosition);
static inline bool ptAtWrldOrigin(Point point);

int screenSetUp();

Map createTestMap();
int clearMap(Map* map);

int printRoom(Room* room);
Room* buildRoom(int y, int x, int height, int width);
int connectRooms(Room* head, Room* tail, enum Orientation orientHead, enum Orientation orientTail);
int freeRoom(Room* room);

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
    //TODO: check for valid spawning point for user
    user = spawnPlayer(14, 14);

    while((ch = getch()) != 'q')
    {
        handleInput(ch, user);
    }
    endwin();
    free(user);
    clearMap(&map);
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
    printw("Welcome Delver!");
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
        case '[':
        case ']':
        case '=':
            return player->position;
        case '.':
            return newPosition;
        case 'D':
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

static inline bool ptAtRelOrigin(Point parent, Point child){
    return parent.x == child.x && parent.y == child.y;
}

static inline bool ptAtWrldOrigin(Point point){
    return point.x == 0 && point.y == 0;
}

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

int printRoom(Room* room) {
    Point position = room->position;
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
    mvprintw(position.y, position.x, "%s", wall);
    for(int i=position.y+1; i<position.y+height-1; i++)
    {
        mvprintw(i, position.x, "%s", interior);
    }
    mvprintw(y+height-1, position.x, "%s", wall);
    free(wall);
    free(interior);
    Point doorPosition;
    for(enum Orientation i=TOP; i<=LEFT; i++)
    {   
        if(ptAtRelOrigin(position, (doorPosition = room->doors[i].position))) {
            continue;
        }else if(ptAtWrldOrigin(doorPosition)) {
            //Error: Doors weren't initialized/Door was set outside of room bounds
            //TODO: Do error handling on bad door for print
            //TODO: Develop error for door outside of room bounds

        }
        if(room->doors[i].isBlocked) {
            switch(i)
            {
                case LEFT:
                mvprintw(doorPosition.y, doorPosition.x, "[");
                break;
                case RIGHT:
                mvprintw(doorPosition.y, doorPosition.x, "]");
                break;
                case TOP:
                case BOTTOM:
                mvprintw(doorPosition.y, doorPosition.x, "=");
                break;
            } 
        }else {
            mvprintw(doorPosition.y, doorPosition.x, "0"); 
        }
        
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
        printf("Free room #%d", i);
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

Map createTestMap() {
    Room** rooms = malloc(sizeof(Room*)*3);
    rooms[0] = buildRoom(13, 13, 6, 12);
    rooms[1] = buildRoom(3, 13, 6, 12);
    rooms[2] = buildRoom(3, 40, 6, 12);

    for(int i=0; i<4; i++){
        buildDoor(rooms[0], i);
        rooms[0]->doors[i].isBlocked=true;
    }
    
    connectRooms(rooms[0], rooms[1], TOP, BOTTOM);
    //connectRooms(rooms[1], rooms[2], Orientation.RIGHT, Orientation.LEFT);

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