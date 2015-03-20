FILE(GLOB fmit_MODULE_SRCS
    DialView.cpp
    GLErrorHistory.cpp
    GLFreqStruct.cpp
    GLFT.cpp
    GLGraph.cpp
    GLSample.cpp
    GLStatistics.cpp
    GLVolumeHistory.cpp
    MicrotonalView.cpp
    View.cpp
)
FILE(GLOB fmit_MODULE_MOC_CLASSES
    DialView.h
    GLErrorHistory.h
    GLFreqStruct.h
    GLFT.h
    GLGraph.h
    GLSample.h
    GLStatistics.h
    GLVolumeHistory.h
    MicrotonalView.h
)

QT4_WRAP_CPP(fmit_MODULE_SRCS_MOC ${fmit_MODULE_MOC_CLASSES})

SET (FILES_TO_TRANSLATE ${FILES_TO_TRANSLATE} ${fmit_MODULE_SRCS} ${fmit_MODULE_MOC_CLASSES} PARENT_SCOPE)

INCLUDE_DIRECTORIES(
    ../../libs
    ${CMAKE_CURRENT_BINARY_DIR}
)

ADD_LIBRARY(fmit_modules STATIC ${fmit_MODULE_SRCS_MOC} ${fmit_MODULE_SRCS})
