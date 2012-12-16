#include "SnowEmitter.h"

SnowEmitter::SnowEmitter()
{
    m_snowflakeCount = 0;
    m_activeSnowflakes = 0;
    m_snowflakes = NULL;
    m_directableCounter = 1024*4;
    m_directableGridSize = 1024;
    m_directableGridInc = 1;
    m_isDirectableSnow = !true;
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

void SnowEmitter::setTextures(vector<GLuint> *textures)
{
    m_flakeTextures = textures;
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
    m_snowflakes[snowflakeIndex].isVisible = false;
    if(m_isDirectableSnow){

        m_snowflakes[snowflakeIndex].pos.y = INITIAL_SNOWFLAKE_HEIGHT;
        m_snowflakes[snowflakeIndex].pos.x = -10.0f+(((float)(m_directableCounter%m_directableGridSize))/m_directableGridSize)*20.0;
        m_snowflakes[snowflakeIndex].pos.z = -10.0f+(((float)m_directableCounter/m_directableGridSize))/(m_directableGridSize)*20.0;
        m_snowflakes[snowflakeIndex].pos.w = 1;
        m_snowflakes[snowflakeIndex].dir.x = 0;
        m_snowflakes[snowflakeIndex].dir.y = -1;
        m_snowflakes[snowflakeIndex].dir.z = 0;
        m_snowflakes[snowflakeIndex].dir.w = 0;
        m_snowflakes[snowflakeIndex].size = 0.003;
        m_directableCounter = (m_directableCounter+1)%(m_directableGridSize*m_directableGridSize);
    }else{
        m_snowflakes[snowflakeIndex].pos.y = INITIAL_SNOWFLAKE_HEIGHT + (float)rand()/((float)RAND_MAX/(10.0));
        m_snowflakes[snowflakeIndex].pos.x = camera_x - SNOWFALL_RADIUS + (float)rand()/((float)RAND_MAX/(SNOWFALL_RADIUS*2));
        m_snowflakes[snowflakeIndex].pos.z = camera_z - SNOWFALL_RADIUS + (float)rand()/((float)RAND_MAX/(SNOWFALL_RADIUS*2));
        m_snowflakes[snowflakeIndex].pos.w = 1;
        m_snowflakes[snowflakeIndex].dir.x = 0;
        m_snowflakes[snowflakeIndex].dir.y = -1;
        m_snowflakes[snowflakeIndex].dir.z = 0;
        m_snowflakes[snowflakeIndex].dir.w = 0;
        m_snowflakes[snowflakeIndex].size = 0.003 + (float)rand()/((float)RAND_MAX/(0.006));
        resetWind(snowflakeIndex);

    }
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
    QFutureSynchronizer<void> sync;
    for( int i = 0; i < m_snowflakeCount; i+= SNOWFLAKE_PROCESSING_BATCH )
    {
        int startIndex = i;
        int endIndex = MIN((i + SNOWFLAKE_PROCESSING_BATCH) - 1, m_snowflakeCount-1);
        QFuture<void> future = QtConcurrent::run(this, &SnowEmitter::rangedTick, startIndex, endIndex);
        sync.addFuture(future);
    }
    sync.waitForFinished();
}

void SnowEmitter::rangedTick(int minIndex, int maxIndex)
{
    for(int i = minIndex; i < maxIndex; i++)
    {
        if( m_snowflakes[i].active )
        {
            if( m_snowflakes[i].pos.y < SNOWFLAKE_CUTOFF ||
                (m_snowflakes[i].pos.y > INITIAL_SNOWFLAKE_HEIGHT &&
                (fabs(m_camera->eye.x - m_snowflakes[i].pos.x) > SNOWFALL_RADIUS || fabs(m_camera->eye.z - m_snowflakes[i].pos.z) > SNOWFALL_RADIUS)) ) {
                m_snowflakes[i].active = false;
                m_activeSnowflakes--;
                continue;
            }

            m_snowflakes[i].dir = m_snowflakes[i].dir + m_snowflakes[i].windForce + Vector4(0, GRAVITY_Y_CHANGE, 0, 0);
            m_snowflakes[i].pos = (m_snowflakes[i].dir*(BASE_FLAKE_SPEED_FACTOR * (*m_speed))) + m_snowflakes[i].pos;

            if( m_snowflakes[i].windExpire == 0 ){
                if(!m_isDirectableSnow)
                    resetWind(i);
            } else {
                m_snowflakes[i].windExpire--;
            }
            if( m_snowflakes[i].pos.y < SNOWFLAKE_CUTOFF ) {
                m_snowflakes[i].active = false;
                m_activeSnowflakes--;
            } else if( m_snowflakes[i].pos.y < INITIAL_SNOWFLAKE_HEIGHT && ! m_snowflakes[i].isVisible ) {
                m_snowflakes[i].isVisible = true;
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


    // Calculate the up and right vectors for use in billboarding
    Vector4 up = m_camera->getUp();
    Vector4 right = m_camera->getDirection().cross(up);

    glColor3f(1.0f, 1.0f, 1.0f);

    int flakesPerTexture = ceil(float(m_snowflakeCount) / float(m_flakeTextures->size()));

    for(int t = 0; t < m_flakeTextures->size(); t++)
    {
        GLuint textureId = m_flakeTextures->at(t);

        glBindTexture(GL_TEXTURE_2D, textureId);
        glBegin(GL_QUADS);

        int startIndex = flakesPerTexture * t;
        int endIndex = flakesPerTexture * (t + 1);

        for(int i = startIndex; i < endIndex && m_snowflakeCount; i++)
        {
            if( m_snowflakes[i].active && m_snowflakes[i].isVisible )
            {
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
            }
        }

        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glEnable(GL_LIGHTING);
}

void SnowEmitter::collisionDetect(SceneObject* obj)
{
    Matrix4x4 inverseTransformationMatrix = obj->getTransformationMatrix().getTranspose().getInverse();

    for(int i = 0; i < m_snowflakeCount; i++){

        if( m_snowflakes[i].active ){
            Vector4 worldSnowPos = m_snowflakes[i].pos;
            Vector4 objSnowPos = inverseTransformationMatrix * worldSnowPos;

            // Check for a collision on the unit cube. No support for non-cube objects.
            if((objSnowPos.x <= 0.5 && objSnowPos.x >= -0.5) &&
               (objSnowPos.z <= 0.5 && objSnowPos.z >= -0.5)){

                if( obj->collide(objSnowPos) ) {
                    m_snowflakes[i].active = false;
                    m_activeSnowflakes--;
                }
            }
        }

    }
}
