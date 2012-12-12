#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include "shapes/Shape.h"
#include <QtOpenGL>

enum TransformationType { ROTATION, TRANSLATION, SCALING };

struct Transformation
{
    TransformationType type;
    Vector4 rotation;
    Vector4 translation;
    Vector4 scaling;
    float angle;
};


/**
  * A scene object stores the data about an object in the scene. There is no
  * nesting of objects like in sceneview. All transformations should be applied
  * from the top level.
  */
class SceneObject
{
    public:
        SceneObject(Shape *shape);
        virtual ~SceneObject();
        void render() const;
        void translate(float x, float y, float z);
        void scale(float x, float y, float z);
        void rotate(float angle, float x, float y, float z);
        void setColor(float r, float g, float b, float a);
        void setColor(Vector4 color);
        Shape *getShape();

    protected:
        void refreshMatrix();

        Shape *m_shape;
        vector<Transformation> m_transformations;
        Vector4 m_color;
        Matrix4x4 m_matrix;

 };

#endif // SCENEOBJECT_H
