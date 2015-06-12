
project(fmit) # the name of your project

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0005 OLD)
endif(COMMAND cmake_policy)

SET (PACKAGE_NAME "fmit")
SET (VERSION_MAJOR "1")
SET (VERSION_MINOR "0")
SET (VERSION_PATCH "0")
SET (VERSION_SUFFIX "")
SET (PACKAGE_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}${VERSION_SUFFIX})
MESSAGE(STATUS "${PACKAGE_NAME} ${PACKAGE_VERSION} (install in ${CMAKE_INSTALL_PREFIX})")
SET(PREFIX ${CMAKE_INSTALL_PREFIX})

# CHECK_INCLUDE_FILES (malloc.h HAVE_MALLOC_H)
# CHECK_INCLUDE_FILES ("sys/param.h;sys/mount.h" HAVE_SYS_MOUNT_H)
# CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# CHECK_INCLUDE_FILE("unistd.h" HAVE_UNISTD_H)
# IF(HAVE_UNISTD_H)
#   ADD_DEFINITIONS(-DHAVE_UNISTD_H)
# ENDIF(HAVE_UNISTD_H)

ADD_DEFINITIONS(-DPREFIX=\\\"${PREFIX}\\\" -DPACKAGE_NAME=\\\"${PACKAGE_NAME}\\\" -DPACKAGE_VERSION=\\\"${PACKAGE_VERSION}\\\")

#FIND DEPENDENCIES
FIND_PACKAGE(Qt4 REQUIRED)
IF (QT4_FOUND)
    MESSAGE(STATUS "Qt4 library found")
ELSE ()
    MESSAGE(FATAL_ERROR "No Qt4 library found !")
ENDIF ()

SET (FILES_TO_TRANSLATE )

SET(QT_USE_QTOPENGL TRUE)
#SET(QT_USE_QT3SUPPORT TRUE)

INCLUDE(${QT_USE_FILE})

FIND_PACKAGE(GLUT REQUIRED)
IF (GLUT_FOUND)
    MESSAGE(STATUS "GLUT library found")
ELSE ()
    MESSAGE(FATAL_ERROR "GLUT library NOT found !")
ENDIF ()

FIND_PACKAGE(OpenGL REQUIRED)
IF (OPENGL_FOUND)
    MESSAGE(STATUS "OpenGL library found")
ELSE ()
    MESSAGE(FATAL_ERROR "OpenGL library NOT found !")
ENDIF ()

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
FIND_PACKAGE(FFTW3 REQUIRED)

# Options ----------------------------------------------------------------------

OPTION(SOUNDSYSTEM_USE_OSS "Build FMIT with OSS support" OFF)
IF (SOUNDSYSTEM_USE_OSS)
    MESSAGE("OSS sound system requested")
    FIND_PACKAGE(OSS)
    IF (OSS_FOUND)
        ADD_DEFINITIONS(-DCAPTURE_OSS)
        MESSAGE(STATUS "OSS library ok")
    ELSE ()
        MESSAGE(FATAL_ERROR "Unable to find OSS library")
    ENDIF ()
ENDIF ()

OPTION(SOUNDSYSTEM_USE_ALSA "Build FMIT with ALSA support" ON)
IF (SOUNDSYSTEM_USE_ALSA)
    MESSAGE("ALSA sound system requested")
    FIND_PACKAGE(ALSA)
    IF (HAVE_ALSA)
        ADD_DEFINITIONS(-DCAPTURE_ALSA)
        MESSAGE(STATUS "ALSA library ok")
    ELSE ()
        MESSAGE(FATAL_ERROR "Unable to find ALSA library")
    ENDIF ()
ENDIF ()

OPTION(SOUNDSYSTEM_USE_PORTAUDIO "Build FMIT with Portaudio support" OFF)
IF (SOUNDSYSTEM_USE_PORTAUDIO)
    MESSAGE("Portaudio sound system requested")
    FIND_PACKAGE(Portaudio)
    IF (PORTAUDIO_FOUND)
        ADD_DEFINITIONS(-DCAPTURE_PORTAUDIO)
        MESSAGE(STATUS "Portaudio library ok")
    ELSE ()
        MESSAGE(FATAL_ERROR "Unable to find Portaudio library")
    ENDIF ()
ENDIF ()

OPTION(SOUNDSYSTEM_USE_JACK "Build FMIT with JACK support" ON)
IF (SOUNDSYSTEM_USE_JACK)
    MESSAGE("JACK sound system requested")
    FIND_PACKAGE(Jack)
    IF (JACK_FOUND)
        ADD_DEFINITIONS(-DCAPTURE_JACK)
        MESSAGE(STATUS "JACK library ok")
    ELSE ()
        MESSAGE(FATAL_ERROR "Unable to find JACK library")
    ENDIF ()
ENDIF ()

# Qt4 Translations
FILE (GLOB TRANSLATIONS_FILES tr/*.ts)
OPTION(UPDATE_TRANSLATIONS "Update source translation tr/*.ts files (WARNING: make clean will delete the source .ts files! Danger!)")

# Building ---------------------------------------------------------------------

# Tell cmake to process CMakeLists.txt in that subdirectory
ADD_SUBDIRECTORY(libs/Music)
ADD_SUBDIRECTORY(libs/CppAddons)
ADD_SUBDIRECTORY(src/modules)
ADD_SUBDIRECTORY(src)

# Installation -----------------------------------------------------------------

# Desktop files
INSTALL(FILES
  distrib/fmit.desktop
  DESTINATION share/applications
)

# AppData file
INSTALL(FILES
  distrib/fmit.appdata.xml
  DESTINATION share/appdata
)

# Icon files
INSTALL(FILES
    ui/images/fmit.svg
    DESTINATION share/icons/hicolor/scalable/apps
)

# Packaging --------------------------------------------------------------------
# build a CPack driven installer package
# cpack --config CPackConfig.cmake
# cpack --config CPackSourceConfig.cmake
INCLUDE(InstallRequiredSystemLibraries)
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING_GPL.txt")
SET(CPACK_GENERATOR "TGZ;TBZ2")
SET(CPACK_PACKAGE_VENDOR "Gilles Degottex")
SET(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
SET(CPACK_SOURCE_GENERATOR "TGZ;TBZ2")
SET(CPACK_SOURCE_IGNORE_FILES
    CVS
    .deps ".*\\\\.ui~"
    CMakeFiles cmake_install.cmake CMakeCache.txt
    Makefile
    build dwl remez old test
    test_build.sh
)
INCLUDE(CPack)
