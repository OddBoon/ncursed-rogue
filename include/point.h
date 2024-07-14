#ifndef POINT_H
#define POINT_H

#ifndef PRE_H_H
#define PRE_H_H
#include <stdbool.h>
#endif

typedef struct Point {
    int x;
    int y;
}Point;

bool ptAtRelOrigin(Point parentPosition, Point childPosition);
bool ptAtWrldOrigin(Point point);
#endif