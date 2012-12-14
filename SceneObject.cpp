#include "SceneObject.h"

#include <CS123Common.h>
//#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glext.h>
//#ifndef __APPLE__
extern "C" {
    GLAPI void APIENTRY glUniform1i(GLint location, GLint v0);
    GLAPI GLuint APIENTRY glGetUniformLocation(GLuint program, const GLchar *name);
    GLAPI void APIENTRY glUseProgram(GLuint program);
    GLAPI void APIENTRY glBindBuffer (GLenum target, GLuint buffer);
    GLAPI void APIENTRY glGenBuffers (GLsizei n, GLuint *buffers);
    GLAPI void APIENTRY glBufferData (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
}
//#endif

SceneObject::SceneObject(Shape* shape, int bumpResolution) : m_shape(shape)
{
    m_displacementResolution = m_shape->getParamOne();
    m_bumpResolution = bumpResolution;
    m_matrix = Matrix4x4::identity();
    m_vbo = 0;
    m_displacementMap = new QImage(m_displacementResolution,m_displacementResolution,QImage::Format_RGB32);
    m_bumpMap = new QImage(m_bumpResolution,m_bumpResolution,QImage::Format_RGB32);

    // set the new images to black
    memset(m_displacementMap->bits(), 0, m_displacementResolution * m_displacementResolution * sizeof(BGRA));
    memset(m_bumpMap->bits(), 0, m_bumpResolution * m_bumpResolution * sizeof(BGRA));
    m_displacementMapId = ResourceLoader::loadHeightMapTexture(m_displacementMap);
    m_bumpMapId = ResourceLoader::loadHeightMapTexture(m_bumpMap);
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

void SceneObject::render(const bool useVbo, const bool useShader, const bool useDisplacement, QGLShaderProgram* shader) const
{
    if(useShader){
        ResourceLoader::reloadHeightMapTexture(m_displacementMap,m_displacementMapId);
        ResourceLoader::reloadHeightMapTexture(m_bumpMap,m_bumpMapId);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        shader->bind();
        shader->setUniformValue("snowDisplacement",0);
        shader->setUniformValue("snowTexture",1);
        shader->setUniformValue("useDisplacement", useDisplacement);
        shader->setUniformValue("color",m_color.x, m_color.y, m_color.z, m_color.w);


        //displacement
        glActiveTexture(GL_TEXTURE0);
        //glUniform1i(glGetUniformLocation(shader->programId(), "snowDisplacement"), 0);
        glBindTexture(GL_TEXTURE_2D,m_displacementMapId);

        //bump
        glActiveTexture(GL_TEXTURE1);
        //glUniform1i(glGetUniformLocation(shader->programId(), "snowTexture"), 1);
        glBindTexture(GL_TEXTURE_2D,m_bumpMapId);

        glActiveTexture(GL_TEXTURE0);
    }

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

    if(useShader){
        shader->release();
        glBindTexture(GL_TEXTURE_2D,0);
        glDisable(GL_BLEND);
    }
}

Shape *SceneObject::getShape()
{
    return m_shape;
}

Vector4 SceneObject::getPosition()
{
    return Vector4(m_matrix.d, m_matrix.h, m_matrix.l, 1);
}


void SceneObject::setColor(float r, float g, float b, float a)
{
    m_color.x = r;
    m_color.y = g;
    m_color.z = b;
    m_color.w = a;
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
    // bump map
    int bx = (objPosition.x + 0.5)*m_bumpResolution;
    int by = (objPosition.z + 0.5)*m_bumpResolution;

    int bindex = by*m_bumpResolution+bx;
    return bindex;
}

int SceneObject::getDisplacementIndex(Vector4 objPosition)
{
    int dx = (objPosition.x + 0.5)*m_displacementResolution;
    int dy = (objPosition.z + 0.5)*m_displacementResolution;
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
\
    int r = color->r;
    int g = color->g;
    int b = color->b;

    // Convert rgb value to int
    int curNum = r + 255*g + 255*255*b;

    // Increment
    curNum++;

    // Convert back to rgb value
    b = curNum / (255*255);
    curNum %= (255*255);
    g = curNum / 255;
    curNum %= 255;
    r = curNum;

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
    return (data[index].r + data[index].g*255 + data[index].b*255*255)*0.00001;
}

/**
  * Returns the bump at the given object position.
  */
float SceneObject::getBump(Vector4 objPosition)
{
    BGRA* data = (BGRA *)m_bumpMap->bits();
    int index = getBumpIndex(objPosition);
    return (data[index].r + data[index].g*255 + data[index].b*255*255)*0.0001;
}
