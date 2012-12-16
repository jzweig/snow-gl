#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include "shapes/Shape.h"
#include <QtOpenGL>
#include "resourceloader.h"
#include <CS123Common.h>
#include "ShaderAssistant.h"

enum ShapeType { PLANE, CUBE };
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
        SceneObject(Shape* shape, ShapeType type, Vector3 originalScale, int gridLength);
        virtual ~SceneObject();
        void render(const bool useVbo) const;
        void translate(float x, float y, float z);
        void scale(float x, float y, float z);
        void rotate(float angle, float x, float y, float z);
        void setColor(float r, float g, float b, float a);
        void setColor(Vector4 color);
        void setVboBuffer(GLuint buffer_name);
        void setPboBuffers(GLuint* buffer_name);
        void setTexture(GLuint colorTextureId);
        GLuint getColorTexture();
        Shape *getShape();
        GLuint getVboBuffer() const;
        GLuint* getPboBuffers() const;
        void recordSnowfall(Vector4 objPos);
        bool collide(Vector4 obj);
        Vector4 getPosition();
        Matrix4x4 getTransformationMatrix() const;
        float getDisplacement(Vector4 objPosition);
        void incrementEndian(BGRA* color);
        int getBumpIndex(Vector4 objPosition);
        int getDisplacementIndex(Vector4 objPosition);
        QImage *getDisplacementMap();
        QImage *getBumpMap();
        GLuint getBumpMapId();
        GLuint getDisplacementMapId();
        Vector3 getOriginalScale();
        Vector4 getColor();
        int m_bumpResolution;
        GLuint* m_pbo;
    protected:
        void refreshMatrix();

        ShapeType m_shapeType;
        Shape *m_shape;
        vector<Transformation> m_transformations;
        Vector4 m_color;
        Vector3 m_originalScale;
        Matrix4x4 m_matrix;
        GLuint m_vbo;
        GLuint m_colorTexture;
        QImage* m_displacementMap;
        GLuint m_displacementMapId;
        //GLuint m_baseMapId;
        QImage* m_bumpMap;
        GLuint m_bumpMapId;
        int m_displacementResolution;
 };

#endif // SCENEOBJECT_H
