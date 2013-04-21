#-------------------------------------------------
#
# Project created by QtCreator 2013-04-15T21:37:00
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Pngyu
TEMPLATE = app


SOURCES += \
    main.cpp\
    pngyumainwindow.cpp \
    pngyu_execute_compress.cpp

HEADERS  += \
    pngyumainwindow.h \
    pngyu_util.h \
    pngyu_option.h \
    pngyu_difines.h \
    pngyu_execute_compress.h

FORMS += \
    pngyumainwindow.ui

RESOURCES += \
    resource/resource.qrc

