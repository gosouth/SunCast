#-------------------------------------------------
#
# Project created by QtCreator 2016-10-24T20:21:44
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = SunCast

target.files = SunCast
target.path = /home/Desktop
INSTALLS+=target

TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    qchart.cpp \
    qtasktimer.cpp \
    qmpptdata.cpp \
    qwebdata.cpp \
    qadcdata.cpp

HEADERS  += mainwindow.h \
    qchart.h \
    qtasktimer.h \
    qmpptdata.h \
    qwebdata.h \
    qadcdata.h

FORMS    += mainwindow.ui

RESOURCES += \
    logo.qrc
