#ifndef VIEW_H
#define VIEW_H

#include <qgl.h>
#include <QTime>
#include <QTimer>
#include <QtOpenGL>
#include "GL/gl.h"
#include "SnowEmitter.h"
#include "common.h"

class View : public QGLWidget
{
    Q_OBJECT

public:
    View(QWidget *parent);
    ~View();

private:
    QTime time;
    QTimer timer;

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private slots:
    void tick();

protected:
     void updateCamera();
     void setupLights();

     //! The snow emitter responsible for tracking snowflakes
     SnowEmitter m_snowEmitter;

     //! The camera settings
     Camera *m_camera;

};

#endif // VIEW_H

