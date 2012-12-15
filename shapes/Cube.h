#ifndef CUBE_H
#define CUBE_H

#include "Shape.h"

/**
 * A tesselated cube.
 */

class Cube : public Shape
{
    public:
        Cube();
        virtual Vector getTexCoords(Vector4 objPos);
        virtual void tesselate();

    protected:
        void drawXYFace(float z);
        void drawYZFace(float x);
        void drawXZFace(float y);
};

#endif // CUBE_H
