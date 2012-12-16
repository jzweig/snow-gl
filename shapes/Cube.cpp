#include "Cube.h"
#include "Vertex.h"
#include "Triangle.h"
#include "float.h"
#include <cstddef>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

Cube::Cube()
{
}

Vector Cube::getTexCoords(Vector4 objPos)
{
    // Which face is closest?
    float topDiff = 0.5 - objPos.y;
    float bottomDiff = 0.5 + objPos.y;
    float leftDiff = 0.5 + objPos.x;
    float rightDiff = 0.5 - objPos.x;
    float frontDiff = 0.5 + objPos.z;
    float backDiff = 0.5 - objPos.z;

    float min = MIN(MIN(topDiff, bottomDiff), MIN(MIN(leftDiff, rightDiff), MIN(frontDiff, backDiff)));
    if( min == topDiff ) {
        return Vector(objPos.x + 0.5, objPos.z + 0.5, 0)*(1.0/6.0);
    } else if (min == bottomDiff) {
        return Vector(objPos.x + 0.5, objPos.z + 0.5, 0)*(1.0/6.0) + Vector(1.0/6.0);
    } else if (min == leftDiff) {
        return Vector(objPos.z + 0.5, objPos.y + 0.5, 0)*(1.0/6.0) + Vector(3.0/6.0);
    } else if (min == rightDiff) {
        return Vector(objPos.z + 0.5, objPos.y + 0.5, 0)*(1.0/6.0) + Vector(2.0/6.0);
    } else if (min == frontDiff) {
        return Vector(objPos.x + 0.5, objPos.y + 0.5, 0)*(1.0/6.0) + Vector(5.0/6.0);
    } else {
        // Back
        return Vector(objPos.x + 0.5, objPos.y + 0.5, 0)*(1.0/6.0) + Vector(4.0/6.0);
    }


    return Vector(0);
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
    Vector texCoordOffset = Vector( z >= 0 ? 4.0/6.0 : 5.0/6.0 );
    Vector currentNormal = Vector(0.0f, 0.0f, z >= 0.0 ? +1.0f : -1.0f);
    for(int r = 0; r < m_pOne; r++) {
        for(int c = 0; c < m_pOne; c++) {
            float leftX = -0.5f + c * triangleWidth;
            float bottomY = -0.5f + r * triangleWidth;
            Vertex bottomLeft, bottomRight, topLeft, topRight;
            bottomLeft = Vertex(leftX, bottomY, z, currentNormal, Vector(leftX+0.5, bottomY+0.5, 0)*(1.0/6.0)+texCoordOffset);
            bottomRight = Vertex(leftX + triangleWidth, bottomY, z, currentNormal, Vector(leftX + triangleWidth+0.5, bottomY+0.5, 0)*(1.0/6.0)+texCoordOffset);
            topLeft = Vertex(leftX, bottomY + triangleWidth, z, currentNormal, Vector(leftX+0.5, bottomY + triangleWidth + 0.5, 0)*(1.0/6.0)+texCoordOffset);
            topRight = Vertex(leftX + triangleWidth, bottomY + triangleWidth, z, currentNormal, Vector(leftX + triangleWidth + 0.5, bottomY + triangleWidth + 0.5, 0)*(1.0/6.0)+texCoordOffset);


            m_triangles[m_triangleIndex++] = Triangle( z >= 0 ? bottomLeft : topLeft, bottomRight, z >= 0 ? topLeft : bottomLeft);
            m_triangles[m_triangleIndex++] = Triangle( z >= 0 ? topLeft : topRight, bottomRight, z >= 0 ? topRight : topLeft);
        }
    }
}

void Cube::drawYZFace(float x)
{
    float triangleWidth = 1.0f / m_pOne;
    Vector texCoordOffset = Vector( x >= 0 ? 2.0/6.0 : 3.0/6.0 );
    Vector currentNormal = Vector(x >= 0 ? +1.0f : -1.0f, 0.0f, 0.0f);
    for(int r = 0; r < m_pOne; r++) {
        for(int c = 0; c < m_pOne; c++) {
            float leftZ = -0.5f + c * triangleWidth;
            float bottomY = -0.5f + r * triangleWidth;
            Vertex bottomLeft, bottomRight, topLeft, topRight;
            bottomLeft = Vertex(x, bottomY, leftZ, currentNormal, Vector(leftZ + 0.5, bottomY + 0.5, 0)*(1.0/6.0)+texCoordOffset);
            bottomRight = Vertex(x, bottomY, leftZ + triangleWidth, currentNormal, Vector(leftZ + 0.5 + triangleWidth, bottomY + 0.5, 0)*(1.0/6.0)+texCoordOffset);
            topLeft = Vertex(x, bottomY + triangleWidth, leftZ, currentNormal, Vector(leftZ + 0.5, bottomY + triangleWidth + 0.5, 0)*(1.0/6.0)+texCoordOffset);
            topRight = Vertex(x, bottomY + triangleWidth, leftZ + triangleWidth, currentNormal, Vector(leftZ + triangleWidth + 0.5, bottomY + triangleWidth + 0.5, 0)*(1.0/6.0)+texCoordOffset);

            m_triangles[m_triangleIndex++] = Triangle( x < 0 ? bottomLeft : topLeft, bottomRight, x < 0 ? topLeft : bottomLeft);
            m_triangles[m_triangleIndex++] = Triangle( x < 0 ? topLeft : topRight, bottomRight, x < 0 ? topRight : topLeft);
        }
    }
}

void Cube::drawXZFace(float y)
{
    float triangleWidth = 1.0f / m_pOne;
    Vector texCoordOffset = Vector(y > 0 ? 0 : 1.0/6.0);
    Vector currentNormal = Vector(0.0f, (y > 0.0 ? +1.0f : -1.0f), 0.0f);
    for(int r = 0; r < m_pOne; r++) {
        for(int c = 0; c < m_pOne; c++) {
            float leftZ = -0.5f + c * triangleWidth;
            float bottomX = -0.5f + r * triangleWidth;
            Vertex bottomLeft, bottomRight, topLeft, topRight;
            bottomLeft = Vertex(bottomX, y, leftZ, currentNormal, Vector(bottomX + 0.5, leftZ + 0.5, 0)*(1.0/6.0)+texCoordOffset);
            bottomRight = Vertex(bottomX, y, leftZ + triangleWidth, currentNormal, Vector(bottomX + 0.5, leftZ + triangleWidth + 0.5, 0)*(1.0/6.0)+texCoordOffset);
            topLeft = Vertex(bottomX + triangleWidth, y, leftZ, currentNormal, Vector(bottomX + triangleWidth + 0.5, leftZ + 0.5, 0)*(1.0/6.0)+texCoordOffset);
            topRight = Vertex(bottomX + triangleWidth, y, leftZ + triangleWidth, currentNormal, Vector(bottomX + triangleWidth + 0.5, leftZ + triangleWidth + 0.5, 0)*(1.0/6.0)+texCoordOffset);

            m_triangles[m_triangleIndex++] = Triangle( y >= 0 ? bottomLeft : topLeft, bottomRight, y >= 0 ? topLeft : bottomLeft);
            m_triangles[m_triangleIndex++] = Triangle( y >= 0 ? topLeft : topRight, bottomRight, y >= 0 ? topRight : topLeft);
        }
    }
}

