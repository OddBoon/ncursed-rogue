#ifndef POINT_H
#define POINT_H

#include <stdbool.h>

typedef struct Point {
    int x;
    int y;
}Point;

bool ptAtRelOrigin(Point parentPosition, Point childPosition);
bool ptAtWrldOrigin(Point point);
#endif