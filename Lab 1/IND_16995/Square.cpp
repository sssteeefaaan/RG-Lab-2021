#include "pch.h"
#include "Square.h"

Square::Square(DPOINT rotationPoint, double sideSize, double gridSize, PEN pen, BRUSH brush, double angleOfRotation, POINT mirror)
	:Figure(4, rotationPoint, sideSize, gridSize, pen, brush, angleOfRotation, mirror)
{}

Square::Square(Square& s)
	:Figure(4, s.rotationPoint, s.sideSize, 1, s.pen, s.brush, s.angleOfRotation, s.mirror)
{}

Square::~Square()
{}