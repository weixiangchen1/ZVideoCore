MODULE = ZCodec

SOURCES += \
    $$PWD/ZCodec.cpp \
    $$PWD/ZEncode.cpp \
    $$PWD/ZDecode.cpp \
    $$PWD/ZFormat.cpp \
    $$PWD/ZThread.cpp \
    $$PWD/ZMux.cpp \
    $$PWD/ZDemux.cpp \
    $$PWD/ZDecodeTask.cpp \
    $$PWD/ZMuxTask.cpp \
    $$PWD/ZDemuxTask.cpp \

HEADERS += \
    $$PWD/ZCodec.h \
    $$PWD/ZEncode.h \
    $$PWD/ZDecode.h \
    $$PWD/ZFormat.h \
    $$PWD/ZThread.h \
    $$PWD/ZMux.h \
    $$PWD/ZDemux.h \
    $$PWD/ZDecodeTask.h \
    $$PWD/ZMuxTask.h \
    $$PWD/ZDemuxTask.h \
    $$PWD/Utils.h \
    $$PWD/ZLog.h \
    $$PWD/ZAVParam.h \

INCLUDEPATH += $$PWD

win32 {
    INCLUDEPATH += $$PWD/../../include
    LIBS += -L$$PWD/../../lib/x86 \
            -lavcodec \
            -lavformat \
            -lavutil \
            -lswscale \
            -lswresample \
            -lSDL2
}

QMAKE_CXXFLAGS += -std=c++11
