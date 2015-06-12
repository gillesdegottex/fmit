FILE(GLOB fmit_SRCS
    AutoQSettings.cpp
    CaptureThread.cpp
    CaptureThreadImplOSS.cpp
    CaptureThreadImplPortAudio.cpp
    CaptureThreadImplJACK.cpp
    CaptureThreadImplALSA.cpp
    CustomInstrumentTunerForm.cpp
    DummyMonoQuantizer.cpp
    LatencyMonoQuantizer.cpp
    MonoQuantizer.cpp
    main.cpp
)
FILE(GLOB fmit_UI_SRC
    ../ui/ConfigForm.ui
    ../ui/InstrumentTunerForm.ui
)
FILE(GLOB fmit_MOC_CLASSES
    CaptureThread.h
    CustomInstrumentTunerForm.h
    MonoQuantizer.h
)
SET(fmit_QRC_SRC
    fmit.qrc
)

QT4_WRAP_UI(fmit_UI_H ${fmit_UI_SRC})

QT4_WRAP_CPP(fmit_SRCS_MOC ${fmit_MOC_CLASSES})

QT4_ADD_RESOURCES(fmit_RESOURCES ${fmit_QRC_SRC})

SET (FILES_TO_TRANSLATE ${FILES_TO_TRANSLATE} ${fmit_SRCS} ${fmit_UI_SRC} ${fmit_MOC_CLASSES} PARENT_SCOPE)

INCLUDE_DIRECTORIES(
    ../libs
    ${CMAKE_CURRENT_BINARY_DIR}
    ${OSS_INCLUDE_DIR}
    ${ALSA_INCLUDES}
    ${PORTAUDIO_INCLUDE_DIRS}
    ${JACK_INCLUDE_DIRS}
)

IF (UPDATE_TRANSLATIONS)
  QT4_CREATE_TRANSLATION(QM_FILES ${FILES_TO_TRANSLATE} ${TRANSLATIONS_FILES})
ELSE (UPDATE_TRANSLATIONS)
  QT4_ADD_TRANSLATION(QM_FILES ${TRANSLATIONS_FILES})
ENDIF (UPDATE_TRANSLATIONS) 
ADD_CUSTOM_TARGET (translations_target DEPENDS ${QM_FILES})

ADD_EXECUTABLE(fmit ${fmit_UI_H} ${fmit_SRCS_MOC} ${fmit_SRCS} ${fmit_RESOURCES} ${QM_FILES})

TARGET_LINK_LIBRARIES(fmit
    CppAddons Music
    fmit_modules
    ${QT_LIBRARIES} ${GLUT_glut_LIBRARY} ${OPENGL_gl_LIBRARY} ${FFTW3_LIBRARIES}
    ${ALSA_LIBRARIES}
    ${PORTAUDIO_LIBRARIES}
    ${JACK_LIBRARIES}
)

INSTALL(TARGETS fmit DESTINATION bin)                # Install FMIT binary 

INSTALL (FILES ${QM_FILES} DESTINATION share/fmit/tr)# Install translation files
