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
//#include "common.h"

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

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    //! Loads the texture at the given location, returning the GL texture id
    GLuint loadTexture(const QString &path);

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
private slots:
    void tick();

protected:
     void updateCamera();
     void setupLights();

     void paintSky();

     //! The snow emitter responsible for tracking snowflakes
     SnowEmitter m_snowEmitter;
     //! The camera settings
     OrbitCamera *m_camera;

};

#endif // VIEW_H

