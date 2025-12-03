#-------------------------------------------------
#
# Project created by QtCreator 2025-11-26T09:58:06
#
#-------------------------------------------------

QT       += core gui serialport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PT
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


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    menuitem.cpp \
    usbbutton.cpp \
    signalstrengthwidget.cpp \
    tipitem.cpp \
    tipwidget.cpp \
    wserialworker.cpp \
    WLog.cpp \
    UdpServer.cpp \
    UdpDataWorkThread.cpp \
    overviewwidget.cpp \
    mapwidget.cpp \
    trackwidget.cpp \
    advancedsettingswidget.cpp \
    algorithm/OneEuroFilter.cpp \
    algorithm/MotionDetectionModule.cpp \
    algorithm/MountingAngleCalibrator.cpp \
    algorithm/StabilizationAlgorithmModule.cpp

HEADERS += \
        mainwindow.h \
    menuitem.h \
    usbbutton.h \
    signalstrengthwidget.h \
    tipitem.h \
    tipwidget.h \
    wserialworker.h \
    global.h \
    WLog.h \
    UdpServer.h \
    UdpDataWorkThread.h \
    overviewwidget.h \
    mapwidget.h \
    trackwidget.h \
    advancedsettingswidget.h \
    algorithm/OneEuroFilter.h \
    algorithm/MotionDetectionModule.h \
    algorithm/MountingAngleCalibrator.h \
    algorithm/StabilizationAlgorithmModule.h \
    protocoldefine.h

FORMS += \
        mainwindow.ui \
    menuitem.ui \
    usbbutton.ui \
    tipitem.ui \
    overviewwidget.ui \
    mapwidget.ui \
    trackwidget.ui \
    advancedsettingswidget.ui

RESOURCES += \
    img.qrc

INCLUDEPATH += $$PWD/3rd/spdlog/include \
               $$PWD/3rd/Eigen3/include/eigen3

win32:LIBPATH += $$PWD/3rd/spdlog/lib \
                 $$PWD/3rd/Eigen3/lib

LIBS += -lspdlog -leigen_blas.dll -leigen_lapack.dll
DESTDIR += ../Out
