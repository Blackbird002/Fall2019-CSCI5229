/*
Use keyboard arrow keys to spin the axis lines
(left/right about the y axis)
(up/down about the x axis)

Key bindings:
  c     - don't draw cube
  ESC   - Exit
Press 

Callbacks in C:
-function that is passed as an argument to another function

Ternary Operator
condition ? value_if_true : value_if_false

Compile with (Windows):
gcc -O3 -Wall -o 3DAxis 3DAxis.c -lglut32cu -lglu32 -lopengl32
*/

#include <stdio.h>
#include <stdarg.h>
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// ----------------------------------------------------------
// Global Variables
// ----------------------------------------------------------
int rotate_y = -45;
int rotate_x = 45;
double dim=2;   // Dimension of orthogonal box
int cube = 1;

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
void drawCube();
void init();
void key(unsigned char ch,int x,int y);

// ----------------------------------------------------------
// special() Callback function
// ----------------------------------------------------------
static void special(int k, int x, int y)
{
  switch (k) {
  case GLUT_KEY_UP:
    rotate_x -= 1.0;
    break;
  case GLUT_KEY_DOWN:
    rotate_x += 1.0;
    break;
  case GLUT_KEY_LEFT:
    rotate_y -= 1.0;
    break;
  case GLUT_KEY_RIGHT:
    rotate_y += 1.0;
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

// ----------------------------------------------------------
// key() Callback function
// ----------------------------------------------------------
void key(unsigned char ch,int x,int y){
  //  Exit on ESC
  if (ch == 27)
    exit(0);
  //  Reset view angle
  else if (ch == 'c'){
    cube = (cube == 1) ? 0 : 1;
  }
    
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
  if(cube == 1)
    drawCube();

  //  Display rotation angles
  glColor3f(1,1,1);
  glWindowPos2i(5,5);
  Print("Angle X= %d",rotate_x);

  if(cube == 1){
    Print("    Cube on!");
  }

  glWindowPos2i(5,25);
  Print("Angle Y= %d",rotate_y);

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
  glVertex3f(1, 0, 0);

  // Y Axis line
  glColor3f(0, 1, 0); //Green
  glVertex3f(0, 0, 0);
  glVertex3f(0, 1, 0);

  // Z Axis line
  glColor3f(0, 0, 1); //Blue
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, 1);
  glEnd();
}

void drawAxisLabels(){
  glColor3f(1, 1, 1); //White
  glRasterPos3d(1.05,0,0);
  Print("X");
  glRasterPos3d(0,1.05,0);
  Print("Y");
  glRasterPos3d(0,0,1.05);
  Print("Z");
}

void drawCube(){
  glColor3f(1,1,1);
  glPointSize(10);
  glBegin(GL_LINES);
  glVertex3d(0.5,0.5,0.5);
  glVertex3d(0.5,0.5,-0.5);

  glVertex3d(0.5,0.5,-0.5);
  glVertex3d(0.5,-0.5,-0.5);

  glVertex3d(0.5,-0.5,-0.5);
  glVertex3d(0.5,-0.5,0.5);

  glVertex3d(0.5,-0.5,0.5);
  glVertex3d(0.5,0.5,0.5);

  glVertex3d(0.5,0.5,0.5);
  glVertex3d(-0.5,0.5,0.5);

  glVertex3d(-0.5,0.5,0.5);
  glVertex3d(-0.5,-0.5,0.5);

  glVertex3d(-0.5,-0.5,0.5);
  glVertex3d(0.5,-0.5,0.5);

  glVertex3d(-0.5,0.5,0.5);
  glVertex3d(-0.5,0.5,-0.5);

  glVertex3d(-0.5,0.5,-0.5);
  glVertex3d(0.5,0.5,-0.5);

  glVertex3d(-0.5,0.5,-0.5);
  glVertex3d(-0.5,-0.5,-0.5);

  glVertex3d(-0.5,-0.5,-0.5);
  glVertex3d(-0.5,-0.5,0.5);

  glVertex3d(-0.5,-0.5,-0.5);
  glVertex3d(0.5,-0.5,-0.5);
  glEnd();
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
  glutInitWindowPosition(0,0);

  // Set window size
  glutInitWindowSize(640,480);

  // Create window
  glutCreateWindow("X, Y, Z Axis");

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