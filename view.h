#ifndef VIEW_H
#define VIEW_H

#include <qgl.h>
#include <QTime>
#include <QTimer>
#include <QtOpenGL>
#include <QString>
#include <QImage>
#include <QFile>
#ifdef __APPLE__
    #include "glut/glut.h"
#endif
#include "SnowEmitter.h"
#include "camera.h"

// Speeds affecting the speed of snow simulation. Fast speed occurs when holding down
// the down arrow.
#define DEFAULT_SPEED 1.0
#define FAST_SPEED 20.0


#include "resourceloader.h"
#include <QHash>
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


    void paintUI();
    void paintSky();

    //! Loads the texture at the given location, returning the GL texture id
    GLuint loadTexture(const QString &path);
    void createShaderPrograms();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void drawUnitAxis(float x, float y, float z);
    void drawWireframeGrid();
    void drawPlane(float color[], float translate[]);
    void drawPlane(float color[], float translate[], float scale[], float rotate[],int angle);
    // Resources
    QHash<QString, QGLShaderProgram *> m_shaderPrograms; // hash map of all shader programs
private slots:
    void tick();

protected:
     void updateCamera();
     void setupLights();


     //! The snow emitter responsible for tracking snowflakes
     SnowEmitter m_snowEmitter;
     //! The camera settings
     OrbitCamera *m_camera;

     float m_speed;

};

#endif // VIEW_H

