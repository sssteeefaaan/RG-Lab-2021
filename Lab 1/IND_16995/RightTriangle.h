#pragma once
#include "Figure.h"

class RightTriangle :
    public Figure
{
private:
    PolyPoints inscribedPolygon;
    PEN penHexagon;
    void SetAttributes(int inscribedPolygonPointNumber, PEN penHexagon);
    PolyPoints CalculateInscribedPolygon(int inscribedPolygonPointNumber);

public:
    RightTriangle(DPOINT rotationPoint, double sideSize, double gridSize, int inscribedPolygonPointNumber, PEN penTriangle, PEN penHexagon, BRUSH brush, double angleOfRotation = 0, POINT mirror = { 1, 1 });
    RightTriangle(RightTriangle& rt);
    virtual ~RightTriangle();

    virtual PolyPoints GetMyPoints();
    virtual void Draw(CDC* pDC);

    inline PolyPoints GetInscribedPolygon() { return this->inscribedPolygon; }
    inline PEN GetPenHexagon() { return this->penHexagon; }

    virtual void Change();
};