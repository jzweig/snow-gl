#include "view.h"
#include <QApplication>
#include <QKeyEvent>
#include <ctime>
#include <CS123Common.h>
#include "camera.h"
#include "GL/glut.h"
static const int MAX_FPS = 60;
View::View(QWidget *parent) : QGLWidget(parent),
        m_timer(this), m_prevTime(0), m_prevFps(0.f), m_fps(0.f),m_font("Deja Vu Sans Mono", 8, 4)
{

    // View needs all mouse move events, not just mouse drag events
    setMouseTracking(true);

    // Hide the cursor since this is a fullscreen app
    //setCursor(Qt::BlankCursor);

    // View needs keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    // Set the starting speed
    m_speed = DEFAULT_SPEED;

    // Seed the random number generator
    srand(QTime::currentTime().msec());

    // The game loop is implemented using a timer
    connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));

    // fps timer
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));

    // Setup the camera
    m_camera = new OrbitCamera();
    m_camera->eye.x = 0.0f, m_camera->eye.y = 1.0f, m_camera->eye.z = 0.0f;
    m_camera->center.x = 0.0f, m_camera->center.y = 0.0f, m_camera->center.z = -1.0f;
    m_camera->up.x = 0.0f, m_camera->up.y = 1.0f, m_camera->up.z = 0.0f;
    m_camera->angle = 45.0f, m_camera->near = .05f, m_camera->far = 1000.0f;

    //added for orbit
    m_camera->zoom = 3.5f;
    m_camera->theta = M_PI * 1.5f, m_camera->phi = -0.2f;
    m_camera->fovy = 60.f;
    m_snowEmitter.setCamera(m_camera);
    m_snowEmitter.setSpeed(&m_speed);

}

View::~View()
{
    safeDelete(m_camera);
    glmDelete(m_dragon.model);
}

/**
  Create shader programs.
 **/
void View::createShaderPrograms()
{
    const QGLContext *ctx = context();
    //m_shaderPrograms["terrain"] = ResourceLoader::newShaderProgram(ctx, ":/shaders/shaders/terrain.vert", ":/shaders/shaders/terrain.frag");
    m_shaderPrograms["pulse"] = ResourceLoader::newShaderProgram(ctx, ":/shaders/shaders/pulse.vert", ":/shaders/shaders/pulse.frag");
  //  m_shaderPrograms["brightpass"] = ResourceLoader::newFragShaderProgram(ctx, "shaders/brightpass.frag");
   // m_shaderPrograms["blur"] = ResourceLoader::newFragShaderProgram(ctx, "shaders/blur.frag");
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

    // Start the drawing timer
    m_timer.start(1000.0f / MAX_FPS);

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

    //load that dragon...
    m_dragon = ResourceLoader::loadObjModel("/course/cs123/bin/models/xyzrgb_dragon.obj");

    // Load the texture
    GLuint textureId = ResourceLoader::loadTexture( ":/textures/textures/snowflake_design.png" );
    m_snowEmitter.setTextureId( textureId );

    updateCamera();
    setupLights();
    glFrontFace(GL_CCW);
    createShaderPrograms();
    cout << "initialized shader programs..." << endl;
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


void View::drawPlane(float color[], float translate[])
{
    glColor3f(color[0],color[1],color[2]);
    glPushMatrix();
    glTranslatef(translate[0],translate[1],translate[2]);

    glBegin(GL_QUADS);
    //XY axis
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glEnd();

    glPopMatrix();
}

void View::drawPlane(float color[], float translate[], float scale[], float rotate[],int angle)
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

void View::drawWireframeGrid()
{
    glLineWidth(1.5);
    glDepthMask(GL_FALSE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(.6, .6, .6);
    glBegin(GL_LINES);
    int pMax = 20;
    int pMax2 = pMax/2;
    for (int i=0; i<=pMax; i++)
    {
        glVertex3i(i-pMax2, 0, -pMax2);
        glVertex3i(i-pMax2, 0, pMax2);
        glVertex3i(-pMax2, 0, i-pMax2);
        glVertex3i(pMax2, 0, i-pMax2);


        glVertex3i(i-pMax2, 0, -pMax2);
        glVertex3i(i-pMax2, 5.9, -pMax2);


        glVertex3i(i-pMax2, 0, pMax2);
        glVertex3i(i-pMax2, 5.9, pMax2);


        glVertex3i(pMax2, 0, i-pMax2);
        glVertex3i(pMax2, 5.9, i-pMax2);

        glVertex3i(-pMax2, 0, i-pMax2);
        glVertex3i(-pMax2, 5.9, i-pMax2);
    }

    glEnd();
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
    glDepthMask(GL_TRUE);
}

void View::drawUnitAxis(float x, float y, float z){

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

void View::paintGL()
{

    glPushMatrix();
    glTranslatef(-.25f, 3.f, 0.f);
    glCallList(m_dragon.idx);
    glPopMatrix();
    // Update the fps
    int time = m_clock.elapsed();
    m_fps = 1000.f / (time - m_prevTime);
    m_prevTime = time;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // NOTE: Opaque objects must be rendered before transparent. This means
    //       objects before snowflakes.

    glEnable(GL_LIGHTING);

    // TODO: Paint scene

    float col[3] = {0.0f, 0.2f, 0.6f};
    float pos[3] = {5.0f, 0.5f, 0.0f};
    float scale[3] = {10.0f, 10.0f, 10.0f};
    float rot[3] ={0.0f, 0.0f, 1.0f};
    m_shaderPrograms["pulse"]->setUniformValue("time",m_clock.elapsed());
    m_shaderPrograms["pulse"]->setUniformValue("color", QVector4D(1.0f, 0.5f, 0.5f, 1.0f));
    m_shaderPrograms["pulse"]->bind();
    drawPlane(col,pos,scale,rot,90);
    m_shaderPrograms["pulse"]->release();
    glDisable(GL_LIGHTING);
    drawUnitAxis(0.f,0.f,0.f);
    //paintSky();


    drawWireframeGrid();
    // Render dem snowflakes
    glEnable(GL_BLEND);
    m_snowEmitter.drawSnowflakes();
    glDisable(GL_BLEND);

    glPopMatrix();

    // Display the frame
    glFlush();
    swapBuffers();

    // Paint GUI
    paintUI();
}



void View::paintSky()
{
    glBegin(GL_QUADS);
    glColor3f(.2, .2, .2);
    glVertex3f(-1000, 6, 1000);
    glVertex3f(-1000, 6, -1000);
    glVertex3f(1000, 6, -1000);
    glVertex3f(1000, 6, 1000);
    glColor3f(0.25f, .5f, 0.35f);
    glVertex3f(-1000, 0, 1000);
    glVertex3f(-1000, 0, -1000);
    glVertex3f(1000, 0, -1000);
    glVertex3f(1000, 0, 1000);
    glEnd();
}
/**
  Draws text for the FPS and screenshot prompt
 **/
void View::paintUI()
{
    glColor3f(1.f, 1.f, 1.f);

    // Combine the previous and current framerate
    if (m_fps >= 0 && m_fps < 1000)
    {
       m_prevFps *= 0.95f;
       m_prevFps += m_fps * 0.05f;
    }

    // QGLWidget's renderText takes xy coordinates, a string, and a font
    renderText(10, 20, "FPS: " + QString::number((int) (m_prevFps)), m_font);
    renderText(10, 35, "S: Save screenshot", m_font);
}

void View::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void View::mousePressEvent(QMouseEvent *event)
{
    m_prevMousePos.x = event->x();
    m_prevMousePos.y = event->y();
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
    }
    m_prevMousePos = pos;

    updateCamera();

}

void View::mouseReleaseEvent(QMouseEvent *event)
{
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

    if (event->key() == Qt::Key_Escape){
        QApplication::quit();
    } else if(event->key() == Qt::Key_Down) {
        m_speed = FAST_SPEED;
    } else if(event->key() == Qt::Key_Shift) {
        m_shift = true;
    } else {
        Vector4 dirVec = m_camera->getDirection();
        dirVec.y = 0;
        dirVec.normalize();

        if(event->key() == Qt::Key_W) {
            m_camera->eye = m_camera->eye + dirVec * getMoveFactor();
        } else if(event->key() == Qt::Key_S) {
            m_camera->eye = m_camera->eye - dirVec * getMoveFactor();
        } else if(event->key() == Qt::Key_A) {
            float cosVal = cos(-M_PI/2.0);
            float sinVal = sin(-M_PI/2.0);
            Vector4 translationVec;
            translationVec.x = dirVec.x * cosVal - dirVec.z * sinVal;
            translationVec.z = dirVec.z * cosVal + dirVec.x * sinVal;
            translationVec.y = 0;
            translationVec.normalize();
            m_camera->eye = m_camera->eye + translationVec * getMoveFactor();
        } else if( event->key() == Qt::Key_D) {
            float cosVal = cos(M_PI/2.0);
            float sinVal = sin(M_PI/2.0);
            Vector4 translationVec;
            translationVec.x = dirVec.x * cosVal - dirVec.z * sinVal;
            translationVec.z = dirVec.z * cosVal + dirVec.x * sinVal;
            translationVec.y = 0;
            translationVec.normalize();
            m_camera->eye = m_camera->eye + translationVec * getMoveFactor();
        }
        updateCamera();
    }
}

float View::getMoveFactor()
{
    if( m_shift )
        return 0.1;
    else
        return .01;
}

void View::keyReleaseEvent(QKeyEvent *event)
{
    if( event->key() == Qt::Key_Down )
        m_speed = DEFAULT_SPEED;
    else if( event->key() == Qt::Key_Shift)
        m_shift = false;
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
