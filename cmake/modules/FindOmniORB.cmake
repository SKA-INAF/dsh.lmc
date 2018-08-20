#INCLUDE(FindPkgConfig)

## Find omniORB4
PKG_CHECK_MODULES(PC_OMNIORB4 omniORB4)

FIND_PATH(
	OMNIORB4_INCLUDE_DIR
  NAMES omniORB4/CORBA.h
  HINTS ${PC_OMNIORB4_INCLUDE_DIRS} $ENV{OMNI_ROOT}/include
  PATHS ${PC_OMNIORB4_INCLUDE_DIRS} $ENV{OMNI_ROOT}/include /usr/local/include /usr/include
)

FIND_LIBRARY(
	OMNIORB4_LIBRARIES
  NAMES omniORB4
  HINTS ${PC_OMNIORB4_LIBRARY_DIRS} $ENV{OMNI_ROOT}/lib
  PATHS ${PC_OMNIORB4_LIBRARY_DIRS} $ENV{OMNI_ROOT}/lib /usr/local/lib /usr/local/lib64 /usr/lib /usr/lib64
)

## Find omniCOS4
PKG_CHECK_MODULES(PC_OMNICOS4 omniCOS4)

FIND_PATH(
	OMNICOS4_INCLUDE_DIR
  NAMES COS/COS_sysdep.h
  HINTS ${PC_OMNICOS4_INCLUDE_DIRS} $ENV{OMNI_ROOT}/include
  PATHS ${PC_OMNICOS4_INCLUDE_DIRS} $ENV{OMNI_ROOT}/include /usr/local/include /usr/include
)

FIND_LIBRARY(
	OMNICOS4_LIBRARIES
  NAMES COS4
  HINTS ${PC_OMNICOS4_LIBRARY_DIRS} $ENV{OMNI_ROOT}/lib
  PATHS ${PC_OMNICOS4_LIBRARY_DIRS} $ENV{OMNI_ROOT}/lib /usr/local/lib /usr/local/lib64 /usr/lib /usr/lib64
)



## Find omniCOS4
PKG_CHECK_MODULES(PC_OMNIDYNAMIC4 omniDynamic4)

FIND_LIBRARY(
	OMNIDYNAMIC4_LIBRARIES
  NAMES omniDynamic4
  HINTS ${PC_OMNIDYNAMIC4_LIBRARY_DIRS} $ENV{OMNI_ROOT}/lib
  PATHS ${PC_OMNIDYNAMIC4_LIBRARY_DIRS} $ENV{OMNI_ROOT}/lib /usr/local/lib /usr/local/lib64 /usr/lib /usr/lib64
)

## Find omnithread3
PKG_CHECK_MODULES(PC_OMNITHREAD3 omnithread3)

FIND_PATH(
	OMNITHREAD3_INCLUDE_DIR
  NAMES omnithread/posix.h
  HINTS ${PC_OMNITHREAD3_INCLUDE_DIRS} $ENV{OMNI_ROOT}/include
  PATHS ${PC_OMNITHREAD3_INCLUDE_DIRS} $ENV{OMNI_ROOT}/include /usr/local/include /usr/include
)

FIND_LIBRARY(
	OMNITHREAD3_LIBRARIES
  NAMES omnithread
  HINTS ${PC_OMNITHREAD3_LIBRARY_DIRS} $ENV{OMNI_ROOT}/lib
  PATHS ${PC_OMNITHREAD3_LIBRARY_DIRS} $ENV{OMNI_ROOT}/lib /usr/local/lib /usr/local/lib64 /usr/lib /usr/lib64
)

## Append all libs & headers
list(APPEND OMNIORB_LIBRARIES ${OMNIORB4_LIBRARIES} ${OMNICOS4_LIBRARIES} ${OMNIDYNAMIC4_LIBRARIES} ${OMNITHREAD3_LIBRARIES})
list(APPEND OMNIORB_INCLUDE_DIRS ${OMNIORB4_INCLUDE_DIR} ${OMNICOS4_INCLUDE_DIR} ${OMNITHREAD3_INCLUDE_DIR})


INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OMNIORB DEFAULT_MSG OMNIORB_LIBRARIES OMNIORB_INCLUDE_DIRS)
MARK_AS_ADVANCED(OMNIORB_LIBRARIES OMNIORB_INCLUDE_DIRS)


#if(NOT DEFINED ENV{OMNI_ROOT})
#	MESSAGE(SEND_ERROR "OMNI_ROOT variable not defined!")
#endif()

#SET (OMNI_ROOT $ENV{OMNI_ROOT})
#MESSAGE(STATUS "OMNI_ROOT: ${OMNI_ROOT}")

#FIND_PATH (OMNIORB_INCLUDE_DIR
#	NAMES omniconfig.h
#  HINTS
#  ${OMNI_ROOT}/include
#)

#FIND_LIBRARY (OMNIORB_LIB1 NAMES omniORB4 HINTS ${OMNI_ROOT}/lib)
#FIND_LIBRARY (OMNIORB_LIB2 NAMES COS4 HINTS ${OMNI_ROOT}/lib)
#FIND_LIBRARY (OMNIORB_LIB3 NAMES omniDynamic4 HINTS ${OMNI_ROOT}/lib)
#FIND_LIBRARY (OMNIORB_LIB4 NAMES omnithread HINTS ${OMNI_ROOT}/lib)
#list(APPEND OMNIORB_LIBRARIES ${OMNIORB_LIB1} ${OMNIORB_LIB2} ${OMNIORB_LIB3} ${OMNIORB_LIB4})

#MARK_AS_ADVANCED (OMNIORB_INCLUDE_DIR OMNIORB_LIBRARIES)
#MESSAGE(STATUS "OMNIORB_INCLUDE_DIR: ${OMNIORB_INCLUDE_DIR}")
#MESSAGE(STATUS "OMNIORB_LIBRARIES: ${OMNIORB_LIBRARIES}")

