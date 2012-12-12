#ifndef VECTOR_H
#define VECTOR_H

#include "CS123Algebra.h"

/**
  Represents a 3-dimensional vector. This class is used to hold normals and
  is subclassed by Vertex to store point locations.
  */
class Vector
{
    public:
        Vector();
        Vector(float x, float y, float z);
        ~Vector();

        // Coordinate getters
        float x() const;
        float y() const;
        float z() const;

        // Coordinate setters
        void x(float x);
        void y(float y);
        void z(float z);

        Vector4 toVec4() const;

        float *getData();

    protected:
        union {
            float data[3];
            struct { float m_x, m_y, m_z; };
        };
};

#endif // VECTOR_H
