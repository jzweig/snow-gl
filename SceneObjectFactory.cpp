#include "SceneObjectFactory.h"

SceneObjectFactory::SceneObjectFactory()
{
    m_tesselationParameter = 10;
}

SceneObjectFactory::~SceneObjectFactory()
{

}

SceneObject *SceneObjectFactory::constructCube()
{
    Cube *cube = new Cube();
    cube->tesselate();
    cube->setParamOne(m_tesselationParameter);
    SceneObject *obj = new SceneObject(cube);
    return obj;
}

void SceneObjectFactory::setTesselationParameter(int parameter)
{
    m_tesselationParameter = parameter;
}
