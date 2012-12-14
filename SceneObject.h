#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include "shapes/Shape.h"
#include <QtOpenGL>
#include "resourceloader.h"
#include <CS123Common.h>

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
        SceneObject(Shape* shape, int gridLength);
        virtual ~SceneObject();
        void render(const bool useVbo, const bool useShader, const bool useDisplacement, QGLShaderProgram* shader) const;
        void translate(float x, float y, float z);
        void scale(float x, float y, float z);
        void rotate(float angle, float x, float y, float z);
        void setColor(float r, float g, float b, float a);
        void setColor(Vector4 color);
        void setVboBuffer(GLuint buffer_name);
        Shape *getShape();
        GLuint getVboBuffer() const;
        void recordSnowfall(Vector4 objPos);
        Vector4 getPosition();
        Matrix4x4 getTransformationMatrix() const;
        float getDisplacement(Vector4 objPosition);
        float getBump(Vector4 objPosition);
        void incrementEndian(BGRA* color);
        int getBumpIndex(Vector4 objPosition);
        int getDisplacementIndex(Vector4 objPosition);
    protected:
        void refreshMatrix();

        Shape *m_shape;
        vector<Transformation> m_transformations;
        Vector4 m_color;
        Matrix4x4 m_matrix;
        GLuint m_vbo;
        QImage* m_displacementMap;
        GLuint m_displacementMapId;
        //GLuint m_baseMapId;
        QImage* m_bumpMap;
        GLuint m_bumpMapId;
        int m_bumpResolution;
        int m_displacementResolution;
 };

#endif // SCENEOBJECT_H
