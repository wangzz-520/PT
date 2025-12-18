#-------------------------------------------------
#
# Project created by QtCreator 2023-01-03T09:16:37
#
#-------------------------------------------------

QT       -= core gui

TARGET = spdlog
TEMPLATE = lib
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \
            SPDLOG_COMPILED_LIB

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32:DESTDIR += ../../3rd/x86/spdlog/lib

INCLUDEPATH += include

SOURCES += \
    src/async.cpp \
    src/cfg.cpp \
    src/color_sinks.cpp \
    src/file_sinks.cpp \
    src/fmt.cpp \
    src/spdlog.cpp \
    src/stdout_sinks.cpp

HEADERS += \
    include/spdlog/async.h \
    include/spdlog/async_logger.h \
    include/spdlog/async_logger-inl.h \
    include/spdlog/common.h \
    include/spdlog/common-inl.h \
    include/spdlog/formatter.h \
    include/spdlog/fwd.h \
    include/spdlog/logger.h \
    include/spdlog/logger-inl.h \
    include/spdlog/pattern_formatter.h \
    include/spdlog/pattern_formatter-inl.h \
    include/spdlog/spdlog.h \
    include/spdlog/spdlog-inl.h \
    include/spdlog/stopwatch.h \
    include/spdlog/tweakme.h \
    include/spdlog/version.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
