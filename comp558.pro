TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += /usr/include/opencv

QMAKE_CXXFLAGS += -std=c++11

LIBS += /usr/lib/libopencv_core.so \
		/usr/lib/libopencv_imgproc.so \
		/usr/lib/libopencv_highgui.so \

SOURCES += main.cpp

