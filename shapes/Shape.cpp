#include "Shape.h"
#include "Vertex.h"
#include "CS123Common.h"
#include <cstddef>
#include <QList>
#include <GL/gl.h>

Shape::Shape()
{
    m_triangles = NULL;
    m_triangleCount = 0;
    m_triangleIndex = 0;
    m_pOne = 80;
    m_pTwo = 35;
    m_pThree = 35;
}

Shape::~Shape()
{
    if( m_triangles )
    {
        delete[] m_triangles;
        m_triangles = NULL;
    }
}

void Shape::setParamOne(int newParamOne)
{
    if( m_pOne != newParamOne )
    {
        // Need to re-tesselate
        m_pOne = newParamOne;
        tesselate();
    }
}

void Shape::setParamTwo(int newParamTwo)
{
    if( m_pTwo != newParamTwo )
    {
        // Need to re-tesselate
        m_pTwo = newParamTwo;
        tesselate();
    }
}

void Shape::setParamThree(int newParamThree)
{
    if( m_pThree != newParamThree )
    {
        // Need to re-tesselate
        m_pThree = newParamThree;
        tesselate();
    }
}

/**
  This renders the shape. Subclasses shouldn't need to
  override this class if they implement tesselate().
  */
void Shape::render()
{
    glBegin(GL_TRIANGLES);
    for(int i = 0; i < m_triangleCount; i++)
        m_triangles[i].render();
    glEnd();
}

/**
  This method is used when the space for the triangles array is first
  allocated. It sets all the triangles in the array to be large ones
  facing the positive Z direction. I use this method so that if I
  allocated too many triangles, I'll notice from the giant triangle.
  */
void Shape::initializeTriangles()
{
    Vector normal = Vector(0, 0, 1.0f);
    Vertex one = Vertex(-1.0f, -1.0f, 0.0f, normal);
    Vertex two = Vertex(0, 1.0f, 0, normal);
    Vertex three = Vertex(1.0f, -1.0f, 0.0f, normal);
    Triangle tri = Triangle(three, two, one);
    for( int i = 0; i < m_triangleCount; i++ )
    {
        m_triangles[i] = tri;
    }
}

Triangle *Shape::getTriangles()
{
    return m_triangles;
}

int Shape::getNumTriangles()
{
    return m_triangleCount;
}

/** Tesselates a unit circle in the XZ plane at the given y. This method
    requires ((MAX(m_pTwo,3)*(m_pOne*2.0 - 1)) available Triangles in
    m_triangles. Allocating too few will result in a segmentation fault and
    allocating too many may result in undesirable garbage triangles appearing
    within the scene.

    @param y  the point on the y axis where the circle should be drawn.
  */
void Shape::tesselateCircle(float y)
{
    int pieSlices = MAX(m_pTwo, 3);
    int sliceSections = m_pOne;

    // Calculate the normal. If the circle is being drawn at a positive y position,
    // make it face up. Otherwise, make it face down.
    Vector normal = Vector(0.0f, (y >= 0.0f ? 1.0f : -1.0f), 0.0f);
    Vertex center = Vertex(0.0f, y, 0.0f, normal);

    float pieSliceRadians = 2 * M_PI / pieSlices;

    // Iterate through the slices that make up the circle.
    for( int p = 0; p < pieSlices; p++ )
    {
        float topDegrees = p*pieSliceRadians + pieSliceRadians/2.0f;
        float bottomDegrees = p*pieSliceRadians - pieSliceRadians/2.0f;
        Vertex topLeft, topRight, bottomLeft, bottomRight;
        float topX = 0.5f * cos(topDegrees);
        float topZ = 0.5f * sin(topDegrees);
        float bottomX = 0.5f * cos(bottomDegrees);
        float bottomZ = 0.5f * sin(bottomDegrees);

        // Iterate through the sections that make up a slice
        for( int s = sliceSections-1; s>0; s-- )
        {
            float rightPct = (s+1.0f)/sliceSections;
            float leftPct = (((float)s)/sliceSections);
            topLeft = Vertex(topX * leftPct, y, topZ * leftPct, normal);
            topRight = Vertex(topX * rightPct, y, topZ * rightPct, normal);
            bottomLeft = Vertex(bottomX * leftPct, y, bottomZ * leftPct, normal);
            bottomRight = Vertex(bottomX * rightPct, y, bottomZ * rightPct, normal);
            m_triangles[m_triangleIndex++] = Triangle(y >= 0 ? topLeft : bottomRight, topRight, y >= 0 ? bottomRight : topLeft);
            m_triangles[m_triangleIndex++] = Triangle(y >= 0 ? bottomRight : topLeft, bottomLeft, y >= 0 ? topLeft : bottomRight);
        }

        float slicePct = 1.0f / sliceSections;

        // Draw the final triangle that connectts with the center of the circle
        Vertex top = Vertex(slicePct*topX, y, topZ * slicePct, normal);
        Vertex bottom = Vertex(slicePct*bottomX, y, bottomZ * slicePct, normal);
        m_triangles[m_triangleIndex++] = Triangle(y < 0 ? top : bottom, center, y < 0 ? bottom : top );
    }

}

