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

    /**
      * Tells the factory to use the given tesselation paramter when
      * tesselating shapes
      */
    void setTesselationParameter(int parameter);


    protected:
        int m_tesselationParameter;
};

#endif // SCENEOBJECTFACTORY_H
