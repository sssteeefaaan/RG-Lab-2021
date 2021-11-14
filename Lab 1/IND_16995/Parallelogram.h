#pragma once
#include "Figure.h"
class Parallelogram :
    public Figure
{
public:
    Parallelogram(DPOINT rotationPoint, double sideSize, double gridSize, PEN pen, BRUSH brush, double angleOfRotation = 0, POINT mirror = { 1, 1 });
    Parallelogram(Parallelogram& p);
    virtual ~Parallelogram();

    virtual PolyPoints GetMyPoints();
    // virtual void Draw(CDC* pDC);
};

