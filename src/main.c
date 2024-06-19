#include "rogue.h"

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

int printConnection(Connection* con){
    Point start = con->roomHead->doors[con->doorSideHead].position;
    Point end = con->roomTail->doors[con->doorSideTail].position;
    Point delta;
    delta.x = end.x-start.x;
    delta.y = end.y-start.y;
    //Initial Movement
    switch(con->doorSideHead){
        case TOP:
            delta.y+=1;
            start.y-=1;
        break;
        case BOTTOM:
            delta.y-=1;
            start.y+=1;
        break;
        case LEFT:
            delta.x+=1;
            start.x-=1;
        break;
        case RIGHT:
            delta.x-=1;
            start.x+=1;
        break;
    }
    mvprintw(start.y, start.x, "#");
    //Find End
    bool Xmovement = delta.y == 0 || (delta.x / delta.y != 0);
    bool hasMoved = false;
    while(delta.x != 0 || delta.y != 0)
    {
        hasMoved = false;
        if(Xmovement) {
            if(delta.x < 0 && (mvinch(start.y, start.x-1) == ' '))
            {
                delta.x+=1;
                start.x-=1;
                mvprintw(start.y, start.x, "#");
                hasMoved = true;
            }else if(delta.x > 0 && (mvinch(start.y, start.x+1) == ' ')) {
                delta.x-=1;
                start.x+=1;
                mvprintw(start.y, start.x, "#");
                hasMoved = true;
            }else if(delta.y < 0 && (mvinch(start.y-1, start.x) == ' ')) {
                delta.y += 1;
                start.y -= 1;
                mvprintw(start.y, start.x, "#");
                hasMoved = true;
            }else if(delta.y > 0 && (mvinch(start.y+1, start.x) == ' ')) {
                delta.y -= 1;
                start.y += 1;
                mvprintw(start.y, start.x, "#");
                hasMoved = true;
            }
        } else {//invert priority of x and y
            if(delta.y < 0 && (mvinch(start.y-1, start.x) == ' ')) {
                delta.y += 1;
                start.y -= 1;
                mvprintw(start.y, start.x, "#");
                hasMoved = true;
            }else if(delta.y > 0 && (mvinch(start.y+1, start.x) == ' ')) {
                delta.y -= 1;
                start.y += 1;
                mvprintw(start.y, start.x, "#");
                hasMoved = true;
            }else if(delta.x < 0 && (mvinch(start.y, start.x-1) == ' ')) {
                delta.x+=1;
                start.x-=1;
                mvprintw(start.y, start.x, "#");
                hasMoved = true;
            }else if(delta.x > 0 && (mvinch(start.y, start.x+1) == ' ')) {
                delta.x-=1;
                start.x+=1;
                mvprintw(start.y, start.x, "#");
                hasMoved = true;
            }
        }
        Xmovement = delta.y == 0 || (delta.x / delta.y) != 0;
        if(!hasMoved) {
            int lastMove = delta.y+delta.x;
            if(lastMove >= -1  && lastMove <= 1 && !(delta.y || delta.x)) {
                return 0;
            }
            return -1;
        }
    }
    return 0;
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

Map createTestMap() {
    Room** rooms = malloc(sizeof(Room*)*3);
    rooms[0] = buildRoom(13, 13, 6, 12);
    rooms[1] = buildRoom(3, 13, 6, 12);
    rooms[2] = buildRoom(20, 40, 12, 12);

    for(int i=0; i<4; i++){
        buildDoor(rooms[0], i);
        rooms[0]->doors[i].isBlocked=true;
    }
    
    connectRooms(rooms[0], rooms[2], TOP, RIGHT);
    //connectRooms(rooms[1], rooms[2], RIGHT, LEFT);

    for(int i=0; i<3; i++ )
    {
        printRoom(rooms[i]);
    }
    printConnection(rooms[0]->doors[TOP].outside);
    //printConnection(rooms[1]->doors[RIGHT].outside);
    Map map;
    map.rooms = rooms;
    map.numRooms = 3;
    map.currentRoom = rooms[0];
    return map;
}