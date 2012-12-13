#include "SceneObject.h"

#include <CS123Common.h>
#ifndef __APPLE__
extern "C" {
    GLAPI void APIENTRY glBindBuffer (GLenum target, GLuint buffer);
    GLAPI void APIENTRY glGenBuffers (GLsizei n, GLuint *buffers);
    GLAPI void APIENTRY glBufferData (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
}
#endif

SceneObject::SceneObject(Shape* shape, int m_gridLength) : m_shape(shape)
{
    m_matrix = Matrix4x4::identity();
    m_vbo = 0;
    m_heightMap = new QImage(m_gridLength,m_gridLength,QImage::Format_RGB32);

    // set the new image to black
    //memset(m_snowHeightMap->bits(), 0, width * height * sizeof(BGRA));
    BGRA* m_data = (BGRA *)m_heightMap->bits();
    for(int i=0;i<m_gridLength;i++){
        for(int j=0;j<m_gridLength;j++){
            float incr = ((float) rand())/RAND_MAX;
            float incg = ((float) rand())/RAND_MAX;
            float incb = ((float) rand())/RAND_MAX;
            m_data[i*m_gridLength+j].r =((int)(incr*255));
            m_data[i*m_gridLength+j].g =((int)(incg*255));
            m_data[i*m_gridLength+j].b =((int)(incb*255));
            m_data[i*m_gridLength+j].a = 255;
        }
    }
}

SceneObject::~SceneObject()
{
    delete m_heightMap;
    delete m_shape;
}

void SceneObject::render(const bool useVbo, const bool useShader, const bool useDisplacement, QGLShaderProgram* shader) const
{
    if(useShader){
        // Load the texture
        GLuint textureId = ResourceLoader::loadHeightMapTexture(m_heightMap);
        glActiveTexture(textureId);
        glBindTexture(GL_TEXTURE_2D,textureId);
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

GLuint SceneObject::getVboBuffer()
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

void SceneObject::paintTexture(float x, float y, float z){

}
