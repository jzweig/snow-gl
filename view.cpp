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
#include "GL/glext.h"
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
extern "C" {
GLAPI void APIENTRY glBindBufferARB (GLenum target, GLuint buffer);
GLAPI void APIENTRY glDeleteBuffersARB (GLsizei n, const GLuint *buffers);
GLAPI void APIENTRY glGenBuffersARB (GLsizei n, GLuint *buffers);
GLAPI GLboolean APIENTRY glIsBufferARB (GLuint buffer);
GLAPI void APIENTRY glBufferDataARB (GLenum target, GLsizeiptrARB size, const GLvoid *data, GLenum usage);
GLAPI void APIENTRY glBufferSubDataARB (GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid *data);
GLAPI void APIENTRY glGetBufferSubDataARB (GLenum target, GLintptrARB offset, GLsizeiptrARB size, GLvoid *data);
GLAPI GLvoid* APIENTRY glMapBufferARB (GLenum target, GLenum access);
GLAPI GLboolean APIENTRY glUnmapBufferARB (GLenum target);
GLAPI void APIENTRY glGetBufferParameterivARB (GLenum target, GLenum pname, GLint *params);
GLAPI void APIENTRY glGetBufferPointervARB (GLenum target, GLenum pname, GLvoid* *params);
}
#endif

View::View(QWidget *parent) : QGLWidget(parent),
    m_timer(this), m_prevTime(0), m_prevFps(0.f), m_fps(0.f),m_font("Deja Vu Sans Mono", 8, 4)
{
    m_pboIndexA =0;
    m_pboIndexB =0;
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


    // The event loop timer
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));

    // Setup the camera
    m_camera = new OrbitCamera();
    m_camera->eye.x = 0.0f, m_camera->eye.y = 1.0f, m_camera->eye.z = 0.0f;
    m_camera->center.x = 0.0f, m_camera->center.y = 0.0f, m_camera->center.z = -1.0f;
    m_camera->up.x = 0.0f, m_camera->up.y = 1.0f, m_camera->up.z = 0.0f;
    m_camera->angle = 45.0f, m_camera->near = .01f, m_camera->far = 1000.0f;

    //added for orbit
    m_camera->zoom = 3.5f;
    m_camera->theta = M_PI * 1.5f, m_camera->phi = -0.2f;
    m_camera->fovy = 60.f;
    m_snowEmitter.setCamera(m_camera);
    m_snowEmitter.setSpeed(&m_speed);

    m_isWireframe = false;
    m_isSolid = true;
    m_showUnitAxis = false;
    m_useVbo = true;
    m_usePbo = true;
    m_useDisplacement = true;
    m_useShader = true;
    m_showSkybox = true;

    m_homeDir = getpwuid(getuid())->pw_dir;
    m_projDir = m_homeDir+"/course/cs123/snow-gl/";
}

View::~View()
{
    // Delete the scene objects
    for( vector<SceneObject *>::iterator it = m_objects.begin(); it != m_objects.end(); it++ ) {
        SceneObject *obj = *it;

        // Free the object's texture if it has one
        GLuint textureId = obj->getColorTexture();
        if( textureId != 0 )
            glDeleteTextures(1, &textureId);
        delete *it;
    }
    m_objects.clear();

    // Delete snowflake textures
    for( vector<GLuint>::iterator it = m_snowflakeTextures.begin(); it != m_snowflakeTextures.end(); it++ ) {
        GLuint textureId = *it;
        glDeleteTextures(1, &textureId);
    }
    m_snowflakeTextures.clear();

    // Delete the shaders
    foreach (QGLShaderProgram *sp, m_shaderPrograms)
        delete sp;

    // Delete the snow texture
    glDeleteTextures(1, &m_snowTextureId);
    m_snowTextureId = 0;

    // Delete Skybox

    glDeleteLists(m_cubeMap, 1);

}

void View::setupScene()
{
    // Make the ground
    m_factory.setTesselationParameter(75);
    m_factory.setBumpResolution(512);
    m_factory.setOriginalScale(20.0, 1.0, 20.0);
    m_factory.setOriginalTranslation(0.0,0.0,0.0);
    SceneObject *ground = m_factory.constructPlane();
    ground->setTexture(ResourceLoader::loadTexture( ":/textures/textures/seamless_rock_texture.jpg" ));
    ground->setColor(1, 0.2, 0.2, 1.0);
    m_objects.push_back(ground);

    // Make a demo box
    m_factory.setTesselationParameter(32);
    m_factory.setBumpResolution(128);
    m_factory.setOriginalScale(1.0, 1.0, 1.0);
    m_factory.setOriginalTranslation(-5.0, 0.40, 5.0);
    SceneObject *demoBox = m_factory.constructCube();
    demoBox->setColor(0.5, 0.15, 0.15, 1.0);
    m_objects.push_back(demoBox);

    // Make a smaller box
    m_factory.setTesselationParameter(16);
    m_factory.setBumpResolution(32);
    m_factory.setOriginalScale(0.5, 0.5, 0.5);
    m_factory.setOriginalTranslation(-3.0, 0.5, 7.0);
    SceneObject *smallBox = m_factory.constructCube();
    smallBox->setColor(0.2, 0.5, 0.2, 1.0);
    m_objects.push_back(smallBox);

    initSceneVbo();
    initScenePbo();
}

void View::initScenePbo()
{
    // Iterate through all of the objects
    for( vector<SceneObject *>::iterator it = m_objects.begin(); it != m_objects.end(); it++ )
    {
        int bufferSize = (*it)->m_bumpResolution * (*it)->m_bumpResolution * sizeof(BGRA);
        GLuint* ids = (*it)->m_pbo;
        //generate two buffers to alternate between during updating for performance (halting)
        glGenBuffersARB(2, ids);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, ids[m_pboIndexA]);
        glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, bufferSize, 0, GL_STREAM_DRAW_ARB);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, ids[m_pboIndexB]);
        glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, bufferSize, 0, GL_STREAM_DRAW_ARB);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
        // Tell the scene object its pbo buffer
        (*it)->setPboBuffers(ids);

    }
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

    glEnable(GL_TEXTURE_2D);
    createShaderPrograms();

    // All OpenGL initialization *MUST* be done during or after this
    // method. Before this method is called, there is no active OpenGL
    // context and all OpenGL calls have no effect.

    // Start a timer that will try to get 60 frames per second (the actual
    // frame rate depends on the operating system and other running programs)
    //time.start();
    //timer.start(1000 / 60);

    // Start the drawing timer
    m_timer.start(1000.0f / MAX_FPS);

    // Center the mouse, which is explained more in mouseMoveEvent() below.
    // This needs to be done here because the mouse may be initially outside
    // the fullscreen window and will not automatically receive mouse move
    // events. This occurs if there are two monitors and the mouse is on the
    // secondary monitor.
    QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));

    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);

    // Enable depth testing, so that objects are occluded based on depth instead of drawing order
    glEnable(GL_DEPTH_TEST);
    // Setup blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // Setup the cube map
    setupCubeMap();

    // Enable alpha
    glEnable(GL_ALPHA_TEST);

    setupScene();

    // Load the snow texture
    m_snowflakeTextures.push_back( ResourceLoader::loadTexture( ":/textures/textures/snowflake_design.png" ) );
    m_snowflakeTextures.push_back( ResourceLoader::loadTexture( ":/textures/textures/second-snowflake.png" ) );
    m_snowflakeTextures.push_back( ResourceLoader::loadTexture( ":/textures/textures/snowball-texture.png" ) );
    m_snowflakeTextures.push_back( ResourceLoader::loadTexture( ":/textures/textures/snowflake-icon.png" ) );
    m_snowflakeTextures.push_back( ResourceLoader::loadTexture( ":/textures/textures/actual-snowflake.png" ) );
    m_snowEmitter.setTextures(&m_snowflakeTextures);

    m_snowTextureId = ResourceLoader::loadTexture( ":/textures/textures/plain-surface.jpg" );

    updateCamera();

    setupLights();
    glFrontFace(GL_CCW);
    paintGL();
}

void View::setupLights()
{
    // TODO: Update to directional light for sunlight/moonlight

    // Set up GL_LIGHT0 with a position and lighting properties
    GLfloat ambientLight[] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat position[] = { 5.0f, 5.5f, 5.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glEnable(GL_LIGHT0);

}
/**
  Load a cube map for the skybox
 **/
void View::setupCubeMap()
{
    QList<QFile *> fileList;

    QString skyType = QString::fromStdString("sky32");
    fileList.append(new QFile( m_projDir + "/skymaps/" + skyType + "/posx.jpg"));
    fileList.append(new QFile( m_projDir + "/skymaps/" + skyType + "/negx.jpg"));
    fileList.append(new QFile( m_projDir + "/skymaps/" + skyType + "/posy.jpg"));
    fileList.append(new QFile( m_projDir + "/skymaps/" + skyType + "/negy.jpg"));
    fileList.append(new QFile( m_projDir + "/skymaps/" + skyType + "/posz.jpg"));
    fileList.append(new QFile( m_projDir + "/skymaps/" + skyType + "/negz.jpg"));
    m_cubeMap = ResourceLoader::loadCubeMap(fileList);
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

void View::drawUnitAxis(float x, float y, float z){

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(x,y,z);
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
    glPopMatrix();
}

void View::renderScene()
{
    QGLShaderProgram *shader = m_shaderPrograms["snow"];

    // Render the wireframes if enabled
    if( m_isWireframe ) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        for(vector<SceneObject *>::iterator it = m_objects.begin(); it != m_objects.end(); it++) {
            (*it)->render(m_useVbo);
        }
    }

    // Render the solid scene
    if( m_isSolid ) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        // Enable cube maps and draw the skybox
        if( m_showSkybox )
        {
            glDisable(GL_LIGHTING);
            glEnable(GL_TEXTURE_CUBE_MAP);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMap);
            GLuint sky = ResourceLoader::loadSkybox();
            glCallList(sky);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            glDisable(GL_TEXTURE_CUBE_MAP);
            glEnable(GL_LIGHTING);
        }

        for(vector<SceneObject *>::iterator it = m_objects.begin(); it != m_objects.end(); it++) {

            SceneObject *obj = *it;

            if(m_useShader){
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                shader->bind();
                shader->setUniformValue("snowTexture", 1);
                shader->setUniformValue("snowDisplacement", 2);
                shader->setUniformValue("localTexture", 3);
                shader->setUniformValue("snowSurfaceTexture", 4);
                shader->setUniformValue("useDisplacement", m_useDisplacement);
                Vector4 color = obj->getColor();
                shader->setUniformValue("color",color.x, color.y, color.z, color.w);
                Vector3 scale = obj->getOriginalScale();
                shader->setUniformValue("scale",scale.x, scale.y, scale.z);
                shader->setUniformValue("tesselationParam", obj->getShape()->getParamOne());

                // Set the blur data
                int radius = 6;
                int dim = radius * 2 + 1;
                GLfloat kernel[dim * dim];
                GLfloat offsets[dim * dim * 2];
                GLfloat ambientAry[4] = {0.7, 0.7, 0.7, 1.0};
                GLfloat diffuseAry[4] = {color.x, color.y, color.z, color.w};
                GLfloat specularAry[4] = {0.9, 0.9, 0.9, 0.0};
                GLfloat m = 0.55;
                GLfloat r0 = 0.7;
                ShaderAssistant::createBlurKernel(radius, width(), height(), &kernel[0], &offsets[0]);
                shader->setUniformValue("arraySize", dim * dim);
                shader->setUniformValueArray("offsets", offsets, 2 * dim * dim, 2);
                shader->setUniformValueArray("ambient", ambientAry, 1, 4);
                shader->setUniformValueArray("diffuse", diffuseAry, 1, 4);
                shader->setUniformValueArray("specular", specularAry, 1, 4);
                shader->setUniformValue("m", m);
                shader->setUniformValue("r0", r0);

                // Displacement
                ResourceLoader::reloadHeightMapTexture(obj->getDisplacementMap(),obj->getDisplacementMapId());
                glActiveTexture(GL_TEXTURE2);
                //glUniform1i(glGetUniformLocation(shader->programId(), "snowDisplacement"), 0);
                glBindTexture(GL_TEXTURE_2D,obj->getDisplacementMapId());


                // Bump
                glActiveTexture(GL_TEXTURE1);
                //QImage img = QGLWidget::convertToGLFormat((* obj->getBumpMap()).mirrored(false,true));
                if(!m_usePbo){
                    ResourceLoader::reloadHeightMapTexture(obj->getBumpMap(),obj->getBumpMapId());
                    glBindTexture(GL_TEXTURE_2D,obj->getBumpMapId());
                }else{
                    m_pboIndexA = (m_pboIndexA + 1) % 2;
                    m_pboIndexB = (m_pboIndexA + 1) % 2;

                    glBindTexture(GL_TEXTURE_2D,obj->getBumpMapId());
                    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, obj->getPboBuffers()[m_pboIndexA]);

                    //PBO -> Texture
                    int width = obj->getBumpMap()->width();
                    int height = obj->getBumpMap()->height();
                    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, height, width, GL_BGRA, GL_UNSIGNED_BYTE, 0);

                    int imgSize = width * height* sizeof(BGRA);
                    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, obj->getPboBuffers()[m_pboIndexB]);
                    glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, imgSize, 0, GL_STREAM_DRAW_ARB);
                    GLubyte* ptr = (GLubyte*)glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
                    if(ptr){
                        unsigned char *pixel = (unsigned char *)ptr;
                        /*for (int i = 0; i < img.height(); i++) {
                            memcpy(pixel, img.scanLine(i), img.bytesPerLine());
                            pixel += img.bytesPerLine();
                        }*/
                        memcpy(pixel, obj->getBumpMap()->bits(), obj->getBumpMap()->byteCount());
                        glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);
                    }
                    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
                }

                if( obj->getColorTexture() != 0 ) {
                    shader->setUniformValue("useLocalTexture", true);
                    glActiveTexture(GL_TEXTURE3);
                    glBindTexture(GL_TEXTURE_2D, obj->getColorTexture());
                } else {
                    shader->setUniformValue("useLocalTexture", false);
                }

                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D, m_snowTextureId);

                glActiveTexture(GL_TEXTURE0);
            }
            (*it)->render(m_useVbo);
            if( m_useShader )
            {
                shader->release();
                glBindTexture(GL_TEXTURE_2D,0);
                glDisable(GL_BLEND);
            }
        }


    }
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void View::paintGL()
{

    updateCamera();
    setupLights();

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

    // Render all the objects in the scene
    renderScene();

    if( m_showUnitAxis )
    {
        glDisable(GL_LIGHTING);
        drawUnitAxis(0.f,0.f,0.f);

        //draw light
            GLfloat position[] = { 5.0f, 5.5f, 5.0f, 1.0f };
            drawUnitAxis(position[0],position[1],position[2]);
        glEnable(GL_LIGHTING);
    }

    // Render dem snowflakes
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    m_snowEmitter.drawSnowflakes();
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);

    glPopMatrix();

    // Display the frame
    //glFlush();
    //swapBuffers();

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
        m_useShader = ! m_useShader;
    } else if(event->key() == Qt::Key_6) {
        m_useDisplacement = ! m_useDisplacement;
    } else if(event->key() == Qt::Key_7) {
        m_usePbo = ! m_usePbo;
    } else if(event->key() == Qt::Key_8) {
        m_showSkybox = ! m_showSkybox;
    } else if(event->key() == Qt::Key_U) {
        m_useUberMode = ! m_useUberMode;
        m_speed = m_useUberMode ? UBER_SPEED : DEFAULT_SPEED;
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
    // Handle collision detections concurrently
    QFutureSynchronizer<void> sync;
    for(vector<SceneObject *>::iterator it = m_objects.begin(); it != m_objects.end(); it++) {
        SceneObject *obj = *it;
        QFuture<void> future = QtConcurrent::run(&m_snowEmitter, &SnowEmitter::collisionDetect, obj);
        sync.addFuture(future);
    }
    sync.waitForFinished();

    m_snowEmitter.tick();

    // Flag this view for repainting (Qt will call paintGL() soon after)
    update();
}
