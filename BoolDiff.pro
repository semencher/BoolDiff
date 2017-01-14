QT += core
QT -= gui

CONFIG += c++11

TARGET = BoolDiff
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    functions.cpp

HEADERS += \
    functions.h \
    global.h
