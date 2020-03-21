#-------------------------------------------------
#
# Project created by QtCreator 2019-07-15T17:42:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LCSF_Generator
TEMPLATE = app
VERSION = 1.1.0.0

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        src/attribute.cpp \
        src/codeextractor.cpp \
        src/codegenerator.cpp \
        src/command.cpp \
        src/deschandler.cpp \
        src/main.cpp \
        src/mainwindow.cpp

HEADERS += \
        src/attribute.h \
        src/codeextractor.h \
        src/codegenerator.h \
        src/command.h \
        src/deschandler.h \
        src/enumtype.h \
        src/mainwindow.h

FORMS += \
        src/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
