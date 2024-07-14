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
    printPlayer(user);

    while((ch = getch()) != 'q')
    {
        handleInput(ch, user);
    }
    endwin();
    free(user);
    clearMap(&map);
    return 0;
}

int printPlayer(Player* player)
{
    mvprintw(player->position.y, player->position.x, "@");
    move(player->position.y, player->position.x);
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
        case '#':
        case '0':
            return newPosition;
        default:
            return player->position;
    }
}

int movePlayer(Point newPosition, Player* player) {
    Point prevPosition = player->position;
    if(prevPosition.x != newPosition.x || prevPosition.y != newPosition.y) { 
        mvprintw(prevPosition.y, prevPosition.x, ".");
        mvprintw(newPosition.y, newPosition.x, "@");
    }
    move(newPosition.y, newPosition.x);
    player->position = newPosition;
}

void incrementConnection(Point* currentPosition, enum Orientation direction)
{
    switch(direction){
        case TOP:
            currentPosition->y--;
            break;
        case BOTTOM:
            currentPosition->y++;
            break;
        case LEFT:
            currentPosition->x--;
            break;
        case RIGHT:
            currentPosition->x++;

    }
    mvprintw(currentPosition->y, currentPosition->x, "#");
}

int printConnection(Connection* con){
    Point start = con->roomHead->doors[con->doorSideHead].position;
    Point end = con->roomTail->doors[con->doorSideTail].position;
    Point delta;
    delta.x = end.x-start.x;
    delta.y = end.y-start.y;

    //Initialization of endpoints
    incrementConnection(&start, con->doorSideHead);
    incrementConnection(&end, con->doorSideTail);
    //Find End
    bool Xmovement = delta.y == 0 || (delta.x / delta.y != 0);
    bool hasMoved = false;
    while(delta.x != 0 || delta.y != 0)
    {
        hasMoved = false;
        if(Xmovement) {
            if(delta.x < 0 && (mvinch(start.y, start.x-1) == ' '))
            {
                incrementConnection(&start, LEFT);
                hasMoved = true;
            }else if(delta.x > 0 && (mvinch(start.y, start.x+1) == ' ')) {
                incrementConnection(&start, RIGHT);
                hasMoved = true;
            }else if(delta.y < 0 && (mvinch(start.y-1, start.x) == ' ')) {
                incrementConnection(&start, TOP);
                hasMoved = true;
            }else if(delta.y > 0 && (mvinch(start.y+1, start.x) == ' ')) {
                incrementConnection(&start, BOTTOM);
                hasMoved = true;
            }
        } else {//invert priority of x and y
            if(delta.y < 0 && (mvinch(start.y-1, start.x) == ' ')) {
                incrementConnection(&start, TOP);
                hasMoved = true;
            }else if(delta.y > 0 && (mvinch(start.y+1, start.x) == ' ')) {
                incrementConnection(&start, BOTTOM);
                hasMoved = true;
            }else if(delta.x < 0 && (mvinch(start.y, start.x-1) == ' ')) {
                incrementConnection(&start, LEFT);
                hasMoved = true;
            }else if(delta.x > 0 && (mvinch(start.y, start.x+1) == ' ')) {
                incrementConnection(&start, RIGHT);
                hasMoved = true;
            }
        }
        delta.x = end.x-start.x, delta.y = end.y-start.y;
        Xmovement = delta.y == 0 || (delta.x / delta.y) != 0;
        if(!hasMoved) {
            int lastMove = delta.y+delta.x;
            if(lastMove >= -1  && lastMove <= 1 && !(delta.y || delta.x)) {
                return 0;
            }else{return -1;}
        }
        mvprintw(3,0,"Delta is y:%d x:%d ", delta.y, delta.x);
        getch();
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
    rooms[2] = buildRoom(13, 35, 6, 12);

    for(int i=0; i<4; i++){
        buildDoor(rooms[0], i);
        rooms[0]->doors[i].isBlocked=true;
    }
    
    connectRooms(rooms[0], rooms[2], TOP, BOTTOM);
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