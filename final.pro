# 
# CS123 Final Project Starter Code
# Adapted from starter code graciously provided by CS195-U: 3D Game Engines
#

QT += core gui opengl

TARGET = final
TEMPLATE = app

# If you add your own folders, add them to INCLUDEPATH and DEPENDPATH, e.g. folder1 folder2
INCLUDEPATH += lib \
    math \
    shaders
DEPENDPATH += lib \
    math \
    shaders

SOURCES += main.cpp \
    mainwindow.cpp \
    view.cpp \
    math/CS123Matrix.cpp \
    SnowEmitter.cpp \
    lib/targa.cpp \
    lib/glm.cpp \
    resourceloader.cpp \
    camera.cpp \
    shapes/Vertex.cpp \
    shapes/Vector.cpp \
    shapes/Triangle.cpp \
    shapes/Shape.cpp \
    shapes/Renderable.cpp \
    shapes/Cube.cpp \
    SceneObject.cpp \
    SceneObjectFactory.cpp

HEADERS += mainwindow.h \
    view.h \
    SnowEmitter.h \
    lib/CS123SceneData.h \
    lib/CS123Common.h \
    lib/targa.h \
    lib/glm.h \
    math/CS123Algebra.h \
    resourceloader.h \
    camera.h \
    lib/targa.h \
    lib/glm.h \
    shapes/Vertex.h \
    shapes/Vector.h \
    shapes/Triangle.h \
    shapes/Shape.h \
    shapes/Renderable.h \
    shapes/Cube.h \
    SceneObject.h \
    SceneObjectFactory.h

FORMS += mainwindow.ui

RESOURCES += textures.qrc \
    shaders.qrc
OTHER_FILES += \
shaders/terrain.frag \
shaders/terrain.vars \
shaders/terrain.vert \
shaders/reflect.frag \
shaders/reflect.vert \
    shaders/snow.frag \
    shaders/snow.vert

# Don't add the -pg flag unless you know what you are doing. It makes QThreadPool freeze on Mac OS X
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS_WARN_ON -= -Wall
QMAKE_CXXFLAGS_WARN_ON += -Waddress \
    -Warray-bounds \
    -Wc++0x-compat \
    -Wchar-subscripts \
    -Wformat \
    -Wmain \
    -Wmissing-braces \
    -Wparentheses \
    -Wreorder \
    -Wreturn-type \
    -Wsequence-point \
    -Wsign-compare \
    -Wstrict-aliasing \
    -Wstrict-overflow=1 \
    -Wswitch \
    -Wtrigraphs \
    -Wuninitialized \
    -Wunused-label \
    -Wunused-variable \
    -Wvolatile-register-var \
    -Wno-extra
