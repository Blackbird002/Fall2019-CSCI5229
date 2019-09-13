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
gcc -O3 -Wall -o lorenz lorenz.c -lglut32cu -lglu32 -lopengl32

The reason I'm generating an array of coordinates is that I want
to use GL_LINE_STRIP - requence of line segments
*/

#include <stdio.h>
#include <stdarg.h>
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <stdbool.h>
#endif

// ----------------------------------------------------------
// Global Variables
// ----------------------------------------------------------
int rotate_y = -45;
int rotate_x = 45;
double dim=50;   // Dimension of orthogonal box

/*  Lorenz Parameters  */
double sigma  = 10.0;
double beta  = (8.0/3.0);
double rho  = 28.0;

bool changeS = false;
bool changeB = false;
bool changeR = false;

GLdouble lorenzPoints[100000][3];

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
void genLorenz();
void drawLorenz();
void theMenu(int value);
void redoLerenz();

// ----------------------------------------------------------
// special() Callback function
// ----------------------------------------------------------
static void special(int k, int x, int y)
{
  switch (k) {
  case GLUT_KEY_UP:
    rotate_x -= 2.0;
    break;
  case GLUT_KEY_DOWN:
    rotate_x += 2.0;
    break;
  case GLUT_KEY_LEFT:
    rotate_y -= 2.0;
    break;
  case GLUT_KEY_RIGHT:
    rotate_y += 2.0;
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
// theMenu() Callback function
// ----------------------------------------------------------
void theMenu(int value){
  if(value == 1){
    changeS = true;
    changeB = false;
    changeR = false;
  }else if(value == 2){
    changeS = false;
    changeB = true;
    changeR = false;
  }else if(value == 3){
    changeS = false;
    changeB = false;
    changeR = true;
  }else if(value == 4){
    exit(0);
  }
}

// ----------------------------------------------------------
// key() Callback function
// ----------------------------------------------------------
void key(unsigned char ch,int x,int y){
  //  Exit on ESC
  if (ch == 27){
    exit(0);
  }else if(ch == '='){
    if(changeS)
      sigma += 0.1;
    else if(changeB)
      beta += 0.1;
    else if(changeR)
      rho += 0.1;
  }else if(ch == '-'){
    if(changeS)
      sigma -= 0.1;
    else if(changeB)
      beta -= 0.1;
    else if(changeR)
      rho -= 0.1;
  }
  redoLerenz();
    
  //  Tell GLUT it is necessary to redisplay the scene
  glutPostRedisplay();
}

// ----------------------------------------------------------
// init() function
// ----------------------------------------------------------
void init(){
  //Right click menu
  int menuId = glutCreateMenu(theMenu);
  glutSetMenu(menuId);
  glutAddMenuEntry("Change Sigma", 1);
  glutAddMenuEntry("Change Beta", 2);
  glutAddMenuEntry("Change Rho", 3);
  glutAddMenuEntry("Quit", 4);

  // Let the menu respond on the right mouse button
	glutAttachMenu(GLUT_RIGHT_BUTTON);

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
  drawLorenz();

  //  Display rotation angles
  glColor3f(1,1,1);
  glWindowPos2i(5,5);
  //Print("Angle X= %d",rotate_x);
  Print("Sigma = %1.1f  Beta = %1.1f  Rho = %1.1f", sigma, beta, rho);

  //glWindowPos2i(5,25);
  //Print("Angle Y= %d",rotate_y);

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
    glVertex3f(50, 0, 0);

    // Y Axis line
    glColor3f(0, 1, 0); //Green
    glVertex3f(0, 0, 0);
    glVertex3f(0, 50, 0);

    // Z Axis line
    glColor3f(0, 0, 1); //Blue
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 50);
  glEnd();
}

void drawAxisLabels(){
  glColor3f(1, 1, 1); //White
  glRasterPos3d(50.05,0,0);
  Print("X");
  glRasterPos3d(0,50.05,0);
  Print("Y");
  glRasterPos3d(0,0,50.05);
  Print("Z");
}

void drawLorenz(){
  glBegin(GL_LINE_STRIP);
    glLineWidth(2);
    glColor3f(1,1,1);
    for (int i=0;i<100000;i++){
      glVertex3d(lorenzPoints[i][0], lorenzPoints[i][1], lorenzPoints[i][2]);
    }
  glEnd();
}

void redoLerenz(){
  genLorenz();
  drawLorenz();
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

void genLorenz(){
  /*  Coordinates  */
  double x = 1.0;
  double y = 1.0;
  double z = 1.0;
  double dx = 0;
  double dy = 0;
  double dz = 0;

  /*  Time step  */
  double dt = 0.001;

  /*
  *  Integrate 50,000 steps (50 time units with dt = 0.001)
  *  Explicit Euler integration
  */
  for (int i=0;i<100000;i++){
    dx = sigma*(y-x);
    dy = x*(rho-z)-y;
    dz = x*y - beta*z;
    x += dt*dx;
    y += dt*dy;
    z += dt*dz;

    // Bulid the array of points
    lorenzPoints[i][0] = x;
    lorenzPoints[i][1] = y;
    lorenzPoints[i][2] = z;


    //printf("%5d %8.3f %8.3f %8.3f\n",i+1,x,y,z);
  }
}

// ----------------------------------------------------------
// main() function
// ----------------------------------------------------------
int main(int argc, char* argv[]){
  genLorenz();

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
  glutCreateWindow("Lorens Model");

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