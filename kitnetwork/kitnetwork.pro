QT -= gui
QT += network

TEMPLATE = lib
DEFINES += KITNETWORK_LIBRARY

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# 名称规则
CONFIG(debug, debug|release) {
     unix: TARGET = $$join(TARGET,,,_debug)
     else: TARGET = $$join(TARGET,,,d)
}
# 动态库生成路径
CONFIG(release, debug|release) {
    DESTDIR = $$PWD/../lib_release
}else {
    DESTDIR = $$PWD/../lib_debug
}

include($$PWD/../kitutil/kitutil.pri)

SOURCES += \
    netframe/netdatareslice.cpp \
    netframe/netframe.cpp \
    netframe/netframedatareslice.cpp \
    netsocket/tcpclient.cpp \
    netsocket/tcpserver.cpp \
    netsocket/udpserver.cpp \
    networkclient.cpp \
    networkserver.cpp

HEADERS += \
    kitnetwork_global.h \
    netframe/netdatareslice.h \
    netframe/netframe.h \
    netframe/netframedatareslice.h \
    netsocket/tcpclient.h \
    netsocket/tcpserver.h \
    netsocket/udpserver.h \
    netstruct/netparam.h \
    networkclient.h \
    networkserver.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
