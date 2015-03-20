##------------------------------------------------------------------------------
## $Id: FindFFTW3.cmake,v 1.3 2011/01/04 23:31:32 norwin Exp $
##------------------------------------------------------------------------------

# - Check for the presence of FFTW3
#
# The following variables are set when FFTW3 is found:
#  HAVE_FFTW3       = Set to true, if all components of FFTW3
#                          have been found.
#  FFTW3_INCLUDES = Include path for the header files of FFTW3
#  FFTW3_LIBRARIES     = Link these to use FFTW3

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (FFTW3_INCLUDES fftw3.h
  PATHS /usr/local/include /usr/include /sw/include
  )

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (FFTW3_LIBRARIES fftw3 fftw
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (FFTW3_INCLUDES AND FFTW3_LIBRARIES)
  SET (HAVE_FFTW3 TRUE)
ELSE (FFTW3_INCLUDES AND FFTW3_LIBRARIES)
  IF (NOT FFTW3_FIND_QUIETLY)
    IF (NOT FFTW3_INCLUDES)
      MESSAGE (STATUS "Unable to find FFTW3 header files!")
    ENDIF (NOT FFTW3_INCLUDES)
    IF (NOT FFTW3_LIBRARIES)
      MESSAGE (STATUS "Unable to find FFTW3 library files!")
    ENDIF (NOT FFTW3_LIBRARIES)
  ENDIF (NOT FFTW3_FIND_QUIETLY)
ENDIF (FFTW3_INCLUDES AND FFTW3_LIBRARIES)

IF (HAVE_FFTW3)
  IF (NOT FFTW3_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for FFTW3")
    MESSAGE (STATUS "FFTW3_INCLUDES  = ${FFTW3_INCLUDES}")
    MESSAGE (STATUS "FFTW3_LIBRARIES = ${FFTW3_LIBRARIES}")
  ENDIF (NOT FFTW3_FIND_QUIETLY)
ELSE (HAVE_FFTW3)
  IF (FFTW3_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Can not find FFTW3 library !")
  ENDIF (FFTW3_FIND_REQUIRED)
ENDIF (HAVE_FFTW3)

## ------------------------------------------------------------------------------
## Mark as advanced ...
