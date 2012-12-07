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

void SnowEmitter::setSpeed(float *speed)
{
    m_speed = speed;
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
    m_snowflakes[snowflakeIndex].pos.w = 0;
    m_snowflakes[snowflakeIndex].dir.x = 0;
    m_snowflakes[snowflakeIndex].dir.y = -1;
    m_snowflakes[snowflakeIndex].dir.z = 0;
    m_snowflakes[snowflakeIndex].dir.w = 0;
    m_snowflakes[snowflakeIndex].speed = 0.5;
    m_snowflakes[snowflakeIndex].size = 0.004 + (float)rand()/((float)RAND_MAX/(0.002));
    resetWind(snowflakeIndex);
}

void SnowEmitter::resetWind(int snowflakeIndex)
{
    m_snowflakes[snowflakeIndex].windForce.x = -MAX_WIND_SPEED + (float)rand()/((float)RAND_MAX/(2.0 * MAX_WIND_SPEED));
    m_snowflakes[snowflakeIndex].windForce.y = -MAX_WIND_SPEED + (float)rand()/((float)RAND_MAX/((2.0-WIND_DOWNWARD_BIAS) * MAX_WIND_SPEED));
    m_snowflakes[snowflakeIndex].windForce.z = -MAX_WIND_SPEED + (float)rand()/((float)RAND_MAX/(2.0 * MAX_WIND_SPEED));
    m_snowflakes[snowflakeIndex].windForce.w = 0;
    m_snowflakes[snowflakeIndex].windExpire = rand() % (MAX_WIND_EXPIRE - MIN_WIND_EXPIRE)/(*m_speed) + MIN_WIND_EXPIRE/(*m_speed);
}

void SnowEmitter::tick()
{
    for(int i = 0; i < m_snowflakeCount; i++)
    {
        if( m_snowflakes[i].active )
        {
            m_snowflakes[i].dir = m_snowflakes[i].dir + m_snowflakes[i].windForce + Vector4(0, GRAVITY_Y_CHANGE, 0, 0);
            m_snowflakes[i].pos = (m_snowflakes[i].dir*(BASE_FLAKE_SPEED_FACTOR * m_snowflakes[i].speed * (*m_speed))) + m_snowflakes[i].pos;

            if( m_snowflakes[i].windExpire == 0 )
                resetWind(i);
            else
                m_snowflakes[i].windExpire--;

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

    Vector4 up = Vector4(0, 1.0, 0, 0);
    Vector4 right = m_camera->getDirection().cross(up);

    glColor3f(1.0f, 1.0f, 1.0f);
    for(int i = 0; i < m_snowflakeCount; i++)
    {
        if( m_snowflakes[i].active )
        {
            glPushMatrix();

            Vector4 normal = m_camera->eye - m_snowflakes[i].pos;
            glNormal3dv(normal.data);

            float size = m_snowflakes[i].size;

            Vector4 a = m_snowflakes[i].pos - (right + up) * size;
            Vector4 b = m_snowflakes[i].pos + (right - up) * size;
            Vector4 c = m_snowflakes[i].pos + (right + up) * size;
            Vector4 d = m_snowflakes[i].pos - (right - up) * size;

            glTexCoord2f(0, 1.0);
            glVertex3f(d.x, d.y, d.z);
            glTexCoord2f(0, 0);
            glVertex3f(a.x, a.y, a.z);
            glTexCoord2f(1.0, 0);
            glVertex3f(b.x, b.y, b.z);
            glTexCoord2f(1.0, 1.0);
            glVertex3f(c.x, c.y, c.z);

            glPopMatrix();
        }
    }
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_LIGHTING);
}
