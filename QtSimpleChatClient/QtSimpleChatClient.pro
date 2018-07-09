include(../QtSimpleChat.pri)

QT += core network gui widgets

TARGET = QtSimpleChatClient
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += $$PWD/../QtSimpleChat
LIBS += -L$$DESTDIR -lQtSimpleChat

HEADERS += \
    chatwindow.h

SOURCES += \
    clientmain.cpp \
    chatwindow.cpp

FORMS += \
    chatwindow.ui
