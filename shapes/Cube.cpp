#include "Cube.h"
#include "Vertex.h"
#include "Triangle.h"
#include "float.h"
#include <cstddef>
#include <GL/gl.h>

Cube::Cube()
{
}

void Cube::tesselate()
{
    if( m_triangles )
    {
        delete[] m_triangles;
        m_triangles = NULL;
    }

    int trianglesPerSide = m_pOne * m_pOne * 2;
    m_triangleCount = trianglesPerSide * 6;
    m_triangles = new Triangle[m_triangleCount];
    m_triangleIndex = 0;
    initializeTriangles();

    // Front face
    drawXYFace( +0.5f );

    // Back face
    drawXYFace( -0.5f );

    // Right face
    drawYZFace( +0.5f );

    // Left face
    drawYZFace( -0.5f );

    // Top Face
    drawXZFace( +0.5f );

    // Bottom face
    drawXZFace( -0.5f );
}

void Cube::drawXYFace(float z)
{
    float triangleWidth = 1.0f / m_pOne;
    Vector currentNormal = Vector(0.0f, 0.0f, z >= 0.0 ? +1.0f : -1.0f);
    for(int r = 0; r < m_pOne; r++) {
        for(int c = 0; c < m_pOne; c++) {
            float leftX = -0.5f + c * triangleWidth;
            float bottomY = -0.5f + r * triangleWidth;
            Vertex bottomLeft, bottomRight, topLeft, topRight;
            Vector texCoord = Vector(0, 0, 0);
            // TODO: Actually compute the tex coord ^
            bottomLeft = Vertex(leftX, bottomY, z, currentNormal, texCoord);
            bottomRight = Vertex(leftX + triangleWidth, bottomY, z, currentNormal, texCoord);
            topLeft = Vertex(leftX, bottomY + triangleWidth, z, currentNormal, texCoord);
            topRight = Vertex(leftX + triangleWidth, bottomY + triangleWidth, z, currentNormal, texCoord);


            m_triangles[m_triangleIndex++] = Triangle( z >= 0 ? bottomLeft : topLeft, bottomRight, z >= 0 ? topLeft : bottomLeft);
            m_triangles[m_triangleIndex++] = Triangle( z >= 0 ? topLeft : topRight, bottomRight, z >= 0 ? topRight : topLeft);
        }
    }
}

void Cube::drawYZFace(float x)
{
    float triangleWidth = 1.0f / m_pOne;
    Vector currentNormal = Vector(x >= 0 ? +1.0f : -1.0f, 0.0f, 0.0f);
    for(int r = 0; r < m_pOne; r++) {
        for(int c = 0; c < m_pOne; c++) {
            float leftZ = -0.5f + c * triangleWidth;
            float bottomY = -0.5f + r * triangleWidth;
            Vertex bottomLeft, bottomRight, topLeft, topRight;
            bottomLeft = Vertex(x, bottomY, leftZ, currentNormal);
            bottomRight = Vertex(x, bottomY, leftZ + triangleWidth, currentNormal);
            topLeft = Vertex(x, bottomY + triangleWidth, leftZ, currentNormal);
            topRight = Vertex(x, bottomY + triangleWidth, leftZ + triangleWidth, currentNormal);
            Vector texCoord = Vector(0, 0, 0);
            // TODO: Actually compute the tex coord ^

            m_triangles[m_triangleIndex++] = Triangle( x < 0 ? bottomLeft : topLeft, bottomRight, x < 0 ? topLeft : bottomLeft);
            m_triangles[m_triangleIndex++] = Triangle( x < 0 ? topLeft : topRight, bottomRight, x < 0 ? topRight : topLeft);
        }
    }
}

void Cube::drawXZFace(float y)
{
    float triangleWidth = 1.0f / m_pOne;
    Vector currentNormal = Vector(0.0f, (y > 0.0 ? +1.0f : -1.0f), 0.0f);
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

