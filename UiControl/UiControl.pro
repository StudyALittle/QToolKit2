QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


# 名称规则
CONFIG(debug, debug|release) {
     unix: TARGET = $$join(TARGET,,,_debug)
     else: TARGET = $$join(TARGET,,,d)
}
# 动态库生成路径
CONFIG(release, debug|release) {
    DESTDIR = $$PWD/../../lib_release
}else {
    DESTDIR = $$PWD/../../lib_debug
}

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dialog.cpp \
    framelesswindow/lesstitlebar.cpp \
    framelesswindow/lesstitlebarbtns.cpp \
    main.cpp \
    widget.cpp

HEADERS += \
    dialog.h \
    framelesswindow/frameless.h \
    framelesswindow/lesstitlebar.h \
    framelesswindow/lesstitlebarbtns.h \
    widget.h

FORMS += \
    dialog.ui \
    framelesswindow/lesstitlebar.ui \
    framelesswindow/lesstitlebarbtns.ui \
    widget.ui

win32 {
SOURCES += \
    framelesswindow/win/winlesswindow.cpp
    #framelesswindow/win/dialoglesswindow.cpp \
    #framelesswindow/win/widgetlesswindow.cpp

HEADERS += \
    framelesswindow/win/winlesswindow.h
    #framelesswindow/win/dialoglesswindow.h \
    #framelesswindow/win/widgetlesswindow.h
}
unix {
SOURCES += \
    framelesswindow/unix/framelesshelper.cpp

HEADERS += \
    framelesswindow/unix/framelesshelper.h
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=
