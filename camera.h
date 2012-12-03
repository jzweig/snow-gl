#ifndef CAMERA_H
#define CAMERA_H

#include <QMouseEvent>
#include "CS123Common.h"
#include "CS123Algebra.h"

#ifndef M_PI
#define M_PI 3.14159265f
#endif

#ifndef M_2PI
#define M_2PI (2 * M_PI)
#endif

/**
    An orbiting perspective camera specified by a center, two angles, and a zoom factor

    @author: Justin Ardini (jardini)
**/
struct OrbitCamera
{
    Vector4 eye, center, up;
    float angle, near, far;

    //Vector4 center, up;
    float theta, phi;
    float fovy;
    float zoom;

    void mouseMove(const Vector2 &delta);
    void mouseWheel(float delta);
    void lookAt(const Vector4 &point);
};

#endif // CAMERA_H
