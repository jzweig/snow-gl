#include "Plane.h"

Plane::Plane()
{
}

void Plane::tesselate()
{
    if( m_triangles )
    {
        delete[] m_triangles;
        m_triangles = NULL;
    }

    int trianglesPerSide = m_pOne * m_pOne * 2;
    m_triangleCount = trianglesPerSide;
    m_triangles = new Triangle[m_triangleCount];
    m_triangleIndex = 0;
    initializeTriangles();

    // Bottom face
    drawXZFace( 0.0f );
}

Vector Plane::getTexCoords(Vector4 objPos)
{
    return Vector(objPos.x + 0.5, objPos.z + 0.5, 0);
}

void Plane::drawXZFace(float y)
{
    float triangleWidth = 1.0f / m_pOne;
    Vector currentNormal = Vector(0.0f, 1.0f, 0.0f);
    for(int r = 0; r < m_pOne; r++) {
        for(int c = 0; c < m_pOne; c++) {
            float leftZ = -0.5f + c * triangleWidth;
            float bottomX = -0.5f + r * triangleWidth;
            Vertex bottomLeft, bottomRight, topLeft, topRight;
            bottomLeft = Vertex(bottomX, y, leftZ, currentNormal, Vector(bottomX + 0.5, leftZ + 0.5, 0));
            bottomRight = Vertex(bottomX, y, leftZ + triangleWidth, currentNormal, Vector(bottomX + 0.5, leftZ + triangleWidth + 0.5, 0));
            topLeft = Vertex(bottomX + triangleWidth, y, leftZ, currentNormal, Vector(bottomX + triangleWidth + 0.5, leftZ + 0.5, 0));
            topRight = Vertex(bottomX + triangleWidth, y, leftZ + triangleWidth, currentNormal, Vector(bottomX + triangleWidth + 0.5, leftZ + triangleWidth + 0.5, 0));

            m_triangles[m_triangleIndex++] = Triangle( y >= 0 ? bottomLeft : topLeft, bottomRight, y >= 0 ? topLeft : bottomLeft);
            m_triangles[m_triangleIndex++] = Triangle( y >= 0 ? topLeft : topRight, bottomRight, y >= 0 ? topRight : topLeft);
        }
    }
}
