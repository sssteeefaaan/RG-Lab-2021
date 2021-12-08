#pragma once

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")

#include <GL\gl.h>
#include <GL\glu.h>

#define _USE_MATH_DEFINES
#include <math.h>

struct COLOR3F {
	float R;
	float G;
	float B;
};

class CGLRender
{
private:
	HGLRC m_hrc;

	GLdouble *lookingAt;
	GLdouble *viewPosition;
	GLdouble *upVector;

	double viewR;
	double viewAngleXY;
	double viewAngleXZ;

	bool showGrid;
	bool showAxes;

	double pieceAngle;

	double sphereR;
	double cylinderH;
	double cylinderR;
	double coneH;
	double coneR;
	int nSeg;

public:
	CGLRender();
	~CGLRender();

	bool CreateGLContext(CDC* pDC);
	void PrepareScene(CDC* pDC);
	void DestroyScene(CDC* pDC);
	void Reshape(CDC* pDC, int w, int h);
	void DrawScene(CDC* pDC);

	void DrawFigure(double angle);
	void DrawCylinder(double h = 1, double rTop = 1, double rBottom = -1, int nSeg = 90, COLOR3F color = { .47, 0, 0 });
	void DrawCone(double h = 1, double r = 1, int nSeg = 90, COLOR3F color = { .47, 0, 0 });
	void DrawSphere(double r = 1, int nSegAlpha = 90, int nSegBeta = 90, COLOR3F color = { 1, 1, 1 });

	void DrawAxes(double lenght = 10, bool useNegative = false);
	void DrawGrid(double width = 10, double height = 10, int nSegW = 10, int nSegH = 10, COLOR3F color = { 1, .6, 0 });

	void RotateView(double dXY, double dXZ);
	void Zoom(bool out);

	inline void ToggleGrid() { this->showGrid = !this->showGrid; }
	inline void ToggleAxes() { this->showAxes = !this->showAxes; }
	inline void RotatePiece(double dAngle) { this->pieceAngle += dAngle; }
};

