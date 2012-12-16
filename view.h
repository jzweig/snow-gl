#ifndef VIEW_H
#define VIEW_H

#include <qgl.h>
#include <QTime>
#include <QTimer>
#include <QHash>
#include <QGLFramebufferObject>
#include <QtOpenGL>
#include <QString>
#include <QImage>
#include <QFile>
#include <iostream>

#ifdef __APPLE__
    #include "glut/glut.h"
#endif
#include "SnowEmitter.h"
#include "camera.h"
#include "shapes/Cube.h"
#include "SceneObject.h"
#include "SceneObjectFactory.h"
#include "resourceloader.h"

// Speeds affecting the speed of snow simulation. Fast speed occurs when holding down
// the down arrow.
#define DEFAULT_SPEED 1.0
#define FAST_SPEED 20.0
#define UBER_SPEED 100.0
#define SPRINT_FACTOR 0.2
#define WALK_FACTOR 0.1
#define SUPERSAMPLING_SQRT_SAMPLES 2

class View : public QGLWidget
{
    Q_OBJECT

public:
    View(QWidget *parent);
    ~View();

private:
    //QTime time;
    //QTimer timer;
    Vector2 m_prevMousePos;
    QTimer m_timer;
    QTime m_clock;
    int m_prevTime;
    float m_prevFps, m_fps;
    QFont m_font; // font for rendering text
    GLuint m_cubeMap;
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);


    void paintUI();

    void createShaderPrograms();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void drawUnitAxis(float x, float y, float z);
    // Resources
    QHash<QString, QGLShaderProgram *> m_shaderPrograms; // hash map of all shader programs
private slots:
    void tick();

protected:
     void setupScene();
     void initSceneVbo();
     void applyProjectionCamera();
     void applyOrthogonalCamera(int width, int height);
     void initScenePbo();
     void setupLights();
     void setupCubeMap();
     float getMoveFactor();
     void renderScene();
     void createFramebufferObjects(int width, int height);
     void renderFramebuffer(QGLFramebufferObject *framebufferObj, int width, int height);

     //! The snow emitter responsible for tracking snowflakes
     SnowEmitter m_snowEmitter;

     //! The camera settings
     OrbitCamera *m_camera;

     //! The overall speed
     float m_speed;

     //! Whether or not shift is pressed
     bool m_isShiftPressed;

     //! The scene object factory to use when constructing scene objects
     SceneObjectFactory m_factory;

     //! The objects in the scene
     vector<SceneObject *> m_objects;

     //! Whether or not to display wireframes
     bool m_isWireframe;

     //! Whether or not to display the solid scene
     bool m_isSolid;

     //! Whether or not to display snow shader material
     bool m_useShader;

     //! Whether or not to display the unit axis
     bool m_showUnitAxis;

     //! Whether or not to show the skybox
     bool m_showSkybox;

     //! Is uber mode?
     bool m_useUberMode;

     //! Snowflake textures
     vector<GLuint> m_snowflakeTextures;

     QImage* m_snowHeightMap;
     BGRA* m_data;

     //! Use scene vbo
     bool m_useVbo;

     //! Use scene pbo
     bool m_usePbo;

     //! Use shader displacement mapping
     bool m_useDisplacement;

     //! The number of scene triangles. This is used for the vbo's and dynamicaly determined
     //! when the vbo is initialized.
     int m_triangleCount;

     //! The main fbo into which we render the snowless scene
     QGLFramebufferObject *m_fbo_main;

     //! The snow fbo into which we render the accumulated snow
     QGLFramebufferObject *m_fbo_snow;

     QGLFramebufferObject *m_fbo_buffer;

     QString m_homeDir;
     QString m_projDir;
     int m_pboIndexA;
     int m_pboIndexB;

     GLuint m_snowTextureId;

     // The width at which to render the scene. This might be greater than the width and height
     // of the window if we're supersampling.
     int m_width;
     int m_height;
};

#endif // VIEW_H

