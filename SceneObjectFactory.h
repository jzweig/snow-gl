#ifndef SCENEOBJECTFACTORY_H
#define SCENEOBJECTFACTORY_H

#include<map>
#include "shapes/Cube.h"
#include "SceneObject.h"

class SceneObjectFactory
{
public:
    SceneObjectFactory();
    virtual ~SceneObjectFactory();

    SceneObject *constructCube();

protected:
    Cube *m_cube;
};

#endif // SCENEOBJECTFACTORY_H
