#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<iostream>

#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))

int program_state = 1;
bool show_vectors = true;
//state = 1 for adding points 
//state = 2 for curve drawn
//state = 3 for update points
//state = 4 for move point along the curve
//state = 5 select point to be updated
//state = 6 or 2 select new point

struct point2d
{
    double x, y;
};


struct curve {
	double ax, bx, cx, dx;
	double ay, by, cy, dy;
};

struct point2d cp[200];
int cpidx;

void drawSquare()
{
    glBegin(GL_QUADS);
    {
        glVertex3d( 3,  3, 0);
        glVertex3d( 3, -3, 0);
        glVertex3d(-3, -3, 0);
        glVertex3d(-3,  3, 0);
    }
    glEnd();
}

void drawCurve() {
	for (int i = 0; i < cpidx; i += 2) {
		point2d p1 = cp[i];
		point2d r1 = { cp[i + 1].x - p1.x, cp[i + 1].y - p1.y };
		point2d p4 = cp[i + 2];
		point2d r4 = { cp[i + 3].x - p4.x, cp[i + 3].y - p4.y };

		double ax = 2 * p1.x - 2 * p4.x + r1.x + r4.x;
		double bx = -3 * p1.x + 3 * p4.x - 2 * r1.x - r4.x;
		double cx = r1.x;
		double dx = p1.x;

		double ay = 2 * p1.y - 2 * p4.y + r1.y + r4.y;
		double by = -3 * p1.y + 3 * p4.y - 2 * r1.y - r4.y;
		double cy = r1.y;
		double dy = p1.y;

		double x[100], y[100];

		for (double t = 0, j = 0; t <= 1; t += 0.01, ++j) {
			x[(int)j] = ax * t*t*t + bx * t*t + cx * t + dx;
			y[(int)j] = ay * t*t*t + by * t*t + cy * t + dy;
		}
		glColor3d(1, 1, 1);
		for (int j = 0; j < 99; ++j) {
			glBegin(GL_LINES); {
				glVertex3d(x[j], y[j], 0);
				glVertex3d(x[j + 1], y[j + 1], 0);
			}
			glEnd();
		}
	}
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

        case 'g':
			show_vectors = !show_vectors;
			break;
		case 'a':
			if (program_state == 2) {
				program_state == 4;
			}
		case 'u':
			if (program_state == 4 || program_state == 2) {
				program_state = 3;
			}
		default:
			break;
	}
}

void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			break;
		case GLUT_KEY_UP:		// up arrow key
			break;

		case GLUT_KEY_RIGHT:
			break;
		case GLUT_KEY_LEFT:
			break;

		case GLUT_KEY_PAGE_UP:
			break;
		case GLUT_KEY_PAGE_DOWN:
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				if (program_state == 1) {
					std::cout << "Clicked point (" << x << ", " << y << ")" << std::endl;

					cp[cpidx].x = (double)x;
					cp[cpidx].y = (double)(600 - y);
					cpidx++;
				}

				else if (program_state == 3) {
					program_state = 5;
				}
				
				else if (program_state == 5) {
					program_state = 2;
				}
			}
			break;

		case GLUT_RIGHT_BUTTON:
			if (state == GLUT_DOWN) {
				if (program_state == 1 && cpidx != 0 && cpidx % 2 == 0) {
					program_state = 2;
				}
			}
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}



void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	//gluLookAt(150*cos(cameraAngle), 150*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(0,0,0,	0,0,-1,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects


	int i;

    for (i = 0; i < cpidx; i++)
    {
        glColor3f(1, 1, 0);
        glPushMatrix();
        {
            glTranslatef(cp[i].x, cp[i].y, 0);
            drawSquare();
        }
        glPopMatrix();
    }

	if (program_state != 1)
		drawCurve();

	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){


	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization

	cpidx = 0;

	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluOrtho2D(0, 800, 0, 600);
	//gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
