#include "SceneObjectFactory.h"

SceneObjectFactory::SceneObjectFactory()
{
    m_cube = NULL;
}

SceneObjectFactory::~SceneObjectFactory()
{
    if( m_cube != NULL )
    {
        delete m_cube;
        m_cube = NULL;
    }
}

SceneObject *SceneObjectFactory::constructCube()
{
    if( m_cube == NULL )
    {
        m_cube = new Cube();
        m_cube->tesselate();
    }
    SceneObject *obj = new SceneObject(m_cube);
    return obj;
}
