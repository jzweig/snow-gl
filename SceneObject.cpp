#include "SceneObject.h"

#include <CS123Common.h>
#ifndef __APPLE__
extern "C" {
    GLAPI void APIENTRY glBindBuffer (GLenum target, GLuint buffer);
    GLAPI void APIENTRY glGenBuffers (GLsizei n, GLuint *buffers);
    GLAPI void APIENTRY glBufferData (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
}
#endif

SceneObject::SceneObject(Shape* shape, int gridLength) : m_shape(shape)
{
    m_gridLength = gridLength;
    m_matrix = Matrix4x4::identity();
    m_vbo = 0;
    m_heightMap = new QImage(m_gridLength,m_gridLength,QImage::Format_RGB32);

    // set the new image to black
    BGRA* m_data = (BGRA *)m_heightMap->bits();
    for(int i=0;i<m_gridLength;i++){
        for(int j=0;j<m_gridLength;j++){
            float incr = ((float) rand())/RAND_MAX;
            float incg = ((float) rand())/RAND_MAX;
            float incb = ((float) rand())/RAND_MAX;
            m_data[i*m_gridLength+j].r = 0;
            m_data[i*m_gridLength+j].g = 0;
            m_data[i*m_gridLength+j].b = 0;
            m_data[i*m_gridLength+j].a = 255;
        }
    }
    m_snowtextureId = ResourceLoader::loadHeightMapTexture(m_heightMap);
}

SceneObject::~SceneObject()
{
    delete m_heightMap;
    delete m_shape;
}

Matrix4x4 SceneObject::getTransformationMatrix() const
{
    return m_matrix;
}

void SceneObject::render(const bool useVbo, const bool useShader, const bool useDisplacement, QGLShaderProgram* shader) const
{
    if(useShader){
        ResourceLoader::reloadHeightMapTexture(m_heightMap,m_snowtextureId);
        glActiveTexture(m_snowtextureId);
        glBindTexture(GL_TEXTURE_2D,m_snowtextureId);
        shader->bind();
        shader->setUniformValue("useDisplacement", useDisplacement);
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

/**
  * Records a snowflake falling on this object at the provided
  * position in object coordinates.
  */
void SceneObject::recordSnowfall(Vector4 objPosition){

    int xcoord = (objPosition.x + 0.5)*m_gridLength;
    int ycoord = (objPosition.z + 0.5)*m_gridLength;

    BGRA* data = (BGRA *)m_heightMap->bits();
    //increment snowmap;
    int incr = 10;
    data[ycoord*m_gridLength+xcoord].r = min(data[ycoord*m_gridLength+xcoord].r+incr,255);
    data[ycoord*m_gridLength+xcoord].g = min(data[ycoord*m_gridLength+xcoord].g+incr,255);
    data[ycoord*m_gridLength+xcoord].b = min(data[ycoord*m_gridLength+xcoord].b+incr,255);

}
