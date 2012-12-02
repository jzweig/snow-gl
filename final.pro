# 
# CS123 Final Project Starter Code
# Adapted from starter code graciously provided by CS195-U: 3D Game Engines
#

QT += core gui opengl

TARGET = final
TEMPLATE = app

# If you add your own folders, add them to INCLUDEPATH and DEPENDPATH, e.g. folder1 folder2
INCLUDEPATH += lib \
    math
DEPENDPATH += lib \
    math
SOURCES += main.cpp \
    mainwindow.cpp \
    view.cpp \
    math/CS123Matrix.cpp \
    SnowEmitter.cpp

HEADERS += mainwindow.h \
    view.h \
    SnowEmitter.h \
    lib/CS123SceneData.h \
    lib/CS123Common.h \
    math/CS123Algebra.h \
    common.h

FORMS += mainwindow.ui

RESOURCES += textures.qrc
