QT += core
QT += network
QT -= gui
QT += sql
QT += widgets
QT += charts


TARGET = TcpServer
CONFIG   += console
CONFIG   -= app_bundle

DISTFILES += esp.txt


TEMPLATE = app


SOURCES += main.cpp \
    mytcpserver.cpp \
    packet.cpp \
    person.cpp \
    point.cpp \
    functions.cpp \
    window.cpp \
    esp.cpp

HEADERS += \
    mytcpserver.h \
    packet.h \
    person.h \
    point.h \
    functions.h \
    window.h \
    esp.h

    mytcpserver.cpp

HEADERS += \
    mytcpserver.h

