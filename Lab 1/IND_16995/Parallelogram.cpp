#include "pch.h"
#include "Parallelogram.h"

Parallelogram::Parallelogram(DPOINT rotationPoint, double sideSize, double gridSize, PEN pen, BRUSH brush, double angleOfRotation , POINT mirror)
	:Figure(4, rotationPoint, sideSize, gridSize, pen, brush, angleOfRotation, mirror)
{}

Parallelogram::Parallelogram(Parallelogram& p)
	:Figure(4, p.rotationPoint, p.sideSize, 1, p.pen, p.brush, p.angleOfRotation, p.mirror)
{}

Parallelogram::~Parallelogram()
{}


PolyPoints Parallelogram::GetMyPoints()
{
	if (this->myPoints.points == nullptr)
	{
		this->myPoints = { new POINT[4], 4 };

		double rotAngle = this->angleOfRotation * M_PI / 180,
			r = this->sideSize / 2;

		DPOINT centerOne =
		{
			this->rotationPoint.x - mirror.x * r * cos(rotAngle),
			this->rotationPoint.y - mirror.y * r * sin(rotAngle)
		}, centerTwo =
		{
			this->rotationPoint.x + mirror.x * r * cos(rotAngle),
			this->rotationPoint.y + mirror.y * r * sin(rotAngle)
		};

		PolyPoints p1 = GetPolyPoints(4, centerOne, r * M_SQRT2, this->angleOfRotation - 45, this->mirror),
			p2 = GetPolyPoints(4, centerTwo, r * M_SQRT2, this->angleOfRotation - 45, this->mirror);

		this->myPoints.points[0] = p1.points[0];
		this->myPoints.points[1] = p1.points[3];
		this->myPoints.points[2] = p2.points[2];
		this->myPoints.points[3] = p2.points[1];

		if (p1.points != nullptr)
		{
			delete[] p1.points;
			p1.points = nullptr;
		}

		if (p2.points != nullptr)
		{
			delete[] p2.points;
			p2.points = nullptr;
		}
	}

	return this->myPoints;
}