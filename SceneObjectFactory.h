#ifndef SCENEOBJECTFACTORY_H
#define SCENEOBJECTFACTORY_H

#include<map>
#include "shapes/Cube.h"
#include "shapes/Plane.h"
#include "SceneObject.h"

class SceneObjectFactory
{
public:
    SceneObjectFactory();
    virtual ~SceneObjectFactory();

    SceneObject *constructCube();
    SceneObject *constructPlane();

    /**
      * Tells the factory to use the given tesselation parameter when
      * tesselating shapes
      */
    void setTesselationParameter(int parameter);


    /**
      * Tells the factory to use the given bump map resolution (will be resolution x resolution texture)
      */
    void setBumpResolution(int resolution);


    /**
      * Tells the factory to use a given scale of the object
      */
    void setOriginalScale(float x, float y, float z);

    /**
      * Tells the factory to use a given location from the origin for the object's position
      */
    void setOriginalTranslation(float x, float y, float z);

    protected:
        int m_tesselationParameter;
        int m_BumpResolution;
        Vector3 m_originalScale;
        Vector3 m_originalTranslation;
};

#endif // SCENEOBJECTFACTORY_H
