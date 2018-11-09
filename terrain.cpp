//3GC3 Assignment 2 - Kumail Naqvi - 400068946

/* 
	Features Implemented: 
		Circles Algorithm
		Lighting & Normals - 'l' to turn off
		Interactive viewpoint control 
		Wireframe representation - 'w' to toggle
		Shading - 's' to toggle
		Reset - 'r' to toggle
		Backface Culling
	
	Extra Features:
		Faults Algorithm - 'f' to toggle
		Improved Camera - (a, d and arrow keys)
		Topographic coloring - 'c' to toggle
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <cmath>
#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

//Globals
float camPos[] = {80, 30, 80};
float eye[] = {100, 100, 100};

float lightPos1[] = {200, 10, 120, 1};
float lightPos2[] = {-10, -300, -230, 1};

//defaults
float defaultCamPos[] = {80, 30, 80};
float defaultEye[] = {100, 100, 100};
float defaultLightPos1[] = {200, 10, 120, 1};
float defaultLightPos2[] = {-10, -300, -230, 1};

float position = 0;
float angle = 0;
float uangle = 0;

int size;

int heightMapWidth = 5;
int heightMapHeight = 5;
int heightMapSize;

float minHeightInMap;
float maxHeightInMap;
float medianHeightInMap;

//State bools
bool wireFrame = false;
bool lighting = true;
bool flip = false;
bool shading = true;
bool circles = true;
bool strips = true;
bool color = true;
int wireframeChoice = 1;
int polygonMode = 1;

//Struct to hold normals
struct Normal {
	float x, y, z;
};

//Height maps of the terrain
float heightMap[300][300];
Normal normalsMap[300][300];

//Algorithm to 
void circlesAlgorithm(int passes){
	for(int i = 0; i < heightMapSize; i++){                                                                                                                                 
		for(int j = 0; j < heightMapSize; j++){
			heightMap[i][j] = 5;
		}
	}
	for(int p = 0; p < passes; p++){
		int random_x = rand() % heightMapSize, random_z = rand() % heightMapSize;                 
        int random_radius = (rand() % (heightMapSize  / 2)) + 3;        
        float disp = (rand() % 10) + 3;    

		for(int i = 0; i < heightMapSize; i++){
			float xran = rand() % heightMapSize;
			float zran = rand() % heightMapSize;
				for(int j = 0; j < heightMapSize; j++){
					int dx = i - random_x, dz = j - random_z;  
					float distance = sqrtf((dx * dx) + (dz * dz));  
                	float pd = (distance * 2) / random_radius;
					if (fabs(pd) <= 1.0) heightMap[i][j] += (disp/2.0) + (cos(pd*3.14)*(disp/2.0)); 
                	
				}
			}
	}

	for(int x = 0; x < heightMapSize; x++){                       
        for(int z = 0; z < heightMapSize; z++){
            if (heightMap[x][z] < minHeightInMap) minHeightInMap = heightMap[x][z];
            else if (heightMap[x][z] > maxHeightInMap) maxHeightInMap = heightMap[x][z];
        }
    }
}

void faultsAlgorithm(int passes){
    for(int i = 0; i < passes; i++){
        float t1 = static_cast <float> (rand()), t2 = sin(t1), t3 = cos(t1);                              
        float tt = sqrt(heightMapSize * heightMapSize + heightMapSize * heightMapSize);    
        float t4 = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * tt - tt/2;
        for (int x = 0; x < heightMapSize; x++){
            for (int z = 0; z < heightMapSize; z++){
                if (t2 * x + t3 * z - t4 > 0) heightMap[x][z] += 1;
    			else heightMap[x][z] -= 1;              
                }
            }
        }
    }


void normalsAlgorithm(){
	Normal v1, v2, v3, v;
    for(int x = 0; x < heightMapSize; x++){
        for(int z = 0; z < heightMapSize; z++){
            v1.x = x + 1;
            v1.y = heightMap[x+1][z] - heightMap[x][z];
            v1.z = z;
            
            v2.x = x+1;
            v2.y = heightMap[x+1][z+1] - heightMap[x][z];
            v2.z = z+1;
            
            v.x = v1.y * v2.z - v1.z * v2.y;
            v.y = v1.z * v2.x - v1.x * v2.z;
            v.z = v1.x * v2.y - v1.x * v2.x;
            float l = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
            
            normalsMap[x][z].x = v.x/l;
            normalsMap[x][z].y = v.y/l;
            normalsMap[x][z].z = v.z/l;
        }
    }
}


//Renders terrain after heightmap is filled
void actuallyRenderTerrain(){
	if (strips) {
		for(int i = 0; i < heightMapSize; i++){
				glColor3f(0.3f, 0.9f, 0.0f);
				glBegin(GL_TRIANGLE_STRIP);
					for(int j = 0; j < heightMapSize; j++){
						float cHeight = (heightMap[j][i] - minHeightInMap) / (maxHeightInMap - minHeightInMap);
						if (cHeight>=0.6 && color){
                            glColor3f(1, 1, 1);
                        }
                        else if(cHeight>=0.25 && color){
							glColor3f(0.8, 0.7, 0.54);
                        }
                        else {
                            glColor3f(0.309, 0.674, 0.258);
                        }
						glNormal3f(normalsMap[j][i].x, normalsMap[j][i].y, normalsMap[j][i].z);
						glVertex3f(float(j), heightMap[j][i], float(i));
						glNormal3f(normalsMap[j][i + 1].x, normalsMap[j][i + 1].y, normalsMap[j][i + 1].z);
						glVertex3f(float(j), heightMap[j][i + 1], float(i + 1));
						}
				glEnd();
			}
	}
	else {
		for(int x = 1; x < heightMapSize -1; x++){
        	for(int z = 1; z < heightMapSize -1; z++){
            	glBegin(GL_QUADS); 
            	for(int i = 0; i < 4; i++){
					int new_x;
					int new_z;
					if(i == 0){
						new_x = 0;
						new_z = 0;
					} else if (i == 1) {
						new_x = 0;
						new_z = -1;
					} else if (i == 2) {
						new_x = -1;
						new_z = -1;
					} else if (i == 3) {
						new_x = -1;
						new_z = 0;
					}
                float new_y = heightMap[new_x][new_z]; 
                glNormal3f(normalsMap[new_x][new_z].x, normalsMap[new_x][new_z].y, normalsMap[new_x][new_z].z);
                glVertex3d(new_x, new_y, new_z);
            }
            glEnd();
        
    	}
	}
}}

renderTerrain(int mode){
	if (mode == 1) {
		//Normal polygons
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		actuallyRenderTerrain();
	} else if (mode == 2) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		actuallyRenderTerrain();
	} else {
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//actuallyRenderTerrain();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		actuallyRenderTerrain();
	}
}


void display(void){
    float di[] = {0.30, 0.80, 0.05, 1.0}, am[] = {0.70, 0.40, 0.50, 1.0}, sp[] = {0.90, 0.80, 0.70, 1.0};
    
	glColorMaterial(GL_AMBIENT, GL_DIFFUSE);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, di);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, am);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, sp);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 30);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    gluLookAt(camPos[0], camPos[1], camPos[2], 0, 0, 0, 0, 1, 0);
    glTranslatef(-1 * (heightMapSize / 2), 0, -1 * (heightMapSize / 2));
	glRotatef(angle, uangle, 1, 0);

	normalsAlgorithm();
	renderTerrain(polygonMode);
    
    glutSwapBuffers();
}

void keyboard(unsigned char key, int xIn, int yIn)
{
	switch(key){
		case 'q':
		case 27:
			exit(0);
			break;
		case 'a':
 			angle += 1;
			break;
		case 'd':
			angle -= 1;
			break;

		case 'j':
			uangle += 1;
			break;
		case 'i':
			uangle -= 1;
			break;
		case 'y':
		case 'Y':
			strips = false;
			break;
		case 't':
		case 'T':
			strips = true;
			break;
		case 'c':
		case 'C':
			color = !color;
			break;
		case 'f':
		case 'F':
			if(circles){
				faultsAlgorithm(100);
				normalsAlgorithm();
				renderTerrain(polygonMode);
			} else{
				circlesAlgorithm(100);
				normalsAlgorithm();
				renderTerrain(polygonMode);
			}
			circles = !circles;
			break;
		case 'w':
		case 'W':
			polygonMode = (polygonMode + 1) % 3;
			break;
		case 'r':
		case 'R': 
			camPos[0] = 80;
			camPos[1] = 30;
			camPos[2] = 80;
			eye[0] = 100;
			eye[1] = 100;
			eye[2] = 100;
			lightPos1[0] = 200;
			lightPos1[1] = 10;
			lightPos1[2] = 120;
			lightPos1[3] = 1;
			lightPos2[0] = -10;
			lightPos2[1] = -300;
			lightPos2[2] = -230;
			lightPos2[3] = 1;
			circlesAlgorithm(100);
			normalsAlgorithm();
			renderTerrain(polygonMode);
			break;
		case 's':
		case 'S':
			if (shading) glShadeModel(GL_FLAT);
			else glShadeModel(GL_SMOOTH);
			shading = !shading;
			break;
		case 'l':
		case 'L':
			if (lighting) {
				glDisable(GL_LIGHT0);
                glDisable(GL_LIGHTING);
			} else {
				glEnable(GL_LIGHT0);
                glEnable(GL_LIGHTING);
			}
			lighting = !lighting;
			break;
		case '1':
            lightPos1[1] -= 10;
            lightPos2[1] -= 10;
            break;
        case '2':
            lightPos1[1] += 10;
            lightPos2[1] += 10;
            break;
        case '3':
            lightPos1[2] += 10;
            lightPos2[2] += 10;
            break;
        case '4':
            lightPos1[2] -= 10;
            lightPos2[2] -= 10;
			//printf("Lightpos at: %f, %f \n", lightPos1[2], lightPos2[2]);
            break;
	}
	
	glMatrixMode(GL_MODELVIEW); 
    glPushMatrix(); 
    glLoadIdentity();
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);
    glPopMatrix();
}

void init(void)
{
 	glClearColor(0, 0, 0, 0);
    glColor3f(1, 1, 1);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1, 1, 400); 
    
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    
}

void special(int key, int x, int y){
	switch(key)
    {
		case GLUT_KEY_RIGHT:
            camPos[2] -= 2;
            break;
        case GLUT_KEY_LEFT:
            camPos[2] +=  2;
            break;
        case GLUT_KEY_UP:
            if (camPos[1] < 0){
                flip = !flip;
                camPos[0]*=-1;
                camPos[2]*=-1;
            }
            if (flip){
                camPos[1] -= 2;
            }
            else{
                camPos[1] += 2;
            }
            break;
        case GLUT_KEY_DOWN:
            if (camPos[1]<0){
                flip = !flip;
                camPos[0]*=-1;
                camPos[2]*=-1;
            }
            if (!flip){
                camPos[1] -= 2;
            }
            else{
                camPos[1] += 2;
            }
            break;
    }

    glutPostRedisplay();
}

void mouse(int btn, int state, int x, int y){
	if(btn == GLUT_LEFT_BUTTON){
	}
}

void motion(int x, int y){
}

void passive(int x, int y){
}

void reshape(int w, int h)
{
/* 	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(100, ((w+0.0f)/h), 1, 100);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h); */
}

void FPS(int val){
	glutPostRedisplay();
	glutTimerFunc(17, FPS, 0);
}

void callBackInit(){
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(passive);
	glutReshapeFunc(reshape);
	glutTimerFunc(0, FPS, 0);
}

int main(int argc, char** argv){
	std::cout << "Enter your size (Between 50 and 250)";
	std::cin >> size; 
	heightMapSize = size;

	printf("Welcome to terrain generator!, keys: \n arrow keys - pan\n a & d - rotate\n s - toggle shading\n w - toggle wireframe\n 1, 2, 3, 4 - adjust lighting\n l - toggle lighting \n f - toggle faults/circles \n y - toggle quads/strips \n c - topographic coloring \n");

	glutInit(&argc, argv);	
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("3GC3 Assignmet 2 - Kumail");
	callBackInit();

	init();

	glEnable(GL_DEPTH_TEST);
	circlesAlgorithm(100);
	normalsAlgorithm();

	glutMainLoop();			
	return(0);				
}
