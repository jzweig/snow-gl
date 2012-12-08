#ifndef SNOWEMITTER_H
#define SNOWEMITTER_H

#define SNOWFALL_RADIUS 10
#define INITIAL_SNOWFLAKE_COUNT 50000
#define INITIAL_SNOWFLAKE_HEIGHT 6.0f
#define SNOWFLAKE_CUTOFF 0
#define BASE_FLAKE_SPEED_FACTOR .0025f
#define SNOWFLAKE_DROP_PROBABILITY .01f
#define MAX_WIND_EXPIRE 50
#define MIN_WIND_EXPIRE 5
#define MAX_WIND_SPEED 0.001
#define WIND_DOWNWARD_BIAS 0
#define GRAVITY_Y_CHANGE -0.0001
#define SNOWFLAKE_PROCESSING_BATCH 15000

//#include "common.h"
#include "CS123Common.h"
#include "CS123Algebra.h"
#include <QtOpenGL>
#include <QtCore>
#include <QFutureSynchronizer>
#include "camera.h"


/**
 * The declaration for a snowflake.
 */
struct Snowflake
{
    /**
      * Determines whether or not this particle is active. If it is not active,
      * it should not be drawn or modified by updateParticles().
      */
    bool active;
    /**
      * Determines the size of the snowflake.
      */
    float size;
    /**
      * The particle's current position in 3D space. Updated every step based on
      * the particle's velocity.
      */
    Vector4 pos;
    /**
      * The direction this particle is currently moving. The velocity of any particle
      * in the system is: velocity = ParticleEmitter.m_speed * Particle.dir
      */
    Vector4 dir;
    /**
      * The force acting on this particle from wind
      */
    Vector4 windForce;
    /**
      * The time remaining on this this snowflake's wind force.
      */
    int windExpire;
    /**
      * Whether or not this snowflake might be visible.
      */
    bool isVisible;
};

class SnowEmitter
{
    public:
        SnowEmitter();
        virtual ~SnowEmitter();

        /**
          * Draws all the active snowflakes.
          */
        void drawSnowflakes();

        /**
          * Executes the effects of a time step.
          */
        void tick();

        /**
          * Executes the effects of a time step on particles within the
          * given range, inclusive.
          */
        void rangedTick(int lowBound, int highBound);

        /**
          * Sets the texture id to use for snowflakes.
          */
        void setTextureId(GLuint textureId);

        /**
          * Sets the camera being used with this snow emitter.
          */
        void setCamera(OrbitCamera *camera);

        /**
          * Sets a pointer to where the snow emitter can find the overall
          * speed of the environment.
          */
        void setSpeed(float *speed);

    protected:

        /**
          * Resets all the snowflakes, using the given number of snowflakes.
          */
        void initializeSnowflakes(int flakeCount);

        /**
          * Reset the wind vector on the snowflake at the given index.
          */
        void resetWind(int snowflake);

        /**
          * Resets a snowflake to fall from the sky at a random location.
          */
        void dropSnowflake(int snowflakeIndex);

        //! Snowflakes array on the heap
        Snowflake *m_snowflakes;
        int m_snowflakeCount;
        int m_activeSnowflakes;

        //! The width of a snowflake
        float m_scale;

        //! A pointer to the overall speed of the environment
        float *m_speed;

        //! The GL texture id for the snowflake texture
        GLuint m_textureId;

        //! Pointer to the camera
        OrbitCamera *m_camera;
};

#endif // SNOWEMITTER_H
