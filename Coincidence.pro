#-------------------------------------------------
#
# Project created by QtCreator 2015-07-22T10:21:42
#
#-------------------------------------------------

QT       += core
QT       += network
QT       += sql
QT       -= gui

TARGET = Coincidence
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    localsocket.cpp \
    jsonparser.cpp \
    mysqlconnector.cpp \
    systemerror.cpp \
    informationerror.cpp

HEADERS += \
    localsocket.h \
    jsonparser.h \
    mysqlconnector.h \
    common.h \
    systemerror.h \
    informationerror.h
