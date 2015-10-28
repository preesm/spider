#-------------------------------------------------
#
# Project created by QtCreator 2015-10-19T12:34:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = demo_stereo
TEMPLATE = app


SOURCES += main.cpp\
		demostereo.cpp\
		actors.cpp\
		fct_stereo.cpp\
		pi_stereo.cpp\
	aggregation.cpp \
	census.cpp \
	costconstruction.cpp \
	weigth_calc.cpp

HEADERS  += demostereo.h actors.h stereomatch.h stereo.h

FORMS    += demostereo.ui

CONFIG(debug, debug|release){
LIBS += -L$$PWD/../../master/projects/linux/Debug/ -lSpider
 message("debug mode")
 DEPENDPATH += $$PWD/../../master/projects/linux/Debug/
}else{
LIBS += -L$$PWD/../../master/projects/linux/Release/ -lSpider
 message("release mode")
DEPENDPATH += $$PWD/../../master/projects/linux/Release/
}

INCLUDEPATH += ../../master/spider
INCLUDEPATH += ../../master/common
INCLUDEPATH += ../../master/lrt
INCLUDEPATH += ../../master/platform_linux

