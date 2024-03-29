#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<iostream>
using namespace std;
#include <windows.h>
#include <glut.h>

const double pi = (2 * acos(0.0));
const double rotate_angle = pi / 90;
const double rotate_limit = pi / 4;
const double translation_speed = 3;
const double wall_distance = -1000;
const double wall_side = 600;

double barrel_angle_y = 0;
double barrel_angle_z = 0;
double main_part_angle_z = 0;
double main_part_tilt_angle = 0;

int drawgrid;
int drawaxes;
const double axis_len = 1500;

struct point
{
	double x, y, z;
};


struct vector {
	double x, y, z;
};

point cameraPosition;
vector l, u, r;

point gun_point;
vector gun_l, gun_u, gun_r;
vector main_gun_l, main_gun_u, main_gun_r;

point points_on_wall[100];
int points_on_wall_size = 0;

void drawAxes()
{
	if (drawaxes == 1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES); {
			glVertex3f(axis_len, 0, 0);
			glVertex3f(-axis_len, 0, 0);

			glVertex3f(0, -axis_len, 0);
			glVertex3f(0, axis_len, 0);

			glVertex3f(0, 0, axis_len);
			glVertex3f(0, 0, -axis_len);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if (drawgrid == 1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES); {
			for (i = -8; i <= 8; i++) {

				if (i == 0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i * 10, -90, 0);
				glVertex3f(i * 10, 90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i * 10, 0);
				glVertex3f(90, i * 10, 0);
			}
		}glEnd();
	}
}


void move_camera_forward() {
	cameraPosition.x += (translation_speed*l.x);
	cameraPosition.y += (translation_speed*l.y);
	cameraPosition.z += (translation_speed*l.z);
}

void move_camera_backward() {
	cameraPosition.x -= (translation_speed*l.x);
	cameraPosition.y -= (translation_speed*l.y);
	cameraPosition.z -= (translation_speed*l.z);
}

void move_camera_right() {
	cameraPosition.x += (translation_speed*r.x);
	cameraPosition.y += (translation_speed*r.y);
	cameraPosition.z += (translation_speed*r.z);
}

void move_camera_left() {
	cameraPosition.x -= (translation_speed*r.x);
	cameraPosition.y -= (translation_speed*r.y);
	cameraPosition.z -= (translation_speed*r.z);
}

void move_camera_up() {
	cameraPosition.x += (translation_speed*u.x);
	cameraPosition.y += (translation_speed*u.y);
	cameraPosition.z += (translation_speed*u.z);
}

void move_camera_down() {
	cameraPosition.x -= (translation_speed*u.x);
	cameraPosition.y -= (translation_speed*u.y);
	cameraPosition.z -= (translation_speed*u.z);
}


void rotate_camera_left() {
	double cos_theta = cos(rotate_angle);
	double sin_theta = sin(rotate_angle);

	l.x = l.x*cos_theta - r.x*sin_theta;
	l.y = l.y*cos_theta - r.y*sin_theta;
	l.z = l.z*cos_theta - r.z*sin_theta;

	r.x = l.y*u.z - l.z*u.y;
	r.y = l.z*u.x - l.x*u.z;
	r.z = l.x*u.y - l.y*u.x;
}

void rotate_camera_right() {
	double cos_theta = cos(-rotate_angle);
	double sin_theta = sin(-rotate_angle);

	l.x = l.x*cos_theta - r.x*sin_theta;
	l.y = l.y*cos_theta - r.y*sin_theta;
	l.z = l.z*cos_theta - r.z*sin_theta;

	r.x = l.y*u.z - l.z*u.y;
	r.y = l.z*u.x - l.x*u.z;
	r.z = l.x*u.y - l.y*u.x;
}

void rotate_camera_up() {
	double cos_theta = cos(rotate_angle);
	double sin_theta = sin(rotate_angle);

	l.x = l.x*cos_theta + u.x*sin_theta;
	l.y = l.y*cos_theta + u.y*sin_theta;
	l.z = l.z*cos_theta + u.z*sin_theta;

	u.x = r.y*l.z - r.z*l.y;
	u.y = r.z*l.x - r.x*l.z;
	u.z = r.x*l.y - r.y*l.x;
}

void rotate_camera_down() {
	double cos_theta = cos(-rotate_angle);
	double sin_theta = sin(-rotate_angle);

	l.x = l.x*cos_theta + u.x*sin_theta;
	l.y = l.y*cos_theta + u.y*sin_theta;
	l.z = l.z*cos_theta + u.z*sin_theta;

	u.x = r.y*l.z - r.z*l.y;
	u.y = r.z*l.x - r.x*l.z;
	u.z = r.x*l.y - r.y*l.x;
}

void tilt_camera_clockwise() {
	double cos_theta = cos(-rotate_angle);
	double sin_theta = sin(-rotate_angle);

	u.x = u.x*cos_theta - r.x*sin_theta;
	u.y = u.y*cos_theta - r.y*sin_theta;
	u.z = u.z*cos_theta - r.z*sin_theta;

	r.x = l.y*u.z - l.z*u.y;
	r.y = l.z*u.x - l.x*u.z;
	r.z = l.x*u.y - l.y*u.x;
}

void tilt_camera_counterclockwise() {
	double cos_theta = cos(rotate_angle);
	double sin_theta = sin(rotate_angle);

	u.x = u.x*cos_theta - r.x*sin_theta;
	u.y = u.y*cos_theta - r.y*sin_theta;
	u.z = u.z*cos_theta - r.z*sin_theta;

	r.x = l.y*u.z - l.z*u.y;
	r.y = l.z*u.x - l.x*u.z;
	r.z = l.x*u.y - l.y*u.x;
}


void drawHalfSphere(double radius, int slices, int stacks)
{
	struct point points[100][100];
	int i, j;
	double h, r;
	//generate points
	for (i = 0; i <= stacks; i++)
	{
		h = radius * sin(((double)i / (double)stacks)*(pi / 2));
		r = radius * cos(((double)i / (double)stacks)*(pi / 2));
		for (j = 0; j <= slices; j++)
		{
			points[i][j].x = r * cos(((double)j / (double)slices) * 2 * pi);
			points[i][j].y = h;
			points[i][j].z = r * sin(((double)j / (double)slices) * 2 * pi);
		}
	}

	//draw quads using generated points
	for (i = 0; i < stacks; i++)
	{
		int white = 0;
		for (j = 0; j < slices; j++)
		{
			glBegin(GL_QUADS); {
				if (white < 3) {
					glColor3f(1, 1, 1);
					++white;
				}
				else {
					glColor3f(0, 0, 0);
					++white;
					if (white == 6) white = 0;
				}
				glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
				glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
				glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
				glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
			}glEnd();
		}
	}
}

void drawOutwardsShape(double lowest_radius, double highest_radius, int segments, int stacks, double len) {
	struct point points[100][100];
	double radius = highest_radius;
	double l = 0;
	for (int i = 0; i <= stacks; ++i) {

		for (int j = 0; j <= segments; ++j) {
			points[i][j].x = radius * cos(((double)j / (double)segments) * 2 * pi);
			points[i][j].y = l;
			points[i][j].z = radius * sin(((double)j / (double)segments) * 2 * pi);
		}
		if (i < stacks / 4)
			radius -= (radius * 4) / stacks;
		else
			radius -= (radius*0.25) / stacks;
		if (radius <= lowest_radius) radius = lowest_radius;
		l += (len / stacks);
	}

	for (int i = 0; i < stacks; i++)
	{
		int white = 0;
		for (int j = 0; j < segments; j++)
		{
			glBegin(GL_QUADS); {
				if (white < 3) {
					glColor3f(1, 1, 1);
					++white;
				}
				else {
					glColor3f(0, 0, 0);
					++white;
					if (white == 6) white = 0;
				}
				glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
				glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
				glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
				glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
			}glEnd();
		}
	}
}

void drawCylinder(double radius, int segments, double length)
{
	int i;
	struct point points[100];

	//generate points
	for (i = 0; i <= segments; i++)
	{
		points[i].x = radius * cos(((double)i / (double)segments) * 2 * pi);
		points[i].z = radius * sin(((double)i / (double)segments) * 2 * pi);
	}

	
	//draw segments using generated points
	int white = 0;
	for (i = 0; i < segments; i++)
	{
		glBegin(GL_QUADS);
		{
			if (white < 3) {
				glColor3f(1, 1, 1);
				++white;
			}
			else {
				glColor3f(0, 0, 0);
				++white;
				if (white == 6) white = 0;
			}
			glVertex3f(points[i].x, 0, points[i].z);
			glVertex3f(points[i + 1].x, 0, points[i + 1].z);
			glVertex3f(points[i + 1].x, length, points[i + 1].z);
			glVertex3f(points[i].x, length, points[i].z);
		}
		glEnd();
	}
}

void rotate_full_gun_left() {
	double cos_theta = cos(rotate_angle);
	double sin_theta = sin(rotate_angle);

	gun_l.x = gun_l.x*cos_theta - gun_r.x*sin_theta;
	gun_l.y = gun_l.y*cos_theta - gun_r.y*sin_theta;
	gun_l.z = gun_l.z*cos_theta - gun_r.z*sin_theta;

	gun_r.x = gun_l.y*gun_u.z - gun_l.z*gun_u.y;
	gun_r.y = gun_l.z*gun_u.x - gun_l.x*gun_u.z;
	gun_r.z = gun_l.x*gun_u.y - gun_l.y*gun_u.x;

	main_gun_l.x = main_gun_l.x*cos_theta - main_gun_r.x*sin_theta;
	main_gun_l.y = main_gun_l.y*cos_theta - main_gun_r.y*sin_theta;
	main_gun_l.z = main_gun_l.z*cos_theta - main_gun_r.z*sin_theta;

	main_gun_r.x = main_gun_l.y*main_gun_u.z - main_gun_l.z*main_gun_u.y;
	main_gun_r.y = main_gun_l.z*main_gun_u.x - main_gun_l.x*main_gun_u.z;
	main_gun_r.z = main_gun_l.x*main_gun_u.y - main_gun_l.y*main_gun_u.x;
}

void rotate_full_gun_right() {
	double cos_theta = cos(-rotate_angle);
	double sin_theta = sin(-rotate_angle);

	gun_l.x = gun_l.x*cos_theta - gun_r.x*sin_theta;
	gun_l.y = gun_l.y*cos_theta - gun_r.y*sin_theta;
	gun_l.z = gun_l.z*cos_theta - gun_r.z*sin_theta;

	gun_r.x = gun_l.y*gun_u.z - gun_l.z*gun_u.y;
	gun_r.y = gun_l.z*gun_u.x - gun_l.x*gun_u.z;
	gun_r.z = gun_l.x*gun_u.y - gun_l.y*gun_u.x;

	main_gun_l.x = main_gun_l.x*cos_theta - main_gun_r.x*sin_theta;
	main_gun_l.y = main_gun_l.y*cos_theta - main_gun_r.y*sin_theta;
	main_gun_l.z = main_gun_l.z*cos_theta - main_gun_r.z*sin_theta;

	main_gun_r.x = main_gun_l.y*main_gun_u.z - main_gun_l.z*main_gun_u.y;
	main_gun_r.y = main_gun_l.z*main_gun_u.x - main_gun_l.x*main_gun_u.z;
	main_gun_r.z = main_gun_l.x*main_gun_u.y - main_gun_l.y*main_gun_u.x;
}

void rotate_full_gun_up() {
	double cos_theta = cos(rotate_angle);
	double sin_theta = sin(rotate_angle);

	gun_l.x = gun_l.x*cos_theta + gun_u.x*sin_theta;
	gun_l.y = gun_l.y*cos_theta + gun_u.y*sin_theta;
	gun_l.z = gun_l.z*cos_theta + gun_u.z*sin_theta;

	gun_u.x = gun_r.y*gun_l.z - gun_r.z*gun_l.y;
	gun_u.y = gun_r.z*gun_l.x - gun_r.x*gun_l.z;
	gun_u.z = gun_r.x*gun_l.y - gun_r.y*gun_l.x;

	main_gun_l.x = main_gun_l.x*cos_theta + main_gun_u.x*sin_theta;
	main_gun_l.y = main_gun_l.y*cos_theta + main_gun_u.y*sin_theta;
	main_gun_l.z = main_gun_l.z*cos_theta + main_gun_u.z*sin_theta;

	main_gun_u.x = main_gun_r.y*main_gun_l.z - main_gun_r.z*main_gun_l.y;
	main_gun_u.y = main_gun_r.z*main_gun_l.x - main_gun_r.x*main_gun_l.z;
	main_gun_u.z = main_gun_r.x*main_gun_l.y - main_gun_r.y*main_gun_l.x;
}

void rotate_full_gun_down() {
	double cos_theta = cos(-rotate_angle);
	double sin_theta = sin(-rotate_angle);

	gun_l.x = gun_l.x*cos_theta + gun_u.x*sin_theta;
	gun_l.y = gun_l.y*cos_theta + gun_u.y*sin_theta;
	gun_l.z = gun_l.z*cos_theta + gun_u.z*sin_theta;

	gun_u.x = gun_r.y*gun_l.z - gun_r.z*gun_l.y;
	gun_u.y = gun_r.z*gun_l.x - gun_r.x*gun_l.z;
	gun_u.z = gun_r.x*gun_l.y - gun_r.y*gun_l.x;

	main_gun_l.x = main_gun_l.x*cos_theta + main_gun_u.x*sin_theta;
	main_gun_l.y = main_gun_l.y*cos_theta + main_gun_u.y*sin_theta;
	main_gun_l.z = main_gun_l.z*cos_theta + main_gun_u.z*sin_theta;

	main_gun_u.x = main_gun_r.y*main_gun_l.z - main_gun_r.z*main_gun_l.y;
	main_gun_u.y = main_gun_r.z*main_gun_l.x - main_gun_r.x*main_gun_l.z;
	main_gun_u.z = main_gun_r.x*main_gun_l.y - main_gun_r.y*main_gun_l.x;
}

void rotate_main_gun_up() {
	double cos_theta = cos(rotate_angle);
	double sin_theta = sin(rotate_angle);

	main_gun_l.x = main_gun_l.x*cos_theta + main_gun_u.x*sin_theta;
	main_gun_l.y = main_gun_l.y*cos_theta + main_gun_u.y*sin_theta;
	main_gun_l.z = main_gun_l.z*cos_theta + main_gun_u.z*sin_theta;

	main_gun_u.x = main_gun_r.y*main_gun_l.z - main_gun_r.z*main_gun_l.y;
	main_gun_u.y = main_gun_r.z*main_gun_l.x - main_gun_r.x*main_gun_l.z;
	main_gun_u.z = main_gun_r.x*main_gun_l.y - main_gun_r.y*main_gun_l.x;
}

void rotate_main_gun_down() {
	double cos_theta = cos(-rotate_angle);
	double sin_theta = sin(-rotate_angle);

	main_gun_l.x = main_gun_l.x*cos_theta + main_gun_u.x*sin_theta;
	main_gun_l.y = main_gun_l.y*cos_theta + main_gun_u.y*sin_theta;
	main_gun_l.z = main_gun_l.z*cos_theta + main_gun_u.z*sin_theta;

	main_gun_u.x = main_gun_r.y*main_gun_l.z - main_gun_r.z*main_gun_l.y;
	main_gun_u.y = main_gun_r.z*main_gun_l.x - main_gun_r.x*main_gun_l.z;
	main_gun_u.z = main_gun_r.x*main_gun_l.y - main_gun_r.y*main_gun_l.x;
}

void modify_gun_point() {
	double temp = gun_point.x*gun_l.x + gun_point.y*gun_l.y + gun_point.z*gun_l.z;
	gun_point.x = temp * gun_l.x;
	gun_point.y = temp * gun_l.y;
	gun_point.z = temp * gun_l.z;
}

void draw_gun_shot() {
	for (int i = 0; i < points_on_wall_size; ++i) {
		glColor3f(1, 0, 0);
		glBegin(GL_QUADS);
		{
			glVertex3f(points_on_wall[i].x + 10, wall_distance + 50, points_on_wall[i].z + 10);
			glVertex3f(points_on_wall[i].x - 10, wall_distance + 50, points_on_wall[i].z + 10);
			glVertex3f(points_on_wall[i].x - 10, wall_distance + 50, points_on_wall[i].z - 10);
			glVertex3f(points_on_wall[i].x + 10, wall_distance + 50, points_on_wall[i].z - 10);
		}
		glEnd();
	}
}

void shoot_gun() {
	double t = (-gun_point.y + wall_distance + 50) / main_gun_l.y;

	point point_on_wall = { gun_point.x + t * main_gun_l.x, gun_point.y + t * main_gun_l.y, gun_point.z + t * main_gun_l.z };
	if (point_on_wall.x < wall_side / 2 && point_on_wall.x > -wall_side / 2
		&& point_on_wall.z < wall_side / 2 && point_on_wall.z > -wall_side / 2)
		points_on_wall[points_on_wall_size++] = point_on_wall;
}


void keyboardListener(unsigned char key, int x, int y) {
	switch (key) {

	case '1':
		rotate_camera_left();
		break;
	case '2':
		rotate_camera_right();
		break;
	case '3':
		rotate_camera_up();
		break;
	case '4':
		rotate_camera_down();
		break;
	case '5':
		tilt_camera_clockwise();
		break;
	case '6':
		tilt_camera_counterclockwise();
		break;
	case 'q':
		if(barrel_angle_y < rotate_limit)
			barrel_angle_y += rotate_angle;
		rotate_full_gun_left();
		modify_gun_point();
		break;
	case 'w':
		if(barrel_angle_y > -rotate_limit)
			barrel_angle_y -= rotate_angle;
		rotate_full_gun_right();
		modify_gun_point();
		break;
	case 'e':
		if (barrel_angle_z > -rotate_limit)
			barrel_angle_z -= rotate_angle;
		rotate_full_gun_up();
		modify_gun_point();
		break;
	case 'r':
		if (barrel_angle_z < rotate_limit)
			barrel_angle_z += rotate_angle;
		rotate_full_gun_down();
		modify_gun_point();
		break;
	case 'a':
		if(main_part_angle_z < rotate_limit)
			main_part_angle_z += rotate_angle;
		rotate_main_gun_up();
		break;
	case 's':
		if (main_part_angle_z > -rotate_limit)
			main_part_angle_z -= rotate_angle;
		rotate_main_gun_down();
		break;
	case 'd':
		if (main_part_tilt_angle < rotate_limit)
			main_part_tilt_angle += rotate_angle;
		break;
	case 'f':
		if (main_part_tilt_angle > -rotate_limit)
			main_part_tilt_angle -= rotate_angle;
		break;
	default:
		break;
	}
}


void specialKeyListener(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_DOWN:		//down arrow key
		move_camera_backward();
		break;
	case GLUT_KEY_UP:		// up arrow key
		move_camera_forward();
		break;

	case GLUT_KEY_RIGHT:
		move_camera_right();
		break;
	case GLUT_KEY_LEFT:
		move_camera_left();
		break;

	case GLUT_KEY_PAGE_UP:
		move_camera_up();
		break;
	case GLUT_KEY_PAGE_DOWN:
		move_camera_down();
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
		if(state == GLUT_DOWN)
			shoot_gun();
		break;

	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN) {		// 2 times?? in ONE click? -- solution is checking DOWN or UP
			drawaxes = 1 - drawaxes;
		}
		break;

	case GLUT_MIDDLE_BUTTON:
		//........
		break;

	default:
		break;
	}
}

void drawWall() {
	glColor3f(0.7, 0.7, 0.7);
	glBegin(GL_QUADS);
	{
		glVertex3f(wall_side/2, wall_distance, wall_side / 2);
		glVertex3f(-wall_side / 2, wall_distance, wall_side / 2);
		glVertex3f(-wall_side / 2, wall_distance, -wall_side / 2);
		glVertex3f(wall_side / 2, wall_distance, -wall_side / 2);
	}
	glEnd();
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
	gluLookAt(cameraPosition.x, cameraPosition.y, cameraPosition.z,
		cameraPosition.x + l.x, cameraPosition.y + l.y, cameraPosition.z + l.z,
		u.x, u.y, u.z);

	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();
	drawWall();
	draw_gun_shot();

	glRotatef((barrel_angle_y * 180) / pi, 0, 0, 1);
	glRotatef((barrel_angle_z * 180) / pi, 1, 0, 0);

	//draw first half sphere
	glTranslatef(0, -10, 0);
	drawHalfSphere(10, 90, 90);

	//draw cylinder between spheres
	glTranslatef(0, -40, 0);
	drawCylinder(10, 90, 40);

	//draw second half sphere
	glRotatef(180, 0, 0, 1);
	drawHalfSphere(10, 90, 90);


	glRotatef((main_part_angle_z * 180) / pi, 1, 0, 0);
	glRotatef((main_part_tilt_angle * 180) / pi, 0, 1, 0);
	//draw third half sphere
	glRotatef(180, 0, 0, 1);
	glTranslatef(0, -16, 0);
	drawHalfSphere(6, 90, 90);

	//draw final cylinder
	glTranslatef(0, -80, 0);
	drawCylinder(6, 90, 80);

	//draw outwards shape thingy
	drawOutwardsShape(6, 10, 90, 90, 15);


	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate() {
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init() {
	//codes for initialization
	drawgrid = 0;
	drawaxes = 1;
	cameraPosition = { 100, 100, 0 };
	u = { 0, 0, 1 };
	r = { -0.7071, 0.7071, 0 };
	l = { -0.7071, -0.7071, 0 };

	gun_point = { 0, -60, 0 };
	main_gun_l = gun_l = { 0, -1, 0 };
	main_gun_r = gun_r = { -1, 0, 0 };
	main_gun_u = gun_u = { 0, 0, 1 };


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
	gluPerspective(80, 1, 1, 1500.0);
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
