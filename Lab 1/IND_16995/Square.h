#pragma once
#include "Figure.h"
class Square :
    public Figure
{
public:
    Square(DPOINT rotationPoint, double sideSize, double gridSize, PEN pen, BRUSH brush, double angleOfRotation = 0, POINT mirror = { 1, 1 });
    Square(Square& s);
    virtual ~Square();

    // virtual PolyPoints GetMyPoints();
    // virtual void Draw(CDC* pDC);
};