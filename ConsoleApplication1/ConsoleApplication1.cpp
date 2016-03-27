// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <math.h>

using namespace std;

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define _USE_MATH_DEFINES

struct Point
{
	int x, y;
	Point() : x(0), y(0)
	{

	}

	Point(int xPos, int yPos) : x(xPos), y(yPos)
	{

	}
};

struct Triple
{
	double x, y;
	double z;
	Triple() : x(0), y(0), z(0)
	{

	}

	Triple(double xPos, double yPos, double zPos) : x(xPos), y(yPos), z(zPos)
	{

	}
};

Point Convert3D2D(Triple coords, Triple pan, Point center, Triple position);
Point Convert3D2D(Triple position, Triple cameraPosition, Triple cameraPan);
void DrawLine(Point point1, Point point2);
int sign(double x);
void DrawTri(Triple top, Triple left, Triple right);
void DrawQuad(Triple topLeft, Triple topRight, Triple bottomLeft, Triple bottomRight);
void DrawCube(Triple topLeft, Triple size);

HBITMAP backBuffer;
HDC dc;
HDC dcBack;

Triple t1 = Triple(100, 100, 1);
Triple t2 = Triple(200, 100, 1);
Triple t3 = Triple(100, 200, 1);
Triple t4 = Triple(200, 200, 1);

Triple t5 = Triple(100, 100, 100);
Triple t6 = Triple(200, 100, 101);
Triple t7 = Triple(100, 200, 101);
Triple t8 = Triple(200, 200, 101);

Triple cameraPan = Triple(45, 45, 45);
Triple cameraPosition = Triple(-840, -360, 0);

int zoom = 1;

int main()
{
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r); //stores the console's current dimensions

	MoveWindow(console, 0, 0, 1280, 720, TRUE);

	int i = 0;


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

		switch (i)
		{
		case KEY_UP:
			cameraPan.y-=0.1;
		case KEY_DOWN:
			cameraPan.x-=0.1;
		}

		cameraPosition = Triple(-840, -360, 0);

		DrawCube(t1, t5);


		BitBlt(dc, 0, 0, r.right, r.bottom, dcBack, 0, 0, SRCCOPY);

		DeleteObject(dcBack);
		DeleteObject(backBuffer);

	}

	ReleaseDC(console, dc);
	cin.ignore();

	return 0;
}

Point Convert3D2D(Triple coords, Triple pan, Point center, Triple position)
{
	Point newPoint;
	Triple newPos;

	coords.x = coords.x + position.x;
	coords.y = coords.y + position.y;
	coords.z = coords.z + position.z;

	newPos.x = coords.x * cos(pan.x) - coords.z * sin(pan.x);
	newPos.z = coords.x * sin(pan.x) + coords.z * cos(pan.x);
	newPos.y = coords.y * cos(pan.y) - coords.z * sin(pan.y);
	coords.z = newPos.y * cos(pan.y) - newPos.z * sin(pan.y);
	coords.x = newPos.x * cos(pan.z) - newPos.y * sin(pan.z);
	coords.y = newPos.x * sin(pan.z) + newPos.y * cos(pan.z);

	if (coords.z > 0)
	{
		newPoint.x = coords.x / coords.z * zoom + center.x;
		newPoint.y = coords.y / coords.z * zoom + center.y;
	}
	else
	{
		newPoint.x = coords.x * zoom + center.x;
		newPoint.y = coords.y * zoom + center.y;
	}

	return newPoint;
}

Point Convert3D2D(Triple position, Triple cameraPosition, Triple cameraPan)
{
	Triple newPos;
	Point p;

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
		p.x = cameraPosition.z / newPos.z * newPos.x - cameraPosition.x;
		p.y = cameraPosition.z / newPos.z * newPos.y - cameraPosition.y;
	}

	return p;
}

void DrawLine(Point point1, Point point2)
{
	double deltaX = point2.x - point1.x;
	double deltaY = point2.y - point1.y;
	double error = 0;
	double deltaErr = 0;
	if (deltaX != 0)
		deltaErr = abs(deltaY / deltaX);
	int y = point1.y;

	for (int x = point1.x; x < point2.x; x++)
	{
		SetPixel(dcBack, x, y, RGB(255, 0, 0));
		error += deltaErr;
		while (error >= 0.5)
		{
			SetPixel(dcBack, x, y, RGB(255, 0, 0));
			y += sign(point2.y - point1.y);
			error -= 1.0;
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

void DrawTri(Triple top, Triple left, Triple right)
{
	Point p1 = Convert3D2D(top, cameraPosition, cameraPan);
	Point p2 = Convert3D2D(left, cameraPosition, cameraPan);
	Point p3 = Convert3D2D(right, cameraPosition, cameraPan);

	if (p1.x < p2.x)
		DrawLine(p1, p2);
	else
		DrawLine(p2, p1);

	if (p1.x < p3.x)
		DrawLine(p1, p3);
	else
		DrawLine(p3, p1);

	if (p2.x < p3.x)
		DrawLine(p2, p3);
	else
		DrawLine(p3, p2);
}

void DrawQuad(Triple topLeft, Triple topRight, Triple bottomLeft, Triple bottomRight)
{
	DrawTri(topLeft, bottomLeft, bottomRight);
	DrawTri(topLeft, topRight, bottomRight);
}

void DrawCube(Triple topLeft, Triple size)
{
	Triple topRight1 = Triple(topLeft.x + size.x, topLeft.y, topLeft.z);
	Triple bottomLeft1 = Triple(topLeft.x, topLeft.y + size.y, topLeft.z);
	Triple bottomRight1 = Triple(topLeft.x + size.x, topLeft.y + size.y, topLeft.z);

	Triple topLeft2 = Triple(topLeft.x, topLeft.y, topLeft.z + size.z);
	Triple topRight2 = Triple(topLeft.x + size.x, topLeft.y, topLeft.z + size.z);
	Triple bottomLeft2 = Triple(topLeft.x, topLeft.y + size.y, topLeft.z + size.z);
	Triple bottomRight2 = Triple(topLeft.x + size.x, topLeft.y + size.y, topLeft.z + size.z);

	// Front
	DrawQuad(topLeft, topRight1, bottomLeft1, bottomRight1);

	// Back
	DrawQuad(topLeft2, topRight2, bottomLeft2, bottomRight2);

	// Left
	DrawQuad(topLeft2, topLeft, bottomLeft2, bottomLeft1);

	// Right
	DrawQuad(topRight1, topRight2, bottomRight1, bottomRight2);

	// Top
	DrawQuad(topLeft2, topRight2, topLeft, topRight1);

	// Bottom
	DrawQuad(bottomLeft1, bottomRight1, bottomLeft2, bottomRight2);

}