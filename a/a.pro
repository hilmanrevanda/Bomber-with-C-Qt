TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

win32:INCLUDEPATH +=  C:/SDL2-2.0.3/include

win32:LIBS += -L C:/SDL2-2.0.3/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf

QMAKE_CXXFLAGS += -std=c++11
