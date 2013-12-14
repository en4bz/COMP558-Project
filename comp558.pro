TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += /usr/include/opencv2 \
				/usr/include/boost/

QMAKE_CXX = g++
QMAKE_CXXFLAGS += -std=c++0x -O2

LIBS += -lpthread -lopencv_core -lopencv_imgproc  -lopencv_highgui -lopencv_features2d \
		-lboost_timer  -lboost_system

SOURCES += main.cpp \
    detectors.cpp \
    edges.cpp

HEADERS += \
	read_frames.hpp \
    detectors.hpp \
    edges.hpp

