#include <iostream>
#include <fstream>
#include <cassert>
#include <iomanip>
#include <cmath>
#include <stack>
#include <queue>
#include <vector>
#include <cmath>
#include "bitmap_image.hpp"
using namespace std;

#define pi (2*acos(0.0))
#define epsilon (1.0e-6)

class homogeneous_point
{
public:
	double x, y, z, w;

	// set the three coordinates, set w to 1
	homogeneous_point(double x, double y, double z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = 1;
	}

	/*
	default constructor. does nothing. allows declarations like below:
		matrix m;
	therefore, usage is dangerous
	*/
	homogeneous_point() {
	}

	// constructs a homogeneous point with given coordinates. forces w to be 1.0
	// if w is zero, raises error
	homogeneous_point(double x, double y, double z, double w)
	{
		assert(w != 0);
		this->x = x / w;
		this->y = y / w;
		this->z = z / w;
		this->w = 1;
	}

	// adds two points. returns a point forcing w to be 1.0
	homogeneous_point operator+ (const homogeneous_point& point)
	{
		double x = this->x + point.x;
		double y = this->y + point.y;
		double z = this->z + point.z;
		double w = this->w + point.w;
		homogeneous_point p(x, y, z, w);
		return p;
	}

	// subtracts one point from another. returns a point forcing w to be 1.0
	homogeneous_point operator- (const homogeneous_point& point)
	{
		double x = this->x - point.x;
		double y = this->y - point.y;
		double z = this->z - point.z;
		double w = this->w - point.w;
		homogeneous_point p(x, y, z, w);
	}

	// Print the coordinates of a point. exists for testing purpose.
	void print()
	{
		cout << "Point: " << endl;
		cout << x << " " << y << " " << z << " " << w << endl;
	}

};


class Vector
{
public:
	double x, y, z;

	// constructs a vector with given components
	Vector(double x, double y, double z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	// keeps the direction same. recalculates the vector to be unit.
	void normalize()
	{
		double r = sqrt(x*x + y * y + z * z);
		x = x / r;
		y = y / r;
		z = z / r;
	}

	// add two vectors
	Vector operator+(const Vector& v)
	{
		Vector v1(x + v.x, y + v.y, z + v.z);
		return v1;
	}

	// subtract one vector from another
	Vector operator-(const Vector& v)
	{
		Vector v1(x - v.x, y - v.y, z - v.z);
		return v1;
	}

	// scale a vector with a given coefficient
	Vector operator* (double m)
	{
		Vector v(x*m, y*m, z*m);
		return v;
	}

	// get the dot product of two vectors
	static double dot(Vector a, Vector b)
	{
		return a.x*b.x + a.y*b.y + a.z*b.z;
	}

	// get the cross product of two vectors
	static Vector cross(Vector a, Vector b)
	{
		Vector v(a.y*b.z - a.z*b.y, b.x*a.z - b.z*a.x, a.x*b.y - a.y*b.x);
		return v;
	}

	// print a vector. only for testing purposes.
	void print()
	{
		cout << "Vector" << endl;
		cout << x << " " << y << " " << z << endl;
	}
};


/*
The matrices are forced to be 4x4. This is because in this assignment, we will deal with points in triangles.
Maximum # of points that we will deal with at once is 3. And all the standard matrices are 4x4 (i.e. scale, translation, rotation etc.)
*/
class matrix
{
public:
	double values[4][4];
	int num_rows, num_cols;

	// only set the number of rows and cols
	matrix(int rows, int cols)
	{
		assert(rows <= 4 && cols <= 4);
		num_rows = rows;
		num_cols = cols;
	}

	// prepare an nxn square matrix
	matrix(int n)
	{
		assert(n <= 4);
		num_rows = num_cols = n;
	}

	// prepare and return an identity matrix of size nxn
	static matrix make_identity(int n)
	{
		assert(n <= 4);
		matrix m(n);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				if (i == j)
					m.values[i][j] = 1;
				else
					m.values[i][j] = 0;
			}
		}
		return m;
	}

	// print the matrix. exists for testing purposes
	void print()
	{
		cout << "Matrix:" << endl;
		for (int i = 0; i < num_rows; i++)
		{
			for (int j = 0; j < num_cols; j++)
			{
				cout << values[i][j] << "\t";
			}
			cout << endl;
		}
	}

	// add the two matrices. Raise error if dimension mismatches
	matrix operator+ (const matrix& m)
	{
		assert(this->num_rows == m.num_rows);
		assert(this->num_cols == m.num_cols);

		matrix m1(num_rows, num_cols);
		for (int i = 0; i < num_rows; i++)
		{
			for (int j = 0; j < num_cols; j++)
			{
				m1.values[i][j] = values[i][j] + m.values[i][j];
			}
		}
		return m1;
	}

	// subtract a matrix from another. raise error if dimension mismatches
	matrix operator- (const matrix& m)
	{
		assert(this->num_rows == m.num_rows);
		assert(this->num_cols == m.num_cols);

		matrix m1(num_rows, num_cols);
		for (int i = 0; i < num_rows; i++)
		{
			for (int j = 0; j < num_cols; j++)
			{
				m1.values[i][j] = values[i][j] - m.values[i][j];
			}
		}
		return m1;
	}

	// multiply two matrices. allows statements like m1 = m2 * m3; raises error is dimension mismatches
	matrix operator* (const matrix& m)
	{
		assert(this->num_cols == m.num_rows);
		matrix m1(this->num_rows, m.num_cols);

		for (int i = 0; i < m1.num_rows; i++) {
			for (int j = 0; j < m1.num_cols; j++) {
				double val = 0;
				for (int k = 0; k < this->num_cols; k++) {
					val += this->values[i][k] * m.values[k][j];
				}
				m1.values[i][j] = val;
			}
		}
		return m1;
	}

	// multiply a matrix with a constant
	matrix operator* (double m)
	{
		matrix m1(this->num_rows, this->num_cols);
		for (int i = 0; i < num_rows; i++) {
			for (int j = 0; j < num_cols; j++) {
				m1.values[i][j] = m * this->values[i][j];
			}
		}
		return m1;
	}

	// multiply a 4x4 matrix with a homogeneous point and return the resulting point.
	// usage: homogeneous_point p = m * p1;
	// here, m is a 4x4 matrix, intended to be the transformation matrix
	// p1 is the point on which the transformation is being made
	// p is the resulting homogeneous point
	homogeneous_point operator* (const homogeneous_point& p)
	{
		assert(this->num_rows == this->num_cols && this->num_rows == 4);

		matrix m(4, 1);
		m.values[0][0] = p.x;
		m.values[1][0] = p.y;
		m.values[2][0] = p.z;
		m.values[3][0] = p.w;

		matrix m1 = (*this)*m;
		homogeneous_point p1(m1.values[0][0], m1.values[1][0], m1.values[2][0], m1.values[3][0]);
		return p1;
	}

	// return the transpose of a matrix
	matrix transpose()
	{
		matrix m(num_cols, num_rows);
		for (int i = 0; i < num_rows; i++) {
			for (int j = 0; j < num_cols; j++) {
				m.values[j][i] = values[i][j];
			}
		}
		return m;
	}

};

/*
A simple class to hold the color components, r, g, b of a certain shade.
*/
class color {
public:
	double r, g, b;
	color(double r, double g, double b) {
		this->r = r;
		this->g = g;
		this->b = b;
	}
	color() {
	}
};


class triangle {
public:
	homogeneous_point points[3];
	color clr;

	triangle() {
	}
};

double eye_x, eye_y, eye_z;
double look_x, look_y, look_z;
double up_x, up_y, up_z;
double fov_x, fov_y, aspectRatio, near, far;
color backgroud;
int screen_x, screen_y;

vector<triangle> tri_vect;
vector<triangle> clipped_triangles;


void scan_convert() {
	ifstream stage3;
	stage3.open("stage3.txt");

	color** pixels = new color*[screen_x];
	double** zs = new double*[screen_x];
	for (int i = 0; i < screen_x; i++) {
		pixels[i] = new color[screen_y];
		for (int j = 0; j < screen_y; j++) {
			pixels[i][j] = backgroud;
		}
		zs[i] = new double[screen_y];
		for (int j = 0; j < screen_y; j++) {
			zs[i][j] = +20; // a very large value intended as +INFINITY
		}
	}

	// perform scan conversion, populate the 2D array pixels
	// the array zs is the z-buffer.
	int num_of_tri = clipped_triangles.size();
	for (int i = 0; i < num_of_tri; ++i) {
		triangle tri = clipped_triangles[i];
		
		for (int p1 = 0; p1 < 3; ++p1) {
			for (int p2 = p1 + 1; p2 < 3; ++p2) {
				if (tri.points[p1].y < tri.points[p2].y) {
					homogeneous_point temp = tri.points[p2];
					tri.points[p2] = tri.points[p1];
					tri.points[p1] = temp;
				}
			}
		}

		homogeneous_point p1 = tri.points[0];
		homogeneous_point p2 = tri.points[1];
		homogeneous_point p3 = tri.points[2];
		color clr = tri.clr;

		int i_start = screen_y - floor((p1.y + 1)*screen_y / 2);
		int i_end = screen_y - floor((p3.y + 1)*screen_y / 2);

		if (i_start < 0) i_start = 0;

		for (int i = i_start; i < i_end && i < screen_y; ++i) {
			double scan_line_y = 1 - (1.0 / screen_y) - (2.0 / screen_y)*i;
			double xl, zl, xr, zr, x1, x2, z1, z2;
			if (p1.y == p2.y || scan_line_y < p2.y) {
				x1 = (scan_line_y - p1.y) / (p1.y - p3.y)*(p1.x - p3.x) + p1.x;
				z1 = (scan_line_y - p1.y) / (p1.y - p3.y)*(p1.z - p3.z) + p1.z;

				x2 = (scan_line_y - p2.y) / (p2.y - p3.y)*(p2.x - p3.x) + p2.x;
				z2 = (scan_line_y - p2.y) / (p2.y - p3.y)*(p2.z - p3.z) + p2.z;
			}

			else if (p2.y == p3.y || scan_line_y > p2.y) {
				x1 = (scan_line_y - p1.y) / (p1.y - p2.y)*(p1.x - p2.x) + p1.x;
				z1 = (scan_line_y - p1.y) / (p1.y - p2.y)*(p1.z - p2.z) + p1.z;

				x2 = (scan_line_y - p1.y) / (p1.y - p3.y)*(p1.x - p3.x) + p1.x;
				z2 = (scan_line_y - p1.y) / (p1.y - p3.y)*(p1.z - p3.z) + p1.z;
			}

			if (x1 <= x2) {
				xl = x1, zl = z1;
				xr = x2, zr = z2;
			}
			else {
				xl = x2, zl = z2;
				xr = x1, zr = z1;
			}

			int j_start = floor((xl + 1)*screen_x / 2);
			int j_end = floor((xr + 1)*screen_x / 2);

			if (j_start < 0) j_start = 0;

			for (int j = j_start; j < j_end && j < screen_x; ++j) {
				double xp = (1.0 / screen_x) + (2.0 / screen_x)*j - 1;
				double zp = zr - (zr - zl)*((xr - xp) / (xr - xl));

				if (zp < zs[j][i]) {
					zs[j][i] = zp;
					pixels[j][i] = clr;
				}
			}
		}
	}


	// the following code generates a bmp image. do not change this.
	bitmap_image image(screen_x, screen_y);
	for (int x = 0; x < screen_x; x++) {
		for (int y = 0; y < screen_y; y++) {
			image.set_pixel(x, y, pixels[x][y].r, pixels[x][y].g, pixels[x][y].b);
		}
	}
	image.save_image("out.bmp");

	// free the dynamically allocated memory
	for (int i = 0; i < screen_x; ++i) {
		free(zs[i]);
		free(pixels[i]);
	}
}


vector<homogeneous_point> clip_triangle(string edge, vector<homogeneous_point> points) {
	int num_of_points = points.size();
	vector<homogeneous_point> output_list;
	if (edge == "near") {
		homogeneous_point S = points[num_of_points - 1];
		homogeneous_point P = points[0];
		for (int i = 0; i < num_of_points; ++i) {
			if (S.z <= -near && P.z <= -near)
				output_list.push_back(P);
			else if (S.z <= -near && P.z > -near) {
				double int_x = ((-near - S.z) / (S.z - P.z))*(S.x - P.x) + S.x;
				double int_y = ((-near - S.z) / (S.z - P.z))*(S.y - P.y) + S.y;
				double int_z = -near;
				homogeneous_point intersection_point(int_x, int_y, int_z);
				output_list.push_back(intersection_point);
			}
			else if (S.z > -near && P.z <= -near) {
				double int_x = ((-near - S.z) / (S.z - P.z))*(S.x - P.x) + S.x;
				double int_y = ((-near - S.z) / (S.z - P.z))*(S.y - P.y) + S.y;
				double int_z = -near;
				homogeneous_point intersection_point(int_x, int_y, int_z);
				output_list.push_back(intersection_point);
				output_list.push_back(P);
			}
			if (i == num_of_points - 1) break;
			S = points[i];
			P = points[i + 1];
		}
	}
	else {
		homogeneous_point S = points[num_of_points - 1];
		homogeneous_point P = points[0];
		for (int i = 0; i < num_of_points; ++i) {
			if (S.z >= -far && P.z >= -far)
				output_list.push_back(P);
			else if (S.z >= -far && P.z < -far) {
				double int_x = ((-far - S.z) / (S.z - P.z))*(S.x - P.x) + S.x;
				double int_y = ((-far - S.z) / (S.z - P.z))*(S.y - P.y) + S.y;
				double int_z = -far;
				homogeneous_point intersection_point(int_x, int_y, int_z);
				output_list.push_back(intersection_point);
			}
			else if (S.z < -far && P.z >= -far) {
				double int_x = ((-far - S.z) / (S.z - P.z))*(S.x - P.x) + S.x;
				double int_y = ((-far - S.z) / (S.z - P.z))*(S.y - P.y) + S.y;
				double int_z = -far;
				homogeneous_point intersection_point(int_x, int_y, int_z);
				output_list.push_back(intersection_point);
				output_list.push_back(P);
			}
			if (i == num_of_points - 1) break;
			S = points[i];
			P = points[i + 1];
		}
	}
	return output_list;
}

void stage3()
{
	if (near == far) return;
	ifstream stage2;
	ofstream stage3;
	stage2.open("stage2.txt");
	stage3.open("stage3.txt");
	stage3 << std::fixed;
	stage3 << std::setprecision(7);

	// process input from stage2 and write to stage3
	fov_x = fov_y * aspectRatio;
	double t = near * tan((fov_y / 2)*(pi / 180));
	double r = near * tan((fov_x / 2)*(pi / 180));

	matrix P(4);
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (i == 0 && j == 0)
				P.values[i][j] = near / r;
			else if (i == 1 && j == 1)
				P.values[i][j] = near / t;
			else if (i == 2 && j == 2)
				P.values[i][j] = (far + near) / (near - far);
			else if (i == 3 && j == 2)
				P.values[i][j] = -1;
			else if (i == 2 && j == 3)
				P.values[i][j] = (2 * far*near) / (near - far);
			else
				P.values[i][j] = 0;
		}
	}
	cout << "matrix P" << endl;
	P.print();

	int cnt = 0;
	int k = 0;
	vector<homogeneous_point> points_to_clip;
	color current_color;
	while (true) {
		if (cnt == 3) {
			current_color = tri_vect[k].clr;
			++k;
			vector<homogeneous_point> clipped_near = clip_triangle("near", points_to_clip);
			vector<homogeneous_point> clipped_all = clip_triangle("far", clipped_near);
			points_to_clip.clear();
			if (clipped_all.size() == 4) {
				triangle tri[2];
				tri[0].clr = tri[1].clr = current_color;
				tri[0].points[0] = clipped_all[0];
				tri[0].points[1] = clipped_all[1];
				tri[0].points[2] = clipped_all[3];

				tri[1].points[0] = clipped_all[1];
				tri[1].points[1] = clipped_all[2];
				tri[1].points[2] = clipped_all[3];

				for (int i = 0; i < 2; ++i) {
					for (int j = 0; j < 3; ++j) {
						homogeneous_point tmp = P * tri[i].points[j];
						tmp = homogeneous_point(tmp.x, tmp.y, tmp.z, tmp.w);
						stage3 << tmp.x << " " << tmp.y << " " << tmp.z << endl;
						tri[i].points[j] = tmp;
					}
					clipped_triangles.push_back(tri[i]);
					stage3 << endl;
				}

			}
			else if (clipped_all.size() == 5) {
				triangle tri[3];
				tri[0].clr = tri[1].clr = tri[2].clr = current_color;
				tri[0].points[0] = clipped_all[0];
				tri[0].points[1] = clipped_all[1];
				tri[0].points[2] = clipped_all[4];

				tri[1].points[0] = clipped_all[1];
				tri[1].points[1] = clipped_all[3];
				tri[1].points[2] = clipped_all[4];

				tri[2].points[0] = clipped_all[1];
				tri[2].points[1] = clipped_all[2];
				tri[2].points[2] = clipped_all[3];

				for (int i = 0; i < 3; ++i) {
					for (int j = 0; j < 3; ++j) {
						homogeneous_point tmp = P * tri[i].points[j];
						tmp = homogeneous_point(tmp.x, tmp.y, tmp.z, tmp.w);
						stage3 << tmp.x << " " << tmp.y << " " << tmp.z << endl;
						tri[i].points[j] = tmp;
					}
					clipped_triangles.push_back(tri[i]);
					stage3 << endl;
				}
			}
			else if (clipped_all.size() == 3) {
				triangle tri;
				tri.clr = current_color;
				for (int j = 0; j < 3; ++j) {
					homogeneous_point tmp = P * clipped_all[j];
					tmp = homogeneous_point(tmp.x, tmp.y, tmp.z, tmp.w);
					stage3 << tmp.x << " " << tmp.y << " " << tmp.z << endl;
					tri.points[j] = tmp;
				}
				clipped_triangles.push_back(tri);
				stage3 << endl;
			}
			cnt = 0;
		}
		double x, y, z;
		stage2 >> x >> y >> z;

		if (stage2.eof()) break;

		++cnt;

		homogeneous_point p(x, y, z);
		points_to_clip.push_back(p);
	}

	stage3.close();
	stage2.close();

}

void stage2()
{
	ifstream stage1;
	ofstream stage2;
	stage1.open("stage1.txt");
	stage2.open("stage2.txt");
	stage2 << std::fixed;
	stage2 << std::setprecision(7);

	// collect input from stage1 and process, write output to stage2
	Vector look(look_x, look_y, look_z);
	Vector eye(eye_x, eye_y, eye_z);
	Vector up(up_x, up_y, up_z);

	Vector l = look - eye;
	l.normalize();
	Vector r = Vector::cross(l, up);
	r.normalize();
	Vector u = Vector::cross(r, l);
	cout << "Look Vector" << endl;
	l.print();
	cout << "Right Vector" << endl;
	r.print();
	cout << "Up Vector" << endl;
	u.print();

	matrix T = matrix::make_identity(4);
	T.values[0][3] = -eye_x;
	T.values[1][3] = -eye_y;
	T.values[2][3] = -eye_z;

	cout << "matrix T" << endl;
	T.print();

	matrix R = matrix::make_identity(4);
	R.values[0][0] = r.x;
	R.values[0][1] = r.y;
	R.values[0][2] = r.z;

	R.values[1][0] = u.x;
	R.values[1][1] = u.y;
	R.values[1][2] = u.z;

	R.values[2][0] = -l.x;
	R.values[2][1] = -l.y;
	R.values[2][2] = -l.z;

	cout << "matrix R" << endl;
	R.print();

	matrix V = R * T;
	cout << "matrix V" << endl;
	V.print();

	int cnt = 0;
	int i = 0;
	while (true) {
		if (cnt == 3) {
			cnt = 0;
			++i;
			stage2 << endl;
		}
		double x, y, z;
		stage1 >> x >> y >> z;

		if (stage1.eof()) break;

		homogeneous_point p(x, y, z);
		homogeneous_point t = V * p;
		t = homogeneous_point(t.x, t.y, t.z, t.w);

		tri_vect[i].points[cnt] = t;

		stage2 << t.x << " " << t.y << " " << t.z << endl;

		++cnt;
	}

	stage1.close();
	stage2.close();

}



Vector R(Vector x, Vector a, double angle) {
	return x * cos(angle) + a * (1 - cos(angle))*(Vector::dot(a, x)) + Vector::cross(a, x)*sin(angle);
}


void stage1()
{
	ifstream scene;
	ofstream stage1;
	scene.open("scene.txt");
	stage1.open("stage1.txt");
	stage1 << std::fixed;
	stage1 << std::setprecision(7);

	string command;

	scene >> eye_x >> eye_y >> eye_z;
	scene >> look_x >> look_y >> look_z;
	scene >> up_x >> up_y >> up_z;
	scene >> fov_y >> aspectRatio >> near >> far;
	scene >> screen_x >> screen_y;
	scene >> backgroud.r >> backgroud.g >> backgroud.b;

	// take other commands as input from scene in a loop
	// process accordingly
	// write to stage1
	stack<matrix> S;
	stack<stack<matrix>> stack_of_S;
	matrix identity_mat = matrix::make_identity(4);
	identity_mat.print();
	S.push(identity_mat);

	while (true) {
		scene >> command;
		cout << command << endl;
		if (command == "triangle") {
			double x[3], y[3], z[3];
			homogeneous_point p[3];
			for (int i = 0; i < 3; ++i) {
				scene >> x[i] >> y[i] >> z[i];
				homogeneous_point temp(x[i], y[i], z[i]);
				p[i] = temp;
			}

			triangle tri;
			color clr;
			scene >> clr.r >> clr.g >> clr.b;

			matrix transform_matrix = S.top();
			for (int i = 0; i < 3; ++i) {
				homogeneous_point resultant_point = transform_matrix * p[i];
				resultant_point = homogeneous_point(resultant_point.x, resultant_point.y,
					resultant_point.z, resultant_point.w);
				stage1 << resultant_point.x << " " << resultant_point.y << " " << resultant_point.z << endl;

				tri.points[i] = resultant_point;
			}
			tri.clr = clr;
			tri_vect.push_back(tri);
			stage1 << endl;
		}
		else if (command == "translate") {
			double tx, ty, tz;
			scene >> tx >> ty >> tz;

			matrix temp = matrix::make_identity(4);
			temp.values[0][3] = tx;
			temp.values[1][3] = ty;
			temp.values[2][3] = tz;
			S.push(S.top()*temp);

			temp.print();
		}
		else if (command == "scale") {
			double sx, sy, sz;
			scene >> sx >> sy >> sz;

			matrix temp(4);
			temp.values[0][0] = sx;
			temp.values[1][1] = sy;
			temp.values[2][2] = sz;
			temp.values[3][3] = 1;
			S.push(S.top()*temp);

			temp.print();
		}
		else if (command == "rotate") {
			double angle, ax, ay, az;
			scene >> angle >> ax >> ay >> az;

			Vector a(ax, ay, az);
			Vector i(1, 0, 0);
			Vector j(0, 1, 0);
			Vector k(0, 0, 1);

			a.normalize();
			a.print();

			angle = (pi*angle) / 180;

			Vector **c = new Vector*[3];
			c[0] = &R(i, a, angle);
			c[1] = &R(j, a, angle);
			c[2] = &R(k, a, angle);

			matrix temp = matrix::make_identity(4);
			for (int i = 0; i < 3; ++i) {
				temp.values[0][i] = c[i]->x;
				temp.values[1][i] = c[i]->y;
				temp.values[2][i] = c[i]->z;
			}
			S.push(S.top()*temp);

			temp.print();
		}
		else if (command == "push") {
			stack_of_S.push(S);
		}
		else if (command == "pop") {
			S = stack_of_S.top();
			stack_of_S.pop();
		}
		else {
			break;
		}
	}

	scene.close();
	stage1.close();

}


int main()
{
	cout << std::fixed;
	cout << std::setprecision(4);

	stage1();
	stage2();
	stage3();
	scan_convert();
	//getchar();
	return 0;
}
