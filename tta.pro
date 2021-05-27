include(../../plugins.pri)
include($$PWD/common/common.pri)

TARGET = $$PLUGINS_PREFIX/Input/tta

HEADERS += decoderttafactory.h \
           ttametadatamodel.h \
           decoder_tta.h \
           ttahelper.h
    
SOURCES += decoderttafactory.cpp \
           ttametadatamodel.cpp \
           decoder_tta.cpp \
           ttahelper.cpp \
           libtta/ttadec.c
           
INCLUDEPATH += $$PWD/libtta

unix {
    target.path = $$PLUGIN_DIR/Input
    INSTALLS += target
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libtta.so
}
