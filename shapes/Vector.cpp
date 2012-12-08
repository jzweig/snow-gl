#include "Vector.h"

Vector::Vector()
{
}

Vector::Vector(float x, float y, float z)
{
    m_x = x;
    m_y = y;
    m_z = z;
}

Vector::~Vector()
{

}

float Vector::x() const
{
    return m_x;
}

float Vector::y() const
{
    return m_y;
}

float Vector::z() const
{
    return m_z;
}

void Vector::x(float x)
{
    m_x = x;
}

void Vector::y(float y)
{
    m_y = y;
}

void Vector::z(float z)
{
    m_z = z;
}

Vector4 Vector::toVec4() const
{
    return Vector4(m_x, m_y, m_z, 0);
}
