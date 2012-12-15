#ifndef PLANE_H
#define PLANE_H

#include "Shape.h"

class Plane : public Shape
{
public:
    Plane();
    virtual void tesselate();
protected:
    void drawXZFace(float y);
};

#endif // PLANE_H
