#-------------------------------------------------
#
# Project created by QtCreator 2020-06-18T11:13:45
#
#-------------------------------------------------

QT       += core gui
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = yingongshe_test
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    global.cpp \
    pyloncamera.cpp \
    thread_imagehandle.cpp \
    thread_imageshow.cpp

HEADERS += \
        mainwindow.h \
    global.h \
    pyloncamera.h \
    thread_imagehandle.h \
    thread_imageshow.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#pylon include
INCLUDEPATH += $$PWD/pylon5/include
DEPENDPATH += $$PWD/pylon5/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/pylon5/lib/Win32/ -lGCBase_MD_VC120_v3_0_Basler_pylon_v5_0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/pylon5/lib/Win32/ -lGCBase_MD_VC120_v3_0_Basler_pylon_v5_0


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/pylon5/lib/Win32/ -lGenApi_MD_VC120_v3_0_Basler_pylon_v5_0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/pylon5/lib/Win32/ -lGenApi_MD_VC120_v3_0_Basler_pylon_v5_0
else:unix: LIBS += -L$$PWD/pylon5/lib/Win32/ -lGenApi_MD_VC120_v3_0_Basler_pylon_v5_0


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/pylon5/lib/Win32/ -lPylonBase_MD_VC120_v5_0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/pylon5/lib/Win32/ -lPylonBase_MD_VC120_v5_0
else:unix: LIBS += -L$$PWD/pylon5/lib/Win32/ -lPylonBase_MD_VC120_v5_0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/pylon5/lib/Win32/ -lPylonC_MD_VC120
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/pylon5/lib/Win32/ -lPylonC_MD_VC120
else:unix: LIBS += -L$$PWD/pylon5/lib/Win32/ -lPylonC_MD_VC120

INCLUDEPATH += $$PWD/pylon5/lib/Win32
DEPENDPATH += $$PWD/pylon5/lib/Win32

#opencv
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/opencv/lib/ -lopencv_world320d
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/opencv/lib/ -lopencv_world320d
else:unix: LIBS += -L$$PWD/opencv/lib/ -lopencv_world320

INCLUDEPATH += $$PWD/opencv/include
DEPENDPATH += $$PWD/opencv/include



