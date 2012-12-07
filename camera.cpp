#include "camera.h"
#include <qgl.h>
#include <iostream>

using namespace std;

void OrbitCamera::mouseMove(const Vector2 &delta)
{
    // Rotate the eye vector around the origin
    theta += delta.x * 0.01f;
    phi += delta.y * 0.01f;

    // Keep theta in [0, 2pi] and phi in [-pi/2, pi/2]
    theta -= floorf(theta / M_2PI) * M_2PI;
    phi = max((float) (0.01f - M_PI / 2), min((float) M_PI / 2 - 0.01f, phi));
}

void OrbitCamera::mouseWheel(float delta)
{
    center = center - Vector4::fromAngles(theta, phi)  * delta;
}

const Vector4 OrbitCamera::getDirection()
{
    return -Vector4::fromAngles(theta, phi);
}

const Vector4 OrbitCamera::getUp()
{
    return -Vector4::fromAngles(theta, phi+M_PI/2);
}

void OrbitCamera::lookAt(const Vector4 &point)
{
    /**
     * Spherical coordinate! http://en.wikipedia.org/wiki/Spherical_coordinate_system#Cartesian_coordinates
     * note, we are using y as the up vector as opposed to z which changes the equation slightly
     */
    Vector4 dir = point.getNormalized();
    phi = asinf(dir.y);
    theta = atanf(dir.z/dir.x);
}

