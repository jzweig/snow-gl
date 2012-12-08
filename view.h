#ifndef VIEW_H
#define VIEW_H

#include <qgl.h>
#include <QTime>
#include <QTimer>
#include <QHash>
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
#define SPRINT_FACTOR 0.1
#define WALK_FACTOR 0.01

class View : public QGLWidget
{
    Q_OBJECT

public:
    View(QWidget *parent);
    ~View();

private:
    QTime time;
    QTimer timer;
    Vector2 m_prevMousePos;
    QTimer m_timer;
    QTime m_clock;
    int m_prevTime;
    float m_prevFps, m_fps;
    QFont m_font; // font for rendering text

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    Model m_dragon; // dragon model

    void paintUI();
    void paintSky();

    void createShaderPrograms();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void drawUnitAxis(float x, float y, float z);
    void drawPlane(float color[], float translate[]);
    void drawPlane(float color[], float translate[], float scale[], float rotate[],int angle);
    // Resources
    QHash<QString, QGLShaderProgram *> m_shaderPrograms; // hash map of all shader programs
private slots:
    void tick();

protected:
     void setupScene();
     void updateCamera();
     void setupLights();
     float getMoveFactor();

     //! The snow emitter responsible for tracking snowflakes
     SnowEmitter m_snowEmitter;

     //! The camera settings
     OrbitCamera *m_camera;

     //! The overall speed
     float m_speed;

     //! Whether or not shift is pressed
     bool m_shift;

     //! The scene object factory to use when constructing scene objects
     SceneObjectFactory m_factory;

     //! The objects in the scene
     vector<SceneObject *> m_objects;

     //! Whether or not to display wireframes
     bool m_wireframes;

     //! Whether or not to display the solid scene
     bool m_solid;
};

#endif // VIEW_H

