//Zackery Leman & Ivy Xing

#include "geom.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>



/* **************************************** */
/* returns the signed area of triangle abc. The area is positive if c
   is to the left of ab, and negative if c is to the right of ab
 */
int signed_area2D(point2D a, point2D b, point2D c) {

  return (b.x - a.x) * (c.y - a.y) - (c.x -a.x) * (b.y - a.y);
}



/* **************************************** */
/* return 1 if p,q,r collinear, and 0 otherwise */
int collinear(point2D p, point2D q, point2D r) {
  
  return signed_area2D(p, q, r) == 0;
}



/* **************************************** */
/* return 1 if c is  strictly left of ab; 0 otherwise */
int left (point2D a, point2D b, point2D c) {
  
  return signed_area2D(a, b, c) > 0;
}


//return 1 if s1 and s2 intersect
int intersect(segment2D s1, segment2D s2) {
    if (intersect_proper(s1, s2)) {
        return 1;
    } else if ( between(s1.start, s1.end, s2.start) ||
               between(s1.start, s1.end, s2.end) ||
               between(s2.start, s2.end, s1.start) ||
               between(s2.start, s2.end, s1.end)) {
        return 1;
    } else {
        return 0;
    }
}

// exclusive or.
int Xor(int x, int y) {
    return !x ^ !y;
}

//return 1 if s1 and s2 intersect at a proper point (point that’s interior to both)
int intersect_proper (segment2D s1, segment2D s2) {
    if (collinear(s1.start, s1.end, s2.start) ||
        collinear(s1.start, s1.end, s2.end) ||
        collinear(s2.start, s2.end, s1.start) ||
        collinear(s2.start, s2.end, s1.end)) {
        return 0;
    }
    
    return Xor(left(s1.start, s1.end, s2.start), left(s1.start, s1.end, s2.end)) &&
            Xor(left(s2.start, s2.end, s1.start), left(s2.start, s2.end, s1.end));
}

//return 1 if  s1 and s2 intersect improperly, i.e. the intersection point is the endpoint of one or both segments
int intersect_improper (segment2D s1,  segment2D s2) {
    if (intersect(s1, s2) && !intersect_proper(s1, s2)) {
        return 1;
    } else {
        return 0;
    }
}

// Check whether point c is between a and b
int between(point2D a, point2D b, point2D c) {
    if (!collinear(a, b, c)) {
        return 0;
    }
    if (a.x != b.x) {
        return ((a.x <= c.x) && (c.x <= b.x)) ||
                ((a.x >= c.x) && (c.x >= b.x));
    } else {
        return ((a.y <= c.y) && (c.y <= b.y)) ||
                ((a.y >= c.y) && (c.y >= b.y));
    }
}
