/*
Riad Shash (Ray)
CSCI 5229

Key bindings:
  ESC   - Exit

  In First Person mode:
  w       - Move worward
  s       - Move backward
  a       - Strafe left
  d       - Strafe right
  L Arrow - Look left (Camera)
  R Arrow - Look right (Camera)
  U Arrow - Look down (Camera)
  D Arrow - Look down (Camera)

  In Perspective mode:
  Use keyboard arrow keys to spin the axis lines
  (left/right about the y axis)
  (up/down about the x axis)

  Orthogonal mode:
  Use keyboard arrow keys to spin the axis lines
  (left/right about the y axis)
  (up/down about the x axis)

*/

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <stdbool.h>
#endif

//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.1415927/180))
#define Sin(x) (sin((x)*3.1415927/180))

// ----------------------------------------------------------
// Global Variables
// ----------------------------------------------------------

// What the camera is looking at vector
double cameraLookX , cameraLookY , cameraLookZ;

// XZ position of the camera in 1st person (eye)
double cameraX=50, cameraY =10 , cameraZ=25;

int th=0;         //  Azimuth of view angle (y)
int ph=0;         //  Elevation of view angle (x)

double dim=70;   // Dimension of orthogonal box
double PI = 3.14159;
int currentScene = 1;
bool drawAxis = true;

int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio

/*
  1 - First person
  2 - Perspective
  3 - Orthogononal
*/
int projectionMode = 1;     

double THX;
double THZ;
double time;

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
void theMenu(int value);
void init();
void strafeLeft(double bX, double bY, double bZ, double moveUnits);
void strafeRight(double bX, double bY, double bZ, double moveUnits);
void moveForward(double bX, double bY, double bZ, double moveUnits);
void moveBackward(double bX, double bY, double bZ, double moveUnits);
void key(unsigned char ch,int x,int y);
void errorCheck(char* where);
static void halfSphere(double x,double y,double z,double r);
static void ArtemisSpaceBomber(double x,double y,double z,
                       double dx,double dy,double dz,
                       double ux,double uy, double uz, double scale, double thx, double thz);
static void FighterJet(double x,double y,double z,
                       double dx,double dy,double dz,
                       double ux,double uy, double uz, double scale, double thx, double thz);
static void XB70Bomber(double x,double y,double z,
                       double dx,double dy,double dz,
                       double ux,double uy, double uz, double scale, double thx, double thz);
void drawCone(double x, double y, double z, double r, double h, double thz);
static void Project();

// ----------------------------------------------------------
// special() Callback function
// ----------------------------------------------------------
static void special(int k, int x, int y)
{

  if(k == GLUT_KEY_UP){
    if(ph <= 90)
      ph += 2.5;
  }else if(k == GLUT_KEY_DOWN){
    if(ph >= -90)
      ph -= 2.5;
  }else if(k == GLUT_KEY_LEFT){
    th -= 2.5;
  }else if(k == GLUT_KEY_RIGHT){
    th += 2.5;
  }else{
    return;
  }

  //  Keep angles to +/-360 degrees
  th %= 360;
  ph %= 360;

   //  Update projection
   Project();

  // Marks the current window as needing to be redisplayed
  glutPostRedisplay();
}

void strafeLeft(double bX, double bY, double bZ, double moveUnits){
  double aX = 0;
  double aY = 1;
  double aZ = 0;

  //Calculate unit vector of lookAt vector:
  double mag = sqrt(bX*bX + bY*bY + bZ*bZ);
  bX /= mag;
  bY /= mag;
  bZ /= mag;

  //Calculate cross product of vector a and b (a x b):
  double crossX = (aY*bZ) - (aZ*bY);
  //double crossY = (aY*bX) - (aX*bZ);
  double crossZ = (aX*bZ) - (aY*bX);

  //Add cross vector to camera position vector 
  cameraX += moveUnits*crossX;
  //cameraY += moveUnits*crossY;
  cameraZ += moveUnits*crossZ;
}

void strafeRight(double bX, double bY, double bZ, double moveUnits){
  double aX = 0;
  double aY = -1;
  double aZ = 0;

  //Calculate unit vector of lookAt vector:
  double mag = sqrt(bX*bX + bY*bY + bZ*bZ);
  bX /= mag;
  bY /= mag;
  bZ /= mag;

  //Calculate cross product of vector a and b (a x b):
  double crossX = (aY*bZ) - (aZ*bY);
  //double crossY = (aY*bX) - (aX*bZ);
  double crossZ = (aX*bZ) - (aY*bX);

  //Add cross vector to camera position vector 
  cameraX += moveUnits*crossX;
  //cameraY += moveUnits*crossY;
  cameraZ += moveUnits*crossZ;
}

void moveForward(double bX, double bY, double bZ, double moveUnits){

  //Calculate unit vector of lookAt vector:
  double mag = sqrt(bX*bX + bY*bY + bZ*bZ);
  bX /= mag;
  bY /= mag;
  bZ /= mag;

  cameraX += moveUnits*bX;
  cameraZ += moveUnits*bZ;
}

void moveBackward(double bX, double bY, double bZ, double moveUnits){

  //Calculate unit vector of lookAt vector:
  double mag = sqrt(bX*bX + bY*bY + bZ*bZ);
  bX /= mag;
  bY /= mag;
  bZ /= mag;

  cameraX -= moveUnits*bX;
  cameraZ -= moveUnits*bZ;
}

void errorCheck(char* where){
  int err = glGetError();
  if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
}

/*
 *  Set projection
 */
static void Project()
{
  //  Tell OpenGL we want to manipulate the projection matrix
  glMatrixMode(GL_PROJECTION);
  //  Undo previous transformations
  glLoadIdentity();

  if(projectionMode == 1){
    //  First person
    gluPerspective(fov,asp,dim/6,10*dim);
  }if (projectionMode == 2){
    //  Perspective transformation
    gluPerspective(fov,asp,dim/4,4*dim);
  }else if (projectionMode == 3){
    //  Orthogonal projection
    glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
  }
  
  //  Switch to manipulating the model matrix
  glMatrixMode(GL_MODELVIEW);
  //  Undo previous transformations
  glLoadIdentity();
}

// ----------------------------------------------------------
// theMenu() Callback function
// ----------------------------------------------------------
void theMenu(int value){
  if(value == 1){
    currentScene = 1;
  }else if(value == 2){
    currentScene = 2;
  }else if(value == 3){
    currentScene = 3;
  }else if(value == 4){
    if(drawAxis == true)
      drawAxis = false;
    else
      drawAxis = true;
  }else if (value == 5){
    exit(0);
  }

  // Marks the current window as needing to be redisplayed
  glutPostRedisplay();
}

/*
 *  Draw vertex in polar coordinates
 *  Polar to Cartesian
 *  x = rcos(theta)
 *  y = rsin(theta)
 * 
 *  Cartesian to Polar
 *  r = sqrt(x^2 + y^2)
 *  theta = tan^(-1) (y/x)
 */
static void Vertex(double th,double ph)
{
  glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
}

/*
 *  Draw a halfSphere
 *     at (x,y,z)
 *     radius (r)
 */
static void halfSphere(double x,double y,double z,double r)
{
  const int d=5;
  int th,ph;

  //  Save transformation
  glPushMatrix();
  //  Offset and scale
  glTranslated(x,y,z);
  glScaled(r,r,r);

  //  Latitude bands
  for (ph=0;ph<90;ph+=d)
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
 *  Draw a Cone
 *     base at (x,y,z)
 *     radius (r)
 *     heght (h)
 *     rotation about Z axis (thz)
 */
void drawCone(double x, double y, double z, double r, double h, double thz){
  const double orgX = x;
  const double orgY = y;
  const double orgZ = z;
  glRotated(thz,0,0,1);
  glBegin(GL_TRIANGLE_FAN);
    glVertex3d(x,y+h,x);
    for(double angle = 0.0; angle < 360; angle += 0.1){
      z = r*sin(angle);
      x = r*cos(angle);
      glVertex3d(x + orgX,y,z + orgZ);
    }
  glEnd();

  //Draw the base of the cone
  glColor3f(1,0,0);
  glBegin(GL_TRIANGLE_FAN);
    glVertex3d(orgX,orgY,orgZ);
    for(double angle = 0.0; angle < 360; angle += 0.1){
      z = r*sin(angle);
      x = r*cos(angle);
      glVertex3d(x + orgX,y,z + orgZ);
    }
  glEnd();
}

// ----------------------------------------------------------
//  ArtemisSpaceBomber
// ----------------------------------------------------------

/*
 *  Draw solid space bomber
 *    at (x,y,z)
 *    nose towards (dx,dy,dz)
 *    UP vector (ux,uy,uz) 
 *    Rotation about X - roll (thx)
 *    Rotation about Z - pitch (thz) 
 */
static void ArtemisSpaceBomber(double x,double y,double z,
                       double dx,double dy,double dz,
                       double ux,double uy, double uz, double scale, double thx, double thz)
{
  //-10
  // Dimensions used to size airplane
  const double wid = 1;   //The "width of the plane's "Fuselage"
  const double shipBowXfront = 0;    //X of front nose
  const double shipBowXend = -4;     //X of end nose
  const double shipSternX = -20;    //X end of ship
  const double cockpitX = -5;        //X center of cockpit

  const double missleBayXfront = -8;
  const double missleBayXend = -15;
  const double missleBaywid = 3;
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
  glRotated(thx,1,0,0);
  glRotated(thz,0,0,1);
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
  halfSphere(cockpitX,1,0,1);

  //Right Canard
  glColor3f(0,1,0);
  glBegin(GL_POLYGON);
    glVertex3d(-7,0,wid);
    glVertex3d(-4,0,wid);
    glVertex3d(-5,0,wid+1);
    glVertex3d(-6,0,wid+1);
  glEnd();

  //Left Canard
  glColor3f(0,1,0);
  glBegin(GL_POLYGON);
    glVertex3d(-7,0,-wid);
    glVertex3d(-4,0,-wid);
    glVertex3d(-5,0,-wid-1);
    glVertex3d(-6,0,-wid-1);
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

// ----------------------------------------------------------
//  FighterJet
// ----------------------------------------------------------

/*
 *  Draw solid fighter jet
 *    front nose at (x,y,z)
 *    nose points towards (dx,dy,dz)
 *    UP vector (ux,uy,uz) 
 *    Rotation about X - roll (thx)
 *    Rotation about Z - pitch (thz) 
 */
static void FighterJet(double x,double y,double z,
                       double dx,double dy,double dz,
                       double ux,double uy, double uz, double scale, double thx, double thz)
{
  //-20
  // Dimensions used to size airplane
  const double wid = 1;   //The "width of the plane's "Fuselage"
  const double shipBowXfront = 0;    //X of front nose
  const double shipBowXend = -4;     //X of end nose
  const double shipSternX = -20;    //X end of ship
  const double cockpitX = -5;        //X center of cockpit

  const double wingXfront = -8;
  const double wingXend = -19;
  const double wingXfrontFold = -17;
  const double wingZ = 10;

  const double canardXend = wingXfront;
  const double canardXfront = shipBowXend;
  const double canardZ = 4;

  const double wingLinefrontX = -16;
  const double wingLineXend = -20;
  const double wingLineZ = wingZ;
  
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
  glRotated(thx,1,0,0);
  glRotated(thz,0,0,1);
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
  halfSphere(cockpitX,1,0,1);

  glBegin(GL_TRIANGLES);
    //Right Canard
    glVertex3d(canardXend, 0, wid);
    glVertex3d(canardXend, 0, canardZ);
    glVertex3d(canardXfront, 0, wid);

    //Left Canard
    glVertex3d(canardXend, 0, -wid);
    glVertex3d(canardXend, 0, -canardZ);
    glVertex3d(canardXfront, 0, -wid);
  glEnd();

  //Wing tips
  glLineWidth(2);
  glColor3f(0,0,1);
  glBegin(GL_LINES);
    //Right wing line
    glVertex3d(wingLineXend, -wid, wingLineZ);
    glVertex3d(wingLinefrontX, -wid, wingLineZ);

    //Left wing line
    glVertex3d(wingLineXend, -wid, -wingLineZ);
    glVertex3d(wingLinefrontX, -wid, -wingLineZ);
  glEnd();
  glLineWidth(1);
 

  //  Vertical tail (plane triangle)
  glColor3f(0,0,1);
  glBegin(GL_POLYGON);
    glVertex3d(shipSternX, 6.0, 0.0);
    glVertex3d(shipSternX, 1.0, 0.0);
    glVertex3d(wingXend+4, 1.0, 0.0);
  glEnd();

  glColor3f(0,1,0);

  //Right wing
  glBegin(GL_POLYGON);
    glVertex3d(wingXend, -wid, wid);
    glVertex3d(wingXend, -wid, wingZ);
    glVertex3d(wingXfrontFold, -wid, wingZ);
    glVertex3d(wingXfront, -wid, wid);
  glEnd();

  //Left wing
  glBegin(GL_POLYGON);
    glVertex3d(wingXend, -wid, -wid);
    glVertex3d(wingXend, -wid, -wingZ);
    glVertex3d(wingXfrontFold, -wid, -wingZ);
    glVertex3d(wingXfront, -wid, -wid);
  glEnd();

  //  Undo transformations
  glPopMatrix();
}

// ----------------------------------------------------------
//  XB70 Bomber jet
// ----------------------------------------------------------

/*
 *  Draw XB70 Bomber jet
 *    front nose at (x,y,z)
 *    nose points towards (dx,dy,dz)
 *    UP vector (ux,uy,uz) 
 *    scale of the plane xyz (scale)
 *    Rotation about X - roll (thx)
 *    Rotation about Z - pitch (thz) 
 */
static void XB70Bomber(double x,double y,double z,
                       double dx,double dy,double dz,
                       double ux,double uy, double uz, double scale, double thx, double thz)
{
  const double shipFrontNoseX = 0.0;
  const double shipRearNoseX = -9.0;
  const double shipWidth = 2.0;
  const double cockpitLocX = shipRearNoseX - shipWidth;
  const double shipRearX = -60.0;
  const double wingsFrontX = -20.0;
  const double wingsRearX = -60.0;
  const double shipFuselageHeight = 2.0;
  const double frontFuselageXEnd = wingsFrontX;

  const double foldingWingZ = 15;
  const double foldingWingXfront = -50;
  const double foldingWingYHeight = 5;

  const double verticalsZ = 10;
  const double verticalsHeight = 7;
  const double verticalsXfront = -50;

  const double canardFrontX = shipRearNoseX - 2;
  const double canardRearX = canardFrontX - 6;
  const double canardZ = 5;

  //Inlets
  const double inletMiddleForntX = wingsFrontX - 3;
  const double inletMiddleRearX = shipRearX;
  const double inletY = -shipFuselageHeight - 1.5;
  const double inletSidefrontX = wingsFrontX - 6;

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
  glRotated(thx,1,0,0);
  glRotated(thz,0,0,1);
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

  //  Fuselage (front section)
  glBegin(GL_QUADS);
    glColor3f(1,0,0);

    //Right side
    glVertex3d(shipRearNoseX, shipFuselageHeight, shipWidth);
    glVertex3d(shipRearNoseX,-shipFuselageHeight, shipWidth);
    glVertex3d(frontFuselageXEnd,-shipFuselageHeight, shipWidth);
    glVertex3d(frontFuselageXEnd, shipFuselageHeight, shipWidth);

    //Left side
    glVertex3d(shipRearNoseX, shipFuselageHeight,-shipWidth);
    glVertex3d(shipRearNoseX,-shipFuselageHeight,-shipWidth);
    glVertex3d(frontFuselageXEnd,-shipFuselageHeight,-shipWidth);
    glVertex3d(frontFuselageXEnd, shipFuselageHeight,-shipWidth);

    //Top side
    glVertex3d(shipRearNoseX, shipFuselageHeight, shipWidth);
    glVertex3d(shipRearNoseX, shipFuselageHeight,-shipWidth);
    glVertex3d(frontFuselageXEnd, -shipFuselageHeight,-shipWidth);
    glVertex3d(frontFuselageXEnd, -shipFuselageHeight, shipWidth);
    
    //Bottom side
    glVertex3d(shipRearNoseX,-shipFuselageHeight, shipWidth);
    glVertex3d(shipRearNoseX,-shipFuselageHeight,-shipWidth);
    glVertex3d(shipRearX,-shipFuselageHeight,-shipWidth);
    glVertex3d(shipRearX,-shipFuselageHeight, shipWidth);

    //Back side of entire fuselage
    glVertex3d(frontFuselageXEnd,-shipFuselageHeight, shipWidth);
    glVertex3d(frontFuselageXEnd, shipFuselageHeight, shipWidth);
    glVertex3d(frontFuselageXEnd, shipFuselageHeight,-shipWidth);
    glVertex3d(frontFuselageXEnd,-shipFuselageHeight,-shipWidth);
  glEnd();

  //Rear fuselage section:
  glBegin(GL_QUADS);
    //Second section top side
    glVertex3d(frontFuselageXEnd, shipFuselageHeight, shipWidth);
    glVertex3d(frontFuselageXEnd, shipFuselageHeight,-shipWidth);
    glVertex3d(shipRearX, -shipFuselageHeight,-shipWidth);
    glVertex3d(shipRearX, -shipFuselageHeight, shipWidth);
  glEnd();

  //Rear fuselage sides
  glBegin(GL_POLYGON);
    //Right side
    glVertex3d(frontFuselageXEnd, shipFuselageHeight, shipWidth);
    glVertex3d(shipRearX, -shipFuselageHeight, shipWidth);
    glVertex3d(frontFuselageXEnd, -shipFuselageHeight, shipWidth);

    //Left side
    glVertex3d(frontFuselageXEnd, shipFuselageHeight, -shipWidth);
    glVertex3d(shipRearX, -shipFuselageHeight, -shipWidth);
    glVertex3d(frontFuselageXEnd, -shipFuselageHeight, -shipWidth);
  glEnd();


  //Left wing 
  glColor3f(0,1,0);
  glBegin(GL_POLYGON);
    glVertex3d(wingsRearX, -shipFuselageHeight, shipWidth);
    glVertex3d(wingsRearX, -shipFuselageHeight, foldingWingZ);
    glVertex3d(foldingWingXfront, -shipFuselageHeight, foldingWingZ);
    glVertex3d(wingsFrontX, -shipFuselageHeight, shipWidth);
  glEnd();

  //Right wing 
  glColor3f(0,1,0);
  glBegin(GL_POLYGON);
    glVertex3d(wingsRearX, -shipFuselageHeight, -shipWidth);
    glVertex3d(wingsRearX, -shipFuselageHeight, -foldingWingZ);
    glVertex3d(foldingWingXfront, -shipFuselageHeight, -foldingWingZ);
    glVertex3d(wingsFrontX, -shipFuselageHeight, -shipWidth);
  glEnd();

  //Left folding wing
  glColor3f(1,0,0);
  glBegin(GL_POLYGON);
    glVertex3d(wingsRearX, -shipFuselageHeight, foldingWingZ);
    glVertex3d(wingsRearX, -foldingWingYHeight-shipFuselageHeight, foldingWingZ+2);
    glVertex3d(foldingWingXfront, -shipFuselageHeight, foldingWingZ);
  glEnd();

  //Right folding wing
  glColor3f(1,0,0);
  glBegin(GL_POLYGON);
    glVertex3d(wingsRearX, -shipFuselageHeight, -foldingWingZ);
    glVertex3d(wingsRearX, -foldingWingYHeight-shipFuselageHeight, -foldingWingZ-2);
    glVertex3d(foldingWingXfront, -shipFuselageHeight, -foldingWingZ);
  glEnd();

  //Righ verical
  glColor3f(0,0,1);
  glBegin(GL_POLYGON);
    glVertex3d(wingsRearX, verticalsHeight, verticalsZ);
    glVertex3d(wingsRearX, -shipFuselageHeight,verticalsZ);
    glVertex3d(verticalsXfront, -shipFuselageHeight, verticalsZ);
  glEnd();

  //Left verical
  glColor3f(0,0,1);
  glBegin(GL_POLYGON);
    glVertex3d(wingsRearX, verticalsHeight, -verticalsZ);
    glVertex3d(wingsRearX, -shipFuselageHeight,-verticalsZ);
    glVertex3d(verticalsXfront, -shipFuselageHeight, -verticalsZ);
  glEnd();

  //Right Canard
  glColor3f(0,1,0);
  glBegin(GL_POLYGON);
    glVertex3d(canardRearX,0,shipWidth);
    glVertex3d(canardRearX+1,0,canardZ);
    glVertex3d(canardRearX+4,0,canardZ);
    glVertex3d(canardFrontX,0,shipWidth);
  glEnd();

  //Left Canard
  glColor3f(0,1,0);
  glBegin(GL_POLYGON);
    glVertex3d(canardRearX,0,-shipWidth);
    glVertex3d(canardRearX+1,0,-canardZ);
    glVertex3d(canardRearX+4,0,-canardZ);
    glVertex3d(canardFrontX,0,-shipWidth);
  glEnd();

  //Inlet middle, left and right side
  glColor3f(1,1,1); //white
  glBegin(GL_QUADS);
    //Middle inlet
    glVertex3d(inletMiddleForntX,inletY,0);
    glVertex3d(inletMiddleRearX, inletY, 0);
    glVertex3d(inletMiddleRearX, -shipFuselageHeight, 0);
    glVertex3d(inletMiddleForntX, -shipFuselageHeight,0);

    //Left inlet side
    glVertex3d(inletSidefrontX,inletY,shipWidth);
    glVertex3d(inletMiddleRearX, inletY, shipWidth);
    glVertex3d(inletMiddleRearX, -shipFuselageHeight, shipWidth);
    glVertex3d(inletSidefrontX, -shipFuselageHeight, shipWidth);

    //Right inlet side
    glVertex3d(inletSidefrontX,inletY,-shipWidth);
    glVertex3d(inletMiddleRearX, inletY, -shipWidth);
    glVertex3d(inletMiddleRearX, -shipFuselageHeight, -shipWidth);
    glVertex3d(inletSidefrontX, -shipFuselageHeight, -shipWidth);
  glEnd();

  //Back inlet left bottom side
  glColor3f(0,0,1);
  glBegin(GL_POLYGON);
    glVertex3f(inletSidefrontX, inletY, shipWidth);
    glVertex3d(inletMiddleRearX, inletY, shipWidth);
    glVertex3d(inletMiddleRearX, inletY, 0);
    glVertex3d(inletMiddleForntX, inletY, 0);
  glEnd();

  //Back inlet right bottom side
  glColor3f(0,0,1);
  glBegin(GL_POLYGON);
    glVertex3f(inletSidefrontX, inletY, -shipWidth);
    glVertex3d(inletMiddleRearX, inletY, -shipWidth);
    glVertex3d(inletMiddleRearX, inletY, 0);
    glVertex3d(inletMiddleForntX, inletY, 0);
  glEnd();

  //Cockpit
  glColor3f(0,1,0);
  halfSphere(cockpitLocX,shipFuselageHeight,0,shipWidth);

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

  //Changing Dim only allowed in Orthogonal and Perspective
  if(projectionMode != 1){
      if (ch == '=' || ch == '+')
      dim += 1;
    else if (ch == '-' || ch == '_')
      dim -= 1;
  }

  //Moving camera only in First person
  if (projectionMode == 1){
    if(ch == 'w' || ch =='W')
      moveForward(cameraLookX, cameraLookY, cameraLookZ, 2);
    else if(ch == 's' || ch == 'S')
      moveBackward(cameraLookX, cameraLookY, cameraLookZ, 2);  
    else if(ch == 'a' || ch == 'A')
      strafeLeft(cameraLookX, cameraLookY, cameraLookZ, 2);
    else if(ch == 'd' || ch == 'D')
      strafeRight(cameraLookX, cameraLookY, cameraLookZ, 2);
  }
  
  // Reproject
  Project();
  
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
  glutAddMenuEntry("Draw Everything!", 1);
  glutAddMenuEntry("Draw only planes", 2);
  glutAddMenuEntry("Auto rotation", 3);
  glutAddMenuEntry("Axis ON/OFF", 4);
  glutAddMenuEntry("Quit", 5);

  glLineWidth(2);
  
  // Let the menu respond on the right mouse button
	glutAttachMenu(GLUT_RIGHT_BUTTON);

  //  Enable Z-buffering in OpenGL
  glEnable(GL_DEPTH_TEST);

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

  if(projectionMode == 1){
    //First personz
    cameraLookX = 2*dim*Sin(th);
    cameraLookZ = -2*dim*Cos(th);
    cameraLookY = 2*dim*Sin(ph);
    gluLookAt(cameraX,cameraY,cameraZ,
              cameraX+cameraLookX,cameraLookY + cameraLookY,cameraZ+cameraLookZ,
              0,1,0); 

  }else if(projectionMode == 2){
    //  Perspective - set eye position
    double Ex = -2*dim*Sin(th)*Cos(ph);
    double Ey = +2*dim        *Sin(ph);
    double Ez = +2*dim*Cos(th)*Cos(ph);
    gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
  }else if (projectionMode == 3){
    //  Orthogonal - set world orientation 
    glRotatef(ph, 1.0, 0.0, 0.0 );  //x coordinate
    glRotatef(th, 0.0, 1.0, 0.0 );  //y coordinate
  }

  if(drawAxis){
    drawAxisLines();
    drawAxisLabels();
  }

  switch(currentScene){
    case 1:
      XB70Bomber(60,10,-20 , 1,0,0, 0,1,0, 0.7, 0, 0);
      FighterJet(20,0,40, 1,0,0, 0,1,0, 1, 25, 25);
      ArtemisSpaceBomber(10,40,10, 1,0,0, 0,1,0, 1, -25, 40);
      XB70Bomber(-20,30,-20 , 1,0,0, 0,1,0, 1, 20, 70);
      FighterJet(-50,30,40, 1,0,0, 0,1,0, 1, 90, 90);
      XB70Bomber(60,5,5 , 1,0,0, 0,1,0, 0.25, 20, 70);
      FighterJet(50,10,-20 , 1,0,0, 0,1,0, 0.3, 25, 25);
      drawCone(5,0,5,5,10,90);
      break;
    case 2:
      FighterJet(20,0,40, 1,0,0, 0,1,0,0.8, 25,0);
      FighterJet(20,0,-40, 1,0,0, 0,1,0,0.8, -25,0);
      FighterJet(-40,20,0, 1,0,0, 0,1,0,0.8, 0, 25);
      XB70Bomber(50,10,0 , 1,0,0, 0,1,0,0.8, 0, 10);
      FighterJet(60,-20,0, 1,0,0, 0,-1,0,0.8, 0, 25);
      XB70Bomber(0,20,40 , 1,0,0, 0,1,0,0.5, 0, 25);
      XB70Bomber(0,20,-40 , 1,0,0, 0,1,0,0.5, -25, -25);
      break;
    case 3:
      FighterJet(-10,-10,30, 1,0,0, 0,1,0,0.8, THX,0);
      XB70Bomber(50,10,-20, 1,0,0, 0,1,0,0.8, -THX, 0);
      FighterJet(-10,-20,-30, 1,0,0, 0,1,0,0.8, 0,-THZ);
      ArtemisSpaceBomber(20, 20, 25, 1,0,0, 0,1,0,0.8, 0,-THZ);
      ArtemisSpaceBomber(40, -20, -10, 1,0,0, 0,1,0,0.8, -THX,0);
      XB70Bomber(-25,10,0, 1,0,0, 0,1,0,0.5, 0, THZ);
      
  }
  
  //  Display rotation angles
  glColor3f(1,1,1);
  glWindowPos2i(5,5);
  Print("Angle X= %d",ph);
  if(projectionMode == 1)
    Print("   Mode: First Person");
  else if (projectionMode == 2)
    Print("   Mode:  Perspective");
  else if(projectionMode == 3)
    Print("   Mode: Orthogonal");

  Print(" Dim=%.1f",dim);
  Print(" CameraX=%.1f",cameraX);
  Print(" CameraY=%.1f",cameraY);
  Print(" CameraZ=%.1f",cameraZ);

  glWindowPos2i(5,25);
  Print("Angle Y= %d",th);

  glWindowPos2i(5,45);
  if(drawAxis == true)
    Print("Axis ON");
  else
    Print("Axis OFF");

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
void idle(){
  if(currentScene == 3){
    time = glutGet(GLUT_ELAPSED_TIME)/1000.0;
    THX = fmod(90*time,360);
    THZ = fmod(90*time,360);
    glutPostRedisplay();
  }
}

// This function is called by GLUT when the window is resized
void reshape(int width,int height)
{
  //  Ratio of the width to the height of the window
  asp = (height>0) ? (double)width/height : 1;
  //  Set the viewport to the entire window
  glViewport(0,0, width,height);
  //  Set projection
  Project();
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
  glutCreateWindow("hw4 Riad Shash (Ray)");

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