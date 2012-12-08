#include "Triangle.h"
#include <GL/gl.h>

Triangle::Triangle()
{
}

Triangle::~Triangle()
{
}

Triangle::Triangle(Vertex one, Vertex two, Vertex three)
{
    m_pointOne = one;
    m_pointTwo = two;
    m_pointThree = three;
}

void Triangle::render()
{
    const Vector *vec = m_pointOne.getNormal();
    glNormal3f(vec->x(), vec->y(), vec->z());
    glVertex3f(m_pointOne.x(), m_pointOne.y(), m_pointOne.z());

    vec = m_pointTwo.getNormal();
    glNormal3f(vec->x(), vec->y(), vec->z());
    glVertex3f(m_pointTwo.x(), m_pointTwo.y(), m_pointTwo.z());

    vec = m_pointThree.getNormal();
    glNormal3f(vec->x(), vec->y(), vec->z());
    glVertex3f(m_pointThree.x(), m_pointThree.y(), m_pointThree.z());
}

Vertex *Triangle::getVertexOne()
{
    return &m_pointOne;
}

Vertex *Triangle::getVertexTwo()
{
    return &m_pointTwo;
}

Vertex *Triangle::getVertexThree()
{
    return &m_pointThree;
}
