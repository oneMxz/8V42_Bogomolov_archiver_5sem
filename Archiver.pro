QT = core

CONFIG += c++17 cmdline

SOURCES += \
        archive.cpp \
        archivemanager.cpp \
        deflatealgorithm.cpp \
        huffmanalgorithm.cpp \
        lz77algorithm.cpp \
        main.cpp \
        zipalgorithm.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    archive.h \
    archivemanager.h \
    compressionalgorithm.h \
    deflatealgorithm.h \
    huffmanalgorithm.h \
    lz77algorithm.h \
    structs.h \
    zipalgorithm.h
