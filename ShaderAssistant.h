#ifndef SHADERASSISTANT_H
#define SHADERASSISTANT_H

#include<QtOpenGL>

/**
  * lol, didn't know what to name this
  */
class ShaderAssistant
{
public:
    ShaderAssistant();
    static void createBlurKernel(int radius, int width, int height,
                                 GLfloat* kernel, GLfloat* offsets);
};

#endif // SHADERASSISTANT_H
