/*
Riad Shash (Ray)
CSCI 5229
HW6 Textures

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

  In Perspective mode (default):
  Use keyboard arrow keys to spin the axis lines
  (left/right about the y axis)
  (up/down about the x axis)
  =/+     - Decrease dim (zoom in)
  -/_     - Increase dim (zoom out)
  z/x        Change field of view of perspective camera only

  Orthogonal mode:
  Use keyboard arrow keys to spin the axis lines
  (left/right about the y axis)
  (up/down about the x axis)

  Perspective mode & Orthogonal mode
  l          Toggles lighting
  a/A        Decrease/increase ambient light
  d/D        Decrease/increase diffuse light
  s/S        Decrease/increase specular light
  e/E        Decrease/increase emitted light
  n/N        Decrease/increase shininess
  F1         Toggle smooth/flat shading
  F2         Toggle local viewer mode
  F3         Toggle light distance (1/5)
  F8         Change ball increment
  F9         Invert bottom normal
  m          Toggles light movement
  []         Lower/rise light
  arrows     Change view angle
  0          Reset view angle
*/

#include "CSCIx229.h"

// ----------------------------------------------------------
// Global Variables
// ----------------------------------------------------------

// What the camera is looking at vector
double cameraLookX , cameraLookY , cameraLookZ;

// XZ position of the camera in 1st person (eye)
double cameraX=50, cameraY =10 , cameraZ=25;

int th=321;         //  Azimuth of view angle (y)
int ph=29;         //  Elevation of view angle (x)

double dim=150;   // Dimension of orthogonal box
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
int projectionMode = 2;     

double THX;
double THZ;
double time;

// Light values
int light     =   1;  //  Lighting
int one       =   1;  // Unit value
int distance  =  50; // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  25;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light
int move      =   1;  //  Move light
unsigned int texture[11]; // Texture names

//Texture settings
int mode=0;       //  Texture mode

// ----------------------------------------------------------
// Function Prototypes
// ----------------------------------------------------------
void display();
void reshape(int,int);
void idle();
static void special(int k,int x,int y);
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
static void Sphere(double x,double y,double z,double r);
static void engineSphere(double x,double y,double z,double r,double yRot);
static void FighterJet(double x,double y,double z,
                       double dx,double dy,double dz,
                       double ux,double uy, double uz, double scale, double thx, double thz);
static void XB70Bomber(double x,double y,double z,
                       double dx,double dy,double dz,
                       double ux,double uy, double uz, double scale, double thx, double thz);
static void cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th);

/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
  //  Set specular color to white
  float white[] = {1,1,1,1};
  float Emission[]  = {0.0,0.0,0.01*emission,1.0};
  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
  //  Save transformation
  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x,y,z);
  glRotated(th,0,1,0);
  glScaled(dx,dy,dz);
  //  Enable textures
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
  glColor3f(1,1,1);

  //  Front
  glBindTexture(GL_TEXTURE_2D,texture[5]);
  glBegin(GL_QUADS);
    glNormal3f( 0, 0, 1);
    glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
    glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
    glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
    glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
  glEnd();

  //  Back
  glBindTexture(GL_TEXTURE_2D,texture[10]);
  glBegin(GL_QUADS);
    glNormal3f( 0, 0,-1);
    glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
    glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
  glEnd();

  //  Right
  glBindTexture(GL_TEXTURE_2D,texture[7]);
  glBegin(GL_QUADS);
    glNormal3f(+1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
    glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
    glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
  glEnd();

  //  Left
  glBindTexture(GL_TEXTURE_2D,texture[6]);
  glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
    glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
    glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
  glEnd();

  //  Top
  glBindTexture(GL_TEXTURE_2D,texture[8]);
  glBegin(GL_QUADS);
    glNormal3f( 0,+1, 0);
    glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
    glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
    glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
    glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
  glEnd();

  //  Bottom
  glBindTexture(GL_TEXTURE_2D,texture[9]);
  glBegin(GL_QUADS);
    glNormal3f( 0,-1, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
    glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
  glEnd();
  //  Undo transformations and textures
  glPopMatrix();
  glDisable(GL_TEXTURE_2D);
}

// ----------------------------------------------------------
// special() Callback function
// ----------------------------------------------------------
static void special(int k,int x,int y)
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
  }else if(k == GLUT_KEY_RIGHT)
    th += 2.5;
  //  Smooth color model
  else if (k == GLUT_KEY_F1)
    smooth = 1-smooth;
  //  Local Viewer
  else if (k == GLUT_KEY_F2)
    local = 1-local;
  else if (k == GLUT_KEY_F3)
    distance = (distance==10) ? 20 : 10;
  //  Toggle ball increment
  else if (k == GLUT_KEY_F8)
    inc = (inc==10)?3:10;
  //  Flip sign
  else if (k == GLUT_KEY_F9)
    one = -one;

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
    fov = 55;
    ph = 0;
    th = -60;
  }else if(value == 2){
    fov=55;
    projectionMode = 2;
  }else if(value == 3){
    if(drawAxis == true)
      drawAxis = false;
    else
      drawAxis = true;
  }else if (value == 4){
    currentScene = 1;
  }else if (value == 5){
    currentScene = 2;
  }else if (value == 6){
    currentScene = 3;
  }else if (value == 7){
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
/*
 *  Draw vertex in polar coordinates
 */
static void Vertex(int th,int ph)
{
  double x = Cos(th)*Cos(ph);
  double y = Sin(th)*Cos(ph);
  double z =         Sin(ph);
  glNormal3d(x,y,z);
  glTexCoord2d(th/360.0,ph/180.0+0.5);
  glVertex3d(x,y,z);
}

/*
 *  Draw a ballEngineSphere
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
 *  Draw a Sphere
 *     at (x,y,z)
 *     radius (r)
 */
static void Sphere(double x,double y,double z,double r)
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
      glTexCoord2f(th/360.0,ph/90.0);
      Vertex(th,ph+d);
    }
    glEnd();
  }

  //  Undo transformations
  glPopMatrix();
}

/*
 *  Draw a Sphere
 *     at (x,y,z)
 *     radius (r)
 */
static void engineSphere(double x,double y,double z,double r, double yRot)
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
  glRotated(yRot,0,1,0);

  glMaterialf(GL_FRONT,GL_SHININESS,shiny);
  glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
  glMaterialfv(GL_FRONT,GL_EMISSION,Emission);

  //  Latitude bands
  for (ph=0;ph<50;ph+=d)
  {
    glBegin(GL_QUAD_STRIP);
    for (th=0;th<=360;th+=d)
    {
      Vertex(th,ph);
      glTexCoord2f(th/360.0,ph/90.0);
      Vertex(th,ph+d);
    }
    glEnd();
  }

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
  const double cockpitX = -4;        //X center of cockpit
  const double cockpitY = 0.70;

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

  //For normal vector calculations
  double aX, aY, aZ;
  double bX, bY, bZ;
  double nX, nY, nZ;

  //  Set specular color to white
  float white[] = {1,1,1,1};
  float Emission[]  = {0.0,0.0,0.01*emission,1.0};
  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

  //  Enable textures
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);

  //  Save current transforms
  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x,y,z);
  glMultMatrixd(mat);
  glRotated(thx,1,0,0);
  glRotated(thz,0,0,1);
  glScaled(scale,scale,scale);

  // ----------------------------------------------------------
  // Front nose (4 sided)
  // ---------------------------------------------------------
  glColor3f(0.8,0.8,0.8);
  findDispVector(shipBowXend,wid,wid,shipBowXfront,0.0,0.0,&aX,&aY,&aZ);
  findDispVector(shipBowXend,wid,wid,shipBowXend,-wid,wid,&bX,&bY,&bZ);
  findNormalVector(aX,aY,aZ,bX,bY,bZ,&nX,&nY,&nZ);
  glBindTexture(GL_TEXTURE_2D,texture[2]);

  //left triangle (-Z)
  glBegin(GL_TRIANGLES);
    glNormal3d(nX,nY,-nZ);
    glTexCoord2f(0.5,0.5);glVertex3d(shipBowXfront, 0.0, 0.0);
    glTexCoord2f(0,0.5);glVertex3d(shipBowXend, wid, wid);
    glTexCoord2f(0,0);glVertex3d(shipBowXend,-wid, wid);

    //right triangle (Z+)
    glNormal3d(nX,nY,nZ);
    glTexCoord2f(0.5,0.5);glVertex3d(shipBowXfront, 0.0, 0.0);
    glTexCoord2f(0,0.5);glVertex3d(shipBowXend, wid,-wid);
    glTexCoord2f(0,0);glVertex3d(shipBowXend,-wid,-wid);

    findDispVector(shipBowXend,wid,wid,shipBowXfront,0.0,0.0,&aX,&aY,&aZ);
    findDispVector(shipBowXend,wid,wid,shipBowXend,wid,-wid,&bX,&bY,&bZ);
    findNormalVector(aX,aY,aZ,bX,bY,bZ,&nX,&nY,&nZ);

    //Top triangle
    glNormal3d(nX,nY,nZ);
    glTexCoord2f(0.5,0.5);glVertex3d(shipBowXfront, 0.0, 0.0);
    glTexCoord2f(0,0.5);glVertex3d(shipBowXend, wid, wid);
    glTexCoord2f(0,0);glVertex3d(shipBowXend, wid,-wid);

    //Bottom triangle
    glNormal3d(nX,-nY,nZ);
    glTexCoord2f(0.5,0.5);glVertex3d(shipBowXfront, 0.0, 0.0);
    glTexCoord2f(0,0.5);glVertex3d(shipBowXend,-wid, wid);
    glTexCoord2f(0,0);glVertex3d(shipBowXend,-wid,-wid);
  glEnd();

  // ----------------------------------------------------------
  // Front tip
  // ---------------------------------------------------------
  glLineWidth(2);
  glBegin(GL_LINES);
    glNormal3d(1,0,0);
    glColor3f(1, 0, 0); 
    glVertex3f(shipBowXfront, 0, 0);
    glVertex3f(shipBowXfront+2, 0, 0);
  glEnd();
  glLineWidth(1);

  // ----------------------------------------------------------
  // Fuselage
  // ---------------------------------------------------------
  glBindTexture(GL_TEXTURE_2D,texture[0]);
  glColor3f(1,1,1);
  glBegin(GL_QUADS);
    glNormal3d(0,0,1);
    glTexCoord2f(3,0.5);glVertex3d(shipBowXend, wid, wid);
    glTexCoord2f(3,0);glVertex3d(shipBowXend,-wid, wid);
    glTexCoord2f(0,0);glVertex3d(shipSternX,-wid, wid);
    glTexCoord2f(0,0.5);glVertex3d(shipSternX, wid, wid);

    glNormal3d(0,0,-1);
    glTexCoord2f(3,0.5);glVertex3d(shipBowXend, wid,-wid);
    glTexCoord2f(3,0);glVertex3d(shipBowXend,-wid,-wid);
    glTexCoord2f(0,0);glVertex3d(shipSternX,-wid,-wid);
    glTexCoord2f(0,0.5);glVertex3d(shipSternX, wid,-wid);

    glNormal3d(0,1,0);
    glTexCoord2f(3,0.5);glVertex3d(shipBowXend, wid, wid);
    glTexCoord2f(3,0);glVertex3d(shipBowXend, wid,-wid);
    glTexCoord2f(0,0);glVertex3d(shipSternX, wid,-wid);
    glTexCoord2f(0,0.5);glVertex3d(shipSternX, wid, wid);

    glNormal3d(0,-1,0);
    glTexCoord2f(3,0.5);glVertex3d(shipBowXend,-wid, wid);
    glTexCoord2f(3,0);glVertex3d(shipBowXend,-wid,-wid);
    glTexCoord2f(0,0);glVertex3d(shipSternX,-wid,-wid);
    glTexCoord2f(0,0.5);glVertex3d(shipSternX,-wid, wid);

    glNormal3d(-1,0,0);
    glTexCoord2f(3,0.5);glVertex3d(shipSternX,-wid, wid);
    glTexCoord2f(3,0);glVertex3d(shipSternX, wid, wid);
    glTexCoord2f(0,0);glVertex3d(shipSternX, wid,-wid);
    glTexCoord2f(0,0.5);glVertex3d(shipSternX,-wid,-wid);
  glEnd();

  // ----------------------------------------------------------
  // Cockpit
  // ---------------------------------------------------------
  glColor3f(0.5,0.5,1);
  glBindTexture(GL_TEXTURE_2D,texture[3]);
  Sphere(cockpitX,cockpitY,0,0.9);

  // ----------------------------------------------------------
  // Canards
  // ---------------------------------------------------------
  glBindTexture(GL_TEXTURE_2D,texture[2]);
  glColor3f(0.8,0.8,0.8);
  glBegin(GL_TRIANGLES);
    //Right Canard (upper)
    glNormal3d(0,1,0);
    glTexCoord2f(0,1);glVertex3d(canardXend, 0, wid);
    glTexCoord2f(1,0);glVertex3d(canardXend, 0, canardZ);
    glTexCoord2f(0,0);glVertex3d(canardXfront, 0, wid);

    //Right Canard (lower)
    glNormal3d(0,-1,0);
    glTexCoord2f(0,1);glVertex3d(canardXend, -0.001, wid);
    glTexCoord2f(1,0);glVertex3d(canardXend, -0.001, canardZ);
    glTexCoord2f(0,0);glVertex3d(canardXfront, -0.001, wid);

    //Left Canard (upper)
    glNormal3d(0,1,0);
    glTexCoord2f(1,1);glVertex3d(canardXend, 0, -wid);
    glTexCoord2f(0,0);glVertex3d(canardXend, 0, -canardZ);
    glTexCoord2f(0,1);glVertex3d(canardXfront, 0, -wid);

    //Left Canard (lower)
    glNormal3d(0,-1,0);
    glTexCoord2f(1,1);glVertex3d(canardXend, -0.001, -wid);
    glTexCoord2f(0,0);glVertex3d(canardXend, -0.001, -canardZ);
    glTexCoord2f(0,1);glVertex3d(canardXfront, -0.001, -wid);
  glEnd();

  // ----------------------------------------------------------
  // Wing tips
  // ---------------------------------------------------------
  glLineWidth(2);
  glColor3f(1,0,0);
  glBegin(GL_LINES);
    //Right wing line
    glNormal3d(0,1,0);
    glVertex3d(wingLineXend, -wid, wingLineZ);
    glVertex3d(wingLinefrontX, -wid, wingLineZ);

    glNormal3d(0,-1,0);
    glVertex3d(wingLineXend, -wid-0.01, wingLineZ);
    glVertex3d(wingLinefrontX, -wid-0.01, wingLineZ);

    //Left wing line
    glNormal3d(0,1,0);
    glVertex3d(wingLineXend, -wid, -wingLineZ);
    glVertex3d(wingLinefrontX, -wid, -wingLineZ);

    glNormal3d(0,-1,0);
    glVertex3d(wingLineXend, -wid-0.01, -wingLineZ);
    glVertex3d(wingLinefrontX, -wid-0.01, -wingLineZ);
  glEnd();
  glLineWidth(1);
 

  // ----------------------------------------------------------
  // Vertical tail
  // ----------------------------------------------------------
  glBindTexture(GL_TEXTURE_2D,texture[2]);

  //  Vertical tail (Z+)
  glColor3f(0.8,0.8,0.8);
  glBegin(GL_POLYGON);
  glNormal3d(0,0,1);
    glTexCoord2f(0,1);glVertex3d(shipSternX, 6.0, 0.001);
    glTexCoord2f(0,0);glVertex3d(shipSternX, 1.0, 0.001);
    glTexCoord2f(1,0);glVertex3d(wingXend+4, 1.0, 0.001);
  glEnd();

  //  Vertical tail (-Z)
  glColor3f(0.8,0.8,0.8);
  glBegin(GL_POLYGON);
    glNormal3d(0,0,-1);
    glTexCoord2f(0,1);glVertex3d(shipSternX, 6.0, 0.0);
    glTexCoord2f(0,0);glVertex3d(shipSternX, 1.0, 0.0);
    glTexCoord2f(1,0);glVertex3d(wingXend+4, 1.0, 0.0);
  glEnd();

  // ----------------------------------------------------------
  // Wings
  // ----------------------------------------------------------
  glBindTexture(GL_TEXTURE_2D,texture[2]);

  glColor3f(0.8,0.8,0.8);
  //Right wing (upper)
  glBegin(GL_POLYGON);
    glNormal3d(0,1,0);
    glTexCoord2f(0,0);glVertex3d(wingXend, -wid, wid);
    glTexCoord2f(0,2);glVertex3d(wingXend, -wid, wingZ);
    glTexCoord2f(2,2);glVertex3d(wingXfrontFold, -wid, wingZ);
    glTexCoord2f(3,0);glVertex3d(wingXfront, -wid, wid);
  glEnd();

  //Right wing (lower)
  glBegin(GL_POLYGON);
    glNormal3d(0,-1,0);
    glTexCoord2f(0,0);glVertex3d(wingXend, -wid-0.01, wid);
    glTexCoord2f(0,2);glVertex3d(wingXend, -wid-0.01, wingZ);
    glTexCoord2f(2,2);glVertex3d(wingXfrontFold-0.01, -wid, wingZ);
    glTexCoord2f(3,0);glVertex3d(wingXfront, -wid-0.01, wid);
  glEnd();

  //Left wing (upper)
  glBegin(GL_POLYGON);
    glNormal3d(0,1,0);
    glTexCoord2f(0,0);glVertex3d(wingXend, -wid, -wid);
    glTexCoord2f(0,2);glVertex3d(wingXend, -wid, -wingZ);
    glTexCoord2f(2,2);glVertex3d(wingXfrontFold, -wid, -wingZ);
    glTexCoord2f(3,0);glVertex3d(wingXfront, -wid, -wid);
  glEnd();

  //Left wing (lower)
  glBegin(GL_POLYGON);
    glNormal3d(0,-1,0);
    glTexCoord2f(0,0);glVertex3d(wingXend, -wid-0.01, -wid);
    glTexCoord2f(0,2);glVertex3d(wingXend, -wid-0.01, -wingZ);
    glTexCoord2f(2,2);glVertex3d(wingXfrontFold, -wid-0.01, -wingZ);
    glTexCoord2f(3,0);glVertex3d(wingXfront, -wid-0.01, -wid);
  glEnd();

  // ----------------------------------------------------------
  // Rear engine
  // ----------------------------------------------------------
  glBindTexture(GL_TEXTURE_2D,texture[4]);
  engineSphere(shipSternX,0,0,1,-90);

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
  const double shipFrontNoseX = 1.0;
  const double shipRearNoseX = -9.0;
  const double shipWidth = 2.0;
  const double cockpitLocX = shipRearNoseX - shipWidth + 2.25;
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
  const double canardZ = 5.5;

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

  //  Set specular color to white
  float white[] = {1,1,1,1};
  float Emission[]  = {0.0,0.0,0.01*emission,1.0};
  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

  //  Enable textures
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
  //glBindTexture(GL_TEXTURE_2D,texture[0]);

  // ----------------------------------------------------------
  // Nose
  // ----------------------------------------------------------

  //  Front Nose (4 sided)
  glColor3f(0.7,0.7,0.7);
  glBindTexture(GL_TEXTURE_2D,texture[2]);
    
    //Right side nose
  glBegin(GL_TRIANGLES);
    findDispVector(shipRearNoseX, shipFuselageHeight, shipWidth, shipFrontNoseX, 0.0,0.0, &aX,&aY,&aZ);
    findDispVector(shipRearNoseX, shipFuselageHeight, shipWidth, shipRearNoseX, -shipFuselageHeight, shipWidth, &bX, &bY, &bZ);
    findNormalVector(aX,aY,aZ,bX,bY,bZ,&nX,&nY,&nZ);
    glNormal3d(nX,nY,-nZ);
    glTexCoord2f(0.5,0.5);glVertex3d(shipFrontNoseX, 0.0, 0.0);
    glTexCoord2f(0,0.5);glVertex3d(shipRearNoseX, shipFuselageHeight, shipWidth);
    glTexCoord2f(0,0);glVertex3d(shipRearNoseX,-shipFuselageHeight, shipWidth);

    //Right side nose
    glNormal3d(nX,nY,nZ);
    glTexCoord2f(0.5,0.5);glVertex3d(shipFrontNoseX, 0.0, 0.0);
    glTexCoord2f(0,0.5);glVertex3d(shipRearNoseX, shipFuselageHeight,-shipWidth);
    glTexCoord2f(0,0);glVertex3d(shipRearNoseX,-shipFuselageHeight,-shipWidth);

    //Top side nose
    findDispVector(shipRearNoseX, shipFuselageHeight, shipWidth, shipFrontNoseX, 0.0,0.0, &aX,&aY,&aZ);
    findDispVector(shipRearNoseX, shipFuselageHeight, shipWidth, shipRearNoseX, shipFuselageHeight, -shipWidth, &bX,&bY,&bZ);
    findNormalVector(aX,aY,aZ,bX,bY,bZ,&nX,&nY,&nZ);

    glNormal3d(nX,nY,nZ);
    glTexCoord2f(0.5,0.5);glVertex3d(shipFrontNoseX, 0.0, 0.0);
    glTexCoord2f(0,0.5);glVertex3d(shipRearNoseX, shipFuselageHeight, shipWidth);
    glTexCoord2f(0,0);glVertex3d(shipRearNoseX, shipFuselageHeight,-shipWidth);

    //Bottom side nose3
    glNormal3d(nX,-nY,nZ);
    glTexCoord2f(0.5,0.5);glVertex3d(shipFrontNoseX, 0.0, 0.0);
    glTexCoord2f(0,0.5);glVertex3d(shipRearNoseX,-shipFuselageHeight, shipWidth);
    glTexCoord2f(0,0);glVertex3d(shipRearNoseX,-shipFuselageHeight,-shipWidth);
  glEnd();
  
  // ----------------------------------------------------------
  // Fuselage
  // ----------------------------------------------------------

  //  Fuselage (front section)
  glBindTexture(GL_TEXTURE_2D,texture[1]);
  glBegin(GL_QUADS);
    glColor3f(1,1,1);
    //Right side
    glNormal3d(0,0,1);
    glTexCoord2f(1,0.5); glVertex3d(shipRearNoseX, shipFuselageHeight, shipWidth);
    glTexCoord2f(1,0);glVertex3d(shipRearNoseX,-shipFuselageHeight, shipWidth);
    glTexCoord2f(0,0);glVertex3d(frontFuselageXEnd,-shipFuselageHeight, shipWidth);
    glTexCoord2f(0,0.5);glVertex3d(frontFuselageXEnd, shipFuselageHeight, shipWidth);
  glEnd();

    //Left side
  glBegin(GL_QUADS);
    glNormal3d(0,0,-1);
    glTexCoord2f(1,0.5);glVertex3d(shipRearNoseX, shipFuselageHeight,-shipWidth);
    glTexCoord2f(1,0);glVertex3d(shipRearNoseX,-shipFuselageHeight,-shipWidth);
    glTexCoord2f(0,0);glVertex3d(frontFuselageXEnd,-shipFuselageHeight,-shipWidth);
    glTexCoord2f(0,0.5);glVertex3d(frontFuselageXEnd, shipFuselageHeight,-shipWidth);
  glEnd();

  glBindTexture(GL_TEXTURE_2D,texture[0]);
    //Top side
  glBegin(GL_QUADS);
    glNormal3d(0,1,0);
    glTexCoord2f(1,1);glVertex3d(shipRearNoseX, shipFuselageHeight, -shipWidth);
    glTexCoord2f(0,1);glVertex3d(frontFuselageXEnd, shipFuselageHeight, -shipWidth);
    glTexCoord2f(0,0);glVertex3d(frontFuselageXEnd, shipFuselageHeight, shipWidth);
    glTexCoord2f(1,0);glVertex3d(shipRearNoseX, shipFuselageHeight, shipWidth);
  glEnd();
    
  glBegin(GL_QUADS);
    //Bottom side
    glNormal3d(0,-1,0);
    glTexCoord2f(0,0);glVertex3d(shipRearNoseX,-shipFuselageHeight, shipWidth);
    glTexCoord2f(0,1);glVertex3d(shipRearNoseX,-shipFuselageHeight,-shipWidth);
    glTexCoord2f(5,1);glVertex3d(shipRearX,-shipFuselageHeight,-shipWidth);
    glTexCoord2f(5,0);glVertex3d(shipRearX,-shipFuselageHeight, shipWidth);
  glEnd();

  //Calculation of normal for second fuselage section
  
  findDispVector(frontFuselageXEnd, shipFuselageHeight, -shipWidth, shipRearX, -shipFuselageHeight, -shipWidth, &aX, &aY, &aZ);
  findDispVector(shipRearX, -shipFuselageHeight, -shipWidth, shipRearX, -shipFuselageHeight, shipWidth, &bX, &bY, &bZ);
  findNormalVector(aX,aY,aZ,bX,bY,bZ,&nX,&nY,&nZ);


  //Rear fuselage section:
  glBegin(GL_QUADS);
    //Second section top side (biggest part)
    glNormal3d(nX,nY,nZ);
    glTexCoord2f(4,0);glVertex3d(frontFuselageXEnd, shipFuselageHeight, shipWidth);
    glTexCoord2f(4,1);glVertex3d(frontFuselageXEnd, shipFuselageHeight,-shipWidth);
    glTexCoord2f(0,1);glVertex3d(shipRearX, -shipFuselageHeight,-shipWidth);
    glTexCoord2f(0,0);glVertex3d(shipRearX, -shipFuselageHeight, shipWidth);
  glEnd();

  //Rear fuselage sides
  glBindTexture(GL_TEXTURE_2D,texture[0]);
  glBegin(GL_POLYGON);
    //Right side
    glNormal3d(0,0,1);
    glTexCoord2f(5,1);glVertex3d(frontFuselageXEnd, shipFuselageHeight, shipWidth);
    glTexCoord2f(0,0);glVertex3d(shipRearX, -shipFuselageHeight, shipWidth);
    glTexCoord2f(5,0);glVertex3d(frontFuselageXEnd, -shipFuselageHeight, shipWidth);
  glEnd();

  glBegin(GL_POLYGON);
    //Left side
    glNormal3d(0,0,-1);
    glTexCoord2f(0,1);glVertex3d(frontFuselageXEnd, shipFuselageHeight, -shipWidth);
    glTexCoord2f(0,0);glVertex3d(frontFuselageXEnd, -shipFuselageHeight, -shipWidth);
    glTexCoord2f(5,0);glVertex3d(shipRearX, -shipFuselageHeight, -shipWidth);
  glEnd();

  // ----------------------------------------------------------
  // Front tip
  // ---------------------------------------------------------
  glLineWidth(2);
  glBegin(GL_LINES);
    glNormal3d(1,0,0);
    glColor3f(1, 0, 0); 
    glVertex3f(shipFrontNoseX, 0, 0);
    glVertex3f(shipFrontNoseX+2, 0, 0);
  glEnd();
  glLineWidth(1);

  // ----------------------------------------------------------
  // Wings
  // ----------------------------------------------------------
  glBindTexture(GL_TEXTURE_2D,texture[2]);
  //Right wing (bottom portion)
  glColor3f(1,1,1);
  glBegin(GL_POLYGON);
    glNormal3d(0,-1,0);
    glTexCoord2f(0,0);glVertex3d(wingsRearX, -shipFuselageHeight, shipWidth);
    glTexCoord2f(0,2);glVertex3d(wingsRearX, -shipFuselageHeight, foldingWingZ);
    glTexCoord2f(2,2);glVertex3d(foldingWingXfront, -shipFuselageHeight, foldingWingZ);
    glTexCoord2f(3,0);glVertex3d(wingsFrontX, -shipFuselageHeight, shipWidth);
  glEnd();

   //Right (upper portion)
  glColor3f(1,1,1);
  glBegin(GL_POLYGON);
    glNormal3d(0,1,0);
    glTexCoord2f(0,0);glVertex3d(wingsRearX, -shipFuselageHeight+0.01, shipWidth);
    glTexCoord2f(0,2);glVertex3d(wingsRearX, -shipFuselageHeight+0.01, foldingWingZ);
    glTexCoord2f(2,2);glVertex3d(foldingWingXfront, -shipFuselageHeight+0.01, foldingWingZ);
    glTexCoord2f(3,0);glVertex3d(wingsFrontX, -shipFuselageHeight+0.01, shipWidth);
  glEnd();

  //Left wing (bottom portion)
  glColor3f(1,1,1);
  glBegin(GL_POLYGON);
    glNormal3d(0,-1,0);
    glTexCoord2f(0,0);glVertex3d(wingsRearX, -shipFuselageHeight, -shipWidth);
    glTexCoord2f(0,2);glVertex3d(wingsRearX, -shipFuselageHeight, -foldingWingZ);
    glTexCoord2f(2,2);glVertex3d(foldingWingXfront, -shipFuselageHeight, -foldingWingZ);
    glTexCoord2f(3,0);glVertex3d(wingsFrontX, -shipFuselageHeight, -shipWidth);
  glEnd();

  //Left wing (upper portion)
  glColor3f(1,1,1);
  glBegin(GL_POLYGON);
    glNormal3d(0,1,0);
    glTexCoord2f(0,0);glVertex3d(wingsRearX, -shipFuselageHeight+0.01, -shipWidth);
    glTexCoord2f(0,2);glVertex3d(wingsRearX, -shipFuselageHeight+0.01, -foldingWingZ);
    glTexCoord2f(2,2);glVertex3d(foldingWingXfront, -shipFuselageHeight+0.01, -foldingWingZ);
    glTexCoord2f(3,0);glVertex3d(wingsFrontX, -shipFuselageHeight+0.01, -shipWidth);
  glEnd();

  // ----------------------------------------------------------
  // Folding Wings
  // ----------------------------------------------------------
  glBindTexture(GL_TEXTURE_2D,texture[0]);

  findDispVector(foldingWingXfront, -shipFuselageHeight, foldingWingZ, wingsRearX, -shipFuselageHeight, foldingWingZ, &aX,&aY,&aZ);
  findDispVector(wingsRearX, -shipFuselageHeight, foldingWingZ, wingsRearX, -foldingWingYHeight-shipFuselageHeight, foldingWingZ+2, &bX,&bY,&bZ);
  findNormalVector(aX,aY,aZ,bX,bY,bZ,&nX,&nY,&nZ);

  //Right folding wing (upper portion)
  glColor3f(1,1,1);
  glBegin(GL_POLYGON);
    glNormal3d(nX,nY,nZ);
    glTexCoord2f(0,0.5);glVertex3d(wingsRearX, -shipFuselageHeight, foldingWingZ);
    glTexCoord2f(0,0);glVertex3d(wingsRearX, -foldingWingYHeight-shipFuselageHeight, foldingWingZ+3);
    glTexCoord2f(1,0);glVertex3d(foldingWingXfront, -shipFuselageHeight, foldingWingZ);
  glEnd();

  //Right folding wing (bottom portion)
  glColor3f(1,1,1);
  glBegin(GL_POLYGON);
    glNormal3d(-nX,-nY,-nZ);
    glTexCoord2f(0,0.5);glVertex3d(wingsRearX, -shipFuselageHeight, foldingWingZ);
    glTexCoord2f(0,0);glVertex3d(wingsRearX, -foldingWingYHeight-shipFuselageHeight-0.01, foldingWingZ+3);
    glTexCoord2f(1,0);glVertex3d(foldingWingXfront, -shipFuselageHeight, foldingWingZ);
  glEnd();

  //Left folding wing (upper portion)
  glColor3f(1,1,1);
  glBegin(GL_POLYGON);
    glNormal3d(nX,nY,-nZ);
    glTexCoord2f(0,0.5);glVertex3d(wingsRearX, -shipFuselageHeight, -foldingWingZ);
    glTexCoord2f(0,0);glVertex3d(wingsRearX, -foldingWingYHeight-shipFuselageHeight, -foldingWingZ-3);
    glTexCoord2f(1,0);glVertex3d(foldingWingXfront, -shipFuselageHeight, -foldingWingZ);
  glEnd();

  //Left folding wing (bottom portion)
  glColor3f(1,1,1);
  glBegin(GL_POLYGON);
    glNormal3d(nX,-nY,nZ);
    glTexCoord2f(0,0.5);glVertex3d(wingsRearX, -shipFuselageHeight, -foldingWingZ);
    glTexCoord2f(0,0);glVertex3d(wingsRearX, -foldingWingYHeight-shipFuselageHeight-0.01, -foldingWingZ-3);
    glTexCoord2f(1,0);glVertex3d(foldingWingXfront, -shipFuselageHeight, -foldingWingZ);
  glEnd();

  // ----------------------------------------------------------
  // Vertical stabilizers
  // ----------------------------------------------------------
  glBindTexture(GL_TEXTURE_2D,texture[0]);
  //Righ verical (Z+)
  glColor3f(0.8,0.8,0.8);
  glBegin(GL_POLYGON);
    glNormal3d(0,0,1);
    glTexCoord2f(0,1);glVertex3d(wingsRearX, verticalsHeight, verticalsZ);
    glTexCoord2f(0,0);glVertex3d(wingsRearX, -shipFuselageHeight,verticalsZ);
    glTexCoord2f(1,0);glVertex3d(verticalsXfront, -shipFuselageHeight, verticalsZ);
  glEnd();

  //Righ verical (-Z)
  glColor3f(0.8,0.8,0.8);
  glBegin(GL_POLYGON);
    glNormal3d(0,0,-1);
    glTexCoord2f(0,1);glVertex3d(wingsRearX, verticalsHeight, verticalsZ-0.01);
    glTexCoord2f(0,0);glVertex3d(wingsRearX, -shipFuselageHeight,verticalsZ-0.01);
    glTexCoord2f(1,0);glVertex3d(verticalsXfront, -shipFuselageHeight, verticalsZ-0.01);
  glEnd();

  //Left verical (Z+)
  glColor3f(0.8,0.8,0.8);
  glBegin(GL_POLYGON);
    glNormal3d(0,0,1);
    glTexCoord2f(0,1);glVertex3d(wingsRearX, verticalsHeight, -verticalsZ);
    glTexCoord2f(0,0);glVertex3d(wingsRearX, -shipFuselageHeight,-verticalsZ);
    glTexCoord2f(1,0);glVertex3d(verticalsXfront, -shipFuselageHeight, -verticalsZ);
  glEnd();

  //Left verical (-Z)
  glColor3f(0.8,0.8,0.8);
  glBegin(GL_POLYGON);
    glNormal3d(0,0,-1);
    glTexCoord2f(0,1);glVertex3d(wingsRearX, verticalsHeight, -verticalsZ-0.01);
    glTexCoord2f(0,0);glVertex3d(wingsRearX, -shipFuselageHeight,-verticalsZ-0.01);
    glTexCoord2f(1,0);glVertex3d(verticalsXfront, -shipFuselageHeight, -verticalsZ-0.01);
  glEnd();

  // ----------------------------------------------------------
  // Canards
  // ----------------------------------------------------------

  //Right Canard (bottom portion)
  glBindTexture(GL_TEXTURE_2D,texture[2]);
  glColor3f(0.75,0.75,0.75);
  glBegin(GL_POLYGON);
    glNormal3d(0,-1,0);
    glTexCoord2f(1,0);glVertex3d(canardRearX,0,shipWidth);
    glTexCoord2f(0.8,0.5);glVertex3d(canardRearX+1,0,canardZ);
    glTexCoord2f(0.3,0.5);glVertex3d(canardRearX+4,0,canardZ);
    glTexCoord2f(0,0);glVertex3d(canardFrontX,0,shipWidth);
  glEnd();

  //Right Canard (upper portion)
  glBegin(GL_POLYGON);
    glNormal3d(0,1,0);
    glTexCoord2f(1,0);glVertex3d(canardRearX,0+0.01,shipWidth);
    glTexCoord2f(0.8,0.5);glVertex3d(canardRearX+1,0+0.01,canardZ);
    glTexCoord2f(0.3,0.5);glVertex3d(canardRearX+4,0+0.01,canardZ);
    glTexCoord2f(0,0);glVertex3d(canardFrontX,0+0.01,shipWidth);  //Frontmost section
  glEnd();

  //Left Canard (bottom portion)
  glBegin(GL_POLYGON);
    glNormal3d(0,-1,0);
    glTexCoord2f(1,0);glVertex3d(canardRearX,0,-shipWidth);
    glTexCoord2f(0.8,0.5);glVertex3d(canardRearX+1,0,-canardZ);
    glTexCoord2f(0.3,0.5);glVertex3d(canardRearX+4,0,-canardZ);
    glTexCoord2f(0,0);glVertex3d(canardFrontX,0,-shipWidth);
  glEnd();

  //Left Canard (upper portion)
  glBegin(GL_POLYGON);
    glTexCoord2f(1,0);glNormal3d(0,1,0);
    glVertex3d(canardRearX,0+0.01,-shipWidth);
    glTexCoord2f(0.8,0.5);glVertex3d(canardRearX+1,0+0.01,-canardZ);
    glTexCoord2f(0.3,0.5);glVertex3d(canardRearX+4,0+0.01,-canardZ);
    glTexCoord2f(0,0);glVertex3d(canardFrontX,0+0.01,-shipWidth);
  glEnd();

  // ----------------------------------------------------------
  // Inlets
  // ----------------------------------------------------------
  glBindTexture(GL_TEXTURE_2D,texture[2]);
  //Inlet middle, left and right side
  glColor3f(1,1,1); //white
  glBegin(GL_QUADS);
    //Middle inlet
    glTexCoord2f(3,0.2);glVertex3d(inletMiddleForntX,inletY,0);
    glTexCoord2f(0,0.2);glVertex3d(inletMiddleRearX, inletY, 0);
    glTexCoord2f(0,0);glVertex3d(inletMiddleRearX, -shipFuselageHeight, 0);
    glTexCoord2f(3,0);glVertex3d(inletMiddleForntX, -shipFuselageHeight,0);
  glEnd();

  glBindTexture(GL_TEXTURE_2D,texture[0]);
    //Right inlet side
  glBegin(GL_QUADS);
    glNormal3d(0,0,1);
    glTexCoord2f(3,0.2);glVertex3d(inletSidefrontX,inletY,shipWidth);
    glTexCoord2f(0,0.2);glVertex3d(inletMiddleRearX, inletY, shipWidth);
    glTexCoord2f(0,0);glVertex3d(inletMiddleRearX, -shipFuselageHeight, shipWidth);
    glTexCoord2f(3,0);glVertex3d(inletSidefrontX, -shipFuselageHeight, shipWidth);
  glEnd();

    //Left inlet side
  glBegin(GL_QUADS);
    glNormal3d(0,0,-1);
    glTexCoord2f(3,0.2);glVertex3d(inletSidefrontX,inletY,-shipWidth);
    glTexCoord2f(0,0.2);glVertex3d(inletMiddleRearX, inletY, -shipWidth);
    glTexCoord2f(0,0);glVertex3d(inletMiddleRearX, -shipFuselageHeight, -shipWidth);
    glTexCoord2f(3,0);glVertex3d(inletSidefrontX, -shipFuselageHeight, -shipWidth);
  glEnd();

  //Back inlet left bottom side
  glColor3f(0.75,0.75,0.75);
  glBegin(GL_POLYGON);
    glNormal3d(0,-1,0);
    glTexCoord2f(2.8,0.2);glVertex3f(inletSidefrontX, inletY, shipWidth);
    glTexCoord2f(0,0.2);glVertex3d(inletMiddleRearX, inletY, shipWidth);
    glTexCoord2f(0,0);glVertex3d(inletMiddleRearX, inletY, 0);
    glTexCoord2f(3,0);glVertex3d(inletMiddleForntX, inletY, 0);
  glEnd();

  //Back inlet right bottom side
  glColor3f(0.75,0.75,0.75);
  glBegin(GL_POLYGON);
    glNormal3d(0,-1,0);
    glTexCoord2f(2.8,0.2);glVertex3f(inletSidefrontX, inletY, -shipWidth);
    glTexCoord2f(0,0.2);glVertex3d(inletMiddleRearX, inletY, -shipWidth);
    glTexCoord2f(0,0);glVertex3d(inletMiddleRearX, inletY, 0);
    glTexCoord2f(3,0);glVertex3d(inletMiddleForntX, inletY, 0);
  glEnd();

  // ----------------------------------------------------------
  // Cockpit
  // ----------------------------------------------------------
  glColor3f(0.5,0.5,1);
  glBindTexture(GL_TEXTURE_2D,texture[3]);
  Sphere(cockpitLocX,shipFuselageHeight - 0.8,0,shipWidth - 0.25);

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

  //Changing Dim only allowed in Perspective
  if(projectionMode == 2){
      if (ch == '=' || ch == '+')
      dim -= 1;
    else if (ch == '-' || ch == '_')
      dim += 1;
  }

  //Moving camera only in First person & FOV
  if (projectionMode == 1){
    if(ch == 'w' || ch =='W')
      moveForward(cameraLookX, cameraLookY, cameraLookZ, 2);
    else if(ch == 's' || ch == 'S')
      moveBackward(cameraLookX, cameraLookY, cameraLookZ, 2);  
    else if(ch == 'a' || ch == 'A')
      strafeLeft(cameraLookX, cameraLookY, cameraLookZ, 2);
    else if(ch == 'd' || ch == 'D')
      strafeRight(cameraLookX, cameraLookY, cameraLookZ, 2);
      //  Change field of view angle
    else if (ch == 'z' && ch>1){
        fov--;   
    }else if (ch == 'x' && ch<179){
        fov++;
    }
  }

  if(projectionMode != 1){
    //  Reset view angle
    if (ch == '0')
      th = ph = 0;
    //  Toggle lighting
    else if (ch == 'l' || ch == 'L')
      light = 1-light;
    //  Toggle light movement
    else if (ch == 'm' || ch == 'M')
      move = 1-move;
    //  Move light
    else if (ch == '<')
      zh += 1;
    else if (ch == '>')
      zh -= 1;
    //  Change field of view angle
    else if (ch == 'z' && ch>1){
      if(projectionMode == 2)
        fov--;   
    }else if (ch == 'x' && ch<179){
      if(projectionMode == 2)
        fov++;
    }
    //  Light elevation
    else if (ch=='[')
      ylight -= 0.1;
    else if (ch==']')
      ylight += 0.1;
    //  Ambient level
    else if (ch=='a' && ambient>0)
      ambient -= 5;
    else if (ch=='A' && ambient<100)
      ambient += 5;
    //  Diffuse level
    else if (ch=='d' && diffuse>0)
      diffuse -= 5;
    else if (ch=='D' && diffuse<100)
      diffuse += 5;
    //  Specular level
    else if (ch=='s' && specular>0)
      specular -= 5;
    else if (ch=='S' && specular<100)
      specular += 5;
    //  Emission level
    else if (ch=='e' && emission>0)
      emission -= 5;
    else if (ch=='E' && emission<100)
      emission += 5;
    //  Shininess level
    else if (ch=='n' && shininess>-1)
      shininess -= 1;
    else if (ch=='N' && shininess<7)
      shininess += 1;
    //  Translate shininess power to value (-1 => 0)
    shiny = shininess<0 ? 0 : pow(2.0,shininess);
  }
  
  // Reproject
  Project(fov, asp, dim, projectionMode);

  //  Animate if requested
  glutIdleFunc(move?idle:NULL);
  
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
  glutAddMenuEntry("Axis ON/OFF", 3);
  glutAddMenuEntry("Scene 1", 4);
  glutAddMenuEntry("Scene 2", 5);
  glutAddMenuEntry("Scene 3", 6);
  glutAddMenuEntry("Quit", 7);

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
    double Ex = -2*(dim/2)*Sin(th)*Cos(ph);
    double Ey = +2*(dim/2)        *Sin(ph);
    double Ez = +2*(dim/2)*Cos(th)*Cos(ph);
    gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
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
    ball(Position[0],Position[1],Position[2] , 0.25);
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
      //FighterJet(0,0,0 , 1,0,0, 0,1,0, 0.5, 0, 5);
      XB70Bomber(10,-5,0 , 1,0,0, 0,1,0, 1.5, 0, 5);
      FighterJet(10,-5,-50 , 1,0,0, 0,1,0, 1.5, 0, 5);
      FighterJet(10,-5,50 , 1,0,0, 0,1,0, 1.5, 0, 5);
      cube(0,0,0,250,250,250,0);
      break;
    case 2:
      FighterJet(10,5,20, 1,0,0, 0,1,0,0.5, 25,0);
      FighterJet(10,5,-20, 1,0,0, 0,1,0,0.5, -25,0);
      FighterJet(-10,20,0, 1,0,0, 0,1,0,0.5, 0, 25);
      XB70Bomber(20,10,0 , 1,0,0, 0,1,0,0.5, 0, 10);
      FighterJet(20,-20,0, 1,0,0, 0,-1,0,0.5, 0, 25);
      //XB70Bomber(-10,20,20 , 1,0,0, 0,1,0,0.5, 0, 25);
      //XB70Bomber(-10,20,-20 , 1,0,0, 0,1,0,0.5, -25, -25);
      break;
    case 3:
      XB70Bomber(10,-5,0 , 1,0,0, 0,1,0, 0.5, THX+90, 0);
      FighterJet(10,-5,-15 , 1,0,0, 0,1,0, 0.5, -THX, 0);
      FighterJet(10,-5,15 , 1,0,0, 0,1,0, 0.5, THX, 0);
      break; 
  }
  
  //  Display parameters
  glColor3f(0,1,0);
  glWindowPos2i(5,5);
  Print("Angle=%d,%d  Dim=%.1f FOV=%d Light=%s",th,ph,dim,fov,light?"On":"Off");
  if (light)
  {
    glWindowPos2i(5,45);
    Print("Model=%s LocalViewer=%s Distance=%d Elevation=%.1f",smooth?"Smooth":"Flat",local?"On":"Off",distance,ylight);
    glWindowPos2i(5,25);
    Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shiny);
  }
  glWindowPos2i(5,65);
  Print("Camera Mode: ");
  if(projectionMode == 1)
    Print(" First Person  ");
  else if (projectionMode == 2)
    Print(" Perspective  ");
  else if(projectionMode == 3)
    Print(" Orthogonal  ");

  glWindowPos2i(5,85);
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
  
  time = glutGet(GLUT_ELAPSED_TIME)/1000.0;
  THX = fmod(60*time,360);
  THZ = fmod(60*time,360);
  zh = fmod(90*time,360.0);
  glutPostRedisplay();
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
  glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-1280)/2,
                       (glutGet(GLUT_SCREEN_HEIGHT)-720)/2);

  // Set window size
  glutInitWindowSize(1280,720);

  // Create window
  glutCreateWindow("hw6 Riad Shash (Ray)");

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

  //  Load textures
  texture[0] = LoadTexBMP("WhiteMetal.bmp");
  texture[1] = LoadTexBMP("MetalUs.bmp");
  texture[2] = LoadTexBMP("al.bmp");
  texture[3] = LoadTexBMP("glass.bmp");
  texture[4] = LoadTexBMP("engineTexture.bmp");

  //  For the skybox
  texture[5] = LoadTexBMP("front.bmp");
  texture[6] = LoadTexBMP("left.bmp");
  texture[7] = LoadTexBMP("right.bmp");
  texture[8] = LoadTexBMP("top.bmp");
  texture[9] = LoadTexBMP("bottom.bmp");
  texture[10] = LoadTexBMP("back.bmp");

  //  Pass control to GLUT for events
  glutMainLoop();

  // ANSI C requires main to return int
  return 0;
}
