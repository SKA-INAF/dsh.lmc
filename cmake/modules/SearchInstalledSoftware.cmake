#================================
#==   Check for PkgConfig       ===
#================================
MESSAGE(STATUS "Looking for PkgConfig")
find_package(PkgConfig REQUIRED)

#================================
#==   GIT VERSIONING          ===
#================================
include(GetGitRevisionDescription)
include(UtilMacros)

#================================
#==   Check for OMNIORB       ===
#================================
MESSAGE(STATUS "Looking for omniORB")
find_package(OmniORB REQUIRED)

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


#================================
#==   Check for ZMQ           ===
#================================
MESSAGE(STATUS "Looking for ZMQ")
find_package(ZMQ REQUIRED)

#if(NOT DEFINED ENV{ZMQ_ROOT})
#	MESSAGE(SEND_ERROR "ZMQ_ROOT variable not defined!")
#endif()

#SET (ZMQ_ROOT $ENV{ZMQ_ROOT})
#MESSAGE(STATUS "ZMQ_ROOT: ${ZMQ_ROOT}")

#FIND_PATH (ZMQ_INCLUDE_DIR
#	NAMES zmq.h
#  HINTS
#  ${ZMQ_ROOT}/include
#)

#FIND_LIBRARY (ZMQ_LIBRARIES NAMES zmq HINTS ${ZMQ_ROOT}/lib)

#MARK_AS_ADVANCED (ZMQ_INCLUDE_DIR ZMQ_LIBRARIES)
#MESSAGE(STATUS "ZMQ_INCLUDE_DIR: ${ZMQ_INCLUDE_DIR}")
#MESSAGE(STATUS "ZMQ_LIBRARIES: ${ZMQ_LIBRARIES}")


#==================================
#==   Check for TANGO           ===
#==================================
MESSAGE(STATUS "Looking for TANGO")
if(NOT DEFINED ENV{TANGO_ROOT})
	MESSAGE(SEND_ERROR "TANGO_ROOT variable not defined!")
endif()

SET (TANGO_ROOT $ENV{TANGO_ROOT})
MESSAGE(STATUS "TANGO_ROOT: ${TANGO_ROOT}")

FIND_PATH (TANGO_INCLUDE_DIR
	NAMES tango.h
  HINTS
  ${TANGO_ROOT}/include/tango
	/usr/include/tango
)

add_definitions(-DAPPENDERS_HAVE_LEVEL_THRESHOLD=1)

FIND_LIBRARY (TANGO_LIB1 NAMES tango HINTS ${TANGO_ROOT}/lib /usr/lib/x86_64-linux-gnu)
FIND_LIBRARY (TANGO_LIB2 NAMES log4tango HINTS ${TANGO_ROOT}/lib /usr/lib/x86_64-linux-gnu)
list(APPEND TANGO_LIBRARIES ${TANGO_LIB1} ${TANGO_LIB2})

MARK_AS_ADVANCED (TANGO_INCLUDE_DIR TANGO_LIBRARIES)
MESSAGE(STATUS "TANGO_INCLUDE_DIR: ${TANGO_INCLUDE_DIR}")
MESSAGE(STATUS "TANGO_LIBRARIES: ${TANGO_LIBRARIES}")

#=================================
#==   Check for YAT4           ===
#=================================
MESSAGE(STATUS "Looking for YAT Library")
if(NOT DEFINED ENV{YAT_ROOT})
	MESSAGE(SEND_ERROR "YAT_ROOT variable not defined!")
endif()

FIND_PATH (YAT_INCLUDE_DIR
	NAMES yat/threading/Task.h
  HINTS
  $ENV{YAT_ROOT}/include
)
FIND_LIBRARY (YAT_LIBRARIES 
	NAMES yat
	HINTS 
	$ENV{YAT_ROOT}/lib
)
MARK_AS_ADVANCED (YAT_INCLUDE_DIR YAT_LIBRARIES)
MESSAGE(STATUS "YAT_INCLUDE_DIR: ${YAT_INCLUDE_DIR}")
MESSAGE(STATUS "YAT_LIBRARIES: ${YAT_LIBRARIES}")
		
#======================================
#==   Check for YAT4TANGO           ===
#======================================
MESSAGE(STATUS "Looking for YAT4TANGO Library")
if(NOT DEFINED ENV{YAT4TANGO_ROOT})
	MESSAGE(SEND_ERROR "YAT4TANGO_ROOT variable not defined!")
endif()

FIND_PATH (YAT4TANGO_INCLUDE_DIR
	NAMES yat4tango/DeviceTask.h
  HINTS
  $ENV{YAT4TANGO_ROOT}/include
)
FIND_LIBRARY (YAT4TANGO_LIBRARIES 
	NAMES yat4tango 
	HINTS 
	$ENV{YAT4TANGO_ROOT}/lib
)
MARK_AS_ADVANCED (YAT4TANGO_INCLUDE_DIR YAT4TANGO_LIBRARIES)
MESSAGE(STATUS "YAT4TANGO_INCLUDE_DIR: ${YAT4TANGO_INCLUDE_DIR}")
MESSAGE(STATUS "YAT4TANGO_LIBRARIES: ${YAT4TANGO_LIBRARIES}")
		
#======================================
#==   Check for TANGO HDB           ===
#======================================
## LIB HDB++
MESSAGE(STATUS "Looking for TANGO HDB Library")
if(NOT DEFINED ENV{HDBPP_DIR})
	MESSAGE(SEND_ERROR "HDBPP_DIR variable not defined!")
endif()

FIND_PATH (HDBPP_INCLUDE_DIR
	NAMES libhdb++/LibHdb++.h
  HINTS
  $ENV{HDBPP_DIR}/include
)
FIND_LIBRARY (HDBPP_LIBRARIES 
	NAMES hdb++ 
	HINTS 
	$ENV{HDBPP_DIR}/lib
)
MARK_AS_ADVANCED (HDBPP_INCLUDE_DIR HDBPP_LIBRARIES)
MESSAGE(STATUS "HDBPP_INCLUDE_DIR: ${HDBPP_INCLUDE_DIR}")
MESSAGE(STATUS "HDBPP_LIBRARIES: ${HDBPP_LIBRARIES}")

#======================================
#==   Check for TANGO HDB-MYSQL     ===
#======================================
## LIB HDB++-MYSQL
MESSAGE(STATUS "Looking for TANGO HDB-MySQL Library")
if(NOT DEFINED ENV{HDBPP_MYSQL_DIR})
	MESSAGE(SEND_ERROR "HDBPP_MYSQL_DIR variable not defined!")
endif()

FIND_LIBRARY (HDBPP_MYSQL_LIBRARIES 
	NAMES hdb++mysql 
	HINTS 
	$ENV{HDBPP_MYSQL_DIR}/lib
)
MARK_AS_ADVANCED (HDBPP_MYSQL_LIBRARIES)
MESSAGE(STATUS "HDBPP_MYSQL_LIBRARIES: ${HDBPP_MYSQL_LIBRARIES}")

#==================================
#== Check for BOOST             ===
#==================================
find_package(Boost REQUIRED COMPONENTS filesystem system regex thread log log_setup)
add_definitions(-DBOOST_LOG_USE_NATIVE_SYSLOG -DBOOST_LOG_DYN_LINK)
message (STATUS "BOOST HEADERS: ${Boost_INCLUDE_DIRS}, LIBS: ${Boost_LIBRARIES}")
#-------------------------



#==================================
#==   Check for JSONCPP         ===
#==================================
MESSAGE(STATUS "Looking for JSONCPP")
#FIND_PACKAGE(Jsoncpp REQUIRED)

if(NOT DEFINED ENV{JSONCPP_ROOT})
	MESSAGE(SEND_ERROR "JSONCPP_ROOT variable not defined!")
endif()

SET (JSONCPP_ROOT $ENV{JSONCPP_ROOT})
MESSAGE(STATUS "JSONCPP_ROOT: ${JSONCPP_ROOT}")

FIND_PATH (JSONCPP_INCLUDE_DIR
	NAMES json/json.h
  HINTS
 ${JSONCPP_ROOT}/include
)

FIND_LIBRARY (JSONCPP_LIBRARIES NAMES jsoncpp HINTS ${JSONCPP_ROOT}/lib)

MARK_AS_ADVANCED (JSONCPP_INCLUDE_DIR JSONCPP_LIBRARIES)
MESSAGE(STATUS "JSONCPP_INCLUDE_DIR: ${JSONCPP_INCLUDE_DIR}")
MESSAGE(STATUS "JSONCPP_LIBRARIES: ${JSONCPP_LIBRARIES}")

#==================================
#==   Check for PUGIXML         ===
#==================================
MESSAGE(STATUS "Looking for PUGIXML")
FIND_PACKAGE(PugiXML REQUIRED)


#if(NOT DEFINED ENV{PUGIXML_ROOT})
#	MESSAGE(SEND_ERROR "PUGIXML_ROOT variable not defined!")
#endif()

#SET (JSONCPP_ROOT $ENV{PUGIXML_ROOT})
#MESSAGE(STATUS "PUGIXML_ROOT: ${PUGIXML_ROOT}")

#FIND_PATH (PUGIXML_INCLUDE_DIR
#	NAMES json/json.h
#  HINTS
#  ${PUGIXML_ROOT}/include
#)

#FIND_LIBRARY (PUGIXML_LIBRARIES NAMES pugixml HINTS ${PUGIXML_ROOT}/lib)

#MARK_AS_ADVANCED (PUGIXML_INCLUDE_DIR PUGIXML_LIBRARIES)
#MESSAGE(STATUS "PUGIXML_INCLUDE_DIR: ${PUGIXML_INCLUDE_DIR}")
#MESSAGE(STATUS "PUGIXML_LIBRARIES: ${PUGIXML_LIBRARIES}")


#==================================
#==   Check for Log4Cxx         ===
#==================================
MESSAGE(STATUS "Looking for Log4Cxx")
FIND_PACKAGE(Log4Cxx REQUIRED)
MESSAGE(STATUS "LOG4CXX_INCLUDE_DIR: ${LOG4CXX_INCLUDE_DIRS}")
MESSAGE(STATUS "LOG4CXX_LIBRARIES: ${LOG4CXX_LIBRARIES}")

#==================================
#==   Check for MySQL           ===
#==================================
MESSAGE(STATUS "Looking for MySQL")
FIND_PACKAGE(MySQL REQUIRED)
MESSAGE(STATUS "MYSQL_INCLUDE_DIR: ${MYSQL_INCLUDE_DIR}")
MESSAGE(STATUS "MYSQL_LIBRARIES: ${MYSQL_LIBRARIES}")

#======================================
#==   Check for Google Test         ===
#======================================
if (ENABLE_TEST)
	MESSAGE(STATUS "Looking for GoogleTest")
	add_definitions(-DENABLE_TEST)
	enable_testing()
	FIND_PACKAGE(GTest REQUIRED)
	MESSAGE(STATUS "GTEST_INCLUDE_DIRS: ${GTEST_INCLUDE_DIRS}")
	MESSAGE(STATUS "GTEST_LIBRARIES: ${GTEST_BOTH_LIBRARIES}")
endif()

#==================================
#==   Check for Doxygen         ===
#==================================
MESSAGE(STATUS "Looking for Doxygen")
find_package(Doxygen)
if (NOT DOXYGEN_FOUND)
	MESSAGE(STATUS "Doxygen not found, cannot generate project documentation!")
endif()
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

#==================================
#==   Check for Sphinx          ===
#==================================
MESSAGE(STATUS "Looking for Sphinx")
find_package(Sphinx)
if (NOT SPHINX_FOUND)
	MESSAGE(STATUS "Sphinx not found, cannot generate sphinx project documentation!")
endif()

