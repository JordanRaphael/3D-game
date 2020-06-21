#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#define ERROR pow(10,(-6))
#define BORDER_ERROR pow(10,(-3))
#define ROUNDING_ERROR pow(10,(-1))
#define HORIZONTAL_GAP 2.2f // the gap between the cubes on x and z axis
#define VERTICAL_GAP 2.1f // the gap between the cubes on y axis
#define ESC 27
#define SPACEBAR 32
#define QUARTER_SPIN_ANIM_REF_RATE 4 //animation refresh rate in milliseconds
#define HALF_SPIN_ANIM_REF_RATE 4 //animation refresh rate in milliseconds
#define MOVEMENT_ANIMATION 25 // movement animation refresh rate
#define COLUMN_FALL_ANIMATION 200 //refresh rate of the falling cubes
#define DIR_BACK 1
#define DIR_FRONT 2
#define DIR_RIGHT 3
#define DIR_LEFT 4
#define TURN_RIGHT -90
#define TURN_LEFT 90
#define TURN_BACK 180
#define MAGENTA -1
#define BLUE 0
#define YELLOW 1
#define RED 2
#define GREEN 3

void init();
void addLifes(int);

typedef struct Cube {
  int colour;
  int state;
  GLfloat colourR;
  GLfloat colourG;
  GLfloat colourB;
} cube;

// number of cubes
int N;
// angle of rotation for the camera direction
float angle=0.0;
// XZ position of the camera
float x=0.0f,y=0.0f,z=0.0f;
// actual vector representing the camera's direction
float lx=-1.0f,ly=0.0f,lz=0.0f;
// the camera's direction 
int cameraDirection;
// light color init
GLfloat red[3] =  {1.0f, 0.0f, 0.0f};
GLfloat green[3] =  {0.0f, 1.0f, 0.0f};
GLfloat blue[3] =   {0.0f, 0.0f, 1.0f};
GLfloat yellow[3] = {1.0f, 1.0f, 0.0f};
GLfloat magenta[3] = {1.0f, 0.0f, 1.0f};
// random colour for each cube init
GLfloat **rng;
// light init
GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 0.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 0.0 };
GLfloat playerLight_diffuse[] = { 1.0, 1.0, 1.0 };
GLfloat default_light[] = { 0.0, 0.0, 0.0, 0.0};
// light position init
GLfloat light0_pos[] = {0, 0, 0, 0};
GLfloat light1_pos[] = {0, 0, 0, 0};
GLfloat light2_pos[] = {0, 0, 0, 0};
GLfloat light3_pos[] = {0, 0, 0, 0};
// light direction init
GLfloat light_dir[3];
GLfloat playerLight_dir[3];
// init of the cube's matrix
cube ***cubes;
//cube fall animation
int cubeFallFlag = 0;
int **floorState;
//movement animation
GLfloat xOffset;
GLfloat yOffset;
GLfloat zOffset;
GLfloat originalX;
GLfloat originalY;
GLfloat originalZ;
int onMove = 0;
//Storage For Our Quadratic Objects (for the pig)
GLUquadricObj *quadraticLeg1;
GLUquadricObj *quadraticLeg2;
GLUquadricObj *quadraticNose;
GLUquadricObj *quadraticHands;
GLUquadricObj *quadraticTail;
// avatar's animation angle
int avatarAngle = -90;
int avatarPreviousAngle = -90;
int avatarRotationAngle;
// secondary camera flag
int secondCamera = 0;

//player stats
int playerStock = 0;
int playerPoints = 50;
int playerLifes = 3;
int wins = 0;
int defeats = 0;

void addPoints(int points){
  if ((playerPoints+points) < 0){
	addLifes(-1);
  } else {
    playerPoints += points;
    if (playerPoints >= 300){
		wins += 1;
		init();
	}
  }
}

void drawAvatar() {
  quadraticLeg1=gluNewQuadric();          // Create A Pointer To The Quadric Object 
  gluQuadricNormals(quadraticLeg1, GLU_SMOOTH);   // Create Smooth Normals 
  gluQuadricTexture(quadraticLeg1, GL_TRUE);      // Create Texture Coords 

  quadraticLeg2=gluNewQuadric();          
  gluQuadricNormals(quadraticLeg2, GLU_SMOOTH);   
  gluQuadricTexture(quadraticLeg2, GL_TRUE);      

  quadraticNose=gluNewQuadric();         
  gluQuadricNormals(quadraticNose, GLU_SMOOTH);  
  gluQuadricTexture(quadraticNose, GL_TRUE);

  quadraticHands=gluNewQuadric();          
  gluQuadricNormals(quadraticHands, GLU_SMOOTH);   
  gluQuadricTexture(quadraticHands, GL_TRUE);
  
  quadraticTail=gluNewQuadric();          
  gluQuadricNormals(quadraticTail, GLU_SMOOTH);   
  gluQuadricTexture(quadraticTail, GL_TRUE);
  
  glColor3f(1.0f, 0.6f, 0.8f);

  // Draw Legs
  glRotatef(180, 0.0f, 1.0f, 1.0f);
  gluCylinder(quadraticLeg1,0.05f,0.2f,0.7f,10,10);

  glTranslatef(0.2f, 0.0f, 0.0f);
  gluCylinder(quadraticLeg2,0.05f,0.2f,0.7f,10,10);
  
  glRotatef(-180, 0.0f, 1.0f, 1.0f);
  
  // Draw Body
  glTranslatef(0.1f, 0.7f, 0.0f);
  glutSolidSphere(0.35f, 20, 20);

  // Draw belly button
  glColor3f(1.0, 0.0, 1.0);
  glTranslatef(0.0f, -0.1f, 0.35f);
  glutSolidSphere(0.02f, 20, 20);
  
  // Draw tail
  glTranslatef(0.0f , 0.0f, -0.8f);
  gluCylinder(quadraticTail,0.001f, 0.02f, 0.2f, 30, 30);

  // Draw Head
  glTranslatef( 0.0f, 0.1f, 0.45f);
  glColor3f(1.0f, 0.6f, 0.8f);

  glTranslatef(0.0f, 0.5f, 0.0f);
  glutSolidSphere(0.25f, 20, 20);


  // Draw Eyes
  glPushMatrix();
  glColor3f(0.0f, 0.0f, 0.0f);
  glTranslatef(0.05f, 0.10f, 0.18f);
  glutSolidSphere(0.05f, 10, 10);
  glTranslatef(-0.1f, 0.0f, 0.0f);
  glutSolidSphere(0.05f, 10, 10);
  glPopMatrix();

  // Draw Ears
  glPushMatrix();
  glColor3f(1.0f, 0.0f, 1.0f);
  glTranslatef(-0.2f, 0.2f, 0.0f);
  glBegin(GL_TRIANGLES);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 0.1f, 0.0f);
  glVertex3f(0.1f, 0.1f, 0.0f);
  glEnd();

  glTranslatef(0.4f, 0.0f, 0.0f);
  glBegin(GL_TRIANGLES);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 0.1f, 0.0f);
  glVertex3f(-0.1f, 0.1f, 0.0f);
  glEnd();
  glPopMatrix();

  // Draw hands
  glColor3f(1.0f, 0.0f, 1.0f);
  gluCylinder(quadraticHands,0.5f,0.5f,0.2f,5,5);
  
  // Draw Nose
  glColor3f(1.0f, 0.0f, 1.0f);
  gluCylinder(quadraticNose,0.08f,0.08f,0.3f,30,30);
  glTranslatef(0.0f, 0.0f, 0.3f);
  gluDisk(quadraticNose, 0.0f, 0.08f, 30, 30);
  glColor3f(0.0f, 0.0f, 0.0f);
  glPushMatrix();
  glColor3f(0.0f, 0.0f, 0.0f);
  glTranslatef(0.025f, -0.025f, 0.0f);
  glutSolidSphere(0.02f, 10, 10);
  glTranslatef(-0.05f, 0.0f, 0.0f);
  glutSolidSphere(0.02f, 10, 10);
  glPopMatrix();
}

void drawAvatarOnMove() {
 
  glColor3f(1.0f, 0.6f, 0.8f);

  // Draw Legs
  glRotatef(180, 0.0f, 1.0f, 1.0f);

  glRotatef(-10, 1.0f, 0.0f, 0.0f);
  gluCylinder(quadraticLeg1,0.05f,0.2f,0.7f,10,10);
  glRotatef(10, 1.0f, 0.0f, 0.0f);
  
  glTranslatef(0.2f, 0.0f, 0.0f);
  glRotatef(10, 1.0f, 0.0f, 0.0f);
  gluCylinder(quadraticLeg2,0.05f,0.2f,0.7f,10,10);
  glRotatef(-10, 1.0f, 0.0f, 0.0f);
  
  glRotatef(-180, 0.0f, 1.0f, 1.0f);
  
  // Draw Body
  glTranslatef(0.1f, 0.7f, 0.0f);
  glutSolidSphere(0.35f, 20, 20);

  // Draw belly button
  glColor3f(1.0, 0.0, 1.0);
  glTranslatef(0.0f, -0.1f, 0.35f);
  glutSolidSphere(0.02f, 20, 20);

  // Draw tail
  glTranslatef(0.0f , 0.0f, -0.8f);
  glRotatef(20, 0.0f, 1.0f, 1.0f);
  gluCylinder(quadraticTail,0.001f, 0.02f, 0.2f, 30, 30);
  glRotatef(-20, 0.0f, 1.0f, 1.0f);
  
  // Draw Head
  glTranslatef( 0.0f, 0.1f, 0.45f);
  glColor3f(1.0f, 0.6f, 0.8f);

  glTranslatef(0.0f, 0.5f, 0.0f);
  glutSolidSphere(0.25f, 20, 20);


  // Draw Eyes
  glPushMatrix();
  glColor3f(0.0f, 0.0f, 0.0f);
  glTranslatef(0.05f, 0.10f, 0.18f);
  glutSolidSphere(0.05f, 10, 10);
  glTranslatef(-0.1f, 0.0f, 0.0f);
  glutSolidSphere(0.05f, 10, 10);
  glPopMatrix();

  // Draw Ears
  glPushMatrix();
  glColor3f(1.0f, 0.0f, 1.0f);
  glTranslatef(-0.2f, 0.2f, 0.0f);
  glBegin(GL_TRIANGLES);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 0.1f, 0.0f);
  glVertex3f(0.1f, 0.1f, 0.0f);
  glEnd();

  glTranslatef(0.4f, 0.0f, 0.0f);
  glBegin(GL_TRIANGLES);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 0.1f, 0.0f);
  glVertex3f(-0.1f, 0.1f, 0.0f);
  glEnd();
  glPopMatrix();


  // Draw hands
  glColor3f(1.0f, 0.0f, 1.0f);
  gluCylinder(quadraticHands,0.5f,0.5f,0.2f,5,5);
  
  // Draw Nose
  glColor3f(1.0f, 0.0f, 1.0f);
  gluCylinder(quadraticNose,0.08f,0.08f,0.3f,30,30);
  glTranslatef(0.0f, 0.0f, 0.3f);
  gluDisk(quadraticNose, 0.0f, 0.08f, 30, 30);
  glColor3f(0.0f, 0.0f, 0.0f);
  glPushMatrix();
  glColor3f(0.0f, 0.0f, 0.0f);
  glTranslatef(0.025f, -0.025f, 0.0f);
  glutSolidSphere(0.02f, 10, 10);
  glTranslatef(-0.05f, 0.0f, 0.0f);
  glutSolidSphere(0.02f, 10, 10);
  glPopMatrix();
}

void addLifes(int lifes){
  if ((playerLifes + lifes) < 0){
    defeats += 1;
    init();
  } else {
    playerLifes += lifes;
  }
}

void colourCube(int x,int y,int z,int colour){
	GLfloat ret;
	
	switch (colour){
  	case BLUE :
      cubes[x][y][z].colourR = blue[0];
      cubes[x][y][z].colourG = blue[1];
      cubes[x][y][z].colourB = blue[2];
  		break;
  	case YELLOW :
  		cubes[x][y][z].colourR = yellow[0];
      cubes[x][y][z].colourG = yellow[1];
      cubes[x][y][z].colourB = yellow[2];
  		break;
  	case RED :
  		cubes[x][y][z].colourR = red[0];
      cubes[x][y][z].colourG = red[1];
      cubes[x][y][z].colourB = red[2];
  		break;
  	case GREEN :
  		cubes[x][y][z].colourR = green[0];
      cubes[x][y][z].colourG = green[1];
      cubes[x][y][z].colourB = green[2];
  		break;
  	case MAGENTA :
  		cubes[x][y][z].colourR = magenta[0];
      cubes[x][y][z].colourG = magenta[1];
      cubes[x][y][z].colourB = magenta[2];
  		break;
  	default:
  		printf("PROBLHMA ME TO colourCube()!!");
  		exit(1);
	}
}

int cubeState(int x,int y,int z){
  if(cubes[x][y][z].state == 1) return 1;
  return 0;
}

void destroyCube(int x,int y,int z){
  cubes[x][y][z].state = 0;
}

int checkForFall(int fallX,int fallY,int fallZ){
  int i;
  int levelsToFall = 0;

  for(i = 0; i < fallY; i++) {
    if (!cubeState(fallX,i,fallZ)){
      levelsToFall += 1;
    } else {
      levelsToFall = 0;
    }
  }

  if(levelsToFall == fallY){
    addLifes(-1);
  } else {
    addPoints(levelsToFall*(-5));
  }

  return levelsToFall;
}

int checkForLevelUp(int x,int y,int z){

  if((cubeState(x,y,z)) && (!cubeState(x,y+1,z))){
    if(y+1 == N-1){
      wins += 1;
      return 1;
    }
    addPoints(5);
    return 1;
  }
  return 0;
}

void spawnRandomCube(int x,int y,int z){
  int i,colour;
  for (i = y; i < N; i++){
    if(cubeState(x,i,z)) continue;
    colour = (int) (rand() % 4);
    colourCube(x,i,z,colour);
    cubes[x][i][z].state = 1;
    cubes[x][i][z].colour = colour;
    addPoints(5);
	playerStock -= 1;
    break;
  }
}

void destroyColumn(int x,int z){
  int i;
  
  for (i = 0; i < N; i++){
    if(cubes[x][i][z].colour == MAGENTA) continue;
    cubes[x][i][z].state = 0;
  }
}

void drawCubes() {
  int i,j,k,cubNumber;

  for( i = 0; i < N; i++) {
    for ( j = 0; j < N; j++) {
      for ( k = 0; k < N; k++) {

        if(cubes[i][j][k].state){
          glPushMatrix();

          glColor3f(cubes[i][j][k].colourR, cubes[i][j][k].colourG, cubes[i][j][k].colourB);
          glTranslatef(i * HORIZONTAL_GAP,j * VERTICAL_GAP,k * HORIZONTAL_GAP);
          glutSolidCube(2);

          glPopMatrix();
        }

      }
    }
  }
  
}

void quarterSpinAnimation(int i){
  avatarAngle = avatarPreviousAngle + avatarRotationAngle*(i*0.025f);
  switch (cameraDirection){
    case DIR_BACK :
      lx = (float)sin(angle*(i*0.025f));
      lz = (float)-cos(angle*(i*0.025f));
      break;
    case DIR_FRONT :
      lx = (float)-sin(angle*(i*0.025f));
      lz = (float)cos(angle*(i*0.025f));
      break;
    case DIR_RIGHT :
      lx = (float)-cos(angle*(i*0.025f));
      lz = (float)sin(angle*(i*0.025f));
      break;
    case DIR_LEFT :
      lx = (float)cos(angle*(i*0.025f));
      lz = (float)-sin(angle*(i*0.025f));
      break;
  }
  glutPostRedisplay();
  if (i == 40) return;
  glutTimerFunc(QUARTER_SPIN_ANIM_REF_RATE, quarterSpinAnimation, i+1);
}

void halfSpinAnimation(int i){
  avatarAngle = avatarPreviousAngle + avatarRotationAngle*(i*0.0125f);
  switch (cameraDirection){
    case DIR_BACK :
      lx = (float)sin(angle*(i*0.0125f));
      lz = (float)-cos(angle*(i*0.0125f));
      break;
    case DIR_FRONT :
      lx = (float)-sin(angle*(i*0.0125f));
      lz = (float)cos(angle*(i*0.0125f));
      break;
    case DIR_RIGHT :
      lx = (float)-cos(angle*(i*0.0125f));
      lz = (float)sin(angle*(i*0.0125f));
      break;
    case DIR_LEFT :
      lx = (float)cos(angle*(i*0.0125f));
      lz = (float)-sin(angle*(i*0.0125f));
      break;
  }
  glutPostRedisplay();
  if (i == 80) return;
  glutTimerFunc(HALF_SPIN_ANIM_REF_RATE, halfSpinAnimation, i+1);
}

void columnFall(int x,int y,int z,int cubeBelowState){
  if (!cubeState(x,y,z)){ // if there is no cube in this position
    if(y+1 < N){
      columnFall(x,y+1,z,0);
    }
    return;
  }
  if (!cubeBelowState){ // if there is no cube below
    if (y == 0){
      cubes[x][y][z].state = 0;
    } else {
      cubes[x][y-1][z].colour = cubes[x][y][z].colour;
      colourCube(x,y-1,z,cubes[x][y-1][z].colour);
      cubes[x][y-1][z].state = 1;
      cubes[x][y][z].state = 0;
    }
    if (y+1 < N){
      columnFall(x,y+1,z,0);
    }
  } else {  // if there is cube below
    if (y+1 < N){
      columnFall(x,y+1,z,1);
    }
  }
}

void cubeFallAnimation(int i){
  int j,k;
  for (j = 0; j < N; j++){
    for (k = 0; k < N; k++){
      columnFall(j,0,k,floorState[j][k]);
    }
  }
  glutPostRedisplay();
  if (i == N) {
	cubeFallFlag = 0;
	addPoints(10);
	return;
  }
  glutTimerFunc(COLUMN_FALL_ANIMATION, cubeFallAnimation, i+1);
}

void movementAnimation(int i){
  x = originalX+xOffset*(i*0.1f);
  y = originalY+yOffset*(i*0.1f);
  z = originalZ+zOffset*(i*0.1f);
  glutPostRedisplay();
  if (i == 10){ 
    onMove = 0;
    return;
  }
  glutTimerFunc(MOVEMENT_ANIMATION, movementAnimation, i+1);
}

void kickCube(int x,int y,int z){
  int cordx,cordz;
  cordx = round(lx);
  cordz = round(lz);

  if(cordx==0.0f && cordz==-1.0f){ //back

    if (z-1 < 0){
      cubes[x][y][z].state = 0;
      return;
    }
    if (cubeState(x,y,z-1)){
      kickCube(x,y,z-1);
    }
    cubes[x][y][z-1].colour = cubes[x][y][z].colour;
    colourCube(x,y,z-1,cubes[x][y][z-1].colour);
    cubes[x][y][z-1].state = 1;
    cubes[x][y][z].state = 0;

  } else if (cordx==0.0f && cordz==1.0f){//front

    if (z+1 > N-1){
      cubes[x][y][z].state = 0;
      return;
    }
    if (cubeState(x,y,z+1)){
      kickCube(x,y,z+1);
    }
    cubes[x][y][z+1].colour = cubes[x][y][z].colour;
    colourCube(x,y,z+1,cubes[x][y][z+1].colour);
    cubes[x][y][z+1].state = 1;
    cubes[x][y][z].state = 0;

  } else if (cordx==-1.0f && cordz==0.0f){//right

    if (x-1 < 0){
      cubes[x][y][z].state = 0;
      return;
    }
    if (cubeState(x-1,y,z)){
      kickCube(x-1,y,z);
    }
    cubes[x-1][y][z].colour = cubes[x][y][z].colour;
    colourCube(x-1,y,z,cubes[x-1][y][z].colour);
    cubes[x-1][y][z].state = 1;
    cubes[x][y][z].state = 0;

  } else if (cordx==1.0f && cordz==0.0f){//left

    if (x+1 > N-1){
      cubes[x][y][z].state = 0;
      return;
    }
    if (cubeState(x+1,y,z)){
      kickCube(x+1,y,z);
    }
    cubes[x+1][y][z].colour = cubes[x][y][z].colour;
    colourCube(x+1,y,z,cubes[x+1][y][z].colour);
    cubes[x+1][y][z].state = 1;
    cubes[x][y][z].state = 0;
  }
}

void drawText(int x,int y) 
{  

  int i;
  char buffer[50];

  glColor3f( 1.0f,1.0f, 1.0f);
  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D( 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0 );

  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();
  glDisable(GL_LIGHTING);
    
  glRasterPos2i(x, y+45);

  sprintf(buffer, "Stock = %d", playerStock);
  for ( i = 0; i < strlen(buffer); i++) {
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, buffer[i]);
  }

  sprintf(buffer, "Points = %d", playerPoints);

  glRasterPos2i(x, y+30);
  for ( i = 0; i < strlen(buffer); i++) {
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, buffer[i]);
  }
  
  sprintf(buffer, "Lifes = %d", playerLifes);
  
  glRasterPos2i(x, y+15);
  for ( i = 0; i < strlen(buffer); i++) {
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, buffer[i]);
  }
  
  sprintf(buffer, "Wins = %d", wins);
  
  glRasterPos2i(x, y);
  for ( i = 0; i < strlen(buffer); i++) {
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, buffer[i]);
  }
  
  sprintf(buffer, "Defeats = %d", defeats);
  
  glRasterPos2i(x, y-15);
  for ( i = 0; i < strlen(buffer); i++) {
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, buffer[i]);
  }

  glEnable(GL_LIGHTING);
  glPopMatrix();

  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
  glMatrixMode( GL_MODELVIEW );

}

void processMouseFunc(int button, int mouseState, int mouseX, int mouseY) {
  int cordx,cordz,tempX,tempY,tempZ;

  if (!((fabs(lx)<ERROR)||(fabs(lz)<ERROR))) return; // if player is on move do nothing.

  cordx = round(lx);
  cordz = round(lz);
  tempX = int((x+ROUNDING_ERROR)/HORIZONTAL_GAP);
  tempY = int((y+ROUNDING_ERROR)/VERTICAL_GAP);
  tempZ = int((z+ROUNDING_ERROR)/HORIZONTAL_GAP);

  switch(button) {
    case GLUT_LEFT_BUTTON:
      if (mouseState==GLUT_DOWN) {
		if (cubeFallFlag) break;
        if (playerStock <= 0) break;
        spawnRandomCube(tempX+int(cordx),tempY,tempZ+int(cordz));
      }
      break;
  case GLUT_RIGHT_BUTTON:
    if (mouseState==GLUT_DOWN) {
      if (!cubeState(tempX+int(cordx),tempY,tempZ+int(cordz))) break;
      kickCube(tempX+int(cordx),tempY,tempZ+int(cordz));
    }
    break;
  }
}

void processNormalKeys(unsigned char key, int xmouse, int ymouse) {
  int i,j,k;
  float horizontalStep = HORIZONTAL_GAP;
  float verticalStep = VERTICAL_GAP;
  int cordx,cordz,tempX,tempY,tempZ;

  if (key == ESC) exit(0);
  if (!((fabs(lx)<ERROR)||(fabs(lz)<ERROR))) return; // if player is on move do nothing.

  cordx = round(lx);
  cordz = round(lz);
  tempX = int((x+ROUNDING_ERROR)/HORIZONTAL_GAP);
  tempY = int((y+ROUNDING_ERROR)/VERTICAL_GAP);
  tempZ = int((z+ROUNDING_ERROR)/HORIZONTAL_GAP);

  switch (key) {
    case 'a' :
    case 'A' :
      avatarRotationAngle = 90;
      avatarPreviousAngle = avatarAngle;
      
      if(cordx==0.0f && cordz==-1.0f){ //back
        angle = -(M_PI/2.0f);
        cameraDirection = DIR_BACK;

      } else if (cordx==0.0f && cordz==1.0f){//front
        angle = -(M_PI/2.0f);
        cameraDirection = DIR_FRONT;

      } else if (cordx==-1.0f && cordz==0.0f){//right
        angle = +(M_PI/2.0f);
        cameraDirection = DIR_RIGHT;

      } else if (cordx==1.0f && cordz==0.0f){//left
        angle = +(M_PI/2.0f);
        cameraDirection = DIR_LEFT;
      }

      glutTimerFunc(QUARTER_SPIN_ANIM_REF_RATE, quarterSpinAnimation, 1);
      
      break;
    case 'd' :
    case 'D' :
	  avatarRotationAngle = -90;
      avatarPreviousAngle = avatarAngle;
      
      if(cordx==0.0f && cordz==-1.0f){ //back
        angle = +(M_PI/2.0f);
        cameraDirection = DIR_BACK;

      } else if (cordx==0.0f && cordz==1.0f){//front
        angle = +(M_PI/2.0f);
        cameraDirection = DIR_FRONT;

      } else if (cordx==-1.0f && cordz==0.0f){//right
        angle = -(M_PI/2.0f);
        cameraDirection = DIR_RIGHT;

      } else if (cordx==1.0f && cordz==0.0f){//left
        angle = -(M_PI/2.0f);
        cameraDirection = DIR_LEFT;
      }
      
      glutTimerFunc(QUARTER_SPIN_ANIM_REF_RATE, quarterSpinAnimation, 1);
	  
      break;
    case 'w' :
    case 'W' :
      if(((x+lx*horizontalStep)>(N-1)*HORIZONTAL_GAP+BORDER_ERROR) || ((z+lz*horizontalStep)>(N-1)*HORIZONTAL_GAP+BORDER_ERROR)) {
        break;
      } else if(((BORDER_ERROR+x+lx*horizontalStep)<0) || ((BORDER_ERROR+z+lz*horizontalStep)<0)){
        break;
      }
      if (onMove) {
        break;
      } else {
        onMove = 1;
      }

      if(checkForLevelUp(tempX+int(cordx),tempY,tempZ+int(cordz))){
        xOffset = lx * horizontalStep;
        yOffset = verticalStep;
        zOffset = lz * horizontalStep;
        if (int((yOffset+y+ROUNDING_ERROR)/VERTICAL_GAP) == N-1){
			init();
			onMove = 0;
			break;
		}
      } else if (cubeState(tempX+int(cordx),tempY,tempZ+int(cordz))){ //if there is a cube in front of the player, he can't move.
		onMove = 0;
        break;
      } else {
        xOffset = lx * horizontalStep;
        yOffset = (-1.0f)*verticalStep*checkForFall(tempX+int(cordx),tempY,tempZ+int(cordz));
        zOffset = lz * horizontalStep;
        if (int(((-1.0f)*yOffset+ROUNDING_ERROR)/VERTICAL_GAP) == tempY) { 
          x = round(N/2)*HORIZONTAL_GAP;
          z = round(N/2)*HORIZONTAL_GAP;
          y = VERTICAL_GAP;
          onMove = 0;
          break;
        }
      }
      originalX = x;
      originalY = y;
      originalZ = z;
      glutTimerFunc(MOVEMENT_ANIMATION, movementAnimation, 1);
      break;
    case 's' :
    case 'S' : 
      avatarRotationAngle = 180;
      avatarPreviousAngle = avatarAngle;
      
      if(cordx==0.0f && cordz==-1.0f){ //back
        angle = +M_PI;
        cameraDirection = DIR_BACK;

      } else if (cordx==0.0f && cordz==1.0f){//front
        angle = +M_PI;
        cameraDirection = DIR_FRONT;

      } else if (cordx==-1.0f && cordz==0.0f){//right
        angle = -M_PI;
        cameraDirection = DIR_RIGHT;

      } else if (cordx==1.0f && cordz==0.0f){//left
        angle = -M_PI;
        cameraDirection = DIR_LEFT;
      }

      glutTimerFunc(HALF_SPIN_ANIM_REF_RATE, halfSpinAnimation, 1);
	 
      break;
    case 'e':
    case 'E':
	  for (i = 0; i < N; i++){
		  if (cubeState(tempX+int(cordx),i,tempZ+int(cordz))){
			  destroyColumn(tempX+int(cordx),tempZ+int(cordz));
			  addLifes(1);
			  addPoints(-20);
			  break;
		  }
	  }
      break;
    case 'c':
    case 'C':
      secondCamera = !secondCamera;
      break;
    case 'r':
    case 'R':
	  if (cubeFallFlag){
		  break;
	  } else {
		  cubeFallFlag = 1;
	  }
      for (i = 0; i < N; i++){
        for (j = 0; j < N; j++){
          floorState[i][j] = cubes[i][0][j].state;
        }
      }
      glutTimerFunc(COLUMN_FALL_ANIMATION, cubeFallAnimation, 1);
      break;
    case 'q':
    case 'Q':
      destroyCube(tempX+int(cordx),tempY,tempZ+int(cordz));
      break;
    case SPACEBAR:

      if (cubes[tempX][tempY-1][tempZ].colour == MAGENTA){
        break;
      } else if(cubes[tempX][tempY-1][tempZ].colour == BLUE){
        break;
      } else {
		addPoints(-5);
        playerStock += 1;
        cubes[tempX][tempY-1][tempZ].colour -= 1;
        colourCube(tempX,tempY-1,tempZ,cubes[tempX][tempY-1][tempZ].colour);
      }
      break;
    }
}

void processSpecialKeys(int key, int xmouse, int ymouse) {

  float angle = 0.1f;

  switch (key) {
      case GLUT_KEY_UP :
        ly += angle;
        break;
      case GLUT_KEY_DOWN :
        ly -= angle;
        break;
  }
}

void renderScene(void) {

  //Clear Color and Depth Buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Reset transformations
  glLoadIdentity();
  if (!secondCamera){
    //first person camera
    gluLookAt(x, y, z,
        x+lx, y+ly,z+lz,
        0.0f, 1.0f,  0.0f);
  } else {
    //third person camera
    gluLookAt(int(round(N/2)), int(round(N/2)), N,
        x, y,z,
        0.0f, 1.0f,  0.0f);
  }
 
  GLfloat playerLight_pos[] = {x+lx,y+ly,z+lz,1.0f};
  glLightfv(GL_LIGHT4,GL_POSITION, playerLight_pos);
  glLightfv(GL_LIGHT4, GL_DIFFUSE, playerLight_diffuse);

  glEnable(GL_DEPTH_TEST);

  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHT2);
  glEnable(GL_LIGHT3);
  glEnable(GL_LIGHT4);
  glEnable(GL_LIGHTING);

  drawCubes();
  glTranslatef(x,y-1.1,z);
  glRotatef(avatarAngle,0.0f,1.0f,0.0f);
  if (onMove){
    drawAvatarOnMove();
  } else {
    drawAvatar();
  }

  glDisable(GL_LIGHTING);
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHT1);
  glDisable(GL_LIGHT2);
  glDisable(GL_LIGHT3);
  glDisable(GL_LIGHT4);

  glDisable(GL_DEPTH_TEST);
  
  drawText(150,30);

  glutSwapBuffers();
}

void changeSize(int w, int h) {

  // Prevent a divide by zero, when window is too short
  // (you cant make a window of zero width).
  if(h == 0)
    h = 1;
  float ratio = 1.0* w / h;

  // Use the Projection Matrix
  glMatrixMode(GL_PROJECTION);
  // Reset Matrix
  glLoadIdentity();

  // Set the viewport to be the entire window
  glViewport(0, 0, w, h);

  // Set the correct perspective.
  gluPerspective(45,ratio,1,1000);

  // Get Back to the Modelview
  glMatrixMode(GL_MODELVIEW);
}

void init(){
  int i, j, k, colour;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  
  playerStock = 0;
  playerPoints = 50;
  playerLifes = 3;

  srand(time(NULL));

  cubes = (cube ***)malloc(N * sizeof(cube **));
  floorState = (int **)malloc(N * sizeof(int *));
  for (i = 0; i < N; i++){
    cubes[i] = (cube **)malloc(N * sizeof(cube *));
    floorState[i] = (int *)malloc(N * sizeof(int ));
    for (j = 0; j < N; j++){
      cubes[i][j] = (cube *)malloc(N * sizeof(cube));
    }
  }

  rng = (GLfloat **)malloc(N*N*sizeof(GLfloat *));
  for (i = 0; i < N*N; i++){
    rng[i] = (GLfloat *)malloc(3*sizeof(GLfloat));
  }

  for (i = 0; i < N; i++){
    for (j = 0; j < N; j++){
      for(k = 0; k < N; k++){

        cubes[i][j][k].colour = -1;
        cubes[i][j][k].state = 0;
        cubes[i][j][k].colourR = -1;
        cubes[i][j][k].colourG = -1;
        cubes[i][j][k].colourB = -1;
      }
    }
  }

  for (i = 0; i < N; i++){
    for (j = 0; j < N; j++){ 
        colour = (int) (rand() % 4);

        cubes[i][0][j].colour = colour;
        cubes[i][0][j].state = 1;
        colourCube(i,0,j,colour);
    }
  }
  cubes[int(round(N/2))][0][int(round(N/2))].colour = MAGENTA;
  cubes[int(round(N/2))][0][int(round(N/2))].state = 1;
  colourCube(int(round(N/2)),0,int(round(N/2)),MAGENTA);

  // player spawns at the magenta cube
  x = round(N/2)*HORIZONTAL_GAP;
  y = 1*VERTICAL_GAP;
  z = round(N/2)*HORIZONTAL_GAP;

  if ((wins == 0)&&(defeats == 0)) {
	  light_dir[0] = round(N/2)*1.0f;
	  light_dir[1] = round(N/2)*1.0f;
	  light_dir[2] = round(N/2)*1.0f;
	  light0_pos[1] = N*1.0f;
	  light1_pos[0] = N*1.0f;
	  light1_pos[1] = N*1.0f;
	  light2_pos[1] = N*1.0f;
	  light2_pos[2] = N*1.0f;
	  light3_pos[0] = N*1.0f;
	  light3_pos[1] = N*1.0f;
	  light3_pos[2] = N*1.0f;

	  glEnable(GL_COLOR_MATERIAL);
	  glEnable(GL_NORMALIZE);
	  glShadeModel(GL_SMOOTH);
	  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	  //disabling default light
	  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, default_light);

	  glLightfv(GL_LIGHT0,GL_POSITION, light0_pos);
	  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

	  glLightfv(GL_LIGHT1,GL_POSITION, light1_pos);
	  glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);

	  glLightfv(GL_LIGHT2,GL_POSITION, light2_pos);
	  glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);

	  glLightfv(GL_LIGHT3,GL_POSITION, light3_pos);
	  glLightfv(GL_LIGHT3, GL_DIFFUSE, light_diffuse);
  }
}

int main (int argc, char **argv) {
  //init Glut and create window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(320, 320);
  glutCreateWindow("Piglet minecraft");

  N = atoi(argv[1]);

  init();
  //register callbacks
  glutDisplayFunc(renderScene);
  glutReshapeFunc(changeSize);
  glutIdleFunc(glutPostRedisplay);
  //Keyboard functions
  glutKeyboardFunc(processNormalKeys);
  glutSpecialFunc(processSpecialKeys);
  glutMouseFunc(processMouseFunc);
  
  glutMainLoop();

  return 1;
}
