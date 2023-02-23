# 头文件路径
INCLUDEPATH += $$PWD

LIB_NAME = kitnetwork

CONFIG(debug, debug|release) {
     unix: LIB_NAME = $$join(LIB_NAME,,,_debug)
     else: LIB_NAME = $$join(LIB_NAME,,,d)
}

CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../../lib_debug -l$$LIB_NAME
} else {
    LIBS += -L$$PWD/../../lib_release -l$$LIB_NAME
}
