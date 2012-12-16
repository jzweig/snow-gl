#include "SceneObjectFactory.h"

SceneObjectFactory::SceneObjectFactory()
{
    m_tesselationParameter = 10;
    m_BumpResolution = 128;
    m_originalScale = Vector3(1.0,1.0,1.0);
    m_originalTranslation = Vector3(0.0,0.0,0.0);
}

SceneObjectFactory::~SceneObjectFactory()
{

}

SceneObject *SceneObjectFactory::constructCube()
{
    Cube *cube = new Cube();
    cube->tesselate();
    cube->setParamOne(m_tesselationParameter);
    SceneObject *obj = new SceneObject(cube, CUBE, m_originalScale, m_BumpResolution);
    obj->translate(m_originalTranslation.x,m_originalTranslation.y,m_originalTranslation.z);
    return obj;
}

SceneObject *SceneObjectFactory::constructPlane()
{
    Plane *plane = new Plane();
    plane->tesselate();
    plane->setParamOne(m_tesselationParameter);
    SceneObject *obj = new SceneObject(plane, PLANE, m_originalScale, m_BumpResolution);
    obj->translate(m_originalTranslation.x,m_originalTranslation.y,m_originalTranslation.z);
    return obj;
}

void SceneObjectFactory::setOriginalScale(float x, float y, float z)
{
    m_originalScale.x = x;
    m_originalScale.y = y;
    m_originalScale.z = z;
}

void SceneObjectFactory::setOriginalTranslation(float x, float y, float z)
{
    m_originalTranslation.x = x;
    m_originalTranslation.y = y;
    m_originalTranslation.z = z;
}

void SceneObjectFactory::setTesselationParameter(int parameter)
{
    m_tesselationParameter = parameter;
}

void SceneObjectFactory::setBumpResolution(int resolution)
{
    m_BumpResolution = resolution;
}
