#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<iostream>
#include<fstream>
#include "bitmap_image.hpp"
using namespace std;
#include <windows.h>
#include <glut.h>
#include <vector>
#include<string>



const double pi = (2 * acos(0.0));
const double rotate_angle = pi / 90;
const double translation_speed = 3;

bool textureEnabled = false;
int textureHeight, textureWidth;



int drawgrid;
int drawaxes;
const double axis_len = 1500;

double near_plane_distance, far_plane_distance, fovY, aspectRatio;
int level_of_recursion, num_of_pixels;
double cell_width;
double checkerboard_ambient, checkerboard_diffuse, checkerboard_reflection;
int num_of_objects;
int num_of_normal_source;
int num_of_spot_source;


struct normal_light_source {
	double normal_source_x, normal_source_y, normal_source_z, normal_source_falloff;
};

struct spot_light_source {
	double spot_source_x, spot_source_y, spot_source_z, spot_source_falloff;
	double spot_look_x, spot_look_y, spot_look_z;
	double spot_cutoff_angle;
};

struct sphere {
	string obj_type = "sphere";
	double center_x, center_y, center_z;
	double radius;
	double color_r, color_g, color_b;
	double ambient, diffuse, specular, reflection;
	double shininess;
};

struct pyramid {
	string obj_type = "pyramid";
	double low_x, low_y, low_z;
	double width, height;
	double color_r, color_g, color_b;
	double ambient, diffuse, specular, reflection;
	double shininess;
};

vector<sphere> spheres;
vector<pyramid> pyramids;
vector<normal_light_source> normal_sources;
vector<spot_light_source> spot_sources;


struct point
{
	double x, y, z;
};


struct vector_ {
	double x, y, z;

};


struct color {
	double r, g, b;
};

color **textureBuffer;

vector_ cross(vector_ a, vector_ b)
{
	vector_ v = { a.y*b.z - a.z*b.y, b.x*a.z - b.z*a.x, a.x*b.y - a.y*b.x };
	return v;
}
vector_ scalar_mul(vector_ a, double b) {
	vector_ v = { a.x*b, a.y*b, a.z*b };
	return v;
}
vector_ sub(vector_ a, vector_ b) {
	vector_ v = { a.x - b.x,a.y - b.y,a.z - b.z };
	return v;

}
vector_ normalize(vector_ a)
{
	double x = a.x;
	double y = a.y;
	double z = a.z;
	double r = sqrt(x*x + y * y + z * z);
	x = x / r;
	y = y / r;
	z = z / r;
	vector_ v = { x, y, z };
	return v;
}

point cameraPosition;
vector_ l, u, r;


point **pointBuffer;
color **colors;


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


void drawSphere(double radius, int slices, int stacks)
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
			points[i][j].y = r * sin(((double)j / (double)slices) * 2 * pi);
			points[i][j].z = h;
		}
	}
	//draw quads using generated points
	for (i = 0; i < stacks; i++)
	{
		//glColor3f((double)i / (double)stacks, (double)i / (double)stacks, (double)i / (double)stacks);
		for (j = 0; j < slices; j++)
		{
			glBegin(GL_QUADS); {
				//upper hemisphere
				glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
				glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
				glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
				glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
				//lower hemisphere
				glVertex3f(points[i][j].x, points[i][j].y, -points[i][j].z);
				glVertex3f(points[i][j + 1].x, points[i][j + 1].y, -points[i][j + 1].z);
				glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, -points[i + 1][j + 1].z);
				glVertex3f(points[i + 1][j].x, points[i + 1][j].y, -points[i + 1][j].z);
			}glEnd();
		}
	}
}


void drawPyramid(double low_x, double low_y, double low_z, double width, double height) {
	double high_x = low_x, high_y = low_y, high_z = low_z + height;
	double p1_x = low_x + width / 2, p1_y = low_y + width / 2, p1_z = low_z;
	double p2_x = low_x + width / 2, p2_y = low_y - width / 2, p2_z = low_z;
	double p3_x = low_x - width / 2, p3_y = low_y - width / 2, p3_z = low_z;
	double p4_x = low_x - width / 2, p4_y = low_y + width / 2, p4_z = low_z;

	glBegin(GL_TRIANGLES); {
		glVertex3f(high_x, high_y, high_z);
		glVertex3f(p1_x, p1_y, p1_z);
		glVertex3f(p2_x, p2_y, p2_z);

		glVertex3f(high_x, high_y, high_z);
		glVertex3f(p2_x, p2_y, p2_z);
		glVertex3f(p3_x, p3_y, p3_z);

		glVertex3f(high_x, high_y, high_z);
		glVertex3f(p3_x, p3_y, p3_z);
		glVertex3f(p4_x, p4_y, p4_z);

		glVertex3f(high_x, high_y, high_z);
		glVertex3f(p4_x, p4_y, p4_z);
		glVertex3f(p1_x, p1_y, p1_z);
	}glEnd();

	glBegin(GL_QUADS); {
		glVertex3f(p1_x, p1_y, p1_z);
		glVertex3f(p2_x, p2_y, p2_z);
		glVertex3f(p3_x, p3_y, p3_z);
		glVertex3f(p4_x, p4_y, p4_z);
	}glEnd();
}
class A {
public:
	point P;
	vector_ dir_temp;
	vector_ normal;
	color clr;
	double ambient;
	double specular;
	double diffuse;
	double reflectance;
	double shininess;
	bool hasIntersected;
	point start;
	A(point P, point start, vector_ dir_temp, vector_ normal, color clr,
		double ambient, double specular, double diffuse, double reflectance, double shininess, bool hasIntersected) {
		this->P = P;
		this->start = start;
		this->dir_temp = dir_temp;
		this->normal = normal;
		this->clr = clr;
		this->ambient = ambient;
		this->specular = specular;
		this->diffuse = diffuse;
		this->reflectance = reflectance;
		this->shininess = shininess;
		this->hasIntersected = hasIntersected;
	}


};

A colorPoints(point, vector_);

double dot(vector_ a, vector_ b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

A recursiveColoring(A classA, int depthOfRecursion) {
	if (depthOfRecursion == 0) {
		A temp(classA.P, classA.start, classA.dir_temp, classA.normal, classA.clr,
			classA.ambient, classA.specular, classA.diffuse, classA.reflectance, classA.shininess, classA.hasIntersected);
		temp.clr = { 0, 0, 0 };
		return temp;
	}
	double lambert = 0.0;
	double phong = 0.0;
	for (int i = 0; i < num_of_normal_source; ++i) {
		normal_light_source nls = normal_sources[i];
		vector_ toSource = { nls.normal_source_x - classA.P.x, nls.normal_source_y - classA.P.y,
			nls.normal_source_z - classA.P.z };
		toSource = normalize(toSource);
		point temp_start = { classA.P.x + toSource.x*0.01, classA.P.y + toSource.y*0.01, classA.P.z + toSource.z*0.01 };
		A temp = colorPoints(temp_start, toSource);
		if (temp.hasIntersected) {
			continue;
		}
		vector_ N = normalize(classA.normal);
		double distance = (nls.normal_source_x - classA.P.x)*(nls.normal_source_x - classA.P.x)
			+ (nls.normal_source_y - classA.P.y)*(nls.normal_source_y - classA.P.y)
			+ (nls.normal_source_z - classA.P.z)*(nls.normal_source_z - classA.P.z);
		distance = sqrt(distance);
		double scaling_factor = exp(-distance * distance*nls.normal_source_falloff);
		lambert += max(0.0, dot(toSource, N)*scaling_factor);
		vector_ R_prime = sub(toSource, scalar_mul(N, dot(toSource, N) * 2));
		R_prime = normalize(R_prime);
		vector_ V = { -classA.dir_temp.x, -classA.dir_temp.y, -classA.dir_temp.z };
		V = normalize(V);
		phong += max(pow(dot(R_prime, V), classA.shininess)*scaling_factor, 0.0);
	}
	for (int i = 0; i < num_of_spot_source; ++i) {
		spot_light_source sls = spot_sources[i];
		vector_ toSource = { sls.spot_source_x - classA.P.x, sls.spot_source_y - classA.P.y,
			sls.spot_source_z - classA.P.z };
		toSource = normalize(toSource);
		vector_ V1 = { classA.P.x - sls.spot_source_x, classA.P.y - sls.spot_source_y, classA.P.z - sls.spot_source_z };
		V1 = normalize(V1);
		vector_ V2 = { sls.spot_look_x - sls.spot_source_x, sls.spot_look_y - sls.spot_source_y, sls.spot_look_z - sls.spot_source_z };
		V2 = normalize(V2);
		double angle = (acos(dot(V1, V2))*180)/pi;
		if (angle > sls.spot_cutoff_angle) {
			continue;
		}
		point temp_start = { classA.P.x + toSource.x*0.01, classA.P.y + toSource.y*0.01, classA.P.z + toSource.z*0.01 };
		A temp = colorPoints(temp_start, toSource);
		if (temp.hasIntersected) {
			continue;
		}
		vector_ N = normalize(classA.normal);
		double distance = (sls.spot_source_x - classA.P.x)*(sls.spot_source_x - classA.P.x)
			+ (sls.spot_source_y - classA.P.y)*(sls.spot_source_y - classA.P.y)
			+ (sls.spot_source_z - classA.P.z)*(sls.spot_source_z - classA.P.z);
		distance = sqrt(distance);
		double scaling_factor = exp(-distance * distance*sls.spot_source_falloff);
		lambert += max(0.0, dot(toSource, N)*scaling_factor);
		vector_ R_prime = sub(toSource, scalar_mul(N, dot(toSource, N) * 2));
		R_prime = normalize(R_prime);
		vector_ V = { -classA.dir_temp.x, -classA.dir_temp.y, -classA.dir_temp.z };
		V = normalize(V);
		phong += max(pow(dot(R_prime, V), classA.shininess)*scaling_factor, 0.0);
	}
	color clr;
	clr.r = classA.ambient*classA.clr.r + classA.diffuse*lambert*classA.clr.r + classA.specular*phong*classA.clr.r;
	clr.g = classA.ambient*classA.clr.g + classA.diffuse*lambert*classA.clr.g + classA.specular*phong*classA.clr.g;
	clr.b = classA.ambient*classA.clr.b + classA.diffuse*lambert*classA.clr.b + classA.specular*phong*classA.clr.b;

	vector_ N = normalize(classA.normal);
	vector_ reflectedRay = sub(classA.dir_temp, scalar_mul(N, dot(classA.dir_temp, N) * 2));
	reflectedRay = normalize(reflectedRay);
	point new_start;
	new_start.x = classA.P.x + reflectedRay.x*0.01;
	new_start.y = classA.P.y + reflectedRay.y*0.01;
	new_start.z = classA.P.z + reflectedRay.z*0.01;
	A retTemp = colorPoints(new_start, reflectedRay);
	if (retTemp.hasIntersected) {
		A retTemp1 = recursiveColoring(retTemp, depthOfRecursion - 1);
		clr.r += classA.reflectance*retTemp1.clr.r;
		clr.g += classA.reflectance*retTemp1.clr.g;
		clr.b += classA.reflectance*retTemp1.clr.b;
		if (clr.r < 0) clr.r = 0;
		else if (clr.r > 1) clr.r = 1;
		if (clr.g < 0) clr.g = 0;
		else if (clr.g > 1) clr.g = 1;
		if (clr.b < 0) clr.b = 0;
		else if (clr.b > 1) clr.b = 1;
		
	}
	vector_ dir_temp = { retTemp.P.x - retTemp.start.x, retTemp.P.y - retTemp.start.y, retTemp.P.z - retTemp.start.z };
	dir_temp = normalize(dir_temp);
	A temp(retTemp.P, retTemp.start, dir_temp, retTemp.normal, clr,
		retTemp.ambient, retTemp.specular, retTemp.diffuse, retTemp.reflectance, retTemp.shininess, retTemp.hasIntersected);
	return temp;
}

void generateRay() {
	pointBuffer = new point*[num_of_pixels];
	for (int i = 0; i < num_of_pixels; ++i) {
		pointBuffer[i] = new point[num_of_pixels];
	}

	double near_plane_height = 2 * near_plane_distance*tan((fovY / 2)*(pi / 180));
	double fovX = aspectRatio * fovY;
	double near_plane_width = 2 * near_plane_distance*tan((fovX / 2)*(pi / 180));

	point midpoint;
	midpoint.x = cameraPosition.x + l.x*near_plane_distance;
	midpoint.y = cameraPosition.y + l.y*near_plane_distance;
	midpoint.z = cameraPosition.z + l.z*near_plane_distance;

	double pixelWidthInNearPlane = near_plane_width / num_of_pixels;
	double pixelHeightInNearPlane = near_plane_height / num_of_pixels;

	pointBuffer[0][0].x = midpoint.x + u.x*(near_plane_height / 2) - r.x*(near_plane_width / 2);
	pointBuffer[0][0].y = midpoint.y + u.y*(near_plane_height / 2) - r.y*(near_plane_width / 2);
	pointBuffer[0][0].z = midpoint.z + u.z*(near_plane_height / 2) - r.z*(near_plane_width / 2);

	for (int i = 1; i < num_of_pixels; ++i) {
		pointBuffer[i][0].x = pointBuffer[i - 1][0].x - u.x*pixelHeightInNearPlane;
		pointBuffer[i][0].y = pointBuffer[i - 1][0].y - u.y*pixelHeightInNearPlane;
		pointBuffer[i][0].z = pointBuffer[i - 1][0].z - u.z*pixelHeightInNearPlane;
	}

	for (int i = 0; i < num_of_pixels; ++i) {
		for (int j = 1; j < num_of_pixels; ++j) {
			pointBuffer[i][j].x = pointBuffer[i][j - 1].x + r.x*pixelWidthInNearPlane;
			pointBuffer[i][j].y = pointBuffer[i][j - 1].y + r.y*pixelWidthInNearPlane;
			pointBuffer[i][j].z = pointBuffer[i][j - 1].z + r.z*pixelWidthInNearPlane;
		}
	}
}



A colorPoints(point start, vector_ direction) {
	double normalize_coeff = sqrt(direction.x*direction.x + direction.y*direction.y + direction.z*direction.z);
	direction.x /= normalize_coeff;
	direction.y /= normalize_coeff;
	direction.z /= normalize_coeff;

	double curr_t = far_plane_distance;
	string curr_object;
	int curr_object_index;
	bool isBase;
	point triangle1, triangle2, triangle3;
	bool hasIntersected = true;
	for (int i = 0; i < spheres.size(); ++i) {
		sphere currSphere = spheres[i];
		point R0;

		//translate 
		R0.x = start.x - currSphere.center_x;
		R0.y = start.y - currSphere.center_y;
		R0.z = start.z - currSphere.center_z;

		double b = 2 * (direction.x*R0.x + direction.y*R0.y + direction.z*R0.z);
		double c = (R0.x*R0.x + R0.y*R0.y + R0.z*R0.z) - currSphere.radius*currSphere.radius;
		double d = b*b - 4 * c;
		if (d < 0) continue;
		else if (d == 0) {
			double t = -b / 2;
			if (t < curr_t && t > 0 && t < far_plane_distance) {
				curr_t = t;
				curr_object = "sphere";
				curr_object_index = i;
			}
		}
		else {
			double t1 = (-b + sqrt(d)) / 2;
			double t2 = (-b - sqrt(d)) / 2;
			if (t1 < curr_t && t1 > 0 && t1 < far_plane_distance) {
				curr_t = t1;
				curr_object = "sphere";
				curr_object_index = i;
			}
			if (t2 < curr_t && t2 > 0 && t2 < far_plane_distance) {
				curr_t = t2;
				curr_object = "sphere";
				curr_object_index = i;
			}
		}
	}

	for (int i = 0; i < pyramids.size(); ++i) {
		pyramid currPyramid = pyramids[i];
		double high_x = currPyramid.low_x, high_y = currPyramid.low_y, high_z = currPyramid.low_z + currPyramid.height;
		double p1_x = currPyramid.low_x + currPyramid.width / 2, p1_y = currPyramid.low_y + currPyramid.width / 2,
			p1_z = currPyramid.low_z;
		double p2_x = currPyramid.low_x + currPyramid.width / 2, p2_y = currPyramid.low_y - currPyramid.width / 2,
			p2_z = currPyramid.low_z;
		double p3_x = currPyramid.low_x - currPyramid.width / 2, p3_y = currPyramid.low_y - currPyramid.width / 2,
			p3_z = currPyramid.low_z;
		double p4_x = currPyramid.low_x - currPyramid.width / 2, p4_y = currPyramid.low_y + currPyramid.width / 2,
			p4_z = currPyramid.low_z;

		//intersection with base
		vector_ n, p1p2, p3p2;
		p1p2.x = p1_x - p2_x;
		p1p2.y = p1_y - p2_y;
		p1p2.z = p1_z - p2_z;
		p3p2.x = p3_x - p2_x;
		p3p2.y = p3_y - p2_y;
		p3p2.z = p3_z - p2_z;
		n.x = p1p2.y*p3p2.z - p1p2.z*p3p2.y;
		n.y = p1p2.z*p3p2.x - p1p2.x*p3p2.z;
		n.z = p1p2.x*p3p2.y - p1p2.y*p3p2.x;
		double n_mod = sqrt(n.x*n.x + n.y*n.y + n.z*n.z);
		n.x /= n_mod;
		n.y /= n_mod;
		n.z /= n_mod;
		double D = -(n.x*p1_x + n.y*p1_y + n.z*p1_z);
		double t = -(D + (n.x*start.x + n.y*start.y + n.z*start.z)) / (n.x*direction.x + n.y*direction.y + n.z*direction.z);
		if (t < curr_t && t > 0 && t < far_plane_distance) {
			point intersected;
			intersected.x = start.x + t * direction.x;
			intersected.y = start.y + t * direction.y;
			intersected.z = start.z + t * direction.z;
			if (intersected.x <= p1_x && intersected.x >= p3_x && intersected.y <= p1_y && intersected.y >= p3_y) {
				curr_t = t;
				curr_object = "pyramid";
				curr_object_index = i;
				isBase = true;
			}
		}

		//intersection with triangle1
		double A = (p1_x - p2_x)*((p1_y - high_y)*direction.z - (p1_z - high_z)*direction.y);
		A += (p1_x - high_x)*(direction.y*(p1_z - p2_z) - direction.z*(p1_y - p2_y));
		A += direction.x*((p1_y - p2_y)*(p1_z - high_z) - (p1_y - high_y)*(p1_z - p2_z));
		double beta = (p1_x - start.x)*(direction.z*(p1_y - high_y) - direction.y*(p1_z - high_z));
		beta += (p1_x - high_x)*(direction.y*(p1_z - start.z) - direction.z*(p1_y - start.y));
		beta += direction.x*((p1_y - start.y)*(p1_z - high_z) - (p1_y - high_y)*(p1_z - start.z));
		beta /= A;
		double gamma = (p1_x - p2_x)*((p1_y - start.y)*direction.z - (p1_z - start.z)*direction.y);
		gamma += (p1_x - start.x)*(direction.y*(p1_z - p2_z) - direction.z*(p1_y - p2_y));
		gamma += direction.x*((p1_y - p2_y)*(p1_z - start.z) - (p1_y - start.y)*(p1_z - p2_z));
		gamma /= A;
		t = (p1_x - p2_x)*((p1_y - high_y)*(p1_z - start.z) - (p1_z - high_z)*(p1_y - start.y));
		t += (p1_x - high_x)*((p1_y - start.y)*(p1_z - p2_z) - (p1_z - start.z)*(p1_y - p2_y));
		t += (p1_x - start.x)*((p1_y - p2_y)*(p1_z - high_z) - (p1_y - high_y)*(p1_z - p2_z));
		t /= A;
		double alpha = 1 - beta - gamma;
		if (alpha > 0 && beta > 0 && gamma > 0 && alpha + beta + gamma == 1) {
			if (t < curr_t && t > 0 && t < far_plane_distance) {
				curr_t = t;
				curr_object = "pyramid";
				curr_object_index = i;
				isBase = false;
				triangle1.x = p1_x, triangle2.x = p2_x;
				triangle1.y = p1_y, triangle2.y = p2_y;
				triangle1.z = p1_z, triangle2.z = p2_z;
				triangle3.x = high_x, triangle3.y = high_y, triangle3.z = high_z;
			}
		}

		//intersection with triangle2
		A = (p2_x - p3_x)*((p2_y - high_y)*direction.z - (p2_z - high_z)*direction.y);
		A += (p2_x - high_x)*(direction.y*(p2_z - p3_z) - direction.z*(p2_y - p3_y));
		A += direction.x*((p2_y - p3_y)*(p2_z - high_z) - (p2_y - high_y)*(p2_z - p3_z));
		beta = (p2_x - start.x)*(direction.z*(p2_y - high_y) - direction.y*(p2_z - high_z));
		beta += (p2_x - high_x)*(direction.y*(p2_z - start.z) - direction.z*(p2_y - start.y));
		beta += direction.x*((p2_y - start.y)*(p2_z - high_z) - (p2_y - high_y)*(p2_z - start.z));
		beta /= A;
		gamma = (p2_x - p3_x)*((p2_y - start.y)*direction.z - (p2_z - start.z)*direction.y);
		gamma += (p2_x - start.x)*(direction.y*(p2_z - p3_z) - direction.z*(p2_y - p3_y));
		gamma += direction.x*((p2_y - p3_y)*(p2_z - start.z) - (p2_y - start.y)*(p2_z - p3_z));
		gamma /= A;
		t = (p2_x - p3_x)*((p2_y - high_y)*(p2_z - start.z) - (p2_z - high_z)*(p2_y - start.y));
		t += (p2_x - high_x)*((p2_y - start.y)*(p2_z - p3_z) - (p2_z - start.z)*(p2_y - p3_y));
		t += (p2_x - start.x)*((p2_y - p3_y)*(p2_z - high_z) - (p2_y - high_y)*(p2_z - p3_z));
		t /= A;
		alpha = 1 - beta - gamma;
		if (alpha > 0 && beta > 0 && gamma > 0 && alpha + beta + gamma == 1) {
			if (t < curr_t && t > 0 && t < far_plane_distance) {
				curr_t = t;
				curr_object = "pyramid";
				curr_object_index = i;
				isBase = false;
				triangle1.x = p2_x, triangle2.x = p3_x;
				triangle1.y = p2_y, triangle2.y = p3_y;
				triangle1.z = p2_z, triangle2.z = p3_z;
				triangle3.x = high_x, triangle3.y = high_y, triangle3.z = high_z;
			}
		}

		//intersection with triangle3
		A = (p3_x - p4_x)*((p3_y - high_y)*direction.z - (p3_z - high_z)*direction.y);
		A += (p3_x - high_x)*(direction.y*(p3_z - p4_z) - direction.z*(p3_y - p4_y));
		A += direction.x*((p3_y - p4_y)*(p3_z - high_z) - (p3_y - high_y)*(p3_z - p4_z));
		beta = (p3_x - start.x)*(direction.z*(p3_y - high_y) - direction.y*(p3_z - high_z));
		beta += (p3_x - high_x)*(direction.y*(p3_z - start.z) - direction.z*(p3_y - start.y));
		beta += direction.x*((p3_y - start.y)*(p3_z - high_z) - (p3_y - high_y)*(p3_z - start.z));
		beta /= A;
		gamma = (p3_x - p4_x)*((p3_y - start.y)*direction.z - (p3_z - start.z)*direction.y);
		gamma += (p3_x - start.x)*(direction.y*(p3_z - p4_z) - direction.z*(p3_y - p4_y));
		gamma += direction.x*((p3_y - p4_y)*(p3_z - start.z) - (p3_y - start.y)*(p3_z - p4_z));
		gamma /= A;
		t = (p3_x - p4_x)*((p3_y - high_y)*(p3_z - start.z) - (p3_z - high_z)*(p3_y - start.y));
		t += (p3_x - high_x)*((p3_y - start.y)*(p3_z - p4_z) - (p3_z - start.z)*(p3_y - p4_y));
		t += (p3_x - start.x)*((p3_y - p4_y)*(p3_z - high_z) - (p3_y - high_y)*(p3_z - p4_z));
		t /= A;
		alpha = 1 - beta - gamma;
		if (alpha > 0 && beta > 0 && gamma > 0 && alpha + beta + gamma == 1) {
			if (t < curr_t && t > 0 && t < far_plane_distance) {
				curr_t = t;
				curr_object = "pyramid";
				curr_object_index = i;
				isBase = false;
				triangle1.x = p3_x, triangle2.x = p4_x;
				triangle1.y = p3_y, triangle2.y = p4_y;
				triangle1.z = p3_z, triangle2.z = p4_z;
				triangle3.x = high_x, triangle3.y = high_y, triangle3.z = high_z;
			}
		}

		//intersection with triangle4
		A = (p4_x - p1_x)*((p4_y - high_y)*direction.z - (p4_z - high_z)*direction.y);
		A += (p4_x - high_x)*(direction.y*(p4_z - p1_z) - direction.z*(p4_y - p1_y));
		A += direction.x*((p4_y - p1_y)*(p4_z - high_z) - (p4_y - high_y)*(p4_z - p1_z));
		beta = (p4_x - start.x)*(direction.z*(p4_y - high_y) - direction.y*(p4_z - high_z));
		beta += (p4_x - high_x)*(direction.y*(p4_z - start.z) - direction.z*(p4_y - start.y));
		beta += direction.x*((p4_y - start.y)*(p4_z - high_z) - (p4_y - high_y)*(p4_z - start.z));
		beta /= A;
		gamma = (p4_x - p1_x)*((p4_y - start.y)*direction.z - (p4_z - start.z)*direction.y);
		gamma += (p4_x - start.x)*(direction.y*(p4_z - p1_z) - direction.z*(p4_y - p1_y));
		gamma += direction.x*((p4_y - p1_y)*(p4_z - start.z) - (p4_y - start.y)*(p4_z - p1_z));
		gamma /= A;
		t = (p4_x - p1_x)*((p4_y - high_y)*(p4_z - start.z) - (p4_z - high_z)*(p4_y - start.y));
		t += (p4_x - high_x)*((p4_y - start.y)*(p4_z - p1_z) - (p4_z - start.z)*(p4_y - p1_y));
		t += (p4_x - start.x)*((p4_y - p1_y)*(p4_z - high_z) - (p4_y - high_y)*(p4_z - p1_z));
		t /= A;
		alpha = 1 - beta - gamma;
		if (alpha > 0 && beta > 0 && gamma > 0 && alpha + beta + gamma == 1) {
			if (t < curr_t && t > 0 && t < far_plane_distance) {
				curr_t = t;
				curr_object = "pyramid";
				curr_object_index = i;
				isBase = false;
				triangle1.x = p4_x, triangle2.x = p1_x;
				triangle1.y = p4_y, triangle2.y = p1_y;
				triangle1.z = p4_z, triangle2.z = p1_z;
				triangle3.x = high_x, triangle3.y = high_y, triangle3.z = high_z;
			}
		}
	}

	double t = -(-1 * start.z) / (-1 * direction.z);
	if (t < curr_t && t > 0 && t < far_plane_distance) {
		curr_t = t;
		curr_object = "checkerboard";
	}

	color clr;
	clr.r = clr.g = clr.b = 0;
	vector_ normal;
	point P;
	P.x = start.x + curr_t * direction.x;
	P.y = start.y + curr_t * direction.y;
	P.z = start.z + curr_t * direction.z;
	double ambient, diffuse, specular, reflectance, shininess;
	if (curr_object == "sphere") {
		sphere obj = spheres[curr_object_index];
		normal.x = P.x - obj.center_x;
		normal.y = P.y - obj.center_y;
		normal.z = P.z - obj.center_z;
		ambient = obj.ambient;
		diffuse = obj.diffuse;
		specular = obj.specular;
		reflectance = obj.reflection;
		shininess = obj.shininess;
		clr.r = obj.color_r;
		clr.g = obj.color_g;
		clr.b = obj.color_b;
	}
	else if (curr_object == "pyramid") {
		pyramid obj = pyramids[curr_object_index];
		if (isBase) {
			normal.x = 0;
			normal.y = 0;
			normal.z = -1;
		}
		else {
			vector_ a, b;
			a.x = triangle1.x - triangle3.x;
			a.y = triangle1.y - triangle3.y;
			a.z = triangle1.z - triangle3.z;
			b.x = triangle2.x - triangle3.x;
			b.y = triangle2.y - triangle3.y;
			b.z = triangle2.z - triangle3.z;
			normal = normalize(cross(a, b));
			if (normal.z < 0) {
				normal.x = -normal.x;
				normal.y = -normal.y;
				normal.z = -normal.z;
			}
		}
		ambient = obj.ambient;
		diffuse = obj.diffuse;
		specular = obj.specular;
		reflectance = obj.reflection;
		shininess = obj.shininess;
		clr.r = obj.color_r;
		clr.g = obj.color_g;
		clr.b = obj.color_b;
	}
	else if (curr_object == "checkerboard") {
		int start_color = 0;
		bool colorSet = false;
		for (double x = -far_plane_distance; x <= far_plane_distance; x += cell_width) {
			int current_color = start_color;
			for (double y = -far_plane_distance; y <= far_plane_distance; y += cell_width) {
				double p1_x = x, p1_y = y;
				double p2_x = x + cell_width, p2_y = y;
				double p3_x = x + cell_width, p3_y = y + cell_width;
				double p4_x = x, p4_y = y + cell_width;
				if (P.x >= p1_x && P.x <= p2_x && P.y >= p1_y && P.y <= p3_y) {
					if (!textureEnabled) {
						clr.r = clr.g = clr.b = current_color;
						colorSet = true;
						break;
					}
					else {
						int width_index = floor(((textureWidth - 1) / cell_width)*(P.x - x));
						int height_index = textureHeight - floor(((textureHeight - 1) / cell_width)*(P.y - y)) - 1;
						clr = textureBuffer[width_index][height_index];
						colorSet = true;
						break;
					}
				}
				current_color ^= 1;
			}
			start_color ^= 1;
		}
		ambient = checkerboard_ambient;
		diffuse = checkerboard_diffuse;
		reflectance = checkerboard_reflection;
		specular = 0;
		shininess = 0;
		normal = { 0, 0, 1 };
	}
	else {
		clr.r = clr.g = clr.b = 0;
		hasIntersected = false;
		ambient = specular = diffuse = reflectance = shininess = 0;
		normal = { 0, 0, 0 };
	}
	vector_ dir_temp = { P.x - start.x, P.y - start.y, P.z - start.z };
	dir_temp = normalize(dir_temp);
	return A(P, start, dir_temp, normal, clr, ambient, specular, diffuse, reflectance, shininess, hasIntersected);
}

void keyboardListener(unsigned char key, int x, int y) {
	switch (key) {

	case '0':
	{
		cout << "Rendering started" << endl;
		generateRay();
		colors = new color*[num_of_pixels];
		for (int i = 0; i < num_of_pixels; ++i) {
			colors[i] = new color[num_of_pixels];
		}
		for (int i = 0; i < num_of_pixels; ++i) {
			double rendered = ((double)i + 1) / num_of_pixels;
			cout << "Rendered " << rendered * 100 << "%" << endl;
			for (int j = 0; j < num_of_pixels; ++j) {
				point start = pointBuffer[i][j];
				vector_ direction;
				direction.x = start.x - cameraPosition.x;
				direction.y = start.y - cameraPosition.y;
				direction.z = start.z - cameraPosition.z;
				A temp = colorPoints(start, direction);
				if (temp.hasIntersected)
					colors[i][j] = recursiveColoring(temp, level_of_recursion).clr;
				else colors[i][j] = temp.clr;
			}
		}
		bitmap_image image(num_of_pixels, num_of_pixels);
		for (int x = 0; x < num_of_pixels; x++) {
			for (int y = 0; y < num_of_pixels; y++) {
				image.set_pixel(x, y, colors[y][x].r * 255, colors[y][x].g * 255, colors[y][x].b * 255);
			}
		}
		image.save_image("out.bmp");
		cout << "Image saved" << endl;
	}
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
	case ' ':
	{
		textureEnabled = true;
		bitmap_image b_img("texture.bmp");

		int height, width;

		height = textureHeight = b_img.height();
		width = textureWidth = b_img.width();
		textureBuffer = new color*[width];
		for (int i = 0; i < width; i++) {
			textureBuffer[i] = new color[height];
			for (int j = 0; j < height; j++) {
				unsigned char r, g, b;
				b_img.get_pixel(i, j, r, g, b);
				color c = { r / 255.0, g / 255.0, b / 255.0 };
				textureBuffer[i][j] = c;
			}
		}
	}
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

	//drawAxes();
	//drawGrid();

	for (int i = 0; i < num_of_normal_source; ++i) {
		normal_light_source nls = normal_sources[i];
		glPushMatrix(); {
			glColor3f(1, 1, 0);
			glTranslatef(nls.normal_source_x, nls.normal_source_y, nls.normal_source_z);
			drawSphere(5, 99, 99);
		}glPopMatrix();
	}

	for (int i = 0; i < num_of_spot_source; ++i) {
		spot_light_source sls = spot_sources[i];
		glPushMatrix(); {
			glColor3f(1, 1, 0);
			glTranslatef(sls.spot_source_x, sls.spot_source_y, sls.spot_source_z);
			drawSphere(5, 99, 99);
		}glPopMatrix();
	}

	for (int i = 0; i < spheres.size(); ++i) {
		sphere to_draw = spheres[i];
		glPushMatrix(); {
			glColor3f(to_draw.color_r, to_draw.color_g, to_draw.color_b);
			glTranslatef(to_draw.center_x, to_draw.center_y, to_draw.center_z);
			drawSphere(to_draw.radius, 99, 99);
		}glPopMatrix();
	}

	for (int i = 0; i < pyramids.size(); ++i) {
		pyramid to_draw = pyramids[i];
		glColor3f(to_draw.color_r, to_draw.color_g, to_draw.color_b);
		drawPyramid(to_draw.low_x, to_draw.low_y, to_draw.low_z, to_draw.width, to_draw.height);
	}

	int start_color = 0;
	for (double x = -5 * far_plane_distance; x <= 5 * far_plane_distance; x += cell_width) {
		int current_color = start_color;
		for (double y = -5 * far_plane_distance; y <= 5 * far_plane_distance; y += cell_width) {
			double p1_x = x, p1_y = y;
			double p2_x = x + cell_width, p2_y = y;
			double p3_x = x + cell_width, p3_y = y + cell_width;
			double p4_x = x, p4_y = y + cell_width;
			glColor3f(current_color, current_color, current_color);
			glBegin(GL_QUADS); {
				glVertex3f(p1_x, p1_y, 0);
				glVertex3f(p2_x, p2_y, 0);
				glVertex3f(p3_x, p3_y, 0);
				glVertex3f(p4_x, p4_y, 0);
			}glEnd();
			current_color ^= 1;
		}
		start_color ^= 1;
	}

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
	cameraPosition = { 100, 0, 100 };
	u = { 0, 0, 1 };
	r = { 0, 1, 0 };
	l = { -1, 0, 0 };


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
	gluPerspective(fovY, aspectRatio, near_plane_distance, far_plane_distance);
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

	ifstream inputFile;
	inputFile.open("description.txt");

	inputFile >> near_plane_distance >> far_plane_distance >> fovY >> aspectRatio;
	inputFile >> level_of_recursion;
	inputFile >> num_of_pixels;
	inputFile >> cell_width;
	inputFile >> checkerboard_ambient >> checkerboard_diffuse >> checkerboard_reflection;
	inputFile >> num_of_objects;

	for (int i = 0; i < num_of_objects; ++i) {
		string object;
		inputFile >> object;
		if (object == "sphere") {
			sphere obj;
			inputFile >> obj.center_x >> obj.center_y >> obj.center_z;
			inputFile >> obj.radius;
			inputFile >> obj.color_r >> obj.color_g >> obj.color_b;
			inputFile >> obj.ambient >> obj.diffuse >> obj.specular >> obj.reflection;
			inputFile >> obj.shininess;
			spheres.push_back(obj);
		}

		else if (object == "pyramid") {
			pyramid obj;
			inputFile >> obj.low_x >> obj.low_y >> obj.low_z;
			inputFile >> obj.width >> obj.height;
			inputFile >> obj.color_r >> obj.color_g >> obj.color_b;
			inputFile >> obj.ambient >> obj.diffuse >> obj.specular >> obj.reflection;
			inputFile >> obj.shininess;
			pyramids.push_back(obj);
		}
	}

	inputFile >> num_of_normal_source;
	for (int i = 0; i < num_of_normal_source; ++i) {
		normal_light_source nls;
		inputFile >> nls.normal_source_x >> nls.normal_source_y >> nls.normal_source_z >> nls.normal_source_falloff;
		normal_sources.push_back(nls);
	}

	inputFile >> num_of_spot_source;
	for (int i = 0; i < num_of_spot_source; ++i) {
		spot_light_source sls;
		inputFile >> sls.spot_source_x >> sls.spot_source_y >> sls.spot_source_z >> sls.spot_source_falloff;
		inputFile >> sls.spot_look_x >> sls.spot_look_y >> sls.spot_look_z;
		inputFile >> sls.spot_cutoff_angle;
		spot_sources.push_back(sls);
	}

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
