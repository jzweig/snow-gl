#include "view.h"
#include <QApplication>
#include <QKeyEvent>
#include <ctime>
#include <CS123Common.h>
#include "camera.h"
#include <pwd.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include "GL/glut.h"
#include "GL/glu.h"
#include <GL/freeglut.h>
#endif

static  QString PROJECT_DIR = "/home/jbowens/course/cs123/snow-gl/";
static const int MAX_FPS = 60;

#ifndef __APPLE__
extern "C" {
    GLAPI void APIENTRY glBindBuffer (GLenum target, GLuint buffer);
    GLAPI void APIENTRY glGenBuffers (GLsizei n, GLuint *buffers);
    GLAPI void APIENTRY glBufferData (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
}
#endif

View::View(QWidget *parent) : QGLWidget(parent),
        m_timer(this), m_prevTime(0), m_prevFps(0.f), m_fps(0.f),m_font("Deja Vu Sans Mono", 8, 4)
{
    m_showShader = true;
    m_gridLength = 60;
    // View needs all mouse move events, not just mouse drag events
    setMouseTracking(true);

    // View needs keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    // Set the starting speed
    m_speed = DEFAULT_SPEED;

    // Seed the random number generator
    srand(QTime::currentTime().msec());

    // The event loop timer
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));

    // Setup the camera
    m_camera = new OrbitCamera();
    m_camera->eye.x = 0.0f, m_camera->eye.y = 1.0f, m_camera->eye.z = 0.0f;
    m_camera->center.x = 0.0f, m_camera->center.y = 0.0f, m_camera->center.z = -1.0f;
    m_camera->up.x = 0.0f, m_camera->up.y = 1.0f, m_camera->up.z = 0.0f;
    m_camera->angle = 45.0f, m_camera->near = .01f, m_camera->far = 1000.0f;

    // Orbit Camera specific settings
    m_camera->zoom = 3.5f;
    m_camera->theta = M_PI * 1.5f, m_camera->phi = -0.2f;
    m_camera->fovy = 60.f;
    m_snowEmitter.setCamera(m_camera);
    m_snowEmitter.setSpeed(&m_speed);

    // Initialize fbos
    m_fbo_main = m_fbo_snow = NULL;

    // Initialize settings
    m_isWireframe = false;
    m_isSolid = true;
    m_showUnitAxis = false;
    m_useVbo = true;

    int terrain_array_size = m_gridLength * m_gridLength;
    m_snowHeight = new float[terrain_array_size];
    //Test terrain heights
    m_snowHeightMap = new QImage(m_gridLength,m_gridLength,QImage::Format_RGB32);

    // set the new image to black
    //memset(m_snowHeightMap->bits(), 0, width * height * sizeof(BGRA));
    m_data = (BGRA *)m_snowHeightMap->bits();
    uchar* temp =  m_snowHeightMap->bits();
    for(int i=0;i<m_gridLength;i++){
        for(int j=0;j<m_gridLength;j++){
            float incr = ((float) rand())/RAND_MAX;
            float incg = ((float) rand())/RAND_MAX;
            float incb = ((float) rand())/RAND_MAX;
            m_snowHeight[i*m_gridLength+j] = incr*100;//(i+j)/(1.0f*m_gridLength);
            m_data[i*m_gridLength+j].r =((int)(incr*255));
            m_data[i*m_gridLength+j].g =((int)(incg*255));
            m_data[i*m_gridLength+j].b =((int)(incb*255));
            m_data[i*m_gridLength+j].a = 255;
        }
    }
/*
    for(int i=0;i<m_gridLength;i++){
        for(int j=0;j<m_gridLength;j++){
            float incr = ((float) rand())/(float)RAND_MAX;
            float incg = ((float) rand())/RAND_MAX;
            float incb = ((float) rand())/RAND_MAX;
            m_snowHeight[i*m_gridLength+j] = incr*100;//(i+j)/(1.0f*m_gridLength);
            m_data[i*m_gridLength+j].r =((int)(incr*0));
            m_data[i*m_gridLength+j].g =((int)(incg*0));
            m_data[i*m_gridLength+j].b =((int)(incb*0));
            m_data[i*m_gridLength+j].a = 255;
            //cout<<incr<<endl;
        }
    }

        for(int j=0;j<m_gridLength;j++){
            m_data[j].r =((int)(255));
            m_data[j].g =((int)(0));
            m_data[j].b =((int)(0));
            m_data[j].a = 255;
            //cout<<incr<<endl;
        }*/

    // Make sure the image file exists
    QFile file("/course/cs123/data/image/BoneHead.jpg");
    if (!file.exists())
        cout<<"/course/cs123/data/image/BoneHead.jpg FILE DOES NOT EXIST"<<endl;

    // Load the file into memory
    //m_snowHeightMap->load(file.fileName());

    m_homeDir = getpwuid(getuid())->pw_dir;
    m_projDir = m_homeDir+"/course/cs123/snow-gl/";
}

View::~View()
{
    safeDelete(m_camera);
    safeDelete(m_snowHeight);
    // Delete the scene objects
    for( vector<SceneObject *>::iterator it = m_objects.begin(); it != m_objects.end(); it++ ) {
        delete *it;
    }
    m_objects.clear();

    // Delete any frame buffer objects
    if( m_fbo_main ) {
        delete m_fbo_main;
        m_fbo_main = NULL;
    }
    if( m_fbo_snow ) {
        delete m_fbo_snow;
        m_fbo_snow = NULL;
    }
}

void View::setupScene()
{
    // Make the ground
    m_factory.setTesselationParameter(m_gridLength);
    SceneObject *ground = m_factory.constructCube();
    ground->setColor(0.2, 0.39, 0.18, 1.0); // when not using the shader
    ground->scale(20.0, 0.2, 20.0);
    ground->translate(0, -0.5, 0);
    m_objects.push_back(ground);
    m_terrain = ground;

    // Make a demo box
    m_factory.setTesselationParameter(50);
    SceneObject *demoBox = m_factory.constructCube();
    demoBox->setColor(0.25, 0.25, 0.25, 1.0);
    demoBox->translate(-5.0, 0.5, 5.0);
    m_objects.push_back(demoBox);

    // Make a smaller box
    m_factory.setTesselationParameter(50);
    SceneObject *smallBox = m_factory.constructCube();
    smallBox->setColor(0.2, 0.2, 0.45, 0.75);
    smallBox->scale(0.5, 0.5, 0.5);
    smallBox->translate(-3.0, 0.5, 7.0);
    m_objects.push_back(smallBox);

    initSceneVbo();
}

void View::initSceneVbo()
{

    // Iterate through all the triangles of all the objects
    for( vector<SceneObject *>::iterator it = m_objects.begin(); it != m_objects.end(); it++ )
    {
        Shape *shape = (*it)->getShape();
        int objTriangleCount = shape->getNumTriangles();

        float *buffer = new float[objTriangleCount*24];
        GLuint buffer_name;
        glGenBuffers(1, &buffer_name);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_name);

        int bufferIndex = 0;

        Triangle *triangles = shape->getTriangles();
        for( int i = 0; i < objTriangleCount; i++ )
        {
            memcpy(buffer + bufferIndex, triangles[i].getVertexOne()->getData(), sizeof(float)*3);
            bufferIndex += 3;
            memcpy(buffer + bufferIndex, triangles[i].getVertexOne()->getNormal()->getData(), sizeof(float)*3);
            bufferIndex += 3;
            buffer[bufferIndex++] = triangles[i].getVertexOne()->getTexCoord()->x();
            buffer[bufferIndex++] = triangles[i].getVertexOne()->getTexCoord()->y();

            memcpy(buffer + bufferIndex, triangles[i].getVertexTwo()->getData(), sizeof(float)*3);
            bufferIndex += 3;
            memcpy(buffer + bufferIndex, triangles[i].getVertexTwo()->getNormal()->getData(), sizeof(float)*3);
            bufferIndex += 3;
            buffer[bufferIndex++] = triangles[i].getVertexTwo()->getTexCoord()->x();
            buffer[bufferIndex++] = triangles[i].getVertexTwo()->getTexCoord()->y();

            memcpy(buffer + bufferIndex, triangles[i].getVertexThree()->getData(), sizeof(float)*3);
            bufferIndex += 3;
            memcpy(buffer + bufferIndex, triangles[i].getVertexThree()->getNormal()->getData(), sizeof(float)*3);
            bufferIndex += 3;
            buffer[bufferIndex++] = triangles[i].getVertexThree()->getTexCoord()->x();
            buffer[bufferIndex++] = triangles[i].getVertexThree()->getTexCoord()->y();
        }

        // Tell the scene object its vbo buffer
        (*it)->setVboBuffer(buffer_name);

        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*objTriangleCount*24, buffer, GL_STATIC_DRAW);

        // unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        delete[] buffer;
    }

}

/**
  Create shader programs.
 **/
void View::createShaderPrograms()
{
    const QGLContext *ctx = context();
    QString vShader = m_projDir+"shaders/snow.vert";
    QString fShader = m_projDir+"shaders/snow.frag";
    m_shaderPrograms["snow"] = ResourceLoader::newShaderProgram(ctx, vShader, fShader);
}


void View::initializeGL()
{
    cout << "Using OpenGL Version " << glGetString(GL_VERSION) << endl << endl;

    cout << "initialized shader programs..." << endl;

    // Start the drawing timer
    m_timer.start(1000.0f / MAX_FPS);

    // Center the mouse
    QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));

    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);

    // Enable depth testing, so that objects are occluded based on depth instead of drawing order
    glEnable(GL_DEPTH_TEST);

    // Setup blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // Enable alpha
    glEnable(GL_ALPHA_TEST);

    // Tesselate the scene and setup all of the transformation matrices
    setupScene();

    // Create the frame buffer objects
    createFramebufferObjects(width(), height());

    // Load the texture
    GLuint textureId = ResourceLoader::loadTexture( ":/textures/textures/snowflake_design.png" );
    m_snowEmitter.setTextureId( textureId );
    applyProjectionCamera();
    setupLights();
    glFrontFace(GL_CCW);
    glEnable(GL_TEXTURE_2D);
    createShaderPrograms();
    paintGL();
}

void View::setupLights()
{
    // TODO: Update to directional light for sunlight/moonlight

    // Set up GL_LIGHT0 with a position and lighting properties
    GLfloat ambientLight[] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat diffuseLight[] = { 0.1f, 0.1f, 0.1f, 0.4f };
    GLfloat specularLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
    GLfloat position[] = { 5.0f, 5.5f, 5.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    glEnable(GL_LIGHT0);
}

/**
  * Applied the current camera position and orientation to the OpenGL modelview and projection matrices.
  */
void View::applyProjectionCamera()
{
    float w = width(), h = height();

    float ratio = ((float) w) / h;
    Vector4 dir(-Vector4::fromAngles(m_camera->theta, m_camera->phi));
    Vector4 eye = m_camera->eye;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(m_camera->fovy, ratio, 0.1f, 100000.f);
    gluLookAt(eye.x, eye.y, eye.z, eye.x + dir.x, eye.y + dir.y, eye.z + dir.z,
              m_camera->up.x, m_camera->up.y, m_camera->up.z);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**
  Called to switch to an orthogonal OpenGL camera.
  Useful for rending a textured quad across the whole screen.
**/
void View::applyOrthogonalCamera()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.f, width(), 0.f, height());
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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

void View::renderScene()
{
    // Render the wireframes if enabled
    if( m_isWireframe ) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        (*m_terrain).render(m_useVbo);
         for(vector<SceneObject *>::iterator it = m_objects.begin(); it != m_objects.end(); it++) {
            (*it)->render(m_useVbo);
         }
    }

    // Render the solid scene
    if( m_isSolid ) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        GLuint textureId = ResourceLoader::loadHeightMapTexture(m_snowHeightMap);
        for(vector<SceneObject *>::iterator it = m_objects.begin(); it != m_objects.end(); it++) {
            if(m_showShader){
                glActiveTexture(textureId);
                glBindTexture(GL_TEXTURE_2D,textureId);
                m_shaderPrograms["snow"]->bind();
                // Load the texture
                //GLuint textureId = ResourceLoader::loadHeightMapTexture(m_snowHeight,m_gridLength,m_gridLength);
                m_shaderPrograms["snow"]->setUniformValue("time", m_clock.elapsed());
                //m_shaderPrograms["snow"]->setUniformValue("snowTexture", textureId);
                GLuint sky = ResourceLoader::loadSkybox();
                glCallList(sky);
                (*m_terrain).render(m_useVbo);

                m_shaderPrograms["snow"]->release();

                glBindTexture(GL_TEXTURE_2D,0);

                (*it)->render(m_useVbo);
            } else {
                (*m_terrain).render(m_useVbo);
                (*it)->render(m_useVbo);
            }
        }
    }
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void View::paintGL()
{
    // NOTE: Opaque objects must be rendered before transparent. This means
    //       objects before snowflakes.

    // Update the fps
    int time = m_clock.elapsed();
    m_fps = 1000.f / (time - m_prevTime);
    m_prevTime = time;

    // Clear the scene
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // Render the scene to a framebuffer
    m_fbo_main->bind();
    glEnable(GL_LIGHTING);
    renderScene();
    if( m_showUnitAxis )
    {
        glDisable(GL_LIGHTING);
        drawUnitAxis(0.f,0.f,0.f);
        glEnable(GL_LIGHTING);
    }
    m_fbo_main->release();

    // Render dem snowflakes
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    m_snowEmitter.drawSnowflakes();
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);

    glPopMatrix();

    // Display the frame
    glFlush();
    swapBuffers();

    // Paint GUI
    paintUI();
}

/**
  Draws text for the FPS
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
}

void View::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    // If the dimensions changed and we have fbo's, we need new fbo's of the right size
    if( m_fbo_main && m_fbo_snow )
        createFramebufferObjects(w, h);
}

void View::mousePressEvent(QMouseEvent *event)
{
    m_prevMousePos.x = event->x();
    m_prevMousePos.y = event->y();
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    Vector2 pos(event->x(), event->y());
    if (event->buttons() & Qt::LeftButton || event->buttons() & Qt::RightButton)
    {
        m_camera->mouseMove(pos - m_prevMousePos);
    }
    m_prevMousePos = pos;

    applyProjectionCamera();
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
    if (event->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if(event->key() == Qt::Key_Down) {
        m_speed = FAST_SPEED;
    } else if(event->key() == Qt::Key_Shift) {
        m_isShiftPressed = true;
    } else if(event->key() == Qt::Key_1) {
        m_isWireframe = ! m_isWireframe;
    } else if(event->key() == Qt::Key_2) {
        m_isSolid = ! m_isSolid;
    } else if(event->key() == Qt::Key_3) {
        m_showUnitAxis = ! m_showUnitAxis;
    } else if(event->key() == Qt::Key_4) {
        m_useVbo = ! m_useVbo;
    } else if(event->key() == Qt::Key_5) {
        m_showShader = ! m_showShader;
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
        applyProjectionCamera();
    }
}

float View::getMoveFactor()
{
    return m_isShiftPressed ? SPRINT_FACTOR : WALK_FACTOR;
}

void View::keyReleaseEvent(QKeyEvent *event)
{
    if( event->key() == Qt::Key_Down )
        m_speed = DEFAULT_SPEED;
    else if( event->key() == Qt::Key_Shift)
        m_isShiftPressed = false;
}

void View::tick()
{
    // Get the number of seconds since the last tick (variable update rate)
    //float seconds = m_clock.restart() * 0.001f;

    m_snowEmitter.tick();

    // Flag this view for repainting (Qt will call paintGL() soon after)
    update();
}

/**
  Allocate framebuffer objects.

  @param width: the viewport width
  @param height: the viewport height
 **/
void View::createFramebufferObjects(int width, int height)
{
    // Deallocate any existing fbo's.
    if( m_fbo_main ) {
        delete m_fbo_main;
        m_fbo_main = NULL;
    }
    if( m_fbo_snow ) {
        delete m_fbo_snow;
        m_fbo_snow = NULL;
    }

    // Allocate the main framebuffer object for rendering the scene to
    // This needs a depth attachment
    m_fbo_main = new QGLFramebufferObject(width, height, QGLFramebufferObject::Depth,
                                          GL_TEXTURE_2D, GL_RGB16F_ARB);
    m_fbo_main->format().setSamples(16);
    // Allocate the secondary framebuffer obejcts for rendering snow displacement.
    // I think this also needs a depth attachment.
    m_fbo_snow = new QGLFramebufferObject(width, height, QGLFramebufferObject::Depth,
                                          GL_TEXTURE_2D, GL_RGB16F_ARB);
    m_fbo_snow->format().setSamples(16);
}
