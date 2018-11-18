#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))


const double circle_radius = 150;
const double small_circle_radius = 15;
const double tri_angle_side = 4;
const double speed = 0.1;
const double steer_angle = pi / 90;

struct point
{
	double x, y;
};


struct vector
{
	double x, y;
};


point p1, p2;
vector v1, v2;



void drawCircle(double radius, int segments)
{
	int i;
	struct point points[100];
	//generate points
	for (i = 0; i <= segments; i++)
	{
		points[i].x = radius * cos(((double)i / (double)segments) * 2 * pi);
		points[i].y = radius * sin(((double)i / (double)segments) * 2 * pi);
	}
	//draw segments using generated points
	for (i = 0; i < segments; i++)
	{
		glBegin(GL_LINES);
		{
			glVertex3f(points[i].x, points[i].y, 0);
			glVertex3f(points[i + 1].x, points[i + 1].y, 0);
		}
		glEnd();
	}
}


void drawTriangle(point p1, point p2, point p3) {
	glBegin(GL_TRIANGLES);
	{
		glVertex3f(p1.x, p1.y, 0);
		glVertex3f(p2.x, p2.y, 0);
		glVertex3f(p3.x, p3.y, 0);
	}
	glEnd();
}


void drawLine(point p1, point p2) {
	glBegin(GL_LINES);
	{
		glVertex3f(p1.x, p1.y, 0);
		glVertex3f(p2.x, p2.y, 0);
	}
	glEnd();
}

void steer_left_circle1() {
	vector v1_perp = { -v1.y, v1.x };
	v1.x = v1.x*cos(steer_angle) + v1_perp.x*sin(steer_angle);
	v1.y = v1.y*cos(steer_angle) + v1_perp.y*sin(steer_angle);
}

void steer_right_circle1() {
	vector v1_perp = { -v1.y, v1.x };
	v1.x = v1.x*cos(-steer_angle) + v1_perp.x*sin(-steer_angle);
	v1.y = v1.y*cos(-steer_angle) + v1_perp.y*sin(-steer_angle);
}

void keyboardListener(unsigned char key, int x, int y) {
	switch (key) {
	default:
		break;
	}
}


void specialKeyListener(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_DOWN:		//down arrow key
		break;
	case GLUT_KEY_UP:		// up arrow key
		break;

	case GLUT_KEY_RIGHT:
		steer_right_circle1();
		break;
	case GLUT_KEY_LEFT:
		steer_left_circle1();
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


void mouseListener(int button, int state, int x, int y) {	//x, y is the x-y of the screen (2D)
	switch (button) {
	case GLUT_LEFT_BUTTON:
		break;

	case GLUT_RIGHT_BUTTON:
		//........
		break;

	case GLUT_MIDDLE_BUTTON:
		//........
		break;

	default:
		break;
	}
}



void display() {

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);	//color black
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
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(0, 0, 200, 0, 0, 0, 0, 1, 0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects
	glColor3f(1, 1, 1);
	drawCircle(circle_radius, 95);
	
	glPushMatrix();
	{
		glColor3f(1, 1, 0);
		glTranslatef(p1.x, p1.y, 0);
		drawCircle(small_circle_radius, 95);

	}
	glPopMatrix();


	glColor3f(1, 1, 1);
	point temp1 = { p1.x + small_circle_radius *v1.x, p1.y + small_circle_radius*v1.y };
	drawLine(p1, temp1);

	point temp2 = { p2.x + small_circle_radius*v2.x, p2.y + small_circle_radius*v2.y };
	drawLine(p2, temp2);

	glColor3f(1, 0, 0);
	point t1 = { temp1.x - tri_angle_side * v1.x, temp1.y - tri_angle_side * v1.y };
	vector v1_perp = { -v1.y, v1.x };
	point t2 = { t1.x + tri_angle_side * v1_perp.x, t1.y + tri_angle_side * v1_perp.y };
	point t3 = { t1.x - tri_angle_side * v1_perp.x, t1.y - tri_angle_side * v1_perp.y };
	drawTriangle(temp1, t2, t3);

	t1 = { temp2.x - tri_angle_side * v2.x, temp2.y - tri_angle_side * v2.y };
	vector v2_perp = { -v2.y, v2.x };
	t2 = { t1.x + tri_angle_side * v2_perp.x, t1.y + tri_angle_side * v2_perp.y };
	t3 = { t1.x - tri_angle_side * v2_perp.x, t1.y - tri_angle_side * v2_perp.y };
	drawTriangle(temp2, t2, t3);


	glColor3f(0, 1, 0);
	glTranslatef(p2.x, p2.y, 0);
	drawCircle(small_circle_radius, 95);


	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate() {
	p1.x += (speed*v1.x);
	p1.y += (speed*v1.y);
	
	p2.x += (speed*v2.x);
	p2.y += (speed*v2.y);

	//check if circles touches boundary
	point temp1 = { p1.x + small_circle_radius * v1.x , p1.y + small_circle_radius * v1.y };
	if (temp1.x*temp1.x + temp1.y*temp1.y >= circle_radius * circle_radius) {
		//reflect the direction
		vector c1 = { -temp1.x, -temp1.y };
		vector c1_unit = { c1.x / sqrt(c1.x*c1.x + c1.y*c1.y), c1.y / sqrt(c1.x*c1.x + c1.y*c1.y) };

		double temp_scaler = 2 * (v1.x*c1_unit.x + v1.y*c1_unit.y);
		v1.x = v1.x - temp_scaler * c1_unit.x;
		v1.y = v1.y - temp_scaler * c1_unit.y;
	}

	point temp2 = { p2.x + small_circle_radius * v2.x , p2.y + small_circle_radius * v2.y };
	if (temp2.x*temp2.x + temp2.y*temp2.y >= circle_radius * circle_radius) {
		//reflect the direction
		vector c2 = { -temp2.x, -temp2.y };
		vector c2_unit = { c2.x / sqrt(c2.x*c2.x + c2.y*c2.y), c2.y / sqrt(c2.x*c2.x + c2.y*c2.y) };

		double temp_scaler = 2 * (v2.x*c2_unit.x + v2.y*c2_unit.y);
		v2.x = v2.x - temp_scaler * c2_unit.x;
		v2.y = v2.y - temp_scaler * c2_unit.y;
	}


	//check if circles touches each other
	if (4*small_circle_radius*small_circle_radius >= (p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y)) {
		
		//reflect circle1
		vector n = { p1.x - p2.x, p1.y - p2.y };
		vector n_unit = { n.x / sqrt(n.x*n.x + n.y*n.y), n.y / sqrt(n.x*n.x + n.y*n.y) };

		double temp_scaler = 2 * (v1.x*n_unit.x + v1.y*n_unit.y);
		v1.x = v1.x - temp_scaler * n_unit.x;
		v1.y = v1.y - temp_scaler * n_unit.y;

		//reflect circle2
		n_unit = { -n_unit.x, -n_unit.y };

		temp_scaler = 2 * (v2.x*n_unit.x + v2.y*n_unit.y);
		v2.x = v2.x - temp_scaler * n_unit.x;
		v2.y = v2.y - temp_scaler * n_unit.y;
	}

	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init() {
	//codes for initialization

	p1 = { 80, 80};
	p2 = { -80, -80};
	v1 = { -0.7071, -0.7071 };
	v2 = { 0.7071, 0.7071 };

	//clear the screen
	glClearColor(0, 0, 0, 0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80, 1, 1, 1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(500, 500);
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
