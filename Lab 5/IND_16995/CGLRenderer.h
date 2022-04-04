#pragma once

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")

#include <GL\gl.h>
#include <GL\glu.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include "CGLMaterial.h"


class CGLRenderer
{
private:
	int viewR;
	float viewAngleXY;
	float viewAngleXZ;

	float viewPosition[3];
	float lookingAt[3];
	float upVector[3];

	bool showAxes;
	bool showNormals;

	CGLMaterial* room;
	CGLMaterial* pedestal;
	CGLMaterial* vase;
	CGLMaterial* light;

	bool yellowLight;
	bool redLight;
	bool purpleLight;

	void CalculatePosition();
public:
	CGLRenderer();
	virtual ~CGLRenderer();

	bool CreateGLContext(CDC* pDC);
	void PrepareScene(CDC* pDC);
	void Reshape(CDC* pDC, int w, int h);
	void DrawScene(CDC* pDC);
	void DestroyScene(CDC* pDC);

	void DrawAxes(double len = 50);

	void OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	void DrawRoom(double l, double w, double h, int nStep);
	void DrawPedestal(double base = 10, int nStep = 8);
	void DrawVase(double from, double h, double rTop, double rBottom, int nStep, bool showNorms = true);
	void DrawLight(double x, double y, double z, double r, double nx, double ny, double nz, int nSize = 32);
	void SetLighting();

	void DrawCuboid(double l, double w, double h, int nStep, bool drawRoof = true, bool inverted = false);
	void DrawCylinder(double h, double r1, double r2, int nStep, bool withBase = true, bool showNormals = false);
	void DrawSphere(double r, int nStep1, int nStep2, int alphaMax = 90, int betaMax = 360);
	void DrawTorus(double r1, double r2, int segNoAlpha, int segNoBeta);

protected:
	HGLRC m_hrc;
};

