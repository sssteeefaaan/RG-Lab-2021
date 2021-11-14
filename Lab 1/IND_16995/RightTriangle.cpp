#include "pch.h"
#include "RightTriangle.h"

RightTriangle::RightTriangle(DPOINT rotationPoint, double sideSize, double gridSize, int inscribedPolygonPointNumber, PEN penTriangle, PEN penHexagon, BRUSH brush, double angleOfRotation, POINT mirror)
	:Figure(3, rotationPoint, sideSize, gridSize, penTriangle, brush, angleOfRotation, mirror)
{
	this->SetAttributes(inscribedPolygonPointNumber, penHexagon);
}

RightTriangle::RightTriangle(RightTriangle& rt)
	:Figure(3, rt.rotationPoint, rt.sideSize, 1, rt.pen, rt.brush, rt.angleOfRotation, rt.mirror)
{
	this->SetAttributes(rt.inscribedPolygon.n, penHexagon);
}

RightTriangle::~RightTriangle()
{
	if (this->inscribedPolygon.points != nullptr)
		delete[] this->inscribedPolygon.points;
	this->inscribedPolygon.points = nullptr;
}

void RightTriangle::SetAttributes(int inscribedPolygonPointNumber, PEN penHexagon)
{
	this->penHexagon = penHexagon;

	if (this->myPoints.points != nullptr)
	{
		delete[] this->myPoints.points;
		this->myPoints.points = nullptr;
	}

	this->GetMyPoints();
	this->inscribedPolygon = this->CalculateInscribedPolygon(inscribedPolygonPointNumber);
}

PolyPoints RightTriangle::CalculateInscribedPolygon(int inscribedPolygonPointNumber)
{
	int a = Figure::GetDistance(this->myPoints.points[0], this->myPoints.points[1]),
		b = Figure::GetDistance(this->myPoints.points[1], this->myPoints.points[2]),
		c = Figure::GetDistance(this->myPoints.points[2], this->myPoints.points[0]),
		sum = a + b + c;

	DPOINT incenter =
	{
		(a * this->myPoints.points[2].x + b * this->myPoints.points[0].x + c * this->myPoints.points[1].x) / sum,
		(a * this->myPoints.points[2].y + b * this->myPoints.points[0].y + c * this->myPoints.points[1].y) / sum
	};
	double length = max(a, b, c) / 6;

	PolyPoints temp = Figure::GetPolyPoints(inscribedPolygonPointNumber, incenter, length),
		ret = { nullptr, inscribedPolygonPointNumber };

	if (temp.points != nullptr)
	{
		ret.points = new POINT[temp.n + 1];
		for (int i = 0; i < temp.n; i++)
			ret.points[i] = temp.points[i];
		ret.points[temp.n] = ret.points[0];

		delete[] temp.points;
	}

	return ret;
}

PolyPoints RightTriangle::GetMyPoints()
{
	if (this->myPoints.points == nullptr)
	{
		double r = this->sideSize * M_SQRT2 / 2;

		PolyPoints temp = this->GetPolyPoints(4, this->rotationPoint, r, this->angleOfRotation, this->mirror);

		if (temp.points != nullptr)
		{
			this->myPoints = { new POINT[3], 3 };
			for (int i = 0; i < 3; i++)
				this->myPoints.points[i] = temp.points[i];

			delete[] temp.points;
			temp.points = nullptr;
		}
	}

	return myPoints;
}
void RightTriangle::Draw(CDC* pDC)
{
	Figure::Draw(pDC);

	CPen* oldPen = pDC->SelectObject(new CPen(this->penHexagon.style, this->penHexagon.width, &this->penHexagon.log));

	pDC->Polyline(this->inscribedPolygon.points, this->inscribedPolygon.n + 1);

	delete pDC->SelectObject(oldPen);
}

void RightTriangle::Change()
{
	Figure::Change();
	this->inscribedPolygon = this->CalculateInscribedPolygon(this->inscribedPolygon.n);
}