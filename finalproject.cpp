

/******************************************
*
* Official Name:  Huahao Shang
*
* Call me: Shang or Wilson
*
* E-mail:  hushang@syr.edu
*
* Assignment:  Final Project
*
* Environment/Compiler:  Xcode 11.5
*
* Date submitted:  April 12,  2022
*
* References:
 1. Checked default glmaterialfv for objects. https://docs.microsoft.com/en-us/windows/win32/opengl/glmaterialfv
* 2. All textures are referenced in documentation.
 3. The rest part are designed by myself and generate some scientific values for planets' data from the internet.
* Interactions:
1.Run the program
2. You can click either solar system or proxima centuri to see different simulations
3. You can press space to pause/resume  the simulation
4. change you views pressing 1 or 2.
5. up, down, left and right arrow key to move your view, in view 1, you won't move if you hit aplanet
6. + and - to speed up and speed down the simulation.
7. 'l' to enable/disable the light, 'i' to enable the sun particles.
8. press 's' and 'p' to switch from solat system and proxima centuri
9. right click the mouse to see the menu and have operations to choose.
10. Have fun to play with the simulation.
*
*******************************************/
//#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>
#include <stdio.h>
#include <cmath>
#include <cstdlib>
#include <fstream>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif
#define INITIAL_POINT_SIZE 5.0
#define MAX_NUM_PARTICLES 1000
#define PI 3.1415926
using namespace std;

//views, and some boolean value to check some states such as pause, clicking, enable lighting...

static bool view1 = true, view2 = false, stop = false, expand = false, clicked = false, lighting = true, sunPartical = false;
int height, width, xClick, yClick;

// my view1 default

static float meX = 0, meY = 0, meZ = -10;
// my spaceship and view2 default calue, and value increment for view movement
float shipX = meX, shipY = meY, shipZ = meZ+2;
static float meX2 = 0, meY2 = 10, meZ2 = -50;
static float angle1 = 0, stepsize = 0.3, turnsize = 5;

//solar value, all related values to draw the planet, their orbital value, orbital speed, etc.

static float sun_radius = 3, sun_self_rotation = 0, sun_self_rotation_increase = 1;

static float earth_radius = 0.5, earth_self_rotation = 0, earth_self_rotation_increase = 5, earthOrb = 10, earthOrbSpeed = 0,earthOrb_increase = 1, moonOrbSpeed = 0, moonOrb_increase = 3,proximaOrb_increase = 1.2,
mercury_radius = 0.2, mercury_self_rotation = 0,  mercury_self_rotation_increase = 1, mercuryOrb =6, mercuryOrbSpeed = 0, mercuryOrb_increase = earthOrb_increase*4,
venus_radius = 0.47, venus_self_rotation = 0,  venus_self_rotation_increase = 0.5, venusOrb = 8, venusOrbSpeed = 0, venusOrb_increase = earthOrb_increase*2,
mars_radius = 0.28, mars_self_rotation = 0,  mars_self_rotation_increase = 5, marsOrb = 13, marsOrbSpeed = 0, marsOrb_increase = earthOrb_increase*.5,
particalOrbSpeed = 0, particleOrbSpeed_increase = earthOrb_increase*0.8,
jupiter_radius = 1.5, jupiter_self_rotation = 0,  jupiter_self_rotation_increase = 10, jupiterOrb = 25, jupiterOrbSpeed = 0, jupiterOrb_increase = earthOrb_increase*.2,
staurn_radius = 1.3, staurn_self_rotation = 0,  staurn_self_rotation_increase = 10, staurnOrb = 30, staurnOrbSpeed = 0, staurnOrb_increase = earthOrb_increase*.1,
uranus_radius = 0.9, uranus_self_rotation = 0,  uranus_self_rotation_increase = 8, uranusOrb = 40, uranusOrbSpeed = 0, uranusOrb_increase = earthOrb_increase*.03,
neptune_radius = 0.8, neptune_self_rotation = 0,  neptune_self_rotation_increase = 8, neptuneOrb = 50, neptuneOrbSpeed = 0, neptuneOrb_increase = earthOrb_increase*.01,

proximaA_radius = 2.5, proximaAOrbSpeed = 0,
proximaC_radius = 1.5, proximaC_self_rotation = 0, proximaC_self_rotation_increase = 8, proximaCOrb = 60, proximaCOrbSpeed = 0, proximaCOrb_increase = 15,
proximaC1_radius = 1, proximaC1_self_rotation = 0, proximaC1_self_rotation_increase = 8, proximaC1Orb = 5, proximaC1OrbSpeed = 0, proximaC1Orb_increase = 20;

// three drawing detection, draw intro page, draw solar system simulation or draw proxima centuri simulationi.
static bool intro = true, solarSystem = false, proxima = false;

//The texture initilzation

static unsigned int texture[20]; // Array of texture indices.
static unsigned char chessboard[64][64][3]; // Storage for chessboard image.
static float angle = 0.0; // Angle to rotate textured square.
static int id = 0; // Currently displayed texture id.
// Routine to draw a stroke character string.

//default materal properitoes
float defaultEm[] = {0,0,0,1}, defaultAmb[] = {0.2,0.2,0.2,1}, defaultDif[] = {0.8,0.8,0.8,1}, defaultSpec[] = {0,0,0,1};
/* globals */

int num_particles; /* number of particles */
float colors[8][3]={{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0},{0.0, 1.0, 0.0},
    {0.0, 0.0, 1.0}, {1.0, 1.0, 0.0}, {1.0, 0.0, 1.0}, {0.0, 1.0, 1.0},
    {1.0, 1.0, 1.0}};
float point_size = INITIAL_POINT_SIZE;
/* particle struct */

// Some light values
const float LIGHT_0_POSITION[]    = { 0.0, 0.0, 0, 1.0};
const float LIGHT_1_POSITION[]    = { 0.0, 4.0, 0.0, 1.0};
const float LIGHT_2_POSITION[]    = { 0.0, -4.0, 0.0, 1.0};
const float LIGHT_3_POSITION[]    = { 0.0, 0.0, 1.0, 1.0};
const float LIGHT_AMBIENT[]       = { 1.0, 1.0, 1.0, 1.0};
const float LIGHT_DIFFUSE[]       = { 0.9, 0.9, 0.9, 1.0};
const float LIGHT_SPECULAR[]      = { 1.0, 1.0, 1.0, 1.0};


//initialize all three lights
void SetLights()
{
    glLightfv(GL_LIGHT0, GL_AMBIENT,  LIGHT_AMBIENT);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  LIGHT_DIFFUSE);
    glLightfv(GL_LIGHT0, GL_SPECULAR, LIGHT_SPECULAR);
    glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_0_POSITION);

    glLightfv(GL_LIGHT1, GL_AMBIENT,  LIGHT_AMBIENT);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  LIGHT_DIFFUSE);
    glLightfv(GL_LIGHT1, GL_SPECULAR, LIGHT_SPECULAR);
    glLightfv(GL_LIGHT1, GL_POSITION, LIGHT_1_POSITION);
    
    glLightfv(GL_LIGHT2, GL_AMBIENT,  LIGHT_AMBIENT);
    glLightfv(GL_LIGHT2, GL_DIFFUSE,  LIGHT_DIFFUSE);
    glLightfv(GL_LIGHT2, GL_SPECULAR, LIGHT_SPECULAR);
    glLightfv(GL_LIGHT2, GL_POSITION, LIGHT_2_POSITION);

    glLightfv(GL_LIGHT3, GL_AMBIENT,  LIGHT_AMBIENT);
    glLightfv(GL_LIGHT3, GL_DIFFUSE,  LIGHT_DIFFUSE);
    glLightfv(GL_LIGHT3, GL_SPECULAR, LIGHT_SPECULAR);
    glLightfv(GL_LIGHT3, GL_POSITION, LIGHT_3_POSITION);
}
// Enable the scene's lighting. //
void EnableLight()
{
    glPushMatrix();
        glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_0_POSITION);
        glLightfv(GL_LIGHT1, GL_POSITION, LIGHT_1_POSITION);
        glLightfv(GL_LIGHT2, GL_POSITION, LIGHT_2_POSITION);
        glLightfv(GL_LIGHT3, GL_POSITION, LIGHT_3_POSITION);
    glPopMatrix();
    if (lighting) glEnable(GL_LIGHTING);
    if (lighting)glEnable(GL_LIGHT0);
    if (!lighting)glDisable(GL_LIGHTING);
    //glEnable(GL_LIGHT1);
}
void UpdateLight2(){
    glPushMatrix();
        glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_0_POSITION);
        glLightfv(GL_LIGHT1, GL_POSITION, LIGHT_1_POSITION);
        glLightfv(GL_LIGHT2, GL_POSITION, LIGHT_2_POSITION);
        glLightfv(GL_LIGHT3, GL_POSITION, LIGHT_3_POSITION);
    glPopMatrix();
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
}
//the particles for solar astroids layers
typedef struct particle
{
     int color;
     float position[3];
     //float velocity[3];
     //float mass;
} particle;
particle particles[MAX_NUM_PARTICLES];

// the particle to have the boiling effect for the sun
typedef struct sunParticle
{
    float ang1;
    float ang2;
    float r;
    bool expand = true;
} sunParticle;
sunParticle sP[MAX_NUM_PARTICLES];

//initialize the sun boiling sphere value
void sunParticleInit(){

    for (int i = 0; i < 400; i++){
        sP[i].ang1 = (int)rand() % 360;
        sP[i].ang2 = (int)rand() % 360;
        sP[i].r = 0.1*((int)rand()%5);
        
        
    }
}
// the sun boiling effect is drawn to random surface on the sun sphere, by translate to the sun radius length
//from the origenal, and rotate based on ang1 and ang2, to move then to random places on the sphere
void drawSunParticle(){
//    float sunEmDis[] = {0,0,0,1};
//    float WindAmbAndDif[] = {150*0.004, 69*0.004, 19*0.004,1};
//    float WindSpec[] = {1,1,1,1};
    float sunEm[] = {1,0.7,0,1};
//    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, WindAmbAndDif);
//    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, WindSpec);
    
    for (int i = 0; i < 400; i++){
        //glMaterialfv(GL_FRONT, GL_EMISSION, sunEm);
        GLUquadricObj* quadro = gluNewQuadric();
        gluQuadricNormals(quadro, GLU_SMOOTH);
        gluQuadricTexture(quadro, GL_TRUE);
        glEnable(GL_TEXTURE_2D);
        glMaterialfv(GL_FRONT, GL_EMISSION, sunEm);
            glPushMatrix();
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                glPushMatrix();
                    glRotatef(sP[i].ang2, 1, 0, 0);
                    glRotatef(sP[i].ang1, 0, 1, 0);
                    //glRotatef( -90.0, 1.0, 0.0, 0.0 );
                    glTranslatef(sun_radius-0.40,0,0);
                    glBindTexture(GL_TEXTURE_2D, texture[1]);
                    gluSphere(quadro, sP[i].r, 40, 40);
                    
                   
                glPopMatrix();
            glPopMatrix();
        glDisable(GL_TEXTURE_2D);
        gluDeleteQuadric(quadro);
        
    }
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, defaultEm);
}
//void sunParticleIdle(){
void TimerFunction2(int value){
    
    for (int i = 0; i<200; i++){
        if (expand == false) sP[i].r -= .1;
        if (sP[i].r == 1) expand = true;
        if (expand == true) sP[i].r += .1;
    }
    glutPostRedisplay();
    glutTimerFunc(20, TimerFunction2, 1);
}
//draw two astroid layers one is between mars and jupiter and other one is out layer of solar system
//their position is ramdomly generated on a range, make sure astroid appears in the right places, used some
//sin and cos calculations. And their shape is distribute into three shapes
void solarpartical(){

    for(int i =0; i<200; i++)
    {
        particles[i].color = i%8;
        float randAngle = ((int) rand() % 360);
        float randR = ((int) rand() % 8)+15;
        particles[i].position[0] = randR*cos(randAngle*(PI/180));
        particles[i].position[1] = 0;
        particles[i].position[2] = randR*sin(randAngle*(PI/180));

        
    }
    //glPointSize(point_size);
    //glClearColor(0.5, 0.5, 0.5, 1.0);
    for(int i =300; i<900; i++)
    {
        particles[i].color = i%8;
        float randAngle = ((int) rand() % 360);
        float randR = ((int) rand() % 10)+55;
        particles[i].position[0] = randR*cos(randAngle*(PI/180));
        particles[i].position[1] = 0;
        particles[i].position[2] = randR*sin(randAngle*(PI/180));
        
        
    }
}
void solarparticalDisplay(){
    
    glPushMatrix();
    glRotatef(particalOrbSpeed, 0, 1, 0);
    glRotatef(2, 0, 0, 1);
//    glBegin(GL_POINTS);
//    for(int i=0; i<num_particles; i++)
//    {
//       glColor3fv(colors[particles[i].color]);
//       glVertex3fv(particles[i].position);
//    }
//    glEnd();
  
    for (int i = 0; i<200; i++){
        
        glPushMatrix();
        glTranslatef(particles[i].position[0], 0, particles[i].position[2]);
        //glColor3f(colors[particles[i].color][0],colors[particles[i].color][1],colors[particles[i].color][2]);
        if (i<100) glutSolidCube(0.05);
        if (i>= 100 && i <150) glutSolidSphere(0.05, 10, 10);
        if (i <200 && i>150) {
            glEnable(GL_NORMALIZE);
            glScalef(0.03, 0.03, 0.03);
            glutSolidDodecahedron();
        }
        glPopMatrix();
    
    }
    glPopMatrix();
    
    glPushMatrix();
    glRotatef(particalOrbSpeed*0.2, 0, 1, 0);
    glRotatef(4, 0, 0, 1);
    for (int i = 300; i<900; i++){
        
        glPushMatrix();
        glTranslatef(particles[i].position[0], 0, particles[i].position[2]);
        glColor3f(colors[particles[i].color][0],colors[particles[i].color][1],colors[particles[i].color][2]);
        if (i<400) glutSolidCube(0.05);
        if (i>= 500 && i <=750) glutSolidSphere(0.05, 10, 10);
        if (i <900 && i>750) {
            glEnable(GL_NORMALIZE);
            glScalef(0.03, 0.03, 0.03);
            glutSolidDodecahedron();
        }
        glPopMatrix();
    
    }
    glPopMatrix();
    
    //cout<<particles[3].position[0]<<endl;
//    glPushMatrix();
//    glColor3f(1, 0, 0);
//    glRotatef(particalOrbSpeed, 0, 1, 0);
//    glTranslatef(particles[2].position[0],particles[2].position[1], particles[2].position[2]);
//    glutSolidCube(5);
//    glPopMatrix();
    glPushMatrix();
    glBegin(GL_POINTS);
    glRotatef(particalOrbSpeed, 0, 1, 0);
    glTranslatef(1, 0, 0);
    glColor3f(1, 0, 0);
    glVertex3f(0, 6, 0);
    glEnd();
    glPopMatrix();
    /* outline of box */
    //glutSwapBuffers();
    
}
/* particle system */

void writeStrokeString(void *font, const char *string)
{
    const char *c;
    for (c = string; *c != '\0'; c++) glutStrokeCharacter(font, *c);
}
//load pic
struct BitMapFile
{
   int sizeX;
   int sizeY;
   unsigned char *data;
};
//Routine to read a bitmap file.
// Works only for uncompressed bmp files of 24-bit color.
BitMapFile *getBMPData(string filename)
{
   BitMapFile *bmp = new BitMapFile;
   unsigned int size, offset, headerSize;
  
   // Read input file name.
   ifstream infile(filename.c_str(), ios::binary);
 
   // Get the starting point of the image data.
   infile.seekg(10);
   infile.read((char *) &offset, 4);
   
   // Get the header size of the bitmap.
   infile.read((char *) &headerSize,4);

   // Get width and height values in the bitmap header.
   infile.seekg(18);
   infile.read( (char *) &bmp->sizeX, 4);
   infile.read( (char *) &bmp->sizeY, 4);

   // Allocate buffer for the image.
   size = bmp->sizeX * bmp->sizeY * 24;
   bmp->data = new unsigned char[size];

   // Read bitmap data.
   infile.seekg(offset);
   infile.read((char *) bmp->data , size);
   
   // Reverse color from bgr to rgb.
    int temp;
    for (int i = 0; i < size; i += 3)
    {
       temp = bmp->data[i];
       bmp->data[i] = bmp->data[i+2];
       bmp->data[i+2] = temp;
    }
    
   return bmp;
}
// Load external textures.
//loaded 16 bmp images and used them to draw on the object
void loadExternalTextures()
{
   // Local storage for bmp image data.
   BitMapFile *image[20];
   
   // Load the texture.
   image[0] = getBMPData("hushangTEXTURES/UniverseSandbox1.bmp");
    image[1] = getBMPData("hushangTEXTURES/sun.bmp");
    image[2] = getBMPData("hushangTEXTURES/Mercury.bmp");
    image[3] = getBMPData("hushangTEXTURES/venus.bmp");
    image[4] = getBMPData("hushangTEXTURES/mars.bmp");
    image[5] = getBMPData("hushangTEXTURES/earth.bmp");
    image[6] = getBMPData("hushangTEXTURES/moon.bmp");
    image[7] = getBMPData("hushangTEXTURES/jupiter.bmp");
    image[8] = getBMPData("hushangTEXTURES/saturn.bmp");
    image[9] = getBMPData("hushangTEXTURES/uranus.bmp");
    image[10] = getBMPData("hushangTEXTURES/neptune.bmp");
    image[11] = getBMPData("hushangTEXTURES/stars_milky_way.bmp");
    image[12] = getBMPData("hushangTEXTURES/saturnringmap.bmp");
    image[13] = getBMPData("hushangTEXTURES/suna.bmp");
    image[14] = getBMPData("hushangTEXTURES/P2.bmp");
    image[15] = getBMPData("hushangTEXTURES/sunc.bmp");
    image[16] = getBMPData("hushangTEXTURES/sunc1.bmp");

   // Activate texture index texture[0].
    //intro pic******
   glBindTexture(GL_TEXTURE_2D, texture[0]);

   // Set texture parameters for wrapping.
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

   // Set texture parameters for filtering.
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

   // Specify an image as the texture to be bound with the currently active texture index.
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[0]->sizeX, image[0]->sizeY, 0,
                GL_RGB, GL_UNSIGNED_BYTE, image[0]->data);
    //Sun ******
    glBindTexture(GL_TEXTURE_2D, texture[1]);

    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[1]->sizeX, image[1]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[1]->data);
    
    // Mercury******
    glBindTexture(GL_TEXTURE_2D, texture[2]);

    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[2]->sizeX, image[2]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[2]->data);

    
    // Venus******
    glBindTexture(GL_TEXTURE_2D, texture[3]);

    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[3]->sizeX, image[3]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[3]->data);
    
    //Mars'*********
    glBindTexture(GL_TEXTURE_2D, texture[4]);

    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[4]->sizeX, image[4]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[4]->data);
    
    //Earth'*********
    glBindTexture(GL_TEXTURE_2D, texture[5]);

    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[5]->sizeX, image[5]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[5]->data);
    
    //moon'*********
    glBindTexture(GL_TEXTURE_2D, texture[6]);

    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[6]->sizeX, image[6]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[6]->data);
    
    //jupiter*********
    glBindTexture(GL_TEXTURE_2D, texture[7]);

    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[7]->sizeX, image[7]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[7]->data);
    
    //staurn*********
    glBindTexture(GL_TEXTURE_2D, texture[8]);

    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[8]->sizeX, image[8]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[8]->data);
    
    //uranus*****
    //staurn*********
    glBindTexture(GL_TEXTURE_2D, texture[9]);

    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[9]->sizeX, image[9]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[9]->data);
    //neptune
    glBindTexture(GL_TEXTURE_2D, texture[10]);

    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[10]->sizeX, image[10]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[10]->data);
    //skybg
    glBindTexture(GL_TEXTURE_2D, texture[11]);

    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[11]->sizeX, image[11]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[11]->data);
    //saturn ring
    glBindTexture(GL_TEXTURE_2D, texture[12]);

    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[12]->sizeX, image[12]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[12]->data);
    
    glBindTexture(GL_TEXTURE_2D, texture[13]);

    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[13]->sizeX, image[13]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[13]->data);
    
    glBindTexture(GL_TEXTURE_2D, texture[14]);

    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[14]->sizeX, image[14]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[14]->data);
    
    glBindTexture(GL_TEXTURE_2D, texture[15]);

    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[15]->sizeX, image[15]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[15]->data);
    
    glBindTexture(GL_TEXTURE_2D, texture[16]);

    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[16]->sizeX, image[16]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[16]->data);
}
// Routine to load a program-generated image as a texture.
//void loadProceduralTextures()
//{
//   // Activate texture index texture[1].
//   glBindTexture(GL_TEXTURE_2D, texture[1]);
//
//   // Set texture parameters for wrapping.
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//   // Set texture parameters for filtering.
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//
//   // Specify an image as the texture to be bound with the currently active texture index.
//   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, chessboard);
//}

// Create 64 x 64 RGB image of a chessboard.
void createChessboard(void)
{
   int i, j;
   for (i = 0; i < 64; i++)
      for (j = 0; j < 64; j++)
         if ( ( ((i/8)%2) && ((j/8)%2) ) || ( !((i/8)%2) && !((j/8)%2) ) )
         {
            chessboard[i][j][0] = 0x00;
            chessboard[i][j][1] = 0x00;
            chessboard[i][j][2] = 0x00;
         }
         else
         {
            chessboard[i][j][0] = 0xFF;
            chessboard[i][j][1] = 0xFF;
            chessboard[i][j][2] = 0xFF;
         }
}
//to draw the intro page
void drawintropic(){
    glPushMatrix();
    glTranslatef(0, 0, 10);
    glRotatef(180, 0.0, 1.0, 0.0);

    // Activate a texture.
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    // Map the texture onto a square polygon.
    glBegin(GL_POLYGON);
       glTexCoord2f(0.0, 0.0); glVertex3f(-30.0, -20.0, 0.0);
       glTexCoord2f(1.0, 0.0); glVertex3f(30.0, -20.0, 0.0);
       glTexCoord2f(1.0, 1.0); glVertex3f(30.0, 20.0, 0.0);
       glTexCoord2f(0.0, 1.0); glVertex3f(-30.0, 20.0, 0.0);
    glEnd();
    
    
    glPopMatrix();
    glLineWidth(3);
    glColor4f(0.9, 0.9, 0.8, 1.0);
    glPushMatrix();
    
    glTranslatef(10, -5, 1);
    glRotatef(180, 0.0, 1.0, 0.0); //line it up with 0 position
    glScalef(0.010, 0.010, 0.010);
    
    //char numString[5];
    //sprintf(numString,"%d",theta);
    writeStrokeString(GLUT_STROKE_ROMAN, "SolarSystem");
        
    glPopMatrix();
    
    glColor4f(0.9, 0.9, 0.8, 1.0);
    glPushMatrix();
    
    glTranslatef(10, -8, 1);
    glRotatef(180, 0.0, 1.0, 0.0); //line it up with 0 position
    glScalef(0.010, 0.010, 0.010);
    
    //char numString[5];
    //sprintf(numString,"%d",theta);
    writeStrokeString(GLUT_STROKE_ROMAN, "ProximaCentauri");
        
    glPopMatrix();
    
    glLineWidth(1);
}
// this function is a generate function to create the signel plante. taking some parameters and draw the planet based on
//those values
void drawNormalPlanet(string PlanetName, GLuint texture, float planetRotation, float planetRadius, float planetOrb, float planetOrbSpeed, float slopeAng){
    GLUquadricObj* quadro = gluNewQuadric();
    gluQuadricNormals(quadro, GLU_SMOOTH);
    gluQuadricTexture(quadro, GL_TRUE);
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glPushMatrix();
    glRotatef(planetOrbSpeed, 0, 1, 0);
    glTranslatef(planetOrb, 0, 0);
        glRotatef(planetRotation, -sin(slopeAng*(PI/180)), 1, 0);
        glRotatef(slopeAng, 0, 0, 1);
        glRotatef( -90.0, 1.0, 0.0, 0.0 );
        glBindTexture(GL_TEXTURE_2D, texture);
        gluSphere(quadro, planetRadius, 40, 40);
    glPopMatrix();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    gluDeleteQuadric(quadro);
}
//to check which simulation is clicked
void checkoption(){
    if (xClick<500 && xClick > 240 &&yClick <250 && yClick > 200&&clicked == false) intro = false, solarSystem = true, proxima = false, clicked = true;
    if (xClick<600 && xClick > 240 &&yClick <145 && yClick > 100 && clicked == false) intro = false, solarSystem = false, proxima = true, clicked = true;
}
//draw sun for solar system simulation
void drawSun(){
    float sunEmDis[] = {0,0,0,1};
    //float WindAmbAndDif[] = {150*0.004, 69*0.004, 19*0.004,1};
    //float WindSpec[] = {1,1,1,1};
    float sunEm[] = {1,0.7,0,1};
   // glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, WindAmbAndDif);
    //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, WindSpec);
    glMaterialfv(GL_FRONT, GL_EMISSION, sunEm);
    GLUquadricObj* quadro = gluNewQuadric();
    gluQuadricNormals(quadro, GLU_SMOOTH);
    gluQuadricTexture(quadro, GL_TRUE);
    glEnable(GL_TEXTURE_2D);
        glPushMatrix();
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glPushMatrix();
                glRotatef(sun_self_rotation, 0, 1, 0);
                glRotatef( -90.0, 1.0, 0.0, 0.0 );
                glBindTexture(GL_TEXTURE_2D, texture[1]);
                gluSphere(quadro, sun_radius, 40, 40);
                glMaterialfv(GL_FRONT, GL_EMISSION, sunEmDis);
            glPopMatrix();
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    gluDeleteQuadric(quadro);
}

// earth and moon is drawn together, this is a two planet system, handled specifically
void drawEarthANDMoon( float planetRotation, float planetRadius, float planetOrb, float planetOrbSpeed){
    GLUquadricObj* quadro = gluNewQuadric();
    gluQuadricNormals(quadro, GLU_SMOOTH);
    gluQuadricTexture(quadro, GL_TRUE);
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glPushMatrix();
            glRotatef(planetOrbSpeed, 0, 1, 0);
            glTranslatef(planetOrb, 0, 0);
            glRotatef(planetRotation, 0, 1, 0);
            glRotatef(-10, 0, 0, 1);
            glRotatef( -90.0, 1.0, 0.0, 0.0 );
            glBindTexture(GL_TEXTURE_2D, texture[5]);
            gluSphere(quadro, planetRadius, 20, 20);
        glPopMatrix();
    //glRotatef(EARTH_INCLINATION, 0.0, 0.0, 1.0);
        glRotatef( earthOrbSpeed, 0.0, 1.0, 0.0);
        glTranslatef(earthOrb, 0.0, 0.0 );
        glRotatef( moonOrbSpeed, 0.0, 1.0, 0.0 );
        glTranslatef( 2, 0.0, 0.0 );
        glBindTexture(GL_TEXTURE_2D, texture[6]);
        gluSphere(quadro, 0.2, 20, 20);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    gluDeleteQuadric(quadro);
    
}
void drawsaturnring(){
    GLUquadricObj* quadro = gluNewQuadric();
    gluQuadricNormals(quadro, GLU_SMOOTH);
    gluQuadricTexture(quadro, GL_TRUE);
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glRotatef(staurnOrbSpeed, 0, 1, 0);
    glTranslatef(staurnOrb, 0, 0);
    glRotatef(staurn_self_rotation*2, -sin(20*(PI/180)), 1, 0);
    glEnable(GL_NORMALIZE);
    glBindTexture(GL_TEXTURE_2D, texture[12]);
    glRotatef(20, 0, 0, 1);
    glScalef(1, 0.02, 1);
    gluSphere(quadro, staurn_radius*2, 40, 40);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    gluDeleteQuadric(quadro);
}
// draw a huge sphere covers all the environment, and bind milky way on the sphere to indicate
// there is a background with stars
void drawbg(){
    GLUquadricObj* quadro = gluNewQuadric();
    gluQuadricNormals(quadro, GLU_SMOOTH);
    gluQuadricTexture(quadro, GL_TRUE);
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, texture[11]);
    gluSphere(quadro, 100, 50, 50);
    glPopMatrix();
}
//the timer function is to animate how the system orbit to each other.
void TimerFunction1(int value)
{
    if (solarSystem && !stop){
        sun_self_rotation += sun_self_rotation_increase;
        mercury_self_rotation += mercury_self_rotation_increase;
        mercuryOrbSpeed += earthOrb_increase*4;
        venus_self_rotation += venus_self_rotation_increase;
        venusOrbSpeed += earthOrb_increase*2;
        mars_self_rotation += mars_self_rotation_increase;
        marsOrbSpeed += earthOrb_increase*.5;
        earth_self_rotation += earth_self_rotation_increase;
        earthOrbSpeed += earthOrb_increase;
        moonOrbSpeed += moonOrb_increase;
        jupiterOrbSpeed += earthOrb_increase*0.2;
        jupiter_self_rotation += jupiter_self_rotation_increase;
        staurnOrbSpeed += earthOrb_increase*.1;
        staurn_self_rotation += staurn_self_rotation_increase;
        uranusOrbSpeed += earthOrb_increase*0.03;
        uranus_self_rotation += uranus_self_rotation_increase;
        neptuneOrbSpeed += earthOrb_increase*0.01;
        neptune_self_rotation += neptune_self_rotation_increase;
        particalOrbSpeed += earthOrb_increase*0.4;
        
        
        for (int i = 0; i<400; i++){
            if (sP[i].expand == true) sP[i].r += .05;
            else sP[i].r -= .05;
            if (sP[i].r <= .6 &&sP[i].r >= .5) sP[i].expand = false;
            if (sP[i].r <= .2 &&sP[i].r >= 0.1) sP[i].expand = true;
        }
    }
        if(proxima && !stop){
            proximaAOrbSpeed += proximaOrb_increase*.3;
            proximaCOrbSpeed += proximaOrb_increase;
            proximaC1OrbSpeed += proximaOrb_increase;
        }
        
    
    glutPostRedisplay();
    glutTimerFunc(20, TimerFunction1, 1);
}
void drawSpaceShip(){
    float WindAmbAndDif[] = {150*0.004, 69*0.004, 19*0.004,1};
    float Wind1AmbAndDif[] = {118*0.004, 143*0.004, 143*0.004,1};
    float windEm[] = {0.5,0.5,0.5, 1};
    float windEmDis[] {0,0,0,1};
    float WindSpec[] = {1,1,1,1};
    float WindShine[] = {80};
    
    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, WindAmbAndDif);
    //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, WindSpec);
   // glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, WindShine);
    
    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Wind1AmbAndDif);
    //if (animation1) glMaterialfv(GL_FRONT, GL_EMISSION, windEm);
    glPushMatrix();
    glTranslatef(meX+sin(angle1*PI/180), meY-.5, meZ+cos(angle1*PI/180)-0.5);
    glEnable(GL_NORMALIZE);
    glScalef(0.01, 0.01, 0.01);
    glutSolidDodecahedron();
    glPopMatrix();
    //glMaterialfv(GL_FRONT, GL_EMISSION, windEmDis);
    glPopMatrix();
    
    glPopMatrix();
    
 
}
//draw Elon Musk's Tesla which is orbit the sun right now
void drawTesla(){
    float tireAmbAndDif[] = {105*0.004,105*0.004,105*0.004,1};
    float carbodyAmbAndDif[] = {255*0.004, 20*0.004, 0*0.004,1};
    float tireSpec[] = {1,1,1,1};
    float tireShine[] = {80};
    
    
    glPushMatrix();
    glEnable(GL_NORMALIZE);
    glTranslatef(4, 0, 0);
    glRotatef(marsOrbSpeed*0.8, 0.1, 1, 0);
    glTranslatef(marsOrb, 0, 0);
    glScalef(0.2, 0.2, 0.2);
    glPushMatrix();
    glTranslatef(1.8, 0, 1);
    glScalef(2, 1.5, 1.2);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, carbodyAmbAndDif);
    glRotatef(90, 0, 0, 1);
    glPushMatrix();
    double rEq[4] = {1,0,0,0};
    glClipPlane(GL_CLIP_PLANE0,rEq);
    glEnable(GL_CLIP_PLANE0);
    glutSolidDodecahedron();
    glDisable(GL_CLIP_PLANE0);
    glPopMatrix();
    
    glPopMatrix();
    
    glPushMatrix();
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, tireAmbAndDif);
    
    glPushMatrix();
    glutSolidTorus(0.2, 0.4, 10, 10);
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(0, 0, 2);
    glutSolidTorus(0.2, 0.4, 10, 10);
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(4, 0, 2);
    glutSolidTorus(0.2, 0.4, 10, 10);
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(4, 0, 0);
    glutSolidTorus(0.2, 0.4, 10, 10);
    glPopMatrix();
    
    glPopMatrix();
    
    glPopMatrix();
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, defaultAmb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, defaultDif);
}


void drawProximaA(){
    
    
    float sunEm[] = {1,0.7,0,1};
   // glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, WindAmbAndDif);
    //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, WindSpec);
    glMaterialfv(GL_FRONT, GL_EMISSION, sunEm);
    GLUquadricObj* quadro = gluNewQuadric();
    gluQuadricNormals(quadro, GLU_SMOOTH);
    gluQuadricTexture(quadro, GL_TRUE);
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glPushMatrix();
        glTranslatef(5, 0,0);
        glRotatef(proximaAOrbSpeed*9, 0, 1, 0);
            
            glTranslatef(15, 0, 0);
            glRotatef(sun_self_rotation*0.9, 0, 1, 0);
            glRotatef( -90.0, 1.0, 0.0, 0.0 );
            glBindTexture(GL_TEXTURE_2D, texture[1]);
            gluSphere(quadro, proximaA_radius, 40, 40);
            //glMaterialfv(GL_FRONT, GL_EMISSION, sunEmDis);
        glPopMatrix();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    gluDeleteQuadric(quadro);

}

void drawProximaB(){
    
    
    float sunEm[] = {1,0.7,0,1};
   // glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, WindAmbAndDif);
    //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, WindSpec);
    glMaterialfv(GL_FRONT, GL_EMISSION, sunEm);
    GLUquadricObj* quadro = gluNewQuadric();
    gluQuadricNormals(quadro, GLU_SMOOTH);
    gluQuadricTexture(quadro, GL_TRUE);
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glPushMatrix();
        glTranslatef(-5, 0,0);
        glRotatef(proximaAOrbSpeed*10, 0.6, 1, 0);
            
            glTranslatef(-13, 0, 0);
            glRotatef(sun_self_rotation*0.9, 0, 1, 0);
            glRotatef( -90.0, 1.0, 0.0, 0.0 );
            glBindTexture(GL_TEXTURE_2D, texture[13]);
            gluSphere(quadro, proximaA_radius, 40, 40);
            //glMaterialfv(GL_FRONT, GL_EMISSION, sunEmDis);
        glPopMatrix();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    gluDeleteQuadric(quadro);

}
void drawProximaC( float planetRotation, float planetRadius, float planetOrb, float planetOrbSpeed){
    GLUquadricObj* quadro = gluNewQuadric();
    gluQuadricNormals(quadro, GLU_SMOOTH);
    gluQuadricTexture(quadro, GL_TRUE);
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glPushMatrix();
            glRotatef(planetOrbSpeed, 0, 1, 0);
            glTranslatef(planetOrb, 0, 0);
            glRotatef(planetRotation, 0, 1, 0);
            glRotatef(-10, 0, 0, 1);
            glRotatef( -90.0, 1.0, 0.0, 0.0 );
            glBindTexture(GL_TEXTURE_2D, texture[15]);
            gluSphere(quadro, planetRadius, 20, 20);
        glPopMatrix();
    //glRotatef(EARTH_INCLINATION, 0.0, 0.0, 1.0);
        glRotatef( proximaCOrbSpeed, 0.0, 1.0, 0.0);
        glTranslatef(proximaCOrb, 0.0, 0.0 );
        glRotatef( proximaC1OrbSpeed, 0.0, 1.0, 0.0 );
        glTranslatef( 4, 0.0, 0.0 );
        glBindTexture(GL_TEXTURE_2D, texture[16]);
        gluSphere(quadro, 0.2, 20, 20);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    gluDeleteQuadric(quadro);
    
}
// this si the collision detection on solar system simulation, in view 1, you are not able to cross the planet,
// you can not move forward if you too close to the planet
bool solarCollisionDetection(float xval,float yval, float zval){
    if (xval <= 3 && xval >= -3 && yval <= 3 && yval >= -3 and zval<= 3 and zval>= -3){
        return false;
    }
    if (xval <= mercuryOrb* cos(mercuryOrbSpeed*(PI/180))+mercury_radius
        && xval >= mercuryOrb* cos(mercuryOrbSpeed*(PI/180))-mercury_radius
        && yval <= mercury_radius && yval >= -mercury_radius
        and zval <= mercuryOrb* sin(mercuryOrbSpeed*(PI/180))+mercury_radius
        and zval >= mercuryOrb* sin(mercuryOrbSpeed*(PI/180))-mercury_radius){
        return false;
    }
    if (xval <= jupiterOrb*cos(jupiterOrbSpeed*(PI/180))+jupiter_radius and
        xval >= jupiterOrb*cos(jupiterOrbSpeed*(PI/180))-jupiter_radius and
        yval <= jupiter_radius and yval >= - jupiter_radius and
        zval <= jupiterOrb*sin(jupiterOrbSpeed*(PI/180))+jupiter_radius and
        zval >= jupiterOrb*sin(jupiterOrbSpeed*(PI/180))-jupiter_radius
        ) return false;
    if (xval <= venusOrb*cos(venusOrbSpeed*(PI/180))+venus_radius and
        xval >= venusOrb*cos(venusOrbSpeed*(PI/180))-venus_radius and
        yval <= venus_radius and yval >= - venus_radius and
        zval <= venusOrb*sin(venusOrbSpeed*(PI/180))+venus_radius and
        zval >= venusOrb*sin(venusOrbSpeed*(PI/180))-venus_radius
        ) return false;
    if (xval <= marsOrb*cos(marsOrbSpeed*(PI/180))+mars_radius and
        xval >= marsOrb*cos(marsOrbSpeed*(PI/180))-mars_radius and
        yval <= mars_radius and yval >= - mars_radius and
        zval <= marsOrb*sin(marsOrbSpeed*(PI/180))+mars_radius and
        zval >= marsOrb*sin(marsOrbSpeed*(PI/180))-mars_radius
        ) return false;
    if (xval <= earthOrb*cos(earthOrbSpeed*(PI/180))+earth_radius and
        xval >= earthOrb*cos(earthOrbSpeed*(PI/180))-earth_radius and
        yval <= earth_radius and yval >= - earth_radius and
        zval <= earthOrb*sin(earthOrbSpeed*(PI/180))+earth_radius and
        zval >= earthOrb*sin(earthOrbSpeed*(PI/180))-earth_radius
        ) return false;
//    if (xval <= moonOrb*cos(moonOrbSpeed*(PI/180))+moon_radius and
//        xval >= moonOrb*cos(moonOrbSpeed*(PI/180))-moon_radius and
//        yval <= moon_radius and yval >= - moon_radius and
//        zval <= moonOrb*sin(moonOrbSpeed*(PI/180))+moon_radius and
//        zval >= moonOrb*sin(moonOrbSpeed*(PI/180))-moon_radius
//        ) return false;
    if (xval <= staurnOrb*cos(staurnOrbSpeed*(PI/180))+staurn_radius and
        xval >= staurnOrb*cos(staurnOrbSpeed*(PI/180))-staurn_radius and
        yval <= staurn_radius and yval >= - staurn_radius and
        zval <= staurnOrb*sin(staurnOrbSpeed*(PI/180))+staurn_radius and
        zval >=staurnOrb*sin(staurnOrbSpeed*(PI/180))-staurn_radius
        ) return false;
    if (xval <= uranusOrb*cos(uranusOrbSpeed*(PI/180))+uranus_radius and
        xval >= uranusOrb*cos(uranusOrbSpeed*(PI/180))-uranus_radius and
        yval <= uranus_radius and yval >= - uranus_radius and
        zval <= uranusOrb*sin(uranusOrbSpeed*(PI/180))+uranus_radius and
        zval >=uranusOrb*sin(uranusOrbSpeed*(PI/180))-uranus_radius
        ) return false;
    if (xval <= neptuneOrb*cos(neptuneOrbSpeed*(PI/180))+neptune_radius and
        xval >= neptuneOrb*cos(neptuneOrbSpeed*(PI/180))-neptune_radius and
        yval <= neptune_radius and yval >= - neptune_radius and
        zval <= neptuneOrb*sin(neptuneOrbSpeed*(PI/180))+neptune_radius and
        zval >= neptuneOrb*sin(neptuneOrbSpeed*(PI/180))-neptune_radius
        ) return false;
    return true;
    
}
void drawScene(){
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    if (view1)gluLookAt(meX, meY, meZ, meX+sin(angle1*PI/180), meY, meZ+cos(angle1*PI/180), 0, 1, 0);
    if (view2)gluLookAt(meX2, meY2, meZ2-15, 0,0,0, 0, 1, 0);
    //gluLookAt(meX, meY-10, meZ, meX, meY, meZ, 0, 1, 0);
    
    if (intro) {
        checkoption();
        drawintropic();
    }
    if (solarSystem){
        //glDisable(GL_DEPTH_TEST);
//        glPushMatrix();
//        glTranslatef(0, 0, 10);
//        glRotatef(180, 0.0, 1.0, 0.0);
//
//        // Activate a texture.
//        glBindTexture(GL_TEXTURE_2D, texture[1]);
//
//        // Map the texture onto a square polygon.
//        glBegin(GL_POLYGON);
//           glTexCoord2f(0.0, 0.0); glVertex3f(-30.0, -20.0, 0.0);
//           glTexCoord2f(1.0, 0.0); glVertex3f(30.0, -20.0, 0.0);
//           glTexCoord2f(1.0, 1.0); glVertex3f(30.0, 20.0, 0.0);
//           glTexCoord2f(0.0, 1.0); glVertex3f(-30.0, 20.0, 0.0);
//        glEnd();
//        glPopMatrix();
        //glEnable(GL_DEPTH_TEST);
        //if (view1) drawSpaceShip();
        drawbg();
        EnableLight();
        drawSun();
        drawTesla();
        //solarpartical();
        solarparticalDisplay();
        if (sunPartical) drawSunParticle();
        drawsaturnring();
        drawNormalPlanet("Mercury", texture[2], mercury_self_rotation, mercury_radius, mercuryOrb, mercuryOrbSpeed,15);
        drawNormalPlanet("Venus", texture[3], venus_self_rotation, venus_radius, venusOrb, venusOrbSpeed,0);
        drawEarthANDMoon(earth_self_rotation, earth_radius, earthOrb, earthOrbSpeed);
        drawNormalPlanet("Mars", texture[4], mars_self_rotation, mars_radius, marsOrb, marsOrbSpeed,15);
        drawNormalPlanet("Jupiter", texture[7], jupiter_self_rotation, jupiter_radius, jupiterOrb, jupiterOrbSpeed,-10);
        drawNormalPlanet("Staurn", texture[8], staurn_self_rotation, staurn_radius, staurnOrb, staurnOrbSpeed,20);
        drawNormalPlanet("uranus", texture[9], uranus_self_rotation, uranus_radius, uranusOrb, uranusOrbSpeed,0);
        drawNormalPlanet("neptune", texture[10], neptune_self_rotation, neptune_radius, neptuneOrb, neptuneOrbSpeed,0);
    }
    if (proxima){
        //UpdateLight2();
        drawbg();
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, defaultEm);
        drawProximaA();
        drawProximaB();
        drawProximaC(proximaC_self_rotation, proximaC_radius, proximaCOrb, proximaCOrbSpeed);
    }

//
//    GLUquadricObj* quadro = gluNewQuadric();
//    gluQuadricNormals(quadro, GLU_SMOOTH);
//    gluQuadricTexture(quadro, GL_TRUE);
//    glEnable(GL_TEXTURE_2D);
//    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//    glPushMatrix();
//    //glColor3f(1, 0, 0);
//    glBindTexture(GL_TEXTURE_2D, texture[0]);
//    gluSphere(quadro, 2, 20, 20);
//    glPopMatrix();
    
    glutSwapBuffers();
    
}

void setup(void){
    glClearColor(0, 0, 0, 0);
    //glClearColor(1, 1, 1, 1);
    //glClearColor(0, 0, 0, 0);
    glGenTextures(17, texture);
    
    loadExternalTextures();
    createChessboard();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    // Light property vectors.
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(GL_BLEND);
}




// OpenGL window reshape routine.
void resize(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90,(float)w/(float)h,0.1,200);
    //glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    height = h;
    width = w;
    
}

void top_menu(int id)
{
   if (id==1) exit(0);
    if (id == 2) {
        if (stop == false)stop = true;
        else stop = false;
        glutPostRedisplay();
    }
    if (id == 5){
        solarSystem = false;
        intro = true;
        clicked = false;
        glutPostRedisplay();
    }
}
void option_menu1(int id){
    if (id == 3){
        if (earthOrb_increase<20) earthOrb_increase += 5;
        glutPostRedisplay();
    }
           
    if (id == 4){
        if (earthOrb_increase > 0) earthOrb_increase -= 5;
        glutPostRedisplay();
    }
    
    }
    
// Routine to make the menu.
void makeMenu(void)
{
   //sub menu to indicate the optionsGryffindor"<<endl;
    //if (solarSystem){
        int sub_menu1;
        sub_menu1 = glutCreateMenu(option_menu1);
    
        glutAddMenuEntry("Speed up", 3);
        glutAddMenuEntry("Speed down",4);
    
        // top menu
        glutCreateMenu(top_menu);
        glutAddMenuEntry("Pause/Resume", 2);
        glutAddSubMenu("Change Speed", sub_menu1);
        glutAddMenuEntry("Main Page", 5);
        glutAddMenuEntry("Quit",1);
        //glutAddSubMenu("AnotherOption",sub_menu1);

        // The menu is attached to a mouse button.
        glutAttachMenu(GLUT_RIGHT_BUTTON);
 //   }
}

void mouseControl(int button, int state, int x, int y)
{
    if(state==GLUT_DOWN && button == GLUT_LEFT)
    {   //selecting=true;
        xClick=x;
        yClick=(height-y); //for screen vs mouse coordinates
        
        glutPostRedisplay();
       
    }
}

void keyInput(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 27:
            exit(0);
            break;
        case 's':
            intro = false;
            solarSystem = true;
            proxima = false;
            glutPostRedisplay();
            break;
        case '1':
            view1 = true;
            view2 = false;
            glutPostRedisplay();
            break;
        case '2':
            view1 = false;
            view2 = true;
            glutPostRedisplay();
            break;
        case ' ':
            stop = !stop;
             
            glutPostRedisplay();
            break;
        case'+':
            if (earthOrb_increase<20) earthOrb_increase += 5;
            glutPostRedisplay();
            break;
        case '-':
            if (earthOrb_increase > 0) earthOrb_increase -= 5;
            glutPostRedisplay();
            break;
        case'b':
            intro = true;
            solarSystem = false;
            proxima = false;
            clicked = false;
            glutPostRedisplay();
            break;
        case 'p':
            intro = false;
            solarSystem = false;
            proxima = true;
            glutPostRedisplay();
            break;
        case 'l':
            lighting = !lighting;
            glutPostRedisplay();
            break;
        case 'i':
            sunPartical = !sunPartical;
            glutPostRedisplay();
            break;
    }
    
}

void specialKeyInput(int key, int x, int y)
{
    if(!intro){
    if (view1){
    switch(key){
        case GLUT_KEY_UP: //forward
            
            if (solarCollisionDetection(meX+stepsize*sin(angle1*PI/180), meY, -meZ-stepsize*cos(angle1*PI/180))){
            meZ += stepsize*cos(angle1*PI/180);
            meX += stepsize*sin(angle1*PI/180);
            }
            
            //cout<<meX<<" "<<meY<<" "<<meZ<<endl;
            glutPostRedisplay();
            break;
        case GLUT_KEY_DOWN: //back
            meZ -= stepsize*cos(angle1*PI/180);
            meX -= stepsize*sin(angle1*PI/180);
            
            //cout<<meX<<" "<<meY<<" "<<meZ<<endl;
            glutPostRedisplay();
            break;
        case GLUT_KEY_RIGHT: //turn right
            angle1 -= turnsize;
            
            //cout<<meX<<" "<<meY<<" "<<meZ<<endl;
            glutPostRedisplay();
            break;
        case GLUT_KEY_LEFT: //turn left
            angle1 += turnsize;
            
            //cout<<meX<<" "<<meY<<" "<<meZ<<endl;
            glutPostRedisplay();
            
            break;
    }
    }
    if (view2){
        switch(key){
            case GLUT_KEY_UP: //forward
                meZ2 += stepsize;
                //meX += stepsize*sin(angle1*PI/180);
                glutPostRedisplay();
                break;
            case GLUT_KEY_DOWN: //back
                meZ2 -= stepsize;
                //meX -= stepsize*sin(angle1*PI/180);
                glutPostRedisplay();
                break;
            case GLUT_KEY_RIGHT: //turn right
                meX2 += stepsize;
                glutPostRedisplay();
                break;
            case GLUT_KEY_LEFT: //turn left
                meX2 -= stepsize;
                glutPostRedisplay();
                
                break;
        }
    }//end switch
    glutPostRedisplay();
    }
}



void interaction(){
    cout<<"Interactions:"<<endl;
    cout<<"1.Run the program"<<endl;
    cout<<"2. You can click either solar system or proxima centuri to see different simulations"<<endl;
    cout<<"3. You can press space to pause/resume  the simulation"<<endl;
    cout<<"4. change you views pressing 1 or 2."<<endl;
    cout<<"5. up, down, left and right arrow key to move your view, in view 1, you won't move if you hit aplanet"<<endl;
    cout<<"6. + and - to speed up and speed down the simulation."<<endl;
    cout<<"7. 'l' to enable/disable the light, 'i' to enable the sun particles."<<endl;
    cout<<"8. press 's' and 'p' to switch from solat system and proxima centuri"<<endl;
    cout<<"9. right click the mouse to see the menu and have operations to choose."<<endl;
    cout<<"10. Have fun to play with the simulation."<<endl;
    
}
int main(int argc, char **argv)
{
    interaction();
    sunParticleInit();
    solarpartical();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Universe Sandbox");
    setup();
    SetLights();
    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);
    glutMouseFunc(mouseControl);
    glutSpecialFunc(specialKeyInput);
    glutTimerFunc(100, TimerFunction1, 1);
    //glutTimerFunc(100, TimerFunction2, 1);
    //glutIdleFunc(sunParticleIdle);
    //glutTimerFunc(10, animation, 1);
    //glutTimerFunc(10, ballwingAnimation, 1);
    makeMenu();
    glutMainLoop();
    
    return 0;
}
