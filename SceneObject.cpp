#include "SceneObject.h"

#include <CS123Common.h>
//#include <GL/glew.h>
#include <QtOpenGL>

#ifndef __APPLE__
extern "C" {
    GLAPI void APIENTRY glUniform1i(GLint location, GLint v0);
    GLAPI GLuint APIENTRY glGetUniformLocation(GLuint program, const GLchar *name);
    GLAPI void APIENTRY glUseProgram(GLuint program);
    GLAPI void APIENTRY glBindBuffer (GLenum target, GLuint buffer);
    GLAPI void APIENTRY glGenBuffers (GLsizei n, GLuint *buffers);
    GLAPI void APIENTRY glBufferData (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
}
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glext.h>
#else
#include <glut/glut.h>
//#include <GL/glext.h>
#endif


SceneObject::SceneObject(Shape* shape, ShapeType type, Vector3 originalScale, int bumpResolution) : m_shape(shape)
{
    m_shapeType = type;
    m_displacementResolution = m_shape->getParamOne();
    m_bumpResolution = bumpResolution;
    m_matrix = Matrix4x4::identity();
    m_vbo = 0;
    m_pbo = new GLuint[2];
    m_displacementMap = new QImage(m_displacementResolution,m_displacementResolution,QImage::Format_RGB32);
    m_bumpMap = new QImage(m_bumpResolution,m_bumpResolution,QImage::Format_RGB32);
    m_colorTexture = 0;

    // set the new images to black
    memset(m_displacementMap->bits(), 0, m_displacementResolution * m_displacementResolution * sizeof(BGRA));
    memset(m_bumpMap->bits(), 0, m_bumpResolution * m_bumpResolution * sizeof(BGRA));
    m_displacementMapId = ResourceLoader::loadHeightMapTexture(m_displacementMap);
    m_bumpMapId = ResourceLoader::loadHeightMapTexture(m_bumpMap);


    m_originalScale = originalScale;
    this->scale(m_originalScale.x,m_originalScale.y,m_originalScale.z);
}

SceneObject::~SceneObject()
{
    delete m_displacementMap;
    delete m_bumpMap;
    delete m_shape;
}

Matrix4x4 SceneObject::getTransformationMatrix() const
{
    return m_matrix;
}

void SceneObject::render(const bool useVbo) const
{
    glPushMatrix();
    glColor4f(m_color.x, m_color.y, m_color.z, m_color.w);
    glMultMatrixd(m_matrix.data);
    if( useVbo && m_vbo ) {
        // Bind the vbo buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        // Tell OpenGL what's up
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(3, GL_FLOAT, sizeof(float)*8, (void *) 0 );
        glNormalPointer(GL_FLOAT, sizeof(float)*8, (void *) (sizeof(float)*3) );
        glTexCoordPointer(2, GL_FLOAT, sizeof(float)*8, (void *) (sizeof(float)*6));

        // Draw dat shit
        glDrawArrays(GL_TRIANGLES, 0, m_shape->getNumTriangles()*3);

        // Unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    } else {
        m_shape->render();
    }
    glPopMatrix();
}

QImage *SceneObject::getDisplacementMap()
{
    return m_displacementMap;
}

QImage *SceneObject::getBumpMap()
{
    return m_bumpMap;
}

GLuint SceneObject::getBumpMapId()
{
    return m_bumpMapId;
}

GLuint SceneObject::getDisplacementMapId()
{
    return m_displacementMapId;
}

Shape *SceneObject::getShape()
{
    return m_shape;
}

Vector4 SceneObject::getPosition()
{
    return Vector4(m_matrix.d, m_matrix.h, m_matrix.l, 1);
}

Vector3 SceneObject::getOriginalScale()
{
    return m_originalScale;
}

void SceneObject::setColor(float r, float g, float b, float a)
{
    m_color.x = r;
    m_color.y = g;
    m_color.z = b;
    m_color.w = a;
}

Vector4 SceneObject::getColor()
{
    return m_color;
}

void SceneObject::setColor(Vector4 color)
{
    m_color = color;
}

void SceneObject::setVboBuffer(GLuint buffer_name)
{
    m_vbo = buffer_name;
}

GLuint SceneObject::getVboBuffer() const
{
    return m_vbo;
}

void SceneObject::setPboBuffers(GLuint* buffer_name)
{
    m_pbo = buffer_name;
}

GLuint* SceneObject::getPboBuffers() const
{
    return m_pbo;
}

void SceneObject::setTexture(GLuint colorTextureId)
{
    m_colorTexture = colorTextureId;
}

GLuint SceneObject::getColorTexture()
{
    return m_colorTexture;
}

void SceneObject::refreshMatrix()
{
    Matrix4x4 mat = Matrix4x4::identity();

    for( std::vector<Transformation>::const_iterator it = m_transformations.begin(); it != m_transformations.end(); ++it ) {
        Transformation t = *it;
        switch( t.type ) {
        case TRANSLATION:
            mat = getTransMat(t.translation).getTranspose() * mat;
            break;

        case SCALING:
            mat = getScaleMat(t.scaling).getTranspose() * mat;
            break;

        case ROTATION:
            mat = getRotMat(Vector4(0, 0, 0, 1), t.rotation, t.angle).getTranspose() * mat;
            break;
        }
    }

    m_matrix = mat;
}


void SceneObject::translate(float x, float y, float z)
{
    Vector4 translationVec = Vector4(x, y, z, 0);
    Transformation transformation;
    transformation.type = TRANSLATION;
    transformation.translation = translationVec;

    m_transformations.push_back(transformation);
    refreshMatrix();
}

void SceneObject::scale(float x, float y, float z)
{
    Vector4 scaleVec = Vector4(x, y, z, 0);
    Transformation transformation;
    transformation.type = SCALING;
    transformation.scaling = scaleVec;

    m_transformations.push_back(transformation);
    refreshMatrix();
}

void SceneObject::rotate(float angle, float x, float y, float z)
{
    Vector4 rotateVec = Vector4(x, y, z, 0);
    Transformation transformation;
    transformation.type = ROTATION;
    transformation.translation = rotateVec;
    transformation.angle = angle;

    m_transformations.push_back(transformation);
    refreshMatrix();
}

int SceneObject::getBumpIndex(Vector4 objPosition)
{
    Vector texCoord = m_shape->getTexCoords(objPosition);

    // bump map
    int bx = (texCoord.x())*m_bumpResolution;
    int by = (texCoord.y())*m_bumpResolution;
    int bindex = by*m_bumpResolution+bx;
    return bindex;
}

int SceneObject::getDisplacementIndex(Vector4 objPosition)
{
    Vector texCoord = m_shape->getTexCoords(objPosition);

    // displacement map
    int dx = texCoord.x()*m_displacementResolution;
    int dy = texCoord.y()*m_displacementResolution;
    int dindex = dy*m_displacementResolution+dx;
    return dindex;
}

/**
  * Records a snowflake falling on this object at the provided
  * position in object coordinates.
  */
void SceneObject::recordSnowfall(Vector4 objPosition){
    // displacement map
    BGRA* dData = (BGRA *)m_displacementMap->bits();
    int dindex = getDisplacementIndex(objPosition);
    incrementEndian(&dData[dindex]);

    BGRA* bData = (BGRA *)m_bumpMap->bits();
    int bindex = getBumpIndex(objPosition);
    incrementEndian(&bData[bindex]);
}

void SceneObject::incrementEndian(BGRA* color){
    int r = color->r;
    int g = color->g;
    int b = color->b;

    // Convert rgb value to int
    int curNum = r + 256*g + 256*256*b;

    // Increment
    curNum++;

    // Convert back to rgb value
    r = curNum % 256;
    curNum /= 256;
    g = curNum % 256;
    curNum /= 256;
    b = curNum;
    color->r = r;
    color->g = g;
    color->b = b;
}

/**
  * Returns the displacement at the given object position.
  */
float SceneObject::getDisplacement(Vector4 objPosition)
{
    BGRA* data = (BGRA *)m_displacementMap->bits();
    int index = getDisplacementIndex(objPosition);
    return (data[index].r + data[index].g*256 + data[index].b*256*256)*0.00001;
}

bool SceneObject::collide(Vector4 objPos)
{
    // Check y with displacement
    float displacement = getDisplacement(objPos);

    if( m_shapeType == PLANE )
    {
        if( objPos.y <= displacement )
        {
            recordSnowfall(objPos);
            return true;
        } else
            return false;
    } else
    {
        if( objPos.y >= -0.5 && objPos.y <= displacement + 0.5 )
        {
            recordSnowfall(objPos);
            return true;
        } else
            return false;
    }

}
