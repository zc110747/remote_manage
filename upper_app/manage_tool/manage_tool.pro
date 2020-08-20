QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

include         ($$PWD/qextserialport/qextserialport.pri)

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    appthread.cpp \
    commandinfo.cpp \
    configfile.cpp \
    imageprocess.cpp \
    main.cpp \
    mainwindow.cpp \
    protocol.cpp \
    tcpclient.cpp \
    uartclient.cpp \
    udpclient.cpp   \
    screenshot/screenshot.cpp

HEADERS += \
    include/appthread.h \
    include/commandinfo.h \
    include/configfile.h \
    include/imageprocess.h \
    include/mainwindow.h \
    include/protocol.h \
    include/tcpclient.h \
    include/typedef.h \
    include/uartclient.h \
    include/udpclient.h \
    screenshot/screenshot.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += $$PWD\..\..\..\opencv_install\include
INCLUDEPATH += $$PWD\include

LIBS += $$PWD\..\..\..\opencv_install\x64\mingw\lib\libopencv_*.a

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    icon.rc

RC_FILE = icon.rc

RESOURCES += main.qrc \
            other/qss.qrc
