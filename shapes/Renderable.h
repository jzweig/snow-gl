#ifndef RENDERABLE_H
#define RENDERABLE_H

/**
  A renderable shape/object/some-dimensional figure.
  */
class Renderable
{
    public:
        Renderable();

        //! Renders this shape.
        virtual void render() = 0;
};

#endif // RENDERABLE_H
