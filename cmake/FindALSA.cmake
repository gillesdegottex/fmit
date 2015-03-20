## $Id: FindALSA.cmake,v 1.1 2010/05/15 15:57:16 norwin Exp $

# Check for the presence of ALSA
#
#  HAVE_ALSA        = Set to true, if all components have been found.
#  ALSA_INCLUDES    = Include path for the header files
#  ALSA_LIBRARIES   = Link these

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (ALSA_INCLUDES alsa/asoundlib.h
  PATHS /usr/local/include /usr/include
  )

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (ALSA_LIBRARIES asound
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (ALSA_INCLUDES AND ALSA_LIBRARIES)
    SET (HAVE_ALSA TRUE)
ELSE (ALSA_INCLUDES AND ALSA_LIBRARIES)
    MESSAGE (STATUS "Unable to find ALSA library")
ENDIF (ALSA_INCLUDES AND ALSA_LIBRARIES)

IF (HAVE_ALSA)
    MESSAGE (STATUS "Found ALSA library")
    MESSAGE (STATUS "ALSA_INCLUDES  = ${ALSA_INCLUDES}")
    MESSAGE (STATUS "ALSA_LIBRARIES = ${ALSA_LIBRARIES}")
ELSE (HAVE_ALSA)
  IF (ALSA_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find ALSA!")
  ENDIF (ALSA_FIND_REQUIRED)
ENDIF (HAVE_ALSA)

## ------------------------------------------------------------------------------
## Mark as advanced ...
