#-------------------------------------------------
#
# Project created by QtCreator 2016-08-30T09:59:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = frp
TEMPLATE = app

# подключаем общие файлы
include(../common.pri)


SOURCES += main.cpp\
    async/abstracttask.cpp \
    async/backgroundworker.cpp \
    async/taskwrap.cpp \
    io/opentask.cpp \
    task/binarizationtask.cpp \
    task/resizetask.cpp \
    ui/imageviewer.cpp \
    ui/mainwindow.cpp \
    util/mattoqimage.cpp

HEADERS  += \
    async/abstracttask.h \
    async/backgroundworker.h \
    async/taskwrap.h \
    io/opentask.h \
    task/binarizationtask.h \
    task/resizetask.h \
    ui/imageviewer.h \
    ui/mainwindow.h \
    util/mattoqimage.h

FORMS += \
    ui/mainwindow.ui

RESOURCES += \
    icons.qrc
