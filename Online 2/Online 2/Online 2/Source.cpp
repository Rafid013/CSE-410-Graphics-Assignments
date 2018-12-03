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

const double delta = 1.0 / 1000.0;
const int move_point_n_max = 1000;


double current_t;

int current_start_point_idx = 0;
double current_delta_x, current_delta_sqr_x, current_delta_cube_x;
double current_delta_y, current_delta_sqr_y, current_delta_cube_y;
double current_ax, current_bx, current_cx, current_dx;
double current_ay, current_by, current_cy, current_dy;
int move_point_n = 0;

double prev_move_x, prev_move_y;
double move_x, move_y;
int selected_point_idx;

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
		glVertex3d(3, 3, 0);
		glVertex3d(3, -3, 0);
		glVertex3d(-3, -3, 0);
		glVertex3d(-3, 3, 0);
	}
	glEnd();
}

void drawCircle(double radius, int segments)
{
	int i;
	struct point2d points[100];
	//generate points
	for (double r = 0; r <= radius; r += 0.5) {
		for (i = 0; i <= segments; i++)
		{
			points[i].x = r * cos(((double)i / (double)segments) * 2 * pi);
			points[i].y = r * sin(((double)i / (double)segments) * 2 * pi);
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
}

void updateMovePointVars() {
	point2d p1, r1, p4, r4;
	int i = current_start_point_idx;
	if (i != cpidx - 2) {
		p1 = cp[i];
		r1 = { cp[i + 1].x - p1.x, cp[i + 1].y - p1.y };
		p4 = cp[i + 2];
		r4 = { cp[i + 3].x - p4.x, cp[i + 3].y - p4.y };
	}
	else {
		p1 = cp[i];
		r1 = { cp[i + 1].x - p1.x, cp[i + 1].y - p1.y };
		p4 = cp[0];
		r4 = { cp[1].x - p4.x, cp[1].y - p4.y };
	}

	current_ax = 2 * p1.x - 2 * p4.x + r1.x + r4.x;
	current_bx = -3 * p1.x + 3 * p4.x - 2 * r1.x - r4.x;
	current_cx = r1.x;
	current_dx = p1.x;

	current_ay = 2 * p1.y - 2 * p4.y + r1.y + r4.y;
	current_by = -3 * p1.y + 3 * p4.y - 2 * r1.y - r4.y;
	current_cy = r1.y;
	current_dy = p1.y;

	//current_delta_x = ((current_ax*delta + current_bx) * delta + current_cx)*delta;
	//current_delta_sqr_x = (6 * current_ax*delta + 2 * current_bx)*delta*delta;
	//current_delta_cube_x = 6 * current_ax*delta*delta*delta;

	//current_delta_y = ((current_ay*delta + current_by) * delta + current_cy)*delta;
	//current_delta_sqr_y = (6 * current_ay*delta + 2 * current_by)*delta*delta;
	//current_delta_cube_y = 6 * current_ay*delta*delta*delta;

	move_x = current_dx;
	move_y = current_dy;
}

void drawMovingPoint() {
	glPushMatrix();
	{
		double rad = atan2(move_y - prev_move_y, move_x - prev_move_x);
		glColor3f(0, 1, 1);
		glTranslatef(move_x, move_y, 0);
		glRotatef((rad * 180) / pi, 0, 0, 1);
		//drawCircle(6, 99);
		glBegin(GL_TRIANGLES); {
			glVertex3d(0, 0, 0);
			glVertex3d(-10, 6, 0);
			glVertex3d(-10, -6, 0);
		}
		glEnd();
	}
	glPopMatrix();
}

void drawSelectedPoint() {
	glPushMatrix();
	{
		glColor3f(0, 1, 1);
		glTranslatef(cp[selected_point_idx].x, cp[selected_point_idx].y, 0);
		drawCircle(6, 99);
	}
	glPopMatrix();
}

void drawPoints_Vectors() {
	for (int i = 0; i < cpidx; ++i)
	{
		if (i % 2 == 0)
			glColor3f(0, 1, 0);
		else
			glColor3f(1, 1, 0);
		glPushMatrix();
		{
			glTranslatef(cp[i].x, cp[i].y, 0);
			drawSquare();
		}
		glPopMatrix();

		if (i % 2 == 0 && i != cpidx - 1) {
			double vx = cp[i + 1].x - cp[i].x;
			double vy = cp[i + 1].y - cp[i].y;

			double x_temp = cp[i].x + 0.8*vx;
			double y_temp = cp[i].y + 0.8*vy;

			double triangle_height = sqrt((cp[i + 1].x - x_temp)*(cp[i + 1].x - x_temp)
				+ (cp[i + 1].y - y_temp)*(cp[i + 1].y - y_temp));

			/*double vx_perp = -vy;
			double vy_perp = vx;

			vx_perp /= sqrt(vx_perp*vx_perp + vy_perp * vy_perp);
			vy_perp /= sqrt(vx_perp*vx_perp + vy_perp * vy_perp);

			double point1_x = x_temp + 4*vx_perp;
			double point1_y = y_temp + 4*vy_perp;

			double point2_x = x_temp - 4*vx_perp;
			double point2_y = y_temp - 4*vy_perp;*/

			double rotate_angle = acos(vy / sqrt(vx * vx + vy * vy));
			rotate_angle *= (180 / pi);

			if (vx >= 0) rotate_angle = -rotate_angle;

			glPushMatrix(); {
				glColor3f(1, 0, 0);
				glTranslatef(x_temp, y_temp, 0);
				glRotatef(rotate_angle, 0, 0, 1);
				glBegin(GL_TRIANGLES); {
					glVertex3d(0, triangle_height, 0);
					glVertex3d(6, 0, 0);
					glVertex3d(-6, 0, 0);
				}
				glEnd();
			} glPopMatrix();

			glColor3f(1, 1, 1);
			glLineWidth(1);
			glBegin(GL_LINES); {
				glVertex3d(cp[i + 1].x, cp[i + 1].y, 0);
				glVertex3d(cp[i].x, cp[i].y, 0);
			}
			glEnd();
		}
	}
}

void drawCurve() {
	for (int i = 0; i < cpidx; i += 2) {
		point2d p1, r1, p4, r4;
		if (i != cpidx - 2) {
			p1 = cp[i];
			r1 = { cp[i + 1].x - p1.x, cp[i + 1].y - p1.y };
			p4 = cp[i + 2];
			r4 = { cp[i + 3].x - p4.x, cp[i + 3].y - p4.y };
		}
		else {
			p1 = cp[i];
			r1 = { cp[i + 1].x - p1.x, cp[i + 1].y - p1.y };
			p4 = cp[0];
			r4 = { cp[1].x - p4.x, cp[1].y - p4.y };
		}

		double ax = 2 * p1.x - 2 * p4.x + r1.x + r4.x;
		double bx = -3 * p1.x + 3 * p4.x - 2 * r1.x - r4.x;
		double cx = r1.x;
		double dx = p1.x;

		double ay = 2 * p1.y - 2 * p4.y + r1.y + r4.y;
		double by = -3 * p1.y + 3 * p4.y - 2 * r1.y - r4.y;
		double cy = r1.y;
		double dy = p1.y;

		double x = dx;
		double delta_x = ((ax*delta + bx) * delta + cx)*delta;
		double delta_sqr_x = (6 * ax*delta + 2 * bx)*delta*delta;
		double delta_cube_x = 6 * ax*delta*delta*delta;

		double y = dy;
		double delta_y = ((ay*delta + by) * delta + cy)*delta;
		double delta_sqr_y = (6 * ay*delta + 2 * by)*delta*delta;
		double delta_cube_y = 6 * ay*delta*delta*delta;

		double *x_list = new double[move_point_n_max + 1];
		double *y_list = new double[move_point_n_max + 1];

		x_list[0] = x;
		y_list[0] = y;

		for (int j = 1; j <= move_point_n_max; ++j) {
			x += delta_x;
			delta_x += delta_sqr_x;
			delta_sqr_x += delta_cube_x;

			y += delta_y;
			delta_y += delta_sqr_y;
			delta_sqr_y += delta_cube_y;

			x_list[j] = x;
			y_list[j] = y;
		}
		glColor3d(1, 1, 1);
		for (int j = 0; j < move_point_n_max; ++j) {
			glLineWidth(2);
			glBegin(GL_LINES); {
				glVertex3d(x_list[j], y_list[j], 0);
				glVertex3d(x_list[j + 1], y_list[j + 1], 0);
			}
			glEnd();
		}
		delete x_list;
		delete y_list;
	}
}

void keyboardListener(unsigned char key, int x, int y) {
	switch (key) {

	case 'g':
		show_vectors = !show_vectors;
		break;
	case 'a':
		if (program_state == 2) {
			program_state = 4;
			current_start_point_idx = 0;
			move_point_n = 0;
			updateMovePointVars();
		}
		else if (program_state == 4) program_state = 2;
		break;
	case 'u':
		if (program_state == 4 || program_state == 2) {
			program_state = 3;
		}
		break;
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


void mouseListener(int button, int state, int x, int y) {	//x, y is the x-y of the screen (2D)
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {		// 2 times?? in ONE click? -- solution is checking DOWN or UP
			if (program_state == 1) {
				std::cout << "Clicked point (" << x << ", " << y << ")" << std::endl;

				cp[cpidx].x = (double)x;
				cp[cpidx].y = (double)(600 - y);
				cpidx++;
			}

			else if (program_state == 3) {
				double x_click = (double)x;
				double y_click = (double)(600 - y);

				double minDistance = 10000;
				for (int i = 0; i < cpidx; ++i) {
					double distance = (cp[i].x - x_click)*(cp[i].x - x_click) + (cp[i].y - y_click)*(cp[i].y - y_click);
					distance = sqrt(distance);
					if (distance < minDistance) {
						minDistance = distance;
						selected_point_idx = i;
					}
				}

				program_state = 5;
			}

			else if (program_state == 5) {
				double x_click = (double)x;
				double y_click = (double)(600 - y);

				cp[selected_point_idx] = { x_click, y_click };

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
	//gluLookAt(150*cos(cameraAngle), 150*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	if (show_vectors)
		drawPoints_Vectors();

	if (program_state != 1)
		drawCurve();

	if (program_state == 4)
		drawMovingPoint();

	if (program_state == 5)
		drawSelectedPoint();

	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate() {

	if (program_state == 4) {
		++move_point_n;

		prev_move_x = move_x;
		prev_move_y = move_y;

		double move_x_bar = (3 * current_ax*current_t + 2 * current_bx)*current_t + current_cx;
		double move_y_bar = (3 * current_ay*current_t + 2 * current_by)*current_t + current_cy;

		double velo = sqrt(move_x_bar*move_x_bar + move_y_bar * move_y_bar);

		current_t += 1 / velo;

		move_x = (current_ax*current_t*current_t + current_bx * current_t + current_cx)*current_t + current_dx;
		move_y = (current_ay*current_t*current_t + current_by * current_t + current_cy)*current_t + current_dy;

		if (current_t > 1) {
			current_t = 0;
			current_start_point_idx += 2;
			if (current_start_point_idx == cpidx) current_start_point_idx = 0;
			updateMovePointVars();
		}
	}

	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init() {
	//codes for initialization

	cpidx = 0;

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
	gluOrtho2D(0, 800, 0, 600);
	//gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
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
