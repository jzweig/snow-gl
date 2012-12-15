#include "SceneObjectFactory.h"

SceneObjectFactory::SceneObjectFactory()
{
    m_tesselationParameter = 10;
    m_BumpResolution = 128;
}

SceneObjectFactory::~SceneObjectFactory()
{

}

SceneObject *SceneObjectFactory::constructCube()
{
    Cube *cube = new Cube();
    cube->tesselate();
    cube->setParamOne(m_tesselationParameter);
    SceneObject *obj = new SceneObject(cube,m_BumpResolution);
    return obj;
}

SceneObject *SceneObjectFactory::constructPlane()
{
    Plane *plane = new Plane();
    plane->tesselate();
    plane->setParamOne(m_tesselationParameter);
    SceneObject *obj = new SceneObject(plane,m_BumpResolution);
    return obj;
}

void SceneObjectFactory::setTesselationParameter(int parameter)
{
    m_tesselationParameter = parameter;
}

void SceneObjectFactory::setBumpResolution(int resolution)
{
    m_BumpResolution = resolution;
}
