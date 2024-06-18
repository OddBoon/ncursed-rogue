#include <ncurses.h>
#include <stdlib.h>

typedef struct Point {
    int x;
    int y;
}Point;

typedef struct Room {
    Point position;
    int width;
    int height;
}Room;

typedef struct Map {
    Room** rooms;
    int numRooms;
}Map;

typedef struct Player {
    Point position;
    int health;
}Player;

int screenSetUp();
Map mapSetUp();
int printRoom(Room* room);
Room* buildRoom(int y, int x, int height, int width);
Point checkCollision(Point newPosition, Player* player);
int movePlayer(Point newPosition, Player* player);
int handleInput(int ch, Player* player);


Player* playerSetUp();


int main () 
{
    Player* user;
    int ch;
    screenSetUp();

    Map map = mapSetUp();

    user = playerSetUp();

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

Player* playerSetUp() {
    Player* newPlayer = malloc(sizeof(Player));
    newPlayer->position.x = 14;
    newPlayer->position.y = 14;
    newPlayer->health = 100;

    mvprintw(newPlayer->position.y, newPlayer->position.x, "@");
    move(newPlayer->position.y, newPlayer->position.x);
    return newPlayer;
}

int screenSetUp() {
    initscr();
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
            position.y -= 1;
            break;
        case 's':
            position.y += 1;
            break;
        case 'a':
            position.x -= 1;
            break;
        case 'd':
            position.x += 1;
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
    return 0;
}



Map mapSetUp() {
    Room** rooms = malloc(sizeof(Room*)*3);
    rooms[0] = buildRoom(13, 13, 6, 12);
    rooms[1] = buildRoom(3, 13, 6, 12);
    rooms[2] = buildRoom(3, 40, 6, 12);
    for(int i=0; i<3; i++ )
    {
        printRoom(rooms[i]);
    }
    Map map;
    map.rooms = rooms;
    map.numRooms = 3;
    return map;
}