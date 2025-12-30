
QT += core gui widgets

# Add core5compat if available (helps macdeployqt on macOS)
qtHaveModule(core5compat): QT += core5compat

TARGET = Pngyu
TEMPLATE = app

CONFIG += sdk_no_version_check

INCLUDEPATH += \
    src/ \
    src/preview_window \
    src/preferences_dialog \
    src/imageoptim_integration_question_dialog

SOURCES += \
    src/main.cpp\
    src/pngyumainwindow.cpp \
    src/pngyu_execute_pngquant_command.cpp \
    src/preview_window/basicimageview.cpp \
    src/preview_window/pngyupreviewwindow.cpp \
    src/executecompressthread.cpp \
    src/preferences_dialog/pngyupreferencesdialog.cpp \
    src/imageoptim_integration_question_dialog/pngyuimageoptimintegrationquestiondialog.cpp \
    src/executecompressworkerthread.cpp

HEADERS  += \
    src/pngyumainwindow.h \
    src/pngyu_util.h \
    src/pngyu_pngquant_option.h \
    src/pngyu_defines.h \
    src/pngyu_execute_pngquant_command.h \
    src/preview_window/basicimageview.h \
    src/preview_window/pngyupreviewwindow.h \
    src/executecompressthread.h \
    src/spinnerwidget.h \
    src/preferences_dialog/pngyupreferencesdialog.h \
    src/imageoptim_integration_question_dialog/pngyuimageoptimintegrationquestiondialog.h \
    src/executecompressworkerthread.h \
    src/pngyu_custom_tablewidget_item.h


FORMS += \
    src/pngyumainwindow.ui \
    src/preview_window/pngyupreviewwindow.ui \
    src/preferences_dialog/pngyupreferencesdialog.ui \
    src/imageoptim_integration_question_dialog/pngyuimageoptimintegrationquestiondialog.ui

RESOURCES += \
    resource/resource.qrc

macx: ICON = resource/icon.icns
macx: QMAKE_INFO_PLIST = resource/info.plist
macx: QMAKE_POST_LINK += mkdir -p $$OUT_PWD/Pngyu.app/Contents/Resources &&
macx: QMAKE_POST_LINK += cp -f $$PWD/pngquant_bin/mac/pngquant $$OUT_PWD/Pngyu.app/Contents/Resources/ &&
macx: QMAKE_POST_LINK += chmod +x $$OUT_PWD/Pngyu.app/Contents/Resources/pngquant

win32: RC_FILE = resource/resource.rc
