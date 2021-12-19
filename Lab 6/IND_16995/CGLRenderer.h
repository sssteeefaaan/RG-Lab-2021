#pragma once

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")

#include <GL\gl.h>
#include <GL\glu.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include "CGLMaterial.h"
#include "CGLTexture.h"


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

	CGLMaterial* platform;
	CGLMaterial* truck;
	CGLMaterial* wheels;

	CGLTexture* texGrass;
	CGLTexture* texAtlas;

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

	void SetLighting();
	void DrawPlatform(double width = 100, double lenght = 100, int repS = 3, int repT = 3);
	void DrawTruck();
	void DrawTruckBody(double a, double texA, double dS, double dT);

	void DrawCylinder(double h, double rTop, double rBottom, int nStep, double s, double t, double rTex);
	void DrawSphere(double r, int nStepAlpha = 16, int nStepBeta = 16, double width = 1, double height = .5);

protected:
	HGLRC m_hrc;
};

