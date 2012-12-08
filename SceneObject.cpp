#include "SceneObject.h"

SceneObject::SceneObject(Shape *shape) : m_shape(shape)
{

}

SceneObject::~SceneObject()
{

}

void SceneObject::render() const
{
    glPushMatrix();
    for( std::vector<Transformation>::const_iterator it = m_transformations.begin(); it != m_transformations.end(); ++it ) {
        Transformation t = *it;
        switch( t.type ) {
        case TRANSLATION:
            glTranslatef(t.translation.x, t.translation.y, t.translation.z);
            break;

        case SCALING:
            glScalef(t.scaling.x, t.scaling.y, t.scaling.z);
            break;

        case ROTATION:
            glRotatef(t.angle, t.rotation.x, t.rotation.y, t.rotation.z);
            break;
        }
    }
    cout << "color = " << m_color << endl;
    glColor4f(m_color.x, m_color.y, m_color.z, m_color.w);
    m_shape->render();
    glPopMatrix();
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

void SceneObject::translate(float x, float y, float z)
{
    Vector4 translationVec = Vector4(x, y, z, 0);
    Transformation transformation;
    transformation.type = TRANSLATION;
    transformation.translation = translationVec;

    m_transformations.push_back(transformation);
}

void SceneObject::scale(float x, float y, float z)
{
    Vector4 scaleVec = Vector4(x, y, z, 0);
    Transformation transformation;
    transformation.type = SCALING;
    transformation.scaling = scaleVec;

    m_transformations.push_back(transformation);
}

void SceneObject::rotate(float angle, float x, float y, float z)
{
    Vector4 rotateVec = Vector4(x, y, z, 0);
    Transformation transformation;
    transformation.type = ROTATION;
    transformation.translation = rotateVec;
    transformation.angle = angle;

    m_transformations.push_back(transformation);
}
