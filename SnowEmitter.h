#ifndef SNOWEMITTER_H
#define SNOWEMITTER_H

#define INITIAL_SNOWFLAKE_COUNT 10000
#define INITIAL_FLAKES_PER_TICK 1

#include "common.h"

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
    float3 pos;
    /**
      * The speed at which the snowflake is travelling
      */
    float speed;
    /**
      * The direction this particle is currently moving. The velocity of any particle
      * in the system is: velocity = ParticleEmitter.m_speed * Particle.dir
      */
    float3 dir;
    /**
      * The force acting on this particle (e.g. from gravity). At each update step,
      * Particle.dir += Particle.force.
      */
    float3 force;
};

class SnowEmitter
{
    public:
        SnowEmitter();

        /**
          * Draws all the active snowflakes.
          */
        void drawSnowflakes();

        /**
          * Executes the effects of a time step.
          */
        void tick();

    protected:

        /**
          * Resets all the snowflakes, using the given number of snowflakes.
          */
        void initializeSnowflakes(int flakeCount);

        /**
          * Resets a snowflake to fall from the sky at a random location.
          */
        void dropSnowflake(int snowflakeIndex);

        //! Snowflakes array on the heap
        Snowflake *m_snowflakes;
        int m_snowflakeCount;
        int m_activeSnowflakes;
        int m_flakesPerTick;

        //! The width of a snowflake
        float m_scale;
};

#endif // SNOWEMITTER_H
