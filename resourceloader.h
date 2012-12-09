#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H

#include <QFile>
#include <QGLShaderProgram>
#include "glm.h"

/**
    A simple model struct
 **/
struct Model
{
    GLMmodel *model;
    GLuint idx;
};

/**
   A resource loader with code to handle loading models, skyboxes, and shader programs.

  @author: Justin Ardini (jardini)
   **/
namespace ResourceLoader
{
    // Returns the model
    Model loadObjModel(QString filePath);

    //! Loads the texture at the given location, returning the GL texture id
    GLuint loadTexture(const QString &path);

    // Returns the skybox ID
    GLuint loadSkybox();

    // These return a new QGLShaderProgram.  THIS MUST BE DELETED BY THE CALLER.
    QGLShaderProgram * newVertShaderProgram(const QGLContext *context, QString vertShader);
    QGLShaderProgram * newFragShaderProgram(const QGLContext *context, QString fragShader);
    QGLShaderProgram * newShaderProgram(const QGLContext *context, QString vertShader, QString fragShader);

    // Returns the cubeMap ID
    GLuint loadCubeMap(QList<QFile *> files);

    GLuint loadHeightMapTexture(float* heightmap,int width, int height);

}

#endif // RESOURCELOADER_H
