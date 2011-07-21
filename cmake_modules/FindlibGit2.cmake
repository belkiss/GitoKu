# - Try to find libgit2
# Once done this will define
#
#  LIBGIT2_FOUND - system has logon
#  LIBGIT2_INCLUDE_DIRS - the logon include directory
#  LIBGIT2_LIBRARIES - Link these to use logon
#  LIBGIT2_DEFINITIONS - Compiler switches required for using logon
#
#  Copyright (c) 2011 Luc Bertrand <grumpy@cacou.net>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


IF (LIBGIT2_LIBRARIES AND LIBGIT2_INCLUDE_DIRS)
  # in cache already
  SET(LIBGIT2_FOUND TRUE)
ELSE (LIBGIT2_LIBRARIES AND LIBGIT2_INCLUDE_DIRS)


FIND_PATH(LIBGIT2_INCLUDE_DIRS NAMES git2.h)

FIND_LIBRARY(LIBGIT2_LIBRARIES NAMES git2 )

SET(LIBGIT2_DEFINITIONS "")

IF (LIBGIT2_LIBRARIES)
  SET (LIBGIT2_FOUND TRUE)
ELSE (LIBGIT2_LIBRARIES)
  SET (LIBGIT2_FOUND FALSE)
ENDIF (LIBGIT2_LIBRARIES)

ENDIF (LIBGIT2_LIBRARIES AND LIBGIT2_INCLUDE_DIRS)

