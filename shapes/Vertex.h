#ifndef VERTEX_H
#define VERTEX_H

#include "Vector.h"

/**
   Represents a vertex of a triangle.
  */
class Vertex : public Vector
{
    public:
        Vertex();

        /**
          The main constructor that should be used. Contains an x, y and z coordinate
          reflecting the position of the vertex and a Vector contianing the normal
          of the vertex.
          */
        Vertex(float x, float y, float z, Vector normal, Vector texCoord);

        Vertex(float x, float y, float z, Vector normal);

        float w();
        void w(float w);

        //! Returns a pointer to the vector that is the normal of this vertex.
        Vector *getNormal();

        //! Returns a pointer to the vector that is the tex coordinate for this vertex.
        Vector *getTexCoord();

    protected:
        float m_w;
        Vector m_normal;
        Vector m_texCoord;
};

#endif // VERTEX_H
