Final
jmzweig & jbowens

Real time snow accumulation

User Interaction
    We implemented an orbital camera, controlled by the mouse (left mouse button held), and WASD movement.
    Settings are controlled using 1-0 keys
    Down Arrow (held) - faster snow
    U key - Uber mode (super fast)

Particle Physics
    Particle positions and velocity are updated at every frame.


Collision detection
    When updated, each snow particle is checked for collision with the scene objects (terrain and cubes). (if they are below any of the objects in the scene.)
    If they are, we catalog the hit by incrementing our height maps.

Displacement mapping
    Vertices are displaced by snow accumulation.  New normals are calculated on the GPU.



Texture mapping
    In addition to a skybox and mapped cubes and the plane, as snow comes down it maps a snow texture onto the ground at the collision points.
    (Texture blending)

Lighting
    Diffuse and Ambient


Performance Enhancements:
VBOs - Vertex Buffer Objects (sends vertices to the GPU for storage)
PBOs - Pixel Buffer Objects (high resolution heightmap textures are held in buffer objects to stream new collisions onto the GPU at every frame).
Multithreading - Collision Detection and Particle physics are done on multiple threads.
