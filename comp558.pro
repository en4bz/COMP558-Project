TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += /usr/include/opencv \
				/usr/include/boost/

QMAKE_CXXFLAGS += -std=c++11

LIBS += /usr/lib/libopencv_core.so \
		/usr/lib/libopencv_imgproc.so \
		/usr/lib/libopencv_highgui.so \
		/usr/lib/libopencv_features2d.so \
		/usr/lib/x86_64-linux-gnu/libboost_timer.so \
		/usr/lib/x86_64-linux-gnu/libboost_system.so.1.53.0

SOURCES += main.cpp \
    detectors.cpp \
    edges.cpp

HEADERS += \
    detectors.hpp \
    edges.hpp

