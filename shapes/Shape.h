#ifndef SHAPE_H
#define SHAPE_H

#include "Renderable.h"
#include "Triangle.h"
#include "Vertex.h"
#include <QList>

class Shape;
struct Intersection;


/**

  Defines a three-dimensional, tesselated shape that can be rendered.

  @author jbowens
 */
class Shape : public Renderable
{
    public:
        Shape();
        virtual ~Shape();
        void setParamOne(int newParamOne);
        void setParamTwo(int newParamTwo);
        void setParamThree(int newParamThree);

        //! Tesselates the given shape
        virtual void tesselate() = 0;

        /**
          This renders the shape. Subclasses shouldn't need to
          override this class if they implement tesselate().
          */
        virtual void render();

        //! Retrieves a pointer to the array of triangles in the tesselation of this shape
        virtual Triangle *getTriangles();

        //! Retrieves the number of triangles contained in the tesselation of this shape
        virtual int getNumTriangles();

    protected:

        //! Tesselates a circle at the given y coordinate
        virtual void tesselateCircle(float y);

        //! This is a utility method to check if too many triangles are being allocated, leaving the possibility
        //! of garbage triangles appearing.
        virtual void initializeTriangles();

        //! The tesselation parameters
        int m_pOne;
        int m_pTwo;
        int m_pThree;

        //! The number of triangles contained within the tesselation
        int m_triangleCount;

        //! An index in the m_triangles array that is used during tesselation
        int m_triangleIndex;

        //! The array of Triangle objects that form the tesselation
        Triangle *m_triangles;
};

#endif // SHAPE_H
