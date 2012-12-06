#include "view.h"
#include <QApplication>
#include <QKeyEvent>
#include <ctime>
#include <CS123Common.h>
#include "camera.h"
#include "GL/glut.h"
View::View(QWidget *parent) : QGLWidget(parent)
{
    // View needs all mouse move events, not just mouse drag events
    setMouseTracking(true);

    // Hide the cursor since this is a fullscreen app
    //setCursor(Qt::BlankCursor);

    // View needs keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    // TODO: Seed the random number generator

    // The game loop is implemented using a timer
    connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));

    // Setup the camera
    m_camera = new OrbitCamera();
    m_camera->eye.x = 0.0f, m_camera->eye.y = 0.0f, m_camera->eye.z = 3.0f;
    m_camera->center.x = 0.0f, m_camera->center.y = 0.0f, m_camera->center.z = -1.0f;
    m_camera->up.x = 0.0f, m_camera->up.y = 1.0f, m_camera->up.z = 0.0f;
    m_camera->angle = 45.0f, m_camera->near = .05f, m_camera->far = 1000.0f;

    //added for orbit
    m_camera->zoom = 3.5f;
    m_camera->theta = M_PI * 1.5f, m_camera->phi = -0.2f;
    m_camera->fovy = 60.f;
    m_snowEmitter.setCamera(m_camera);


}

View::~View()
{
    safeDelete(m_camera);
}

GLuint View::loadTexture(const QString &path)
{
    QFile file(path);

    QImage image, texture;
    if(!file.exists()) {
        std::cout << "Texture file " << path.toStdString() << " does not exist" << std::endl;
        return -1;
    }
    image.load(file.fileName());
    texture = QGLWidget::convertToGLFormat(image);

    glEnable(GL_TEXTURE_2D);
    GLuint textureid;
    glGenTextures(1, &textureid);
    glBindTexture(GL_TEXTURE_2D, textureid);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    return textureid;
}

void View::initializeGL()
{
    // All OpenGL initialization *MUST* be done during or after this
    // method. Before this method is called, there is no active OpenGL
    // context and all OpenGL calls have no effect.

    // Start a timer that will try to get 60 frames per second (the actual
    // frame rate depends on the operating system and other running programs)
    time.start();
    timer.start(1000 / 60);

    // Center the mouse, which is explained more in mouseMoveEvent() below.
    // This needs to be done here because the mouse may be initially outside
    // the fullscreen window and will not automatically receive mouse move
    // events. This occurs if there are two monitors and the mouse is on the
    // secondary monitor.
    QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));

    glClearColor(0.0f,0.0f,0.0f,0.0f);

    // Start with flat shading
    glShadeModel(GL_FLAT);

    // Enable depth testing, so that objects are occluded based on depth instead of drawing order
    glEnable(GL_DEPTH_TEST);

    // Setup blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);

    // Enable alpha
    glEnable(GL_ALPHA_TEST);

    // Load the texture
    GLuint textureId = loadTexture( ":/textures/textures/snowflake_design.png" );
    m_snowEmitter.setTextureId( textureId );

    updateCamera();
    setupLights();

    paintGL();
}

void View::setupLights()
{
    // TODO: Update to directional light for sunlight/moonlight

    // Set up GL_LIGHT0 with a position and lighting properties
    GLfloat ambientLight[] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat position[] = { 2.0f, 2.0f, 2.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    glEnable(GL_LIGHT0);
}

/**
  * Applied the current camera position and orientation to the OpenGL modelview and projection matrices.
  */
void View::updateCamera()
{
    float w = width(), h = height();
    /*float ratio = w / h;

    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(m_camera->angle, ratio, m_camera->near, m_camera->far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(m_camera->eye.x, m_camera->eye.y, m_camera->eye.z,
              m_camera->center.x, m_camera->center.y, m_camera->center.z,
              m_camera->up.x, m_camera->up.y, m_camera->up.z);



    */




    float ratio = ((float) w) / h;
    Vector4 dir(-Vector4::fromAngles(m_camera->theta, m_camera->phi));
    Vector4 eye = m_camera->eye;//(m_camera->center - dir * m_camera->zoom);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(m_camera->fovy, ratio, 0.1f, 100000.f);
    gluLookAt(eye.x, eye.y, eye.z, eye.x + dir.x, eye.y + dir.y, eye.z + dir.z,
              m_camera->up.x, m_camera->up.y, m_camera->up.z);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void View::createPlane(float color[], float translate[])
{
    glColor3f(color[0],color[1],color[2]);
    glPushMatrix();
    glTranslatef(translate[0],translate[1],translate[2]);

    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 1.0f, 1.0f);
    glEnd();

    glPopMatrix();
}

void View::createPlane(float color[], float translate[], float scale[], float rotate[],int angle)
{
    glColor3f(color[0],color[1],color[2]);
    glPushMatrix();
    glTranslatef(translate[0],translate[1],translate[2]);
    glScalef(scale[0],scale[1],scale[2]);
    glRotatef(angle,rotate[0],rotate[1],rotate[2]);

    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 1.0f, 1.0f);
    glEnd();

    glPopMatrix();
}

void View::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // NOTE: Opaque objects must be rendered before transparent. This means
    //       objects before snowflakes.

    glEnable(GL_LIGHTING);

    // TODO: Paint scene

    glDisable(GL_LIGHTING);

    paintSky();
    /*
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1.0);
    glColor3f(0.1, 0.1, 0.1);
    glVertex3f(-10, -10, -50);
    glVertex3f(10, -10, -50);
    glVertex3f(10, 10, -50);
    glVertex3f(-10, 10, -50);
    glEnd();
    */
    float col[3] = {0.0f, 0.7f, 0.93f};
    float pos[3] = {-5.0f, -1.0f, 5.0f};
    float scale[3] = {10.0f, 10.0f, 10.0f};
    float rot[3] ={0.0f, 1.0f, 0.0f};
    createPlane(col,pos,scale,rot,90);

    // Render dem snowflakes
    m_snowEmitter.drawSnowflakes();

    glPopMatrix();

    // Display the frame
    glFlush();
    swapBuffers();
}

void View::paintSky()
{
    glBegin(GL_QUADS);
    glColor3f(.2, .2, .2);
    glVertex3f(-1000, 3.0f, 1000);
    glVertex3f(-1000, 3, -1000);
    glVertex3f(1000, 3, -1000);
    glVertex3f(1000, 3, 1000);
    glColor3f(0.25f, .5f, 0.35f);
    glVertex3f(-1000, -3, 1000);
    glVertex3f(-1000, -3, -1000);
    glVertex3f(1000, -3, -1000);
    glVertex3f(1000, -3, 1000);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(1,0,0); // x - red
    glVertex3f(0,0,0);
    glVertex3f(1,0,0 );
    glColor3f(0,1,0); // y - green
    glVertex3f(0,0,0);
    glVertex3f(0,1,0);
    glColor3f(0,0,1); // z - blue
    glVertex3f(0,0,0);
    glVertex3f(0,0,1);
    glEnd();
}

void View::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void View::mousePressEvent(QMouseEvent *event)
{
    m_prevMousePos.x = event->x();
    m_prevMousePos.y = event->y();
    //cout<<"pressed"<<endl;
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    // This starter code implements mouse capture, which gives the change in
    // mouse position since the last mouse movement. The mouse needs to be
    // recentered after every movement because it might otherwise run into
    // the edge of the screen, which would stop the user from moving further
    // in that direction. Note that it is important to check that deltaX and
    // deltaY are not zero before recentering the mouse, otherwise there will
    // be an infinite loop of mouse move events.
    //int deltaX = event->x() - width() / 2;
    //int deltaY = event->y() - height() / 2;
    //if (!deltaX && !deltaY) return;
    //QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));

    // TODO: Handle mouse movements here

    Vector2 pos(event->x(), event->y());
    if (event->buttons() & Qt::LeftButton || event->buttons() & Qt::RightButton)
    {
        m_camera->mouseMove(pos - m_prevMousePos);
        //cout<<"moved"<<endl;
    }
    m_prevMousePos = pos;

    updateCamera();

}

void View::mouseReleaseEvent(QMouseEvent *event)
{
    //cout<<"released"<<endl;
}



/**
  Called when the mouse wheel is turned.  Zooms the camera in and out.
**/
void View::wheelEvent(QWheelEvent *event)
{
    if (event->orientation() == Qt::Vertical)
    {
        m_camera->mouseWheel(event->delta());
    }
}

void View::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) QApplication::quit();

    if(event->key() == Qt::Key_W) {
        Vector4 translationVec = m_camera->center;
        translationVec.y = 0;
        translationVec.normalize();
        m_camera->eye = m_camera->eye + .01 * translationVec;
        updateCamera();
    } else if(event->key() == Qt::Key_S) {
        Vector4 translationVec = m_camera->center;
        translationVec.y = 0;
        translationVec.normalize();
        m_camera->eye = m_camera->eye - .01 * translationVec;
        updateCamera();
    } else if(event->key() == Qt::Key_A) {
        Vector4 translationVec;
        float cosVal = cos(-M_PI/2.0);
        float sinVal = sin(-M_PI/2.0);
        translationVec.x = m_camera->center.x * cosVal - m_camera->center.z * sinVal;
        translationVec.y = 0;
        translationVec.z = m_camera->center.z * cosVal + m_camera->center.x * sinVal;
        translationVec.normalize();
        m_camera->eye = m_camera->eye + .01 * translationVec;
        updateCamera();
    } else if( event->key() == Qt::Key_D) {
        Vector4 translationVec;
        float cosVal = cos(M_PI/2.0);
        float sinVal = sin(M_PI/2.0);
        translationVec.x = m_camera->center.x * cosVal - m_camera->center.z * sinVal;
        translationVec.y = 0;
        translationVec.z = m_camera->center.z * cosVal + m_camera->center.x * sinVal;
        translationVec.normalize();
        m_camera->eye = m_camera->eye + .01 * translationVec;
        updateCamera();
    }
}

void View::keyReleaseEvent(QKeyEvent *event)
{
}

void View::tick()
{
    // Get the number of seconds since the last tick (variable update rate)
    float seconds = time.restart() * 0.001f;

    // TODO: Implement the demo update here
    m_snowEmitter.tick();

    // Flag this view for repainting (Qt will call paintGL() soon after)
    update();
}
