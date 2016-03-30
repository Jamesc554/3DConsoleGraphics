/// 3D Graphics Engine using:
/// Perspective 3D Projection: https://en.wikipedia.org/wiki/3D_projection#Perspective_projection
/// Bresenham's Line Algorithm: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
/// Author: James 'GammaS' Carter

#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <math.h>

using namespace std;

// TODO -- Fix issue where program will crash, when varibles were set to doubles not ints
struct Vector2
{
	int x, y;
	Vector2() : x(0), y(0)
	{

	}

	Vector2(int xPos, int yPos) : x(xPos), y(yPos)
	{

	}

	void Zero()
	{
		x = 0;
		y = 0;
	}
};

struct Vector3
{
	double x, y;
	double z;
	Vector3() : x(0), y(0), z(0)
	{

	}

	Vector3(double xPos, double yPos, double zPos) : x(xPos), y(yPos), z(zPos)
	{

	}

	void Zero()
	{
		x = 0;
		y = 0;
		z = 0;
	}
};

struct Vertex
{
	Vector3 positionA, positionB;
	Vector2 pointA, pointB;
	Vertex()
	{
		positionA = Vector3();
		positionB = Vector3();
	}

	Vertex(Vector3 point1, Vector3 point2) : positionA(point1), positionB(point2)
	{

	}
};

Vector2 Convert3D2D(Vector3 position, Vector3 cameraPosition, Vector3 cameraPan, int angle);
void DrawVertex(Vertex vertex, int angle, COLORREF colour);
void DrawLine(Vector2 point1, Vector2 point2, COLORREF colour);
int sign(double x);
void DrawCube(Vector3 topLeft, Vector3 size, int angle);

HBITMAP backBuffer;
HDC dc;
HDC dcBack;

Vector3 cubeTopLeft = Vector3(100, 100, 1);
Vector3 cubeTopLeft2 = Vector3(300, 100, 1);
Vector3 cubeSize = Vector3(100, 100, 100);

Vector3 cameraPan = Vector3(0, 0, 0);
Vector3 camPos = Vector3(-640, -360, 0);
Vector3 cameraPosition = Vector3(-640, -360, 0);

int main()
{
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r); //stores the console's current dimensions

	MoveWindow(console, 0, 0, 1280, 720, TRUE);

	int i = 0;
	int angle1 = 0;
	int angle2 = 0;

	while (1 == 1)
	{
		dc = GetDC(console);
		dcBack = CreateCompatibleDC(dc);
		GetClientRect(console, &r);
		backBuffer = CreateCompatibleBitmap(dc, r.right, r.bottom);
		SelectObject(dcBack, backBuffer);
		FloodFill(dcBack, 0, 0, RGB(0, 0, 0));

		i = 0;

		i = _getch();

		if (i == 72)
			angle1 += 5;
		else if (i == 80)
			angle2 += 5;
		else if (i == 75)
			cameraPan.z -= 5;
		else if (i == 77)
			camPos.x -= 5;

		cameraPosition = camPos;

		DrawCube(cubeTopLeft, cubeSize, angle1);
		DrawCube(cubeTopLeft2, cubeSize, angle2);

		BitBlt(dc, 0, 0, r.right, r.bottom, dcBack, 0, 0, SRCCOPY);

		DeleteObject(dcBack);
		DeleteObject(backBuffer);
	}

	ReleaseDC(console, dc);
	cin.ignore();

	return 0;
}

Vector2 Convert3D2D(Vector3 position, Vector3 cameraPosition, Vector3 cameraPan, int angle)
{
	Vector3 newPos;
	Vector2 p;

	cameraPan.x = (angle - cameraPan.x) * 0.0174533;
	cameraPan.y = (angle - cameraPan.y) * 0.0174533;
	cameraPan.z = (angle - cameraPan.z) * 0.0174533;

	newPos.x = cos(cameraPan.y) * (sin(cameraPan.z) * position.y + cos(cameraPan.z) * position.x) - sin(cameraPan.y) * position.z;
	newPos.y = sin(cameraPan.x) * (cos(cameraPan.y) * position.z + sin(cameraPan.y) * (sin(cameraPan.z) * position.y + cos(cameraPan.z) * position.x)) + cos(cameraPan.x) * (cos(cameraPan.z) * position.y - sin(cameraPan.z) * position.x);
	newPos.z = cos(cameraPan.x) * (cos(cameraPan.y) * position.z + sin(cameraPan.y) * (sin(cameraPan.z) * position.y + cos(cameraPan.z) * position.x)) - sin(cameraPan.x) * (cos(cameraPan.z) * position.y - sin(cameraPan.z) * position.x);

	if (cameraPosition.z == 0)
	{
		p.x = newPos.x - cameraPosition.x;
		p.y = newPos.y - cameraPosition.y;
	}
	else
	{
		p.x = int(cameraPosition.z / newPos.z * newPos.x - cameraPosition.x);
		p.y = int(cameraPosition.z / newPos.z * newPos.y - cameraPosition.y);
	}

	return p;
}

void DrawVertex(Vertex vertex, int angle, COLORREF colour)
{
	Vector2 pointA = Convert3D2D(vertex.positionA, cameraPosition, cameraPan, angle);
	Vector2 pointB = Convert3D2D(vertex.positionB, cameraPosition, cameraPan, angle);

	if (pointA.x < pointB.x)
		DrawLine(pointA, pointB, colour);
	else if (pointB.x < pointA.x)
		DrawLine(pointB, pointA, colour);
	else
	{
		if (pointA.y < pointB.y)
			DrawLine(pointA, pointB, colour);
		else
			DrawLine(pointB, pointA, colour);
	}
}

void DrawLine(Vector2 point1, Vector2 point2, COLORREF colour)
{
	double deltaX = point2.x - point1.x;
	double deltaY = point2.y - point1.y;
	double error = 0;
	double deltaErr = abs(deltaY / deltaX);
	int y = point1.y;

	if (point1.x == point2.x)
	{
		for (int y = point1.y; y < point2.y; y++)
		{
			SetPixel(dcBack, point1.x, y, colour);
		}
	}
	else
	{
		for (int x = point1.x; x < point2.x; x++)
		{
			SetPixel(dcBack, x, y, colour);
			error += deltaErr;
			while (error >= 0.5)
			{
				SetPixel(dcBack, x, y, colour);
				y += sign(point2.y - point1.y);
				error -= 1.0;
			}
		}
	}
}

int sign(double x)
{
	if (x < 0)
		return -1;
	else if (x == 0)
		return 0;
	else
		return 1;
}

void DrawCube(Vector3 topLeft, Vector3 size, int angle)
{
	Vertex vertices[12];

	Vector3 topRight = Vector3(topLeft.x + size.x, topLeft.y, topLeft.z);
	Vector3 bottomLeft = Vector3(topLeft.x, topLeft.y + size.y, topLeft.z);
	Vector3 bottomRight = Vector3(topLeft.x + size.x, topLeft.y + size.y, topLeft.z);

	Vector3 topLeft2 = Vector3(topLeft.x, topLeft.y, topLeft.z + size.z);
	Vector3 topRight2 = Vector3(topLeft.x + size.x, topLeft.y, topLeft.z + size.z);
	Vector3 bottomLeft2 = Vector3(topLeft.x, topLeft.y + size.y, topLeft.z + size.z);
	Vector3 bottomRight2 = Vector3(topLeft.x + size.x, topLeft.y + size.y, topLeft.z + size.z);

	vertices[0] = Vertex(topLeft, topRight);
	vertices[1] = Vertex(topLeft, bottomLeft);
	vertices[2] = Vertex(topRight, bottomRight);
	vertices[3] = Vertex(bottomLeft, bottomRight);

	vertices[4] = Vertex(topLeft2, topRight2);
	vertices[5] = Vertex(topLeft2, bottomLeft2);
	vertices[6] = Vertex(topRight2, bottomRight2);
	vertices[7] = Vertex(bottomLeft2, bottomRight2);

	vertices[8] = Vertex(topLeft, topLeft2);
	vertices[9] = Vertex(topRight, topRight2);
	vertices[10] = Vertex(bottomLeft, bottomLeft2);
	vertices[11] = Vertex(bottomRight, bottomRight2);

	for (int i = 0; i < 12; i++)
	{
		DrawVertex(vertices[i], angle, RGB((i * 20), 150, 0));
	}
}