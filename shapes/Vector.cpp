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

Vector::Vector(float oneValue)
{
    m_x = oneValue;
    m_y = oneValue;
    m_z = oneValue;
}

Vector::~Vector()
{

}

Vector Vector::operator+(const Vector& other)
{
    return Vector(other.x() + x(), other.y() + y(), other.z() + z());
}

Vector Vector::operator*(float other)
{
    return Vector(x() * other, y() * other, z() * other);
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

float *Vector::getData()
{
    return &data[0];
}

Vector4 Vector::toVec4() const
{
    return Vector4(m_x, m_y, m_z, 0);
}
