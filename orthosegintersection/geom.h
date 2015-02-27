#ifndef __geom_h
#define __geom_h


typedef struct _point2d {
  int x,y; 
} point2D;


typedef struct _segment2d {
  point2D start; 
  point2D end; 
} segment2D;


typedef struct _event {
    char eventType;
    int eventXCoord ;
    struct _segment2d segment;
} event;



//a list of points 
typedef struct _pointNode pointNode; 
struct _pointNode  {
  point2D p;
  pointNode* next;
}; 



/* returns the signed area of triangle abc. The area is positive if c
   is to the left of ab, and negative if c is to the right of ab
 */
int signed_area2D(point2D a, point2D b, point2D c); 


/* return 1 if p,q,r collinear, and 0 otherwise */
int collinear(point2D p, point2D q, point2D r);


/* return 1 if c is  strictly left of ab; 0 otherwise */
int left (point2D a, point2D b, point2D c); 


#endif
