/*
Riad Shash (Ray)
CSCI 5229
HW5 Lighting

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
  =/+     - Decrease dim (zoom in)
  -/_     - Increase dim (zoom out)

  Orthogonal mode:
  Use keyboard arrow keys to spin the axis lines
  (left/right about the y axis)
  (up/down about the x axis)

*/

#include "CSCIx229.h"

// ----------------------------------------------------------
// Global Variables
// ----------------------------------------------------------

// What the camera is looking at vector
double cameraLookX , cameraLookY , cameraLookZ;

// XZ position of the camera in 1st person (eye)
double cameraX=80, cameraY =10 , cameraZ=25;

int th=-64;         //  Azimuth of view angle (y)
int ph=0;         //  Elevation of view angle (x)

double dim=35;   // Dimension of orthogonal box
double PI = 3.14159;
int currentScene = 1;
bool drawAxis = true;

int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio

/*
  1 - First person (starts with FP mode)
  2 - Perspective
  3 - Orthogononal
*/
int projectionMode = 3;     

double THX;
double THZ;
double time;

// Light values
int light = 1;        //  Lighting
int one       =   1;  // Unit value
int distance  =   25;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light

// ----------------------------------------------------------
// Function Prototypes
// ----------------------------------------------------------
void display();
void reshape(int,int);
void idle();
static void special(int k, int x, int y);
void drawAxisLines();
void drawAxisLabels();
void theMenu(int value);
void init();
void strafeLeft(double bX, double bY, double bZ, double moveUnits);
void strafeRight(double bX, double bY, double bZ, double moveUnits);
void moveForward(double bX, double bY, double bZ, double moveUnits);
void moveBackward(double bX, double bY, double bZ, double moveUnits);
void findDispVector(double x1, double y1, double z1, double x2, double y2, double z2, double* vX, double* vY, double* vZ);
void findNormalVector(double x1, double y1, double z1, double x2, double y2, double z2, double* uX, double* uY, double* uZ);
void key(unsigned char ch,int x,int y);
static void ball(double x,double y,double z,double r);
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
// ----------------------------------------------------------
// special() Callback function
// ----------------------------------------------------------
static void special(int k, int x, int y)
{
  if(k == GLUT_KEY_UP){
    if(projectionMode == 1){
      if(ph < 90)
        ph += 2.5; 
    }else
    {
      ph += 2.5;
    }
    
  }else if(k == GLUT_KEY_DOWN){
    if(projectionMode == 1){
      if(ph > -90)
        ph -= 2.5;
    }else
    {
        ph -= 2.5;
    }
    
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
   Project(fov,asp,dim,projectionMode);

  // Marks the current window as needing to be redisplayed
  glutPostRedisplay();
}

void findDispVector(double x1, double y1, double z1, double x2, double y2, double z2, double* vX, double* vY, double* vZ){
  *vX = x2 - x1;
  *vY = y2 - y1;
  *vZ = z2 - z1;
}

void findNormalVector(double x1, double y1, double z1, double x2, double y2, double z2, double* uX, double* uY, double* uZ){
  double crossX = (y1*z2) - (z1*y2);
  double crossY = (z1*x2) - (x1*z2);
  double crossZ = (x1*y2) - (y1*x2);

  //Normalize the vector(finds the unit vector of the normal)
  double mag = sqrt(crossX*crossX + crossY*crossY + crossZ*crossZ);
  *uX = crossX /= mag;
  *uY = crossY /= mag;
  *uZ = crossZ /= mag;
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

// ----------------------------------------------------------
// theMenu() Callback function
// ----------------------------------------------------------
void theMenu(int value){
  if(value == 1){
    projectionMode = 1;
    // XZ position of the camera in 1st person (eye)
    cameraX=50;
    cameraY =10; 
    cameraZ=25;
    ph = 0;
    th = -60;
  }else if(value == 2){
    projectionMode = 2;
    th = 245;
    ph = 45;
  }else if(value == 3){
    projectionMode = 3;
    th = 245;
    ph = 45;
  }else if(value == 4){
    if(drawAxis == true)
      drawAxis = false;
    else
      drawAxis = true;
  }else if (value == 5){
    currentScene = 1;
  }else if (value == 6){
    currentScene = 2;
  }else if (value == 7){
    currentScene = 3;
  }else if (value == 8){
    exit(0);
  }

  //Call project again to be sure
  Project(fov, asp, dim, projectionMode);

  // Marks the current window as needing to be redisplayed
  glutPostRedisplay();
}

/*
 *  Draw vertex in polar coordinates with normal
 */
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
static void ball(double x,double y,double z,double r)
{
   int th,ph;
   float yellow[] = {1.0,1.0,0.0,1.0};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball
   glColor3f(1,1,1);
   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   //  Bands of latitude
   for (ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}

/*
 *  Draw a halfSphere
 *     at (x,y,z)
 *     radius (r)
 */
static void halfSphere(double x,double y,double z,double r)
{
  const int d=10;
  int th,ph;
  float yellow[] = {1.0,1.0,0.0,1.0};
  float Emission[]  = {0.0,0.0,0.01*emission,1.0};

  //  Save transformation
  glPushMatrix();
  //  Offset and scale
  glTranslated(x,y,z);
  glScaled(r,r,r);

  glMaterialf(GL_FRONT,GL_SHININESS,shiny);
  glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
  glMaterialfv(GL_FRONT,GL_EMISSION,Emission);

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
void XB70Bomber(double x,double y,double z,
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

  //For normal vector calculations
  double aX, aY, aZ;
  double bX, bY, bZ;
  double nX, nY, nZ;
  
  // ----------------------------------------------------------
  // Nose
  // ----------------------------------------------------------

  //  Front Nose (4 sided)
  glColor3f(0,0,1);
  glBegin(GL_TRIANGLES);
    
    //Right side nose
    glNormal3d(0,0,1);
    glVertex3d(shipFrontNoseX, 0.0, 0.0);
    glVertex3d(shipRearNoseX, shipFuselageHeight, shipWidth);
    glVertex3d(shipRearNoseX,-shipFuselageHeight, shipWidth);

    //Left side nose
    glNormal3d(0,0,-1);
    glVertex3d(shipFrontNoseX, 0.0, 0.0);
    glVertex3d(shipRearNoseX, shipFuselageHeight,-shipWidth);
    glVertex3d(shipRearNoseX,-shipFuselageHeight,-shipWidth);

    //Top side nose
    glNormal3d(0,1,0);
    glVertex3d(shipFrontNoseX, 0.0, 0.0);
    glVertex3d(shipRearNoseX, shipFuselageHeight, shipWidth);
    glVertex3d(shipRearNoseX, shipFuselageHeight,-shipWidth);

    //Bottom side nose
    glNormal3d(0,-1,0);
    glVertex3d(shipFrontNoseX, 0.0, 0.0);
    glVertex3d(shipRearNoseX,-shipFuselageHeight, shipWidth);
    glVertex3d(shipRearNoseX,-shipFuselageHeight,-shipWidth);
  glEnd();

  //  Fuselage (front section)
  glBegin(GL_QUADS);
    glColor3f(1,1,1);

    //Right side
    glNormal3d(0,0,1);
    glVertex3d(shipRearNoseX, shipFuselageHeight, shipWidth);
    glVertex3d(shipRearNoseX,-shipFuselageHeight, shipWidth);
    glVertex3d(frontFuselageXEnd,-shipFuselageHeight, shipWidth);
    glVertex3d(frontFuselageXEnd, shipFuselageHeight, shipWidth);

    //Left side
    glNormal3d(0,0,-1);
    glVertex3d(shipRearNoseX, shipFuselageHeight,-shipWidth);
    glVertex3d(shipRearNoseX,-shipFuselageHeight,-shipWidth);
    glVertex3d(frontFuselageXEnd,-shipFuselageHeight,-shipWidth);
    glVertex3d(frontFuselageXEnd, shipFuselageHeight,-shipWidth);

    //Top side
    glNormal3d(0,1,0);
    glVertex3d(shipRearNoseX, shipFuselageHeight, -shipWidth);
    glVertex3d(frontFuselageXEnd, shipFuselageHeight, -shipWidth);
    glVertex3d(frontFuselageXEnd, shipFuselageHeight, shipWidth);
    glVertex3d(shipRearNoseX, shipFuselageHeight, shipWidth);
    
    
    //Bottom side
    glNormal3d(0,-1,0);
    glVertex3d(shipRearNoseX,-shipFuselageHeight, shipWidth);
    glVertex3d(shipRearNoseX,-shipFuselageHeight,-shipWidth);
    glVertex3d(shipRearX,-shipFuselageHeight,-shipWidth);
    glVertex3d(shipRearX,-shipFuselageHeight, shipWidth);
  glEnd();

  //Calculation of normal for second fuselage section
  
  findDispVector(frontFuselageXEnd, shipFuselageHeight, -shipWidth, shipRearX, -shipFuselageHeight, -shipWidth, &aX, &aY, &aZ);
  findDispVector(shipRearX, -shipFuselageHeight, -shipWidth, shipRearX, -shipFuselageHeight, shipWidth, &bX, &bY, &bZ);
  findNormalVector(aX,aY,aZ,bX,bY,bZ,&nX,&nY,&nZ);


  //Rear fuselage section:
  glBegin(GL_QUADS);
    //Second section top side
    glNormal3d(nX,nY,nZ);
    glVertex3d(frontFuselageXEnd, shipFuselageHeight, shipWidth);
    glVertex3d(frontFuselageXEnd, shipFuselageHeight,-shipWidth);
    glVertex3d(shipRearX, -shipFuselageHeight,-shipWidth);
    glVertex3d(shipRearX, -shipFuselageHeight, shipWidth);
  glEnd();

  //Rear fuselage sides
  glBegin(GL_POLYGON);
    //Right side
    glNormal3d(0,0,1);
    glVertex3d(frontFuselageXEnd, shipFuselageHeight, shipWidth);
    glVertex3d(shipRearX, -shipFuselageHeight, shipWidth);
    glVertex3d(frontFuselageXEnd, -shipFuselageHeight, shipWidth);
  glEnd();

  glBegin(GL_POLYGON);
    //Left side
    glNormal3d(0,0,-1);
    glVertex3d(frontFuselageXEnd, shipFuselageHeight, -shipWidth);
    glVertex3d(frontFuselageXEnd, -shipFuselageHeight, -shipWidth);
    glVertex3d(shipRearX, -shipFuselageHeight, -shipWidth);
  glEnd();

  // ----------------------------------------------------------
  // Wings
  // ----------------------------------------------------------

  //Left wing (bottom portion)
  glColor3f(0.5,0.5,0.5);
  glBegin(GL_POLYGON);
    glNormal3d(0,-1,0);
    glVertex3d(wingsRearX, -shipFuselageHeight, shipWidth);
    glVertex3d(wingsRearX, -shipFuselageHeight, foldingWingZ);
    glVertex3d(foldingWingXfront, -shipFuselageHeight, foldingWingZ);
    glVertex3d(wingsFrontX, -shipFuselageHeight, shipWidth);
  glEnd();

   //Left wing (upper portion)
  glColor3f(0.5,0.5,0.5);
  glBegin(GL_POLYGON);
    glNormal3d(0,1,0);
    glVertex3d(wingsRearX, -shipFuselageHeight+0.01, shipWidth);
    glVertex3d(wingsRearX, -shipFuselageHeight+0.01, foldingWingZ);
    glVertex3d(foldingWingXfront, -shipFuselageHeight+0.01, foldingWingZ);
    glVertex3d(wingsFrontX, -shipFuselageHeight+0.01, shipWidth);
  glEnd();

  //Right wing (bottom portion)
  glColor3f(0.5,0.5,0.5);
  glBegin(GL_POLYGON);
    glNormal3d(0,-1,0);
    glVertex3d(wingsRearX, -shipFuselageHeight, -shipWidth);
    glVertex3d(wingsRearX, -shipFuselageHeight, -foldingWingZ);
    glVertex3d(foldingWingXfront, -shipFuselageHeight, -foldingWingZ);
    glVertex3d(wingsFrontX, -shipFuselageHeight, -shipWidth);
  glEnd();

  //Right wing (upper portion)
  glColor3f(0.5,0.5,0.5);
  glBegin(GL_POLYGON);
    glNormal3d(0,1,0);
    glVertex3d(wingsRearX, -shipFuselageHeight+0.01, -shipWidth);
    glVertex3d(wingsRearX, -shipFuselageHeight+0.01, -foldingWingZ);
    glVertex3d(foldingWingXfront, -shipFuselageHeight+0.01, -foldingWingZ);
    glVertex3d(wingsFrontX, -shipFuselageHeight+0.01, -shipWidth);
  glEnd();

  // ----------------------------------------------------------
  // Folding Wings
  // ----------------------------------------------------------

  //Right folding wing (upper portion)
  findDispVector(foldingWingXfront, -shipFuselageHeight, foldingWingZ, wingsRearX, -shipFuselageHeight, foldingWingZ, &aX,&aY,&aZ);
  findDispVector(wingsRearX, -shipFuselageHeight, foldingWingZ, wingsRearX, -foldingWingYHeight-shipFuselageHeight, foldingWingZ+2, &bX,&bY,&bZ);
  findNormalVector(aX,aY,aZ,bX,bY,bZ,&nX,&nY,&nZ);

  glColor3f(1,1,1);
  glBegin(GL_POLYGON);
    glNormal3d(nX,nY,nZ);
    glVertex3d(wingsRearX, -shipFuselageHeight, foldingWingZ);
    glVertex3d(wingsRearX, -foldingWingYHeight-shipFuselageHeight, foldingWingZ+2);
    glVertex3d(foldingWingXfront, -shipFuselageHeight, foldingWingZ);
  glEnd();

  //Right folding wing (bottom portion)
  glColor3f(1,1,1);
  glBegin(GL_POLYGON);
    glNormal3d(-nX,-nY,-nZ);
    glVertex3d(wingsRearX, -shipFuselageHeight, foldingWingZ);
    glVertex3d(wingsRearX, -foldingWingYHeight-shipFuselageHeight-0.01, foldingWingZ+2);
    glVertex3d(foldingWingXfront, -shipFuselageHeight, foldingWingZ);
  glEnd();

  //Left folding wing (upper portion)
  glColor3f(1,1,1);
  glBegin(GL_POLYGON);
    glNormal3d(nX,nY,-nZ);
    glVertex3d(wingsRearX, -shipFuselageHeight, -foldingWingZ);
    glVertex3d(wingsRearX, -foldingWingYHeight-shipFuselageHeight, -foldingWingZ-2);
    glVertex3d(foldingWingXfront, -shipFuselageHeight, -foldingWingZ);
  glEnd();

  //Left folding wing (bottom portion)
  glColor3f(1,1,1);
  glBegin(GL_POLYGON);
    glNormal3d(nX,-nY,-nZ);
    glVertex3d(wingsRearX, -shipFuselageHeight, -foldingWingZ);
    glVertex3d(wingsRearX, -foldingWingYHeight-shipFuselageHeight-0.01, -foldingWingZ-2);
    glVertex3d(foldingWingXfront, -shipFuselageHeight, -foldingWingZ);
  glEnd();

  // ----------------------------------------------------------
  // Vertical stabilizers
  // ----------------------------------------------------------

  //Righ verical (Z+)
  glColor3f(0,0,1);
  glBegin(GL_POLYGON);
    glNormal3d(0,0,1);
    glVertex3d(wingsRearX, verticalsHeight, verticalsZ);
    glVertex3d(wingsRearX, -shipFuselageHeight,verticalsZ);
    glVertex3d(verticalsXfront, -shipFuselageHeight, verticalsZ);
  glEnd();

  //Righ verical (-Z)
  glColor3f(0,0,1);
  glBegin(GL_POLYGON);
    glNormal3d(0,0,-1);
    glVertex3d(wingsRearX, verticalsHeight, verticalsZ-0.01);
    glVertex3d(wingsRearX, -shipFuselageHeight,verticalsZ-0.01);
    glVertex3d(verticalsXfront, -shipFuselageHeight, verticalsZ-0.01);
  glEnd();

  //Left verical (Z+)
  glColor3f(0,0,1);
  glBegin(GL_POLYGON);
    glNormal3d(0,0,1);
    glVertex3d(wingsRearX, verticalsHeight, -verticalsZ);
    glVertex3d(wingsRearX, -shipFuselageHeight,-verticalsZ);
    glVertex3d(verticalsXfront, -shipFuselageHeight, -verticalsZ);
  glEnd();

  //Left verical (-Z)
  glColor3f(0,0,1);
  glBegin(GL_POLYGON);
    glNormal3d(0,0,-1);
    glVertex3d(wingsRearX, verticalsHeight, -verticalsZ-0.01);
    glVertex3d(wingsRearX, -shipFuselageHeight,-verticalsZ-0.01);
    glVertex3d(verticalsXfront, -shipFuselageHeight, -verticalsZ-0.01);
  glEnd();

  // ----------------------------------------------------------
  // Canards
  // ----------------------------------------------------------

  //Right Canard (bottom portion)
  glColor3f(0.5,0.5,0.5);
  glBegin(GL_POLYGON);
    glNormal3d(0,-1,0);
    glVertex3d(canardRearX,0,shipWidth);
    glVertex3d(canardRearX+1,0,canardZ);
    glVertex3d(canardRearX+4,0,canardZ);
    glVertex3d(canardFrontX,0,shipWidth);
  glEnd();

  //Right Canard (upper portion)
  glColor3f(0.5,0.5,0.5);
  glBegin(GL_POLYGON);
    glNormal3d(0,1,0);
    glVertex3d(canardRearX,0+0.01,shipWidth);
    glVertex3d(canardRearX+1,0+0.01,canardZ);
    glVertex3d(canardRearX+4,0+0.01,canardZ);
    glVertex3d(canardFrontX,0+0.01,shipWidth);
  glEnd();

  //Left Canard (bottom portion)
  glColor3f(0,1,0);
  glBegin(GL_POLYGON);
    glNormal3d(0,-1,0);
    glVertex3d(canardRearX,0,-shipWidth);
    glVertex3d(canardRearX+1,0,-canardZ);
    glVertex3d(canardRearX+4,0,-canardZ);
    glVertex3d(canardFrontX,0,-shipWidth);
  glEnd();

  //Left Canard (upper portion)
  glColor3f(0.5,0.5,0.5);
  glBegin(GL_POLYGON);
    glNormal3d(0,1,0);
    glVertex3d(canardRearX,0+0.01,-shipWidth);
    glVertex3d(canardRearX+1,0+0.01,-canardZ);
    glVertex3d(canardRearX+4,0+0.01,-canardZ);
    glVertex3d(canardFrontX,0+0.01,-shipWidth);
  glEnd();

  // ----------------------------------------------------------
  // Inlets
  // ----------------------------------------------------------

  //Inlet middle, left and right side
  glColor3f(1,1,1); //white
  glBegin(GL_QUADS);
    //Middle inlet
    glVertex3d(inletMiddleForntX,inletY,0);
    glVertex3d(inletMiddleRearX, inletY, 0);
    glVertex3d(inletMiddleRearX, -shipFuselageHeight, 0);
    glVertex3d(inletMiddleForntX, -shipFuselageHeight,0);

    //Right inlet side
    glNormal3d(0,0,1);
    glVertex3d(inletSidefrontX,inletY,shipWidth);
    glVertex3d(inletMiddleRearX, inletY, shipWidth);
    glVertex3d(inletMiddleRearX, -shipFuselageHeight, shipWidth);
    glVertex3d(inletSidefrontX, -shipFuselageHeight, shipWidth);

    //Left inlet side
    glNormal3d(0,0,-1);
    glVertex3d(inletSidefrontX,inletY,-shipWidth);
    glVertex3d(inletMiddleRearX, inletY, -shipWidth);
    glVertex3d(inletMiddleRearX, -shipFuselageHeight, -shipWidth);
    glVertex3d(inletSidefrontX, -shipFuselageHeight, -shipWidth);
  glEnd();

  //Back inlet left bottom side
  glColor3f(0,0,1);
  glBegin(GL_POLYGON);
    glNormal3d(0,-1,0);
    glVertex3f(inletSidefrontX, inletY, shipWidth);
    glVertex3d(inletMiddleRearX, inletY, shipWidth);
    glVertex3d(inletMiddleRearX, inletY, 0);
    glVertex3d(inletMiddleForntX, inletY, 0);
  glEnd();

  //Back inlet right bottom side
  glColor3f(0,0,1);
  glBegin(GL_POLYGON);
    glNormal3d(0,-1,0);
    glVertex3f(inletSidefrontX, inletY, -shipWidth);
    glVertex3d(inletMiddleRearX, inletY, -shipWidth);
    glVertex3d(inletMiddleRearX, inletY, 0);
    glVertex3d(inletMiddleForntX, inletY, 0);
  glEnd();

  // ----------------------------------------------------------
  // Cockpit
  // ----------------------------------------------------------
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
  if(projectionMode == 2){
      if (ch == '=' || ch == '+')
      dim -= 1;
    else if (ch == '-' || ch == '_')
      dim += 1;
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
  Project(fov, asp, dim, projectionMode);
  
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
  glutAddMenuEntry("First Person", 1);
  glutAddMenuEntry("Perspective", 2);
  glutAddMenuEntry("Orthogonal", 3);
  glutAddMenuEntry("Axis ON/OFF", 4);
  glutAddMenuEntry("Scene 1", 5);
  glutAddMenuEntry("Scene 2", 6);
  glutAddMenuEntry("Scene 3", 7);
  glutAddMenuEntry("Quit", 8);

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
  //  Two sided mode
  //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);

  // Reset transformations
  glLoadIdentity();

  if(projectionMode == 1){
    //First person
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

  //  Flat or smooth shading
  glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

  //  Light switch
  if (light)
  {
    //  Translate intensity to color vectors
    float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
    float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
    float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
    //  Light position
    float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
    //  Draw light position as ball (still no lighting here)
    glColor3f(1,1,1);
    ball(Position[0],Position[1],Position[2] , 0.5);
    //  OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);
    //  Enable lighting
    glEnable(GL_LIGHTING);
    //  Location of viewer for specular calculations
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
    //  glColor sets ambient and diffuse color materials
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    //  Enable light 0
    glEnable(GL_LIGHT0);
    //  Set ambient, diffuse, specular components and position of light 0
    glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
    glLightfv(GL_LIGHT0,GL_POSITION,Position);
  }
  else{
    glDisable(GL_LIGHTING);
  }

  if(drawAxis){
    drawAxisLines();
    drawAxisLabels();
  }

  switch(currentScene){
    case 1:
      XB70Bomber(10,-5,0 , 1,0,0, 0,1,0, 0.5, 0, 0);
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
      FighterJet(20,20,25, 1,0,0, 0,1,0,0.8, -THX,0);
      ArtemisSpaceBomber(40, -20, -10, 1,0,0, 0,1,0,0.8, -THX,0);
      XB70Bomber(-30,10,-20, 1,0,0, 0,0,1,0.8, THX, 0);
  }
  
  //  Display rotation angles
  glColor3f(1,1,1);
  glWindowPos2i(5,5);
  Print("Angle X= %d",ph);
  Print("   Camera Mode: ");
  if(projectionMode == 1)
    Print(" First Person  ");
  else if (projectionMode == 2)
    Print(" Perspective  ");
  else if(projectionMode == 3)
    Print(" Orthogonal  ");

  if(projectionMode == 2)
    Print(" Dim=%.1f",dim);

  if(projectionMode == 1){
    Print(" CameraX=%.1f",cameraX);
    Print(" CameraY=%.1f",cameraY);
    Print(" CameraZ=%.1f",cameraZ);
  }
  
  glWindowPos2i(5,25);
  Print("Angle Y= %d",th);

  glWindowPos2i(5,45);
  if(drawAxis == true)
    Print("Axis ON");
  else
    Print("Axis OFF");

  //Check for errors
  ErrCheck("display");

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

// This function is called by GLUT when idle
void idle(){
  if(currentScene == 1){
    time = glutGet(GLUT_ELAPSED_TIME)/1000.0;
    THX = fmod(60*time,360);
    THZ = fmod(60*time,360);
    zh = fmod(90*time,360.0);
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
  Project(fov, asp, dim, projectionMode);
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
  glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-800)/2,
                       (glutGet(GLUT_SCREEN_HEIGHT)-600)/2);

  // Set window size
  glutInitWindowSize(800,600);

  // Create window
  glutCreateWindow("hw5 Riad Shash (Ray)");

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