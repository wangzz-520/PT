#-------------------------------------------------
#
# Project created by QtCreator 2025-11-26T09:58:06
#
#-------------------------------------------------

QT       += core gui serialport network serialbus printsupport

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
msvc {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}


SOURCES += \
    DataCollection/DataCollectionA1088.cpp \
    DataCollection/DataCollectionDS80.cpp \
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
    algorithm/StabilizationAlgorithmModule.cpp \
    qtmodbusrtu.cpp \
    grayimageglwidget.cpp \
    customplot.cpp \
    3rd/qcustomplot/qcustomplot.cpp

HEADERS += \
    DataCollection/DataCollectionA1088.h \
    DataCollection/DataCollectionDS80.h \
    DataCollection/DataCollectionFactory.h \
    DataCollection/DataCollectionInterface.h \
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
    protocoldefine.h \
    qtmodbusrtu.h \
    grayimageglwidget.h \
    customplot.h \
    3rd/qcustomplot/qcustomplot.h

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
               $$PWD/3rd/Eigen3/include/eigen3 \
               $$PWD/3rd/qcustomplot \
               $$PWD/3rd/xvsdk/include \
               $$PWD/3rd/xvsdk/include/xvsdk \
               $$PWD/3rd/xvsdk/include2 \
               $$PWD/3rd/opencv/include

win32:LIBPATH += $$PWD/3rd/spdlog/lib \
                 $$PWD/3rd/xvsdk/lib \
                 $$PWD/3rd/opencv/lib

LIBS += -lspdlog -lxvsdk -lopencv_world480d
DESTDIR += ../Out
