/**
 * 3407ICT Graphics Programming
 * Tutorial Graphics Rendering Framework
 * @author Nathan Hoad (nathan@getoffmalawn.com)
 */
#include "Assignment1.h"
#include "ObjectLoader.h"
#include "Transformation.h"
#include "Clipper.h"

#include <algorithm>
using std::sort;

#include <cassert>

#define round(x) (int)( x + 0.5)

#if defined(__MACH__) && defined(__APPLE__)
// Allow SDL main hack, because of the OS X Cocoa binding
#else
// SDL main hackery disabled for Windows and Linux
#define SDL_main main
#endif

// SDL's glext.h version disabled
#define NO_SDL_GLEXT

// http://www.libsdl.org/
#include "SDL.h"
// http://www.libsdl.org/projects/SDL_ttf/
#include "SDL_ttf.h"
// http://glew.sourceforge.net/
#include "glew.h"

#include "HPTime.h"

#include <iostream>

using namespace std;

///
/// Program Entry
///
int main(int argc, char* argv[])
{

	Assignment1 example;
	example.start();
	return 0;
}

void Assignment1::preprocess()
{
	// build the cube
	ObjectLoader loader;

	loader.read("Assets/Cube.obj");

	cube.faces = loader.object();
	//mesh = loader.object();
	cube.scale = 0.125;

	cube.x = 0;
	cube.y = 0;
	angle = 15.0;
	cube.speed = 10;

	xInc = 1 / cube.speed;
	yInc = ((float) width / height) / cube.speed;

	// load identity matrix
	projection = new Mat4();

	Mat4 perspectiveMatrix = Mat4::perspectiveFrustum(45.0, ((float) width / height), 1.0, 20.0);
	(*projection) = Mat4::mul((*projection), perspectiveMatrix);
	//gluPerspective(45.0, ((float)width / height), 1.0, 20.0);

	modelview = new Mat4();

	camera = new Vec4(0.0, 0.0, 3.5);
	target = new Vec4(0.0, 0.0, 0.0);
	up = new Vec4(0.0, 1.0, 0.0);

	//(*modelview) = Mat4::mul((*modelview), Mat4::lookAt(*camera, *target, *up));
	//gluLookAt(0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void Assignment1::drawCube(Cube cube)
{
	float x = cube.centreX();
	float y = cube.centreY();
	float z = cube.centreZ();

	float scale = cube.scale;
	float normScale = (scale * 2) / scale;

	float curX = (cube.x / width) * normScale * 2 - normScale;
	float curY = (cube.y / height) * normScale * 2 - normScale;

	Mat4 model;

	Mat4::mul(model, Mat4::translate(curX, curY, -z));
	Mat4::mul(model, Mat4::scale(scale, scale, scale));

	Mat4::mul(model, Mat4::rotateX(angle));
	Mat4::mul(model, Mat4::rotateY(angle));
	Mat4::mul(model, Mat4::rotateZ(angle));

	Mat4::mul(model, Mat4::translate(-x, -y, -z));

	(*modelview) = Mat4::mul(model, Mat4::lookAt(*camera, *target, *up));

	cout << "model: " << model << endl;

	Mat4 modelViewPerspective = Mat4::mul(*projection, *modelview);

	for (unsigned i=0; i < cube.faces.size(); i++)
	{
		Face currentFace = cube.faces[i];
		Face newFace;
		vector<Vec4> vec4Face;

		for (unsigned j=0; j < currentFace.size(); j++)
		{
			// change each Vertex to a Vec4.
			Vertex v = currentFace[j];
			Vec4 vec(v(0), v(1), v(2), v(3));
			vec4Face.push_back(vec);

			// get the NDC
			vec = Mat4::mul(modelViewPerspective, vec);

			// normalised x, y and z against w
	/*		v(0) /= v(3);
			v(1) /= v(3);
			v(2) /= v(3);*/

			cout << v(3) << endl << endl;

			int x = (v(0) + 1) * (width / 2);
			int y = height - ((v(1) + 1) * (height / 2));

			int r = currentFace[i](4) * 255;
			int g = currentFace[i](5) * 255;
			int b = currentFace[i](6) * 255;

			newFace.push_back(Vertex(x, y, 0, 0, r, g, b, 1));
		}

		drawPolygon(newFace);
	}

	// multiply view (lookat) by the model to get modelview matrix
	// multiply perspective by modelview to get your model view perspective matrix

	// for loop for each face of the cube
	// store each vector as a Vec4
	// multiply modelview perspective by each vec4 for the NDC
	// divide each point in the NDC by w for homogeneous normalised points
	// use the NDC to get the actual pixels.
	// get the colours from the original shape
	// make a new vertex using the colours and x and y values.
	// add that vertex to a new face to preserve the original faces.
	// draw the polygon using the face.
}

void Assignment1::moveCube(Cube & cube)
{
	angle += elapsedTime * 100;

	if (cube.y >= height || cube.y < 0)
		yInc = -yInc;

	if (cube.x >= width || cube.x < 0)
		xInc = -xInc;

	cube.x += xInc;
	cube.y += yInc;
}

void Assignment1::drawPolygon(vector<Vertex> polygon)
{
	vector<Vertex> clipped = Clipper::clip(polygon, width, height);
	vector<Vertex> decomposed = decompose(clipped);

	for (unsigned i = 0; i < decomposed.size() -2; i+=3)
		triangle(decomposed[i], decomposed[i+1], decomposed[i+2]);
}

void Assignment1::triangle(Vertex a, Vertex b, Vertex c)
{
	// first, we sort the vertices on the Y axis, using sort from algorithm library.
	vector<Vertex> sorted;

	sorted.push_back(a);
	sorted.push_back(b);
	sorted.push_back(c);

	sort(sorted.begin(), sorted.end(), compareOnY);

	a = sorted[0];
	b = sorted[1];
	c = sorted[2];

	// we'll make all the edges now rather than littering them through code.
	vector<Vertex> ab_edge = makeLine(a, b);
	vector<Vertex> bc_edge = makeLine(b, c);
	vector<Vertex> ac_edge = makeLine(a, c);

	vector<Vertex> l_edge, r_edge;
	l_edge = ab_edge;
	r_edge = ac_edge;

	// find the left and right edges by comparing x values.
	if (b(0) < c(0))
	{

		if (l_edge.size() < r_edge.size())
		{
			for (unsigned i = 0; i < bc_edge.size(); i++)
				l_edge.push_back(bc_edge[i]);
		}
		else
		{
			for (unsigned i = 0; i < bc_edge.size(); i++)
				r_edge.push_back(bc_edge[i]);
		}
	}
	else
	{
		swap(l_edge, r_edge);

		if (l_edge.size() < r_edge.size())
		{
			for (unsigned i = 0; i < bc_edge.size(); i++)
				l_edge.push_back(bc_edge[i]);
		}
		else
		{
			for (unsigned i = 0; i < bc_edge.size(); i++)
				r_edge.push_back(bc_edge[i]);
		}
	}

	// now, let's paint it.
	for (unsigned i=0; i < l_edge.size();i++)
		scanLine(l_edge[i], r_edge[i]);
}

vector<Vertex> Assignment1::makeLine(Vertex a, Vertex b)
{
	vector<Vertex> result;

	double x = a(0);

	double red = a(4);
	double green = a(5);
	double blue = a(6);

	double dy = b(1) - a(1);
	double dx = b(0) - a(0);

	double slope = dy / dx;

	double r_inc = (double) (b(4) - a(4)) / dy;
	double g_inc = (double) (b(5) - a(5)) / dy;
	double b_inc = (double) (b(6) - a(6)) / dy;

	for (int y = round(a(1)); y < b(1); y++)
	{
		result.push_back(Vertex(round(x), y, 0, 0, round(red), round(green), round(blue), 1));

		red += r_inc;
		green += g_inc;
		blue += b_inc;
		x += 1 / slope;
	}
	return result;
}

void Assignment1::scanLine(Vertex a, Vertex b)
{
	double red = a(4);
	double green = a(5);
	double blue = a(6);
	double dx = b(0) - a(0);

	if (a(0) > b(0))
		swap<Vertex>(a, b);

	double r_inc = (double) (b(4) - a(4)) / dx;
	double g_inc = (double) (b(5) - a(5)) / dx;
	double b_inc = (double) (b(6) - a(6)) / dx;

	int x = a(0);
	while (x <= b(0))
	{
		putpixel(x, a(1), round(red), round(green), round(blue));
		x++;

		red += r_inc;
		blue += b_inc;
		green += g_inc;
	}
}

vector<Vertex> Assignment1::decompose(vector<Vertex> & polygon)
{
	vector<Vertex> result;

	assert(polygon.size() >= 3);

	for (unsigned i=1; i < polygon.size() -1; i++)
	{
		result.push_back(polygon[0]);
		result.push_back(polygon[i]);
		result.push_back(polygon[i+1]);
	}

	return result;
}

void Assignment1::render()
{
	// Draw Objects here

    SDL_FillRect(buffer, NULL, 0);

    SDL_LockSurface(buffer);
    /////////////////////////////////////
    // Draw objects here

	drawCube(cube);
	moveCube(cube);

    /////////////////////////////////////
    SDL_UnlockSurface(buffer);
    // Flip the buffers
    SDL_Flip(buffer);
}
