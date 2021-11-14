#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

struct DPOINT {
	double x;
	double y;
};

struct PolyDPoints {
	DPOINT* points;
	int n;
};

struct PolyPoints {
	POINT* points;
	int n;
};

struct PEN {
	int style;
	int width;
	LOGBRUSH log;
};

struct BRUSH {
	int hatch;
	COLORREF color;
	COLORREF backColor;
};

class Figure
{
protected:
	PolyPoints myPoints;
	DPOINT rotationPoint;
	double angleOfRotation;
	double sideSize;
	POINT mirror;
	PEN pen;
	BRUSH brush;

protected:
	void SetAttributes(int points, DPOINT rotationPoint, double sideSize, double gridSize, PEN pen, BRUSH brush, double angleOfRotation = 0, POINT mirror = { 1, 1 });

public:
	Figure(int points, DPOINT rotationPoint, double sideSize, double gridSize, PEN pen, BRUSH brush, double angleOfRotation = 0, POINT mirror = { 1, 1 });
	Figure(Figure&);
	virtual ~Figure();

	inline DPOINT GetRotationPoint() { return this->rotationPoint; }
	inline double GetAngleOfRotation() { return this->angleOfRotation; }
	inline double GetSideSize() { return this->sideSize; }
	inline POINT GetMirrorPoint() { return this->mirror; }
	inline PEN GetPen() { return this->pen; }
	inline BRUSH GetBrush() { return this->brush; }

	virtual PolyPoints GetMyPoints();
	virtual void Draw(CDC* pDC);

	static PolyPoints GetPolyPoints(int n, DPOINT center, double r, double fi = 0, POINT mirror = { 1, 1 });
	static double GetDistance(POINT a, POINT b);

	virtual PEN Select(PEN p);
	virtual void Move(DPOINT diff);
	virtual void Rotate(double fi);
	virtual void Mirror(POINT mirror);
	virtual void Change();
};

