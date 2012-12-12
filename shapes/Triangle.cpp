#include "Triangle.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

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
    const Vector *texVec = m_pointOne.getTexCoord();
    glNormal3f(vec->x(), vec->y(), vec->z());
    glTexCoord2f(texVec->x(), texVec->y());
    glVertex3f(m_pointOne.x(), m_pointOne.y(), m_pointOne.z());

    vec = m_pointTwo.getNormal();
    texVec = m_pointTwo.getTexCoord();
    glNormal3f(vec->x(), vec->y(), vec->z());
    glTexCoord2f(texVec->x(), texVec->y());
    glVertex3f(m_pointTwo.x(), m_pointTwo.y(), m_pointTwo.z());

    vec = m_pointThree.getNormal();
    texVec = m_pointThree.getTexCoord();
    glNormal3f(vec->x(), vec->y(), vec->z());
    glTexCoord2f(texVec->x(), texVec->y());
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
