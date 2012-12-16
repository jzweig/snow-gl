#ifndef PLANE_H
#define PLANE_H

#include "Shape.h"

class Plane : public Shape
{
public:
    Plane();
    virtual void tesselate();
    virtual Vector getTexCoords(Vector4 objPos);
protected:
    void drawXZFace(float y);
};

#endif // PLANE_H
