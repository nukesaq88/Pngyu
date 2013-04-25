#-------------------------------------------------
#
# Project created by QtCreator 2013-04-15T21:37:00
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Pngyu
TEMPLATE = app

INCLUDEPATH += \
    preview_window

SOURCES += \
    main.cpp\
    pngyumainwindow.cpp \
    pngyu_execute_pngquant_command.cpp \
    preview_window/basicimageview.cpp \
    preview_window/pngyupreviewwindow.cpp \
    preview_window/executecompressthread.cpp

HEADERS  += \
    pngyumainwindow.h \
    pngyu_util.h \
    pngyu_option.h \
    pngyu_difines.h \
    pngyu_execute_pngquant_command.h \
    preview_window/basicimageview.h \
    preview_window/pngyupreviewwindow.h \
    preview_window/executecompressthread.h \
    spinnerwidget.h

FORMS += \
    pngyumainwindow.ui \
    preview_window/pngyupreviewwindow.ui

RESOURCES += \
    resource/resource.qrc

macx: ICON = resource/icon.icns
