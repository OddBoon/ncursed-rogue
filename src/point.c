#include "point.h"

bool ptAtRelOrigin(Point parent, Point child){
    return parent.x == child.x && parent.y == child.y;
}

bool ptAtWrldOrigin(Point point){
    return point.x == 0 && point.y == 0;
}