#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Renderable.h"
#include "Vertex.h"

/**
  Represents a triangle. The Shape class tesselates by creating hundreds of these
  objects to represent the larger 3-dimensional object.
  */
class Triangle : public Renderable
{
    public:
        Triangle();

        //! Constructs a triangle with the given vertices.
        Triangle(Vertex one, Vertex two, Vertex three);

        ~Triangle();

        //! Renders the triangle to OpenGL
        virtual void render();

        //! Returns pointers to the vertices of this triangle
        Vertex *getVertexOne();
        Vertex *getVertexTwo();
        Vertex *getVertexThree();

    protected:
        //! The vertices that make up this triangle.
        Vertex m_pointOne;
        Vertex m_pointTwo;
        Vertex m_pointThree;

 };

#endif // TRIANGLE_H
