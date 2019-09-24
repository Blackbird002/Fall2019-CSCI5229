/*
Use keyboard arrow keys to spin the axis lines
(left/right about the y axis)
(up/down about the x axis)

Key bindings:
  ESC   - Exit

Callbacks in C:
-function that is passed as an argument to another function

Ternary Operator
condition ? value_if_true : value_if_false

Compile with (Windows):
gcc -O3 -Wall -o HW#3 HW#3.c -lglut32cu -lglu32 -lopengl32
*/

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.1415927/180))
#define Sin(x) (sin((x)*3.1415927/180))

// ----------------------------------------------------------
// Global Variables
// ----------------------------------------------------------
int rotate_y = -45;
int rotate_x = 45;
double dim=70;   // Dimension of orthogonal box

// ----------------------------------------------------------
// Function Prototypes
// ----------------------------------------------------------
void display();
void reshape(int,int);
void idle();
void Print(const char* format , ...);
static void special(int k, int x, int y);
void drawAxisLines();
void drawAxisLabels();
void init();
void key(unsigned char ch,int x,int y);
void errorCheck(char* where);
static void sphere2(double x,double y,double z,double r);
static void ArtemisSpaceBomber(double x,double y,double z,
                       double dx,double dy,double dz,
                       double ux,double uy, double uz, double scale);
static void XB70Bomber(double x,double y,double z,
                       double dx,double dy,double dz,
                       double ux,double uy, double uz, double scale);

// ----------------------------------------------------------
// special() Callback function
// ----------------------------------------------------------
static void special(int k, int x, int y)
{
  switch (k) {
  case GLUT_KEY_UP:
    rotate_x -= 5.0;
    break;
  case GLUT_KEY_DOWN:
    rotate_x += 5.0;
    break;
  case GLUT_KEY_LEFT:
    rotate_y -= 5.0;
    break;
  case GLUT_KEY_RIGHT:
    rotate_y += 5.0;
    break;
  default:
    return;
  }
  //  Keep angles to +/-360 degrees
  rotate_x %= 360;
  rotate_y %= 360;

  // Marks the current window as needing to be redisplayed
  glutPostRedisplay();
}

void errorCheck(char* where){
  int err = glGetError();
  if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
}

/*
 *  Draw vertex in polar coordinates
 */
static void Vertex(double th,double ph)
{
  glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
}

/*
 *  Draw a sphere (version 2)
 *     at (x,y,z)
 *     radius (r)
 */
static void sphere2(double x,double y,double z,double r)
{
  const int d=5;
  int th,ph;

  //  Save transformation
  glPushMatrix();
  //  Offset and scale
  glTranslated(x,y,z);
  glScaled(r,r,r);

  //  Latitude bands
  for (ph=-90;ph<90;ph+=d)
  {
    glBegin(GL_QUAD_STRIP);
    for (th=0;th<=360;th+=d)
    {
        Vertex(th,ph);
        Vertex(th,ph+d);
    }
    glEnd();
  }

  //  Undo transformations
  glPopMatrix();
}

/*
 *  Draw solid space bomber
 *    at (x,y,z)
 *    nose towards (dx,dy,dz)
 *    UP vector (ux,uy,uz) 
 */
static void ArtemisSpaceBomber(double x,double y,double z,
                       double dx,double dy,double dz,
                       double ux,double uy, double uz, double scale)
{
  // Dimensions used to size airplane
  const double wid = 1;   //The "width of the plane's "Fuselage"
  const double shipBowXfront = 10;    //X of front nose
  const double shipBowXend = 6;     //X of end nose
  const double shipSternX = -10;    //X end of ship
  const double cockpitX = 5;        //X center of cockpit

  const double missleBayXfront = 2;
  const double missleBayXend = -5;
  const double missleBaywid = 4;
  const double missleBayHight = 3;

  //  Unit vector in direction of flght
  double D0 = sqrt(dx*dx+dy*dy+dz*dz);
  double X0 = dx/D0;
  double Y0 = dy/D0;
  double Z0 = dz/D0;

  //  Unit vector in "up" direction
  double D1 = sqrt(ux*ux+uy*uy+uz*uz);
  double X1 = ux/D1;
  double Y1 = uy/D1;
  double Z1 = uz/D1;

  //  Cross product gives the third vector
  double X2 = Y0*Z1-Y1*Z0;
  double Y2 = Z0*X1-Z1*X0;
  double Z2 = X0*Y1-X1*Y0;

  //  Rotation matrix
  double mat[16];
  mat[0] = X0;   mat[4] = X1;   mat[ 8] = X2;   mat[12] = 0;
  mat[1] = Y0;   mat[5] = Y1;   mat[ 9] = Y2;   mat[13] = 0;
  mat[2] = Z0;   mat[6] = Z1;   mat[10] = Z2;   mat[14] = 0;
  mat[3] =  0;   mat[7] =  0;   mat[11] =  0;   mat[15] = 1;

  //  Save current transforms
  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x,y,z);
  glMultMatrixd(mat);
  glScaled(scale,scale,scale);

  //  Front Nose (4 sided)
  glColor3f(0,0,1);
  glBegin(GL_TRIANGLES);
  glVertex3d(shipBowXfront, 0.0, 0.0);
  glVertex3d(shipBowXend, wid, wid);
  glVertex3d(shipBowXend,-wid, wid);

  glVertex3d(shipBowXfront, 0.0, 0.0);
  glVertex3d(shipBowXend, wid,-wid);
  glVertex3d(shipBowXend,-wid,-wid);

  glVertex3d(shipBowXfront, 0.0, 0.0);
  glVertex3d(shipBowXend, wid, wid);
  glVertex3d(shipBowXend, wid,-wid);

  glVertex3d(shipBowXfront, 0.0, 0.0);
  glVertex3d(shipBowXend,-wid, wid);
  glVertex3d(shipBowXend,-wid,-wid);
  glEnd();

  //Front tip
  glBegin(GL_LINES);
    glColor3f(0, 1, 0); 
    glVertex3f(shipBowXfront, 0, 0);
    glVertex3f(shipBowXfront+2, 0, 0);
    glVertex3f(shipBowXfront+2, 0, 0);
    glVertex3f(shipBowXfront+2, 1, 0);
    glVertex3f(shipBowXfront+2, 0, 0);
    glVertex3f(shipBowXfront+2, 0, 1);
    glVertex3f(shipBowXfront+2, 0, 0);
    glVertex3f(shipBowXfront+2, -1, 0);
    glVertex3f(shipBowXfront+2, 0, 0);
    glVertex3f(shipBowXfront+2, 0, -1);
  glEnd();

  //  Fuselage (square tube)
  glBegin(GL_QUADS);
  glColor3f(1,0,0);
  glVertex3d(shipBowXend, wid, wid);
  glVertex3d(shipBowXend,-wid, wid);
  glVertex3d(shipSternX,-wid, wid);
  glVertex3d(shipSternX, wid, wid);

  glVertex3d(shipBowXend, wid,-wid);
  glVertex3d(shipBowXend,-wid,-wid);
  glVertex3d(shipSternX,-wid,-wid);
  glVertex3d(shipSternX, wid,-wid);

  glVertex3d(shipBowXend, wid, wid);
  glVertex3d(shipBowXend, wid,-wid);
  glVertex3d(shipSternX, wid,-wid);
  glVertex3d(shipSternX, wid, wid);

  glVertex3d(shipBowXend,-wid, wid);
  glVertex3d(shipBowXend,-wid,-wid);
  glVertex3d(shipSternX,-wid,-wid);
  glVertex3d(shipSternX,-wid, wid);

  glVertex3d(shipSternX,-wid, wid);
  glVertex3d(shipSternX, wid, wid);
  glVertex3d(shipSternX, wid,-wid);
  glVertex3d(shipSternX,-wid,-wid);
  glEnd();

  //  Cockpit
  glColor3f(0,1,0);
  sphere2(cockpitX,1,0,1);

  //Right Canard
  glColor3f(1,0.5,0);
  glBegin(GL_POLYGON);
    glVertex3d(3,0,wid);
    glVertex3d(6,0,wid);
    glVertex3d(5,0,wid+1);
    glVertex3d(4,0,wid+1);
  glEnd();

  //Left Canard
  glColor3f(1,0.5,0);
  glBegin(GL_POLYGON);
    glVertex3d(3,0,-wid);
    glVertex3d(6,0,-wid);
    glVertex3d(5,0,-wid-1);
    glVertex3d(4,0,-wid-1);
  glEnd();

  //  Right missle bay
  glColor3f(0,0,1);
  glBegin(GL_QUADS);
    //Outside side
    glVertex3d(missleBayXfront, missleBayHight, missleBaywid+wid);
    glVertex3d(missleBayXfront,-missleBayHight, missleBaywid+wid);
    glVertex3d(missleBayXend,-missleBayHight, missleBaywid+wid);
    glVertex3d(missleBayXend, missleBayHight, missleBaywid+wid);

    //Inside side
    glVertex3d(missleBayXend, missleBayHight, wid);
    glVertex3d(missleBayXend, -missleBayHight, wid);
    glVertex3d(missleBayXfront, -missleBayHight, wid);
    glVertex3d(missleBayXfront, missleBayHight, wid);

    //Top side
    glVertex3d(missleBayXend, missleBayHight, wid);
    glVertex3d(missleBayXend, missleBayHight, missleBaywid + wid);
    glVertex3d(missleBayXfront, missleBayHight, missleBaywid + wid);
    glVertex3d(missleBayXfront, missleBayHight, wid);

    //Bottom side
    glVertex3d(missleBayXend, -missleBayHight, wid);
    glVertex3d(missleBayXend, -missleBayHight, missleBaywid + wid);
    glVertex3d(missleBayXfront, -missleBayHight, missleBaywid + wid);
    glVertex3d(missleBayXfront, -missleBayHight, wid);

    //Front face of missle bay
    glVertex3d(missleBayXfront,missleBayHight,missleBaywid + wid);
    glVertex3d(missleBayXfront,-missleBayHight,missleBaywid + wid);
    glVertex3d(missleBayXfront,-missleBayHight,wid);
    glVertex3d(missleBayXfront,missleBayHight,wid);

    //Back face of missle bay
    glVertex3d(missleBayXend,missleBayHight,missleBaywid + wid);
    glVertex3d(missleBayXend,-missleBayHight,missleBaywid + wid);
    glVertex3d(missleBayXend,-missleBayHight,wid);
    glVertex3d(missleBayXend,missleBayHight,wid);
  glEnd();

  //  Left missle bay
  glBegin(GL_QUADS);
    glVertex3d(missleBayXfront, missleBayHight, -missleBaywid-wid);
    glVertex3d(missleBayXfront,-missleBayHight, -missleBaywid-wid);
    glVertex3d(missleBayXend,-missleBayHight, -missleBaywid-wid);
    glVertex3d(missleBayXend, missleBayHight, -missleBaywid-wid);

    glVertex3d(missleBayXend, missleBayHight, -wid);
    glVertex3d(missleBayXend, -missleBayHight, -wid);
    glVertex3d(missleBayXfront, -missleBayHight, -wid);
    glVertex3d(missleBayXfront, missleBayHight, -wid);


    glVertex3d(missleBayXend, missleBayHight, -wid);
    glVertex3d(missleBayXend, missleBayHight, -missleBaywid - wid);
    glVertex3d(missleBayXfront, missleBayHight, -missleBaywid - wid);
    glVertex3d(missleBayXfront, missleBayHight, -wid);

    glVertex3d(missleBayXend, -missleBayHight, -wid);
    glVertex3d(missleBayXend, -missleBayHight, -missleBaywid - wid);
    glVertex3d(missleBayXfront, -missleBayHight, -missleBaywid - wid);
    glVertex3d(missleBayXfront, -missleBayHight, -wid);

    //Front face of missle bay
    glVertex3d(missleBayXfront,missleBayHight,-missleBaywid - wid);
    glVertex3d(missleBayXfront,-missleBayHight,-missleBaywid - wid);
    glVertex3d(missleBayXfront,-missleBayHight,-wid);
    glVertex3d(missleBayXfront,missleBayHight,-wid);

    //Back face of missle bay
    glVertex3d(missleBayXend,missleBayHight,-missleBaywid - wid);
    glVertex3d(missleBayXend,-missleBayHight,-missleBaywid - wid);
    glVertex3d(missleBayXend,-missleBayHight,-wid);
    glVertex3d(missleBayXend,missleBayHight,-wid);
  glEnd();

  //  Vertical tail (plane triangle)
  glColor3f(0,1,0);
  glBegin(GL_POLYGON);
  glVertex3d(shipSternX, 6.0, 0.0);
  glVertex3d(shipSternX, 1.0, 0.0);
  glVertex3d(missleBayXend, 1.0, 0.0);
  glEnd();

  //  Right Elevator
  glColor3f(0,1,0);
  glBegin(GL_POLYGON);
  glVertex3d(shipSternX,-1.0, 1.0);
  glVertex3d(shipSternX,-4.0, 3.0);
  glVertex3d(missleBayXend-2,-1.0, 1.0);
  glEnd();

  //  Left Elevator
  glColor3f(0,1,0);
  glBegin(GL_POLYGON);
  glVertex3d(shipSternX,-1.0, -1.0);
  glVertex3d(shipSternX,-4.0, -3.0);
  glVertex3d(missleBayXend-2,-1.0, -1.0);
  glEnd();

  //  Undo transformations
  glPopMatrix();
}

static void XB70Bomber(double x,double y,double z,
                       double dx,double dy,double dz,
                       double ux,double uy, double uz, double scale)
{
  const double shipFrontNoseX = 30.0;
  const double shipRearNoseX = 22.0;
  const double shipRear = -10.0;
  const double cockpitLocX = 0.0;
  const double wingsFrontX = 0.0;
  const double wingsRearX = 0.0;
  const double shipWidth = 2.0;
  const double shipFuselageHeight = 1.0

  //  Unit vector in direction of flght
  double D0 = sqrt(dx*dx+dy*dy+dz*dz);
  double X0 = dx/D0;
  double Y0 = dy/D0;
  double Z0 = dz/D0;

  //  Unit vector in "up" direction
  double D1 = sqrt(ux*ux+uy*uy+uz*uz);
  double X1 = ux/D1;
  double Y1 = uy/D1;
  double Z1 = uz/D1;

  //  Cross product gives the third vector
  double X2 = Y0*Z1-Y1*Z0;
  double Y2 = Z0*X1-Z1*X0;
  double Z2 = X0*Y1-X1*Y0;

  //  Rotation matrix
  double mat[16];
  mat[0] = X0;   mat[4] = X1;   mat[ 8] = X2;   mat[12] = 0;
  mat[1] = Y0;   mat[5] = Y1;   mat[ 9] = Y2;   mat[13] = 0;
  mat[2] = Z0;   mat[6] = Z1;   mat[10] = Z2;   mat[14] = 0;
  mat[3] =  0;   mat[7] =  0;   mat[11] =  0;   mat[15] = 1;

  //  Save current transforms
  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x,y,z);
  glMultMatrixd(mat);
  glScaled(scale,scale,scale);

  //  Front Nose (4 sided)
  glColor3f(0,0,1);
  glBegin(GL_TRIANGLES);
    glVertex3d(shipFrontNoseX, 0.0, 0.0);
    glVertex3d(shipRearNoseX, shipFuselageHeight, shipWidth);
    glVertex3d(shipRearNoseX,-shipFuselageHeight, shipWidth);

    glVertex3d(shipFrontNoseX, 0.0, 0.0);
    glVertex3d(shipRearNoseX, shipFuselageHeight,-shipWidth);
    glVertex3d(shipRearNoseX,-shipFuselageHeight,-shipWidth);

    glVertex3d(shipFrontNoseX, 0.0, 0.0);
    glVertex3d(shipRearNoseX, shipFuselageHeight, shipWidth);
    glVertex3d(shipRearNoseX, shipFuselageHeight,-shipWidth);

    glVertex3d(shipFrontNoseX, 0.0, 0.0);
    glVertex3d(shipRearNoseX,-shipFuselageHeight, shipWidth);
    glVertex3d(shipRearNoseX,-shipFuselageHeight,-shipWidth);
  glEnd();

  //  Undo transformations
  glPopMatrix();

}

// ----------------------------------------------------------
// key() Callback function
// ----------------------------------------------------------
void key(unsigned char ch,int x,int y){
  //  Exit on ESC
  if (ch == 27)
    exit(0);
    
  //  Tell GLUT it is necessary to redisplay the scene
  glutPostRedisplay();
}

// ----------------------------------------------------------
// init() function
// ----------------------------------------------------------
void init(){

  //  Clear values for the color buffers (background color)
  glClearColor(0.1f, 0.1f, 0.1f, 0.0);  //Dark Gray
}

// ----------------------------------------------------------
// display() Callback function
// ----------------------------------------------------------
void display(){

  //  Clear screen and Z-buffer
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  // Reset transformations
  glLoadIdentity();

  // Rotate 
  glRotatef(rotate_x, 1.0, 0.0, 0.0 );  //x coordinate
  glRotatef(rotate_y, 0.0, 1.0, 0.0 );  //y coordinate

  drawAxisLines();
  drawAxisLabels();
  //ArtemisSpaceBomber( 0, 0, 0 , 1,0,0 , 0, 1,0,1);
  //ArtemisSpaceBomber( -20, 0, 20 , 1,0,0 , 0, 1,0,1);
  //ArtemisSpaceBomber( -20, 0, -20 , 1,0,0 , 0, 1,0,1);
  //ArtemisSpaceBomber( -50, 0, 0 , 0,0,1 , 0, 1,0,1.5);
  //ArtemisSpaceBomber( 30, 10, 5 , 0,1,0 , 1,0,0,1.5);
  XB70Bomber(0,0,0 , 1,0,0, 0,1,0,1);

  //  Display rotation angles
  glColor3f(1,1,1);
  glWindowPos2i(5,5);
  Print("Angle X= %d",rotate_x);

  glWindowPos2i(5,25);
  Print("Angle Y= %d",rotate_y);

  //Check for errors
  errorCheck("display");

  // Force the execution of queued commands
  glFlush();

  //swaps the buffers of the current window if double buffered
  glutSwapBuffers();
}

void drawAxisLines(){
  // X Axis line
  glBegin(GL_LINES);
  glColor3f(1, 0, 0); //Red
  glVertex3f(0, 0, 0);
  glVertex3f(25, 0, 0);

  // -X Line
  glVertex3f(0, 0, 0);
  glVertex3f(-25, 0, 0);

  // Y Axis line
  glColor3f(0, 1, 0); //Green
  glVertex3f(0, 0, 0);
  glVertex3f(0, 25, 0);

  // -Y line
  glVertex3f(0, 0, 0);
  glVertex3f(0, -25, 0);

  // Z Axis line
  glColor3f(0, 0, 1); //Blue
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, 25);

  // -Z line
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, -25);

  glEnd();
}

void drawAxisLabels(){
  glColor3f(1, 1, 1); //White
  glRasterPos3d(25.05,0,0);
  Print("X");
  glRasterPos3d(-25.05,0,0);
   Print("-X");
  glRasterPos3d(0,25.05,0);
  Print("Y");
  glRasterPos3d(0,-25.05,0);
  Print("-Y");
  glRasterPos3d(0,0,25.05);
  Print("Z");
  glRasterPos3d(0,0,-25.05);
  Print("-Z");
}

// Convenience function for text
#define LEN 8192  //  Maximum amount of text
void Print(const char* format , ...)
{
  char    buf[LEN]; // Text storage
  char*   ch=buf;   // Text pointer
  //  Create text to be display
  va_list args;
  va_start(args,format);
  vsnprintf(buf,LEN,format,args);
  va_end(args);
  //  Display text string
  while (*ch)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

// This function is called by GLUT when idle
void idle(){}

// This function is called by GLUT when the window is resized
void reshape(int width,int height)
{
   //  Calculate width to height ratio
   double w2h = (height>0) ? (double)width/height : 1;
   //  Set viewport as entire window
   glViewport(0,0, width,height);
   //  Select projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Set projection to identity
   glLoadIdentity();
   //  Orthogonal projection:  unit cube adjusted for aspect ratio
   glOrtho(-dim*w2h,+dim*w2h, -dim,+dim, -dim,+dim);
   //  Select model view matrix
   glMatrixMode(GL_MODELVIEW);
   //  Set model view to identity
   glLoadIdentity();
}

// ----------------------------------------------------------
// main() function
// ----------------------------------------------------------
int main(int argc, char* argv[]){

  //  Initialize GLUT and process user parameters
  glutInit(&argc,argv);

  //  Request double buffered true color window with Z-buffer
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

  // Set window position 
  // Center of Screen -> (source: Kornel Kisielewicz StackOverflow)
  glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-640)/2,
                       (glutGet(GLUT_SCREEN_HEIGHT)-480)/2);

  // Set window size
  glutInitWindowSize(800,600);

  // Create window
  glutCreateWindow("3D Scene");

  //  Enable Z-buffer depth test
  glEnable(GL_DEPTH_TEST);

  // Callback functions
  glutDisplayFunc(display);
  glutIdleFunc(idle);
  glutReshapeFunc(reshape);  //Reshaping
  glutSpecialFunc(special); 
  glutKeyboardFunc(key);

  // Initialization
  init();

  //  Pass control to GLUT for events
  glutMainLoop();

  // ANSI C requires main to return int
  return 0;
}