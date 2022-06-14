# 头文件路径
INCLUDEPATH += $$PWD

# 动态库名称
LIB_NAME = kitutil

# 动态库名称规则
CONFIG(debug, debug|release) {
     unix: LIB_NAME = $$join(LIB_NAME,,,_debug)
     else: LIB_NAME = $$join(LIB_NAME,,,d)
}

# 动态库路径
CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../lib_debug -l$$LIB_NAME
}else {
    LIBS += -L$$PWD/../lib_release -l$$LIB_NAME
}
