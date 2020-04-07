include(../../plugins.pri)
include($$PWD/common/common.pri)

TARGET = $$PLUGINS_PREFIX/Input/tta

HEADERS += decoderttafactory.h \
           ttametadatamodel.h \
           decoder_tta.h \
           ttahelper.h \
           filter.h \
           ttadec.h
    
SOURCES += decoderttafactory.cpp \
           ttametadatamodel.cpp \
           decoder_tta.cpp \
           ttahelper.cpp \
           ttadec.c
           
unix {
    target.path = $$PLUGIN_DIR/Input
    INSTALLS += target
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libtta.so
}
