/* view.c
 
 Laura Toma, Ivy Xing, Zackery Leman
 
 What it does:
 
 Draws a set of horizontal and vertical line segments in the default 2D
 projection. Then computes their intersections using the line sweep
 algorithm, and  simulates the algorithm as it runs.
 
 */
#include <set>
#include <vector>
#include "geom.h"
#include "rtimer.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


using namespace std;

//Comparator for active structure, that sorts segements by their y-coordinates
struct yCoordinate {
    bool operator() (const segment2D& first, const segment2D& second) const{
        return first.end.y < second.end.y;
    }
};

//Comparator for auxiliary active structure containing only y-coordinate ints
struct yCoordinateInt{
    bool operator() (const int& first, const int& second) const{
        return first < second;
    }
};




GLfloat red[3] = {1.0, 0.0, 0.0};
GLfloat green[3] = {0.0, 1.0, 0.0};
GLfloat blue[3] = {0.0, 0.0, 1.0};
GLfloat black[3] = {0.0, 0.0, 0.0};
GLfloat white[3] = {1.0, 1.0, 1.0};
GLfloat gray[3] = {0.5, 0.5, 0.5};
GLfloat yellow[3] = {1.0, 1.0, 0.0};
GLfloat orange[3] = {1.0, 0.5, 0.0};
GLfloat magenta[3] = {1.0, 0.0, 1.0};
GLfloat cyan[3] = {0.0, 1.0, 1.0};

GLint fillmode = 0;



/* forward declarations of functions */
void display(void);
void keypress(unsigned char key, int x, int y);
void timerfunc();
void drawCircle(float cx, float cy, float r, int num_segments);


void initialize_segments_random();
void initialize_segments_horizontal();
void print_segments();

//renders the sweep line
void draw_sweep_line();
//renders the active structure
void draw_active_structure();
//renders the intersection points
void draw_intersection_points();





/* global variables */
const int WINDOWSIZE = 500;

int init_case = 0;
const int NB_TEST_CASES = 2;

//NOTE: all the structures below need to be global so that they can be rendered

//current position of sweep line
int sweep_line_x = 0;


//number of segments requested by user
int n;

//the array of  segments
vector<segment2D>  segments;


//the intersections points of the segments
vector<point2D> intpoints;

//the active structure that stores the segments intersecting the sweep line
multiset<segment2D,yCoordinate> as;

//the auxiliary active structure that stores the y-coordinates of the segments intersecting the sweep line
multiset<int,yCoordinateInt> asY;
//Interator for the active structures
multiset<int,yCoordinateInt>::iterator itlow,itup;

//the events
vector<event> events;

//Keeps track of last event looked at to prevent unnecessary repeat iterations
int lastEventScanned = 0;


//Generic openGL draw circle method
void drawCircle(float cx, float cy, float r, int num_segments)
{
   	float theta = 2 * 3.1415926 / float(num_segments);
    float c = cosf(theta);//precalculate the sine and cosine
    float s = sinf(theta);
    float t;
    
    float x = r;//we start at angle = 0
    float y = 0;
    
    glBegin(GL_LINE_LOOP);
    for(int ii = 0; ii < num_segments; ii++)
    {
        glVertex2f(x + cx, y + cy);//output vertex
        
        //apply the rotation matrix
        t = x;
        x = c * x - s * y;
        y = s * t + c * y;
    }
    glEnd();
}



/*Comparator for events vector*/
bool comp(const event& first, const event& second) { return first.eventXCoord < second.eventXCoord; }

/*Called to sort the events vector*/
void sortEvents() {
    sort(events.begin(), events.end(), comp);
}

/* Called before sweeping line begins moving to add all events from the 
 * generated line segments to the event vector.
 * Creates two events from a horizontal segment (start, end)
 * and one for a vertical segment. Events are indexed by their x-coordinate
 */
void creatEvents() {
    
    for (int i=0; i<segments.size(); i++) {
        event e1;
        event e2;
        segment2D seg = segments[i];
        if(seg.start.x != seg.end.x){
            //Segment start event
            e1.segment = seg;
            e1.eventType = 'S';
            e1.eventXCoord = seg.start.x;
            events.push_back(e1);
            //Segment end event
            e2.segment = seg;
            e2.eventType = 'E';
            e2.eventXCoord = seg.end.x;
            events.push_back(e2);
            
        }else{//Else segement is vertical so create one event
            e1.segment = seg;
            e1.eventType = 'V';
            e1.eventXCoord = seg.start.x;
            events.push_back(e1);
        }
        
    }
    
}

void timerfunc() {
    
    //Iterate from the last element/event looked at until it reaches elements/events that have an equal x-coordinate to the current sweep line position
    int i;
    for (i = lastEventScanned; events[i].eventXCoord == sweep_line_x; i++) {

        //Get event that the sweep line is currently on
        event e = events[i];
        //If event is the start of a horizontal line segement
        if(e.eventType == 'S'){
            //Add to active structure
            as.insert(e.segment);
            asY.insert(e.segment.start.y);
            
        }else if (e.eventType == 'E'){//If event is the end of a horizontal line segement
            
            asY.erase(asY.find(e.segment.start.y));
            as.erase(as.find(e.segment));
        }else{//If event is the a vertical line segement
            
            int start = e.segment.start.y;
            int end = e.segment.end.y;
            
            //Swap to make sure start start value is always a lower y
            if (start > end) {
                int temp = start;
                start = end;
                end= temp;
            }
            
            //Get iterators for multiset from two y-coord bounds
            itlow = asY.lower_bound(start);
            itup = asY.upper_bound(end);
            
            //Using iterators add all intersections formed from this vertical line and appropriate horizontal lines in the active structure.
            for (multiset<int,yCoordinateInt>::iterator it = itlow; it != itup; ++it){
                printf("Intersection: (%i,%d)\n",e.segment.start.x,*it);
                point2D intersect;
                intersect.x = e.segment.start.x;
                intersect.y = *it;
                intpoints.push_back(intersect);
                
            }
        }
    }
    
    glutPostRedisplay();
    sweep_line_x++;
    //Set next starting location for loop
    lastEventScanned = i;
    
}

//Draw all the elements in the active structure
void draw_active_structure() {
    
    //set color
    glColor3fv(orange);
    
    for (multiset<segment2D,yCoordinate>::iterator it = as.begin(); it != as.end(); ++it){
        glBegin(GL_LINES);
        glVertex2f(it->start.x, it->start.y);
        glVertex2f(it->end.x, it->end.y);
        glEnd();
    }
    
}


//Draw all the elements in intpoints (i.e draw a circle around all identified intersections)
void draw_intersection_points() {
    //set color
    glColor3fv(white);
    for (int i =0; i < intpoints.size(); i++){
        drawCircle(intpoints[i].x, intpoints[i].y,1,20);
    }
}





/* ****************************** */
int main(int argc, char** argv) {
    
    //read number of points from user
    if (argc!=2) {
        printf("usage: viewPoints <nbPoints>\n");
        exit(1);
    }
    n = atoi(argv[1]);
    printf("you entered n=%d\n", n);
    assert(n >0);
    
    initialize_segments_random();
    print_segments();
    creatEvents();
    sortEvents();
    
    
    /* initialize GLUT  */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WINDOWSIZE, WINDOWSIZE);
    glutInitWindowPosition(100,100);
    glutCreateWindow(argv[0]);
    
    /* register callback functions */
    glutDisplayFunc(display);
    glutKeyboardFunc(keypress);
    glutIdleFunc(timerfunc);
    
    /* init GL */
    /* set background color black*/
    glClearColor(0.3,0.3, 0.3, 0.3);
    /* here we can enable depth testing and double buffering and so
     on */
    
    
    /* give control to event handler */
    glutMainLoop();
    
    return 0;
}
/* ************************************************** */
void initialize_segments() {
    
    switch (init_case)  {
            
        case 0:
            initialize_segments_random();
            break;
            
        case 1:
            initialize_segments_horizontal();
            break;
            
        default:
            initialize_segments_random();
    }
    
    init_case = (init_case+1) % NB_TEST_CASES;
    return;
}




/* ************************************************** */
void initialize_segments_horizontal() {
    
    int i;
    point2D a,b;
    segment2D s;
    
    //clear the vector
    segments.clear();
    
    //a long horizontal segment
    a.x = 1;
    a.y = WINDOWSIZE/2;
    b.x = WINDOWSIZE - 10;
    b.y = a.y;
    
    s.start = a; s.end = b;
    segments.push_back(s);
    
    //n-1 vertical segments
    for (i=0; i<n-1; i++) {
        
        a.x = i*WINDOWSIZE/n;
        a.y = WINDOWSIZE/2 - random() % ((int)(.4*WINDOWSIZE));
        b.x = a.x;
        b.y = WINDOWSIZE/2 + random() % ((int)(.4*WINDOWSIZE));
        s.start = a; s.end = b;
        segments.push_back(s);
    }
    
}


/* ****************************** */
/* initialize  the array of points stored in global variable points[] with random points */
void initialize_segments_random() {
    
    //clear the vector
    segments.clear();
    
    int i;
    point2D a, b;
    segment2D s;
    for (i=0; i<n; i++) {
        if (random()%2 == 0) {
            //horizontal segment
            a.x = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE));
            a.y =  (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE));
            b.y = a.y;
            b.x = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE));
            
            if (a.x < b.x) {
                s.start = a; s.end = b;
            } else {
                s.start = b; s.end = a;
            }
            
        } else {
            //vertical segment
            a.x = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE));
            b.x = a.x;
            a.y = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE));
            b.y = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE));
            
            if (a.y < b.y) {
                s.start = a; s.end = b;
            } else {
                s.start = b; s.end = a;
            }
        }
        
        //insert the segment in the array of segments
        segments.push_back (s);
    } //for i
}



/* ************************************************** */
void print_segments() {
    for (int i=0; i<segments.size(); i++) {
        printf("segment %d: [(%d,%d), (%d,%d)]\n",
               i, segments[i].start.x, segments[i].start.y, segments[i].end.x, segments[i].end.y);
        
    }
    
}



/*Draw the segments stored in global variable segments */
void draw_segments(){
    
    //set color
    glColor3fv(yellow);
    int i;
    for (i=0; i<segments.size(); i++) {
        glBegin(GL_LINES);
        glVertex2f(segments[i].start.x, segments[i].start.y);
        glVertex2f(segments[i].end.x, segments[i].end.y);
        glEnd();
    }
    
}


//draw the sweep line
void draw_sweep_line() {
    
    //sweep line color
    glColor3fv(red);
    
    //the current position of sweep line is sweep_line_x; assume it's a
    //segment from y=0 to y=windowsize;
    glBegin(GL_LINES);
    glVertex2f(sweep_line_x, 0);
    glVertex2f(sweep_line_x, WINDOWSIZE);
    glEnd();
}

//draw a segment with current color
void draw_segment(segment2D s) {
    glBegin(GL_LINES);
    glVertex2f(s.start.x, s.start.y);
    glVertex2f(s.end.x, s.end.y);
    glEnd();
}




/* ****************************** */
void display(void) {
    
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); //clear the matrix
    
    
    /* The default GL window is [-1,1]x[-1,1] with the origin in the
     center.
     
     The points are in the range (0,0) to (WINSIZE,WINSIZE), so they
     need to be mapped to [-1,1]x [-1,1]x */
    glScalef(2.0/WINDOWSIZE, 2.0/WINDOWSIZE, 1.0);
    glTranslatef(-WINDOWSIZE/2, -WINDOWSIZE/2, 0);
    
    draw_segments();
    draw_active_structure();
    draw_intersection_points();
    draw_sweep_line();
    
    
    /* execute the drawing commands */
    glFlush();
}



/* ****************************** */
void keypress(unsigned char key, int x, int y) {
    switch(key) {
        case 'q':
            exit(0);
            break;
            
        case 'i':
            initialize_segments();
            glutPostRedisplay();
            break;
    }
}


/* Handler for window re-size event. Called back when the window first appears and
 whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
    
    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);
    
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset
    gluOrtho2D(0.0, (GLdouble) width, 0.0, (GLdouble) height);
}




