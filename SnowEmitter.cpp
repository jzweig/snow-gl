#include "SnowEmitter.h"
#include <QtOpenGL>

SnowEmitter::SnowEmitter()
{
    m_snowflakeCount = 0;
    m_activeSnowflakes = 0;
    m_snowflakes = NULL;

    m_flakesPerTick = INITIAL_FLAKES_PER_TICK;

    // Initialize the snow flakes
    initializeSnowflakes(INITIAL_SNOWFLAKE_COUNT);
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
    // TODO: Update to drop around the camera, if we're going to
    // allow moving around, panning, etc.

    float camera_x = 0.0;
    float camera_z = 0.0;

    // One more active snowflake falling
    m_activeSnowflakes++;

    m_snowflakes[snowflakeIndex].active = true;
    m_snowflakes[snowflakeIndex].pos.y = 3.0 + (float)rand()/((float)RAND_MAX/(10.0)) ;
    m_snowflakes[snowflakeIndex].pos.x = camera_x - 5.0 + (float)rand()/((float)RAND_MAX/(10.0));
    m_snowflakes[snowflakeIndex].pos.z = camera_z - 5.0 + (float)rand()/((float)RAND_MAX/(10.0));
    m_snowflakes[snowflakeIndex].dir.x = 0;
    m_snowflakes[snowflakeIndex].dir.y = -1;
    m_snowflakes[snowflakeIndex].dir.z = 0;
    m_snowflakes[snowflakeIndex].speed = 0.5;
    m_snowflakes[snowflakeIndex].size = 0.01;
}

void SnowEmitter::tick()
{


    for(int i = 0; i < m_snowflakeCount; i++)
    {
        if( m_snowflakes[i].active )
        {
            // TODO: Do physics updates
            m_snowflakes[i].pos = .01f * m_snowflakes[i].speed * m_snowflakes[i].dir + m_snowflakes[i].pos;
            if( m_snowflakes[i].pos.y < -3 )
            {
                m_snowflakes[i].active = false;
                m_activeSnowflakes--;
            }
        }
        else
        {
            int inactiveCount = m_snowflakeCount - m_activeSnowflakes;
            if( (qrand() % 1000 ) < inactiveCount * .01 )
            {
                // Activate this snowflake
                dropSnowflake(i);
            }
        }
    }
}

void SnowEmitter::drawSnowflakes()
{
    // TODO: Bind dat texture
    float halfScale = m_scale / 2.0f;
    //glBindTexture(GL_TEXTURE_2D, m_textureID);

    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBegin(GL_QUADS);

    glNormal3f(0, 0, 1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    for(unsigned i = 0; i < m_snowflakeCount; i++)
    {
        if( m_snowflakes[i].active )
        {

            float size = m_snowflakes[i].size;

            glVertex3f(m_snowflakes[i].pos.x - size, m_snowflakes[i].pos.y + size, m_snowflakes[i].pos.z );
            glVertex3f(m_snowflakes[i].pos.x - size, m_snowflakes[i].pos.y - size, m_snowflakes[i].pos.z );
            glVertex3f(m_snowflakes[i].pos.x + size, m_snowflakes[i].pos.y - size, m_snowflakes[i].pos.z );
            glVertex3f(m_snowflakes[i].pos.x + size, m_snowflakes[i].pos.y + size, m_snowflakes[i].pos.z );

            /*
            //glTexCoord2f(0.0f, 1.0f);
            glVertex3f(m_snowflakes[i].pos.x - halfScale, m_snowflakes[i].pos.y + halfScale, -2 );
            //glTexCoord2f(0.0f, 0.0f);
            glVertex3f(m_snowflakes[i].pos.x - halfScale, m_snowflakes[i].pos.y - halfScale, -2 );
            //glTexCoord2f(1.0f, 0.0f);
            glVertex3f(m_snowflakes[i].pos.x + halfScale, m_snowflakes[i].pos.y - halfScale, -2 );
            //glTexCoord2f(1.0f, 1.0f);
            glVertex3f(m_snowflakes[i].pos.x + halfScale, m_snowflakes[i].pos.y + halfScale, -2 );
            */

        }
    }
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}
