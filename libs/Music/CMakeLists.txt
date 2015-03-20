SET(libMusic_SRCS
    Algorithm.cpp
    Convolution.cpp
    CumulativeDiff.cpp
    FreqAnalysis.cpp
    Music.cpp
    TimeAnalysis.cpp
    CumulativeDiffAlgo.cpp
    MultiCumulativeDiffAlgo.cpp
    CombedFT.cpp
    CFFTW3.cpp
    SPWindow.cpp
    Filter.cpp
    Autocorrelation.cpp
    LPC.cpp
)

INCLUDE_DIRECTORIES(
    ../../libs
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(CMAKE_CXX_FLAGS "-Wall")

add_library(Music STATIC ${libMusic_SRCS})
