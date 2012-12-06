#include "SnowEmitter.h"

SnowEmitter::SnowEmitter()
{
    m_snowflakeCount = 0;
    m_activeSnowflakes = 0;
    m_snowflakes = NULL;

    // Initialize the snow flakes
    initializeSnowflakes(INITIAL_SNOWFLAKE_COUNT);
}

SnowEmitter::~SnowEmitter()
{
    if( m_snowflakes != NULL )
    {
        delete m_snowflakes;
        m_snowflakes = NULL;
        m_snowflakeCount = 0;
    }
}

void SnowEmitter::setTextureId(GLuint textureId)
{
    m_textureId = textureId;
}

void SnowEmitter::setCamera(OrbitCamera *camera)
{
    m_camera = camera;
}

void SnowEmitter::initializeSnowflakes(int flakeCount)
{
    if( m_snowflakes != NULL )
    {
        // Free the snowflakes!
        delete m_snowflakes;
        m_snowflakes = NULL;
        m_snowflakeCount = 0;
    }

    // Create the snowflake array on the heap
    m_snowflakes = new Snowflake[flakeCount];
    m_snowflakeCount = flakeCount;
    for( int i = 0; i < m_snowflakeCount; i++ )
    {
        m_snowflakes[i].active = false;
    }
}

void SnowEmitter::dropSnowflake(int snowflakeIndex)
{
    float camera_x = m_camera->eye.x;
    float camera_z = m_camera->eye.z;

    // One more active snowflake falling
    m_activeSnowflakes++;

    m_snowflakes[snowflakeIndex].active = true;
    m_snowflakes[snowflakeIndex].pos.y = INITIAL_SNOWFLAKE_HEIGHT + (float)rand()/((float)RAND_MAX/(10.0)) ;
    m_snowflakes[snowflakeIndex].pos.x = camera_x - 2.0 + (float)rand()/((float)RAND_MAX/(4.0));
    m_snowflakes[snowflakeIndex].pos.z = camera_z - 2.0 + (float)rand()/((float)RAND_MAX/(4.0));
    m_snowflakes[snowflakeIndex].dir.x = 0;
    m_snowflakes[snowflakeIndex].dir.y = -1;
    m_snowflakes[snowflakeIndex].dir.z = 0;
    m_snowflakes[snowflakeIndex].speed = 0.5;
    m_snowflakes[snowflakeIndex].size = 0.004 + (float)rand()/((float)RAND_MAX/(0.002));
}

void SnowEmitter::tick()
{
    for(int i = 0; i < m_snowflakeCount; i++)
    {
        if( m_snowflakes[i].active )
        {
            // TODO: Do physics updates
            m_snowflakes[i].pos = (m_snowflakes[i].dir*(BASE_FLAKE_SPEED_FACTOR * m_snowflakes[i].speed)) + m_snowflakes[i].pos;
            if( m_snowflakes[i].pos.y < SNOWFLAKE_CUTOFF )
            {
                m_snowflakes[i].active = false;
                m_activeSnowflakes--;
            }
        }
        else
        {
            int inactiveCount = m_snowflakeCount - m_activeSnowflakes;
            if( (qrand() % 1000 ) < inactiveCount * SNOWFLAKE_DROP_PROBABILITY )
            {
                // Activate this snowflake
                dropSnowflake(i);
            }
        }
    }
}

void SnowEmitter::drawSnowflakes()
{
    glDisable(GL_LIGHTING);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    glBegin(GL_QUADS);
    Vector4 camUp = m_camera->up;
    Vector4 camLook = m_camera->center;
    Vector4 camRight = (camUp*camLook).getNormalized();
    Matrix4x4 billboardMat = Matrix4x4(
            camRight.x,camRight.y,camRight.z,0,
            camUp.x,camUp.y,camUp.z,0,
            camLook.x,camLook.y,camLook.z,0,
            0,0,0,0
            );
    glColor3f(1.0f, 1.0f, 1.0f);
    for(int i = 0; i < m_snowflakeCount; i++)
    {
        if( m_snowflakes[i].active )
        {
            glPushMatrix();

            glMultMatrixd(billboardMat.data);
            Vector4 normal = m_camera->eye - m_snowflakes[i].pos;
            glNormal3dv(normal.data);

            float size = m_snowflakes[i].size;

            glTexCoord2f(0.0, 1.0);
            glVertex3f(m_snowflakes[i].pos.x - size, m_snowflakes[i].pos.y + size, m_snowflakes[i].pos.z );
            glTexCoord2f(0.0, 0.0);
            glVertex3f(m_snowflakes[i].pos.x - size, m_snowflakes[i].pos.y - size, m_snowflakes[i].pos.z );
            glTexCoord2f(1.0, 0.0);
            glVertex3f(m_snowflakes[i].pos.x + size, m_snowflakes[i].pos.y - size, m_snowflakes[i].pos.z );
            glTexCoord2f(1.0, 1.0);
            glVertex3f(m_snowflakes[i].pos.x + size, m_snowflakes[i].pos.y + size, m_snowflakes[i].pos.z );

            glPopMatrix();
        }
    }
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_LIGHTING);
}
