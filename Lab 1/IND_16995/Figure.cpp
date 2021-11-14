#include "pch.h"
#include "Figure.h"

Figure::Figure(int points, DPOINT rotationPoint, double sideSize, double gridSize, PEN pen, BRUSH brush, double angleOfRotation, POINT mirror)
{
	this->SetAttributes(points, rotationPoint, sideSize, gridSize, pen, brush, angleOfRotation, mirror);
}

Figure::Figure(Figure& f)
{
	this->SetAttributes(f.myPoints.n, f.rotationPoint, f.sideSize, 1, f.pen, f.brush, f.angleOfRotation, f.mirror);
	if (this->myPoints.points != nullptr)
		delete[] this->myPoints.points;

	this->myPoints.points = new POINT[f.myPoints.n];

	for (int i = 0; i < f.myPoints.n; i++)
		this->myPoints.points[i] = f.myPoints.points[i];
}

Figure::~Figure()
{
	if (myPoints.points != nullptr)
	{
		delete[] myPoints.points;
		myPoints.points = nullptr;
	}
}

void Figure::SetAttributes(int points, DPOINT rotationPoint, double sideSize, double gridSize, PEN pen, BRUSH brush, double angleOfRotation, POINT mirror)
{
	this->myPoints = { nullptr, points };
	this->rotationPoint = { rotationPoint.x * gridSize, rotationPoint.y * gridSize };
	this->sideSize = sideSize * gridSize;
	this->pen = pen;
	this->brush = brush;
	this->angleOfRotation = angleOfRotation;
	this->mirror = mirror;
}

PolyPoints Figure::GetPolyPoints(int n, DPOINT center, double r, double fi0, POINT mirror)
{
	PolyPoints ret = { new POINT[n], n };
	double dw = 2 * M_PI / n,
		fi = fi0 * M_PI / 180;

	for (int i = 0; i < n; i++)
	{
		ret.points[i].x = int(ceil(center.x + mirror.x * (r * cos(i * dw + fi))));
		ret.points[i].y = int(ceil(center.y + mirror.y * (r * sin(i * dw + fi))));
	}

	return ret;
}

PolyPoints Figure::GetMyPoints()
{
	if (this->myPoints.points == nullptr)
		this->myPoints = Figure::GetPolyPoints(this->myPoints.n, this->rotationPoint, this->sideSize * M_SQRT2 / 2, this->angleOfRotation, this->mirror);

	return this->myPoints;
}

void Figure::Draw(CDC* pDC)
{
	CBrush* oldBrush = nullptr;
	CPen* oldPen = pDC->SelectObject(new CPen(this->pen.style, this->pen.width, &this->pen.log));
	COLORREF bkColor;

	if (this->brush.hatch != -1)
	{
		oldBrush = pDC->SelectObject(new CBrush(this->brush.hatch, this->brush.color));
		bkColor = pDC->SetBkColor(this->brush.backColor);
	}
	else
		oldBrush = pDC->SelectObject(new CBrush(this->brush.color));

	if (this->myPoints.points == nullptr)
		this->GetMyPoints();

	pDC->Polygon(myPoints.points, myPoints.n);

	if (this->brush.hatch != -1)
		pDC->SetBkColor(bkColor);

	delete pDC->SelectObject(oldBrush);
	delete pDC->SelectObject(oldPen);
}

double Figure::GetDistance(POINT a, POINT b) { return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)); }

void Figure::Move(DPOINT diff)
{
	this->rotationPoint.x += diff.x;
	this->rotationPoint.y += diff.y;

	this->Change();
}
void Figure::Rotate(double fi)
{
	this->angleOfRotation += fi;

	this->Change();
}
void Figure::Mirror(POINT mirror)
{
	this->mirror.x *= mirror.x;
	this->mirror.y *= mirror.y;

	this->Change();
}
void Figure::Change()
{
	if (this->myPoints.points != nullptr)
	{
		delete[] this->myPoints.points;
		this->myPoints.points = nullptr;
	}

	this->GetMyPoints();
}
PEN Figure::Select(PEN pen)
{
	PEN copy = this->pen;
	this->pen = pen;
	return copy;
}