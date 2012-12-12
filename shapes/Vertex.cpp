#include "Vertex.h"

Vertex::Vertex()
{
    m_w = 1.0f;
}

Vertex::Vertex(float x, float y, float z, Vector normal, Vector texCoord) : m_normal(normal), m_texCoord(texCoord)
{
    m_x = x;
    m_y = y;
    m_z = z;
    m_w = 1.0f;
}

Vertex::Vertex(float x, float y, float z, Vector normal) : m_normal(normal)
{
    m_x = x;
    m_y = y;
    m_z = z;
    m_w = 1.0f;
}

float Vertex::w()
{
    return m_w;
}

void Vertex::w(float w)
{
    m_w = w;
}

Vector *Vertex::getNormal()
{
    return &m_normal;
}

Vector *Vertex::getTexCoord()
{
    return &m_texCoord;
}
