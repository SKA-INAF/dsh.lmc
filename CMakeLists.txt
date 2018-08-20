#----------------------------------------------------------------
# - Enforce an out-of-source builds before anything else
# ---------------------------------------------------------------
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(STATUS "DSH_LMC requires an out-of-source build.")
    message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
    message(STATUS "CMakeCache.txt")
    message(STATUS "CMakeFiles")
    message(STATUS "Once these files are removed, create a separate directory")
    message(STATUS "and run CMake from there")
    message(FATAL_ERROR "in-source build detected")
endif()

# -------------------------------------------
# CMAKE VERSIONS
# -------------------------------------------
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
cmake_policy(SET CMP0005 NEW)

#---------------------------------------------
# - Project definition
# --------------------------------------------
project(DSH_LMC)

#--- Set directory paths
##set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_PREFIX}/include)
set(CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX}/lib)
## set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)
set(BINARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/bin)
set(CMAKE_TEST_COMMAND ctest -VV)
set(SHARE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share)
set(CONFIG_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/config)
set(SCRIPT_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/scripts)
set(ANSIBLE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/ansible)
set(VAGRANT_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/vagrant)

set(DSHLMC ${CMAKE_BINARY_DIR})
set(HEADER_OUTPUT_PATH ${CMAKE_BINARY_DIR}/include)
set(DSHLMC_INCLUDE_DIR ${HEADER_OUTPUT_PATH})
set(SHARE_TEST_SRC_DIR ${CMAKE_SOURCE_DIR}/share/tests)
set(CONFIG_DIR ${CMAKE_SOURCE_DIR}/config)
set(SCRIPT_DIR ${CMAKE_SOURCE_DIR}/scripts)

# ----------------------------------------------------------------------------------------------
#---Where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked-------------
# ----------------------------------------------------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)

# - Versioning. We do this here for now
#set(${PROJECT_NAME}_VERSION "1.1.0")
#set(${PROJECT_NAME}_VERSION_MAJOR "1")
#set(${PROJECT_NAME}_VERSION_MINOR "1")
#set(${PROJECT_NAME}_VERSION_PATCH "0")

# The project version number.
#set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
#set(VERSION_MINOR   0   CACHE STRING "Project minor version number.")
#set(VERSION_PATCH   1   CACHE STRING "Project patch version number.")
#mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

# Options. Turn on with 'cmake -Dmyvarname=ON'.
option(ENABLE_TEST "Build all tests." OFF) # Makes boolean 'test' available.
option(ENABLE_BUILD_EXTERNALS "Build external projects." OFF) 

# ----------------------------------------------------------------------------------------------
#---Load some basic macros which are needed later for the configuration and build----------------
#-----------------------------------------------------------------------------------------------
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_REENTRANT -Dlinux -g -std=c++11 -O2 -Wall -Wunused -Wuninitialized -fPIC -pthread -m64")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_REENTRANT -Dlinux -g -std=c++0x -O2 -Wall -Wunused -Wuninitialized -fPIC -pthread -m64")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

#-----------------------------------------------------------------------------------------------
#---Here we look for installed software and switch on and of the different build options--------
include(SearchInstalledSoftware)
#-----------------------------------------------------------------------------------------------


#===============================================
#===      VERSIONING
#===============================================
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
MESSAGE(STATUS "Extracted GIT version: GIT_REFSPEC=${GIT_REFSPEC}, GIT_SHA1=${GIT_SHA1}")

# Parse the version information into pieces.
#git_describe(VERSION --tags --dirty=-dirty)
#MESSAGE(STATUS "VERSION= ${VERSION}")
#string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION}")
#string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION}")
#string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION}")
#string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" VERSION_SHA1 "${VERSION}")
#set(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
#MESSAGE(STATUS "VERSION_SHORT= ${VERSION_SHORT}, VERSION_MAJOR= ${VERSION_MAJOR}, ")

# Get version from git tag and append to CMAKE flags
GetGitVersion("${VERSION_MAJOR}" "${VERSION_MINOR}" "${VERSION_PATCH}" "${RELEASE}")
MESSAGE(STATUS "Version: ${VERSION} (${VERSION_MAJOR} ${VERSION_MINOR} ${VERSION_PATCH}) RELEASE=${RELEASE}")
set(DSHLMC_VERSION_DEFINES "-DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\"")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DSHLMC_VERSION_DEFINES}")


#-----------------------------------------------------------------------------------------------
#---  SET INCLUDES & LIBS
#-----------------------------------------------------------------------------------------------
set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
set(LINKED_LIBS "")

# Append subdirs (headers & sources files are appended in subdirs)
add_subdirectory(src)
add_subdirectory(external)
add_subdirectory(config)
add_subdirectory(scripts)
add_subdirectory(ansible)
add_subdirectory(vagrant)

# Append external includes found 
list (APPEND INCLUDE_DIRS ${OMNIORB_INCLUDE_DIRS})
list (APPEND INCLUDE_DIRS ${ZMQ_INCLUDE_DIR})
list (APPEND INCLUDE_DIRS ${TANGO_INCLUDE_DIR})
list (APPEND INCLUDE_DIRS ${YAT_INCLUDE_DIR})
list (APPEND INCLUDE_DIRS ${YAT4TANGO_INCLUDE_DIR})
list (APPEND INCLUDE_DIRS ${HDBPP_INCLUDE_DIR})
list (APPEND INCLUDE_DIRS ${JSONCPP_INCLUDE_DIR})
list (APPEND INCLUDE_DIRS ${PUGIXML_INCLUDE_DIR})
list (APPEND INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
list (APPEND INCLUDE_DIRS ${LOG4CXX_INCLUDE_DIRS})
list (APPEND INCLUDE_DIRS ${MYSQL_INCLUDE_DIR})
if (ENABLE_TEST)
	list (APPEND INCLUDE_DIRS ${GTEST_INCLUDE_DIRS})
endif()

include_directories(${INCLUDE_DIRS})

# Append external libs found 
list (APPEND LINKED_LIBS ${OMNIORB_LIBRARIES})
list (APPEND LINKED_LIBS ${ZMQ_LIBRARIES})
list (APPEND LINKED_LIBS ${TANGO_LIBRARIES})
list (APPEND LINKED_LIBS ${YAT_LIBRARIES})
list (APPEND LINKED_LIBS ${YAT4TANGO_LIBRARIES})
list (APPEND LINKED_LIBS ${HDBPP_LIBRARIES})
list (APPEND LINKED_LIBS ${HDBPP_MYSQL_LIBRARIES})
list (APPEND LINKED_LIBS ${JSONCPP_LIBRARIES})
list (APPEND LINKED_LIBS ${PUGIXML_LIBRARIES})
list (APPEND LINKED_LIBS ${Boost_LIBRARIES})
list (APPEND LINKED_LIBS ${LOG4CXX_LIBRARIES})
list (APPEND LINKED_LIBS ${MYSQL_LIBRARIES})
if (ENABLE_TEST)
  list (APPEND LINKED_LIBS ${GTEST_BOTH_LIBRARIES})
endif()
message (STATUS "LINKED_LIBS: ${LINKED_LIBS}")

set(DEVICE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/devices)
set(DEVICE_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/src/devices)



#-----------------------------------------------------------------------------------------------
#---  CLEAN STUFF
#-----------------------------------------------------------------------------------------------
#--- Set external WCTOOLS lib to be removed by clean command
#--- Set copied headers to be removed by clean command
##set(ADDITIONAL_CLEAN_TARGETS ${BINARY_INSTALL_DIR}/BaseDevice)
##list(APPEND ADDITIONAL_CLEAN_TARGETS ${BINARY_INSTALL_DIR}/LMCInterface)

set(ADDITIONAL_CLEAN_TARGETS "")

set(CLEANED_HEADERS "")
foreach (_headerFile ${UTILS_HEADERS_WPATH})
  set(_cleanedHeader ${CMAKE_INSTALL_INCLUDEDIR}/${_headerFile})
  list(APPEND CLEANED_HEADERS ${_cleanedHeader})
endforeach()

list(REMOVE_DUPLICATES CLEANED_HEADERS)
list(APPEND ADDITIONAL_CLEAN_TARGETS ${CLEANED_HEADERS})

##message(STATUS "ADDITIONAL_CLEAN_TARGETS: ${ADDITIONAL_CLEAN_TARGETS}")
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${ADDITIONAL_CLEAN_TARGETS}") ## do not forget to put "" in final vars otherwise you'll get a parsing error


#===============================================================================================
#===             BUILD PROJECT SHARED LIBS
#===============================================================================================

###  UTILS LIBS  ###
add_library(LMCUtils SHARED ${UTILS_SRCS})
target_link_libraries(LMCUtils PUBLIC ${LINKED_LIBS})

###  CORE LIBS  ###
add_library(LMCDevice SHARED ${LMCDevice_SRCS})
add_dependencies(LMCDevice LMCUtils)
target_link_libraries(LMCDevice PUBLIC ${LINKED_LIBS} LMCUtils)

#==============================================================================================
#===                    BUILD PROJECT TASKS EXEs
#==============================================================================================
###### EXTERNAL ######
if(ENABLE_BUILD_EXTERNALS)
  ## AlarmSystem 
  add_executable(AlarmSystemExe ${Main_AlarmSystem_SRCS} ${AlarmSystem_SRCS})
  target_link_libraries(AlarmSystemExe ${LINKED_LIBS})
  set_target_properties(AlarmSystemExe PROPERTIES OUTPUT_NAME alarm-srv)

  ## FormulaConf 
  set (FormulaConf_FLAGDEF "RELEASE='\"N$Name:  $ \"'")
  set (FormulaConf_FLAGS "-DRELEASE='\"N$Name:  $ \"'")
  add_executable(FormulaConfExe ${Main_FormulaConf_SRCS} ${FormulaConf_SRCS})
  target_link_libraries(FormulaConfExe ${LINKED_LIBS})
  set_target_properties(FormulaConfExe PROPERTIES OUTPUT_NAME formulaconf COMPILE_DEFINITIONS ${FormulaConf_FLAGDEF} COMPILE_FLAGS ${FormulaConf_FLAGS})
endif()

#####################

###### CORE #########
## LMCDevice
#add_executable(LMCDeviceExe ${Main_LMCDevice_SRCS} ${LMCDevice_SRCS})
#target_link_libraries(LMCDeviceExe LMCUtils LMCDevice ${LINKED_LIBS})
#set_target_properties(LMCDeviceExe PROPERTIES OUTPUT_NAME LMCDevice)
#####################

###### LOGGING ######
## ElementLogger 
add_executable(ElementLoggerExe ${Main_ElementLogger_SRCS} ${ElementLogger_SRCS})
target_link_libraries(ElementLoggerExe LMCUtils LMCDevice ${LINKED_LIBS})
set_target_properties(ElementLoggerExe PROPERTIES OUTPUT_NAME ElementLogger)
#####################

###### MONITORING ######
## DishMonitor 
##add_executable(DishMonitorExe ${Main_DishMonitor_SRCS} ${DishMonitor_SRCS})
##target_link_libraries(DishMonitorExe LMCUtils LMCDevice ${LINKED_LIBS})
##set_target_properties(DishMonitorExe PROPERTIES OUTPUT_NAME DishMonitor)

## LMCMonitor 
add_executable(LMCMonitorExe ${Main_LMCMonitor_SRCS} ${LMCMonitor_SRCS})
target_link_libraries(LMCMonitorExe LMCUtils LMCDevice ${LINKED_LIBS})
set_target_properties(LMCMonitorExe PROPERTIES OUTPUT_NAME LMCMonitor)
########################

###### Emulators ######
## SPFEmulator
add_executable(SPFEmulatorExe ${Main_SPFEmulator_SRCS} ${SPFEmulator_SRCS})
target_link_libraries(SPFEmulatorExe LMCUtils LMCDevice ${LINKED_LIBS})
set_target_properties(SPFEmulatorExe PROPERTIES OUTPUT_NAME SPFEmulator)

## RxEmulator
add_executable(RxEmulatorExe ${Main_RxEmulator_SRCS} ${RxEmulator_SRCS})
target_link_libraries(RxEmulatorExe LMCUtils LMCDevice ${LINKED_LIBS})
set_target_properties(RxEmulatorExe PROPERTIES OUTPUT_NAME RxEmulator)

## DSEmulator
add_executable(DSEmulatorExe ${Main_DSEmulator_SRCS} ${DSEmulator_SRCS})
target_link_libraries(DSEmulatorExe LMCUtils LMCDevice ${LINKED_LIBS})
set_target_properties(DSEmulatorExe PROPERTIES OUTPUT_NAME DSEmulator)

## TMEmulator
##add_executable(TMEmulatorExe ${Main_TMEmulator_SRCS} ${TMEmulator_SRCS})
##target_link_libraries(TMEmulatorExe ${LINKED_LIBS})
##set_target_properties(TMEmulatorExe PROPERTIES OUTPUT_NAME TMEmulator)
#########################

####### CONTROL DRIVERS #########
# DSManager
add_executable(DSManagerExe ${Main_DSManager_SRCS} ${DSManager_SRCS})
target_link_libraries(DSManagerExe LMCUtils LMCDevice ${LINKED_LIBS})
set_target_properties(DSManagerExe PROPERTIES OUTPUT_NAME DSManager)
################################

###### MASTER ######
## DishManager 
add_executable(DishManagerExe ${Main_DishManager_SRCS} ${DishManager_SRCS})
#target_link_libraries(DishManagerExe LMCUtils LMCDevice DishManager ${LINKED_LIBS})
target_link_libraries(DishManagerExe LMCUtils LMCDevice ${LINKED_LIBS})
set_target_properties(DishManagerExe PROPERTIES OUTPUT_NAME DishManager)

## LMCStarter
add_executable(LMCStarterExe ${Main_LMCStarter_SRCS} ${LMCStarter_SRCS})
target_link_libraries(LMCStarterExe LMCUtils LMCDevice ${LINKED_LIBS})
set_target_properties(LMCStarterExe PROPERTIES OUTPUT_NAME LMCStarter)

## LMCManager
#add_executable(LMCManagerExe ${Main_LMCManager_SRCS} ${LMCManager_SRCS})
#target_link_libraries(LMCManagerExe LMCUtils LMCDevice ${LINKED_LIBS})
#set_target_properties(LMCManagerExe PROPERTIES OUTPUT_NAME LMCManager)
#####################


###### ALARM HANDLING ######
## AlarmHandler 
add_executable(AlarmHandlerExe ${Main_AlarmHandler_SRCS} ${AlarmHandler_SRCS})
target_link_libraries(AlarmHandlerExe LMCUtils LMCDevice ${LINKED_LIBS})
set_target_properties(AlarmHandlerExe PROPERTIES OUTPUT_NAME AlarmHandler)
#####################

###### ARCHIVER ######
## HdbEventSubscriber 
add_executable(HdbEventSubscriberExe ${Main_HdbEventSubscriber_SRCS} ${HdbEventSubscriber_SRCS})
target_link_libraries(HdbEventSubscriberExe LMCUtils LMCDevice ${LINKED_LIBS})
set_target_properties(HdbEventSubscriberExe PROPERTIES OUTPUT_NAME LMCArchiver)

## HdbConfigurationManager 
add_executable(HdbConfigurationManagerExe ${Main_HdbConfigurationManager_SRCS} ${HdbConfigurationManager_SRCS})
target_link_libraries(HdbConfigurationManagerExe LMCUtils LMCDevice ${LINKED_LIBS})
set_target_properties(HdbConfigurationManagerExe PROPERTIES OUTPUT_NAME LMCArchiverConfigurator)
#####################

#==============================================================================================
#                  OPTIONAL TARGETS
#==============================================================================================

add_library(lmcutils EXCLUDE_FROM_ALL SHARED ${UTILS_SRCS} )
target_link_libraries(lmcutils PUBLIC ${LINKED_LIBS})
#set_target_properties(lmcutils PROPERTIES EXCLUDE_FROM_ALL TRUE)
#===============================================================================================


#==============================================================================================
#===                    TESTING
#==============================================================================================
if (ENABLE_TEST)
	# Append subdirs (headers & sources files are appended in subdirs)
	set(TEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIRS})
	add_subdirectory(test)

	enable_testing()

  # Include test & Google Test headers
  include_directories(${TEST_INCLUDE_DIRS})

	# Append external libs found 
	list (APPEND LINKED_LIBS ${GTEST_BOTH_LIBRARIES})
	message (STATUS "LINKED_LIBS: ${LINKED_LIBS}")
	
  ##############
  # Unit Tests
  ##############	
	add_executable(runUnitTests ${UNIT_TEST_MAIN} ${UNIT_TEST_SRCS})
	add_dependencies(runUnitTests LMCUtils)

  # Standard linking to gtest stuff.
	target_link_libraries(runUnitTests PUBLIC ${LINKED_LIBS} LMCUtils)

  # This is so you can do 'make test' to see all your tests run, instead of
  # manually running the executable runUnitTests to see those specific tests.
	add_test(UnitTests ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/runUnitTests --gtest_output="xml:UnitTestResults.xml")

  # You can also omit NAME and COMMAND. The second argument could be some other
  # test executable.
#  add_test(that-other-test-I-made runUnitTests)
endif()

#==============================================================================================
#                  CPPCHECK TARGETS
#==============================================================================================
list (APPEND ALL_SOURCES ${UTILS_SRCS})
list (APPEND ALL_SOURCES ${LMCDevice_SRCS})
list (APPEND ALL_SOURCES ${Main_ElementLogger_SRCS} ${ElementLogger_SRCS})
#list (APPEND ALL_SOURCES ${Main_DishMonitor_SRCS} ${DishMonitor_SRCS})
list (APPEND ALL_SOURCES ${Main_DSEmulator_SRCS} ${DSEmulator_SRCS})
list (APPEND ALL_SOURCES ${Main_SPFEmulator_SRCS} ${SPFEmulator_SRCS})
list (APPEND ALL_SOURCES ${Main_RxEmulator_SRCS} ${RxEmulator_SRCS})
list (APPEND ALL_SOURCES ${Main_DSManager_SRCS} ${DSManager_SRCS})
list (APPEND ALL_SOURCES ${Main_DishManager_SRCS} ${DishManager_SRCS})
list (APPEND ALL_SOURCES ${Main_LMCStarter_SRCS} ${LMCStarter_SRCS})
#list (APPEND ALL_SOURCES ${Main_LMCManager_SRCS} ${LMCManager_SRCS})
list (APPEND ALL_SOURCES ${Main_AlarmHandler_SRCS} ${AlarmHandler_SRCS})
list (APPEND ALL_SOURCES ${Main_LMCMonitor_SRCS} ${LMCMonitor_SRCS})
list (APPEND ALL_SOURCES ${Main_HdbEventSubscriber_SRCS} ${HdbEventSubscriber_SRCS})
list (APPEND ALL_SOURCES ${Main_HdbConfigurationManager_SRCS} ${HdbConfigurationManager_SRCS})
if (ENABLE_TEST)
	list (APPEND ALL_SOURCES ${UNIT_TEST_SRCS})
endif()

add_custom_target(
  cppcheck
 	COMMAND cppcheck
  	--enable=warning,performance,portability,information,missingInclude
    --std=c++11
    --library=qt.cfg
    --suppress=missingIncludeSystem
    --template="[{severity}][{id}] {message} {callstack} \(On {file}:{line}\)"
    --verbose
    --xml-version=2
    #--quiet
    ${ALL_SOURCES}
)
#===============================================================================================


#-----------------------------------------------------------------------------------------------
#---  INSTALL STUFF
#-----------------------------------------------------------------------------------------------

# -- Install exe targets
if(ENABLE_BUILD_EXTERNALS)
  install(TARGETS AlarmSystemExe DESTINATION ${BINARY_INSTALL_DIR})
  install(TARGETS FormulaConfExe DESTINATION ${BINARY_INSTALL_DIR})
endif()
install(TARGETS ElementLoggerExe DESTINATION ${BINARY_INSTALL_DIR})
#install(TARGETS DishMonitorExe DESTINATION ${BINARY_INSTALL_DIR})
install(TARGETS SPFEmulatorExe DESTINATION ${BINARY_INSTALL_DIR})
install(TARGETS RxEmulatorExe DESTINATION ${BINARY_INSTALL_DIR})
install(TARGETS DSEmulatorExe DESTINATION ${BINARY_INSTALL_DIR})
install(TARGETS DSManagerExe DESTINATION ${BINARY_INSTALL_DIR})
install(TARGETS DishManagerExe DESTINATION ${BINARY_INSTALL_DIR})
install(TARGETS LMCStarterExe DESTINATION ${BINARY_INSTALL_DIR})
#install(TARGETS LMCManagerExe DESTINATION ${BINARY_INSTALL_DIR})
install(TARGETS AlarmHandlerExe DESTINATION ${BINARY_INSTALL_DIR})
install(TARGETS LMCMonitorExe DESTINATION ${BINARY_INSTALL_DIR})
install(TARGETS HdbEventSubscriberExe DESTINATION ${BINARY_INSTALL_DIR})
install(TARGETS HdbConfigurationManagerExe DESTINATION ${BINARY_INSTALL_DIR})
if (ENABLE_TEST)
	install(TARGETS runUnitTests DESTINATION ${BINARY_INSTALL_DIR})
endif()

# -- Install python Tango LMC devices
install(FILES ${PDUEmulatorExe} DESTINATION ${BINARY_INSTALL_DIR} RENAME PDUEmulator PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE)
install(FILES ${PDUManagerExe} DESTINATION ${BINARY_INSTALL_DIR} RENAME PDUManager PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE)
install(FILES ${TMEmulatorExe} DESTINATION ${BINARY_INSTALL_DIR} RENAME TMEmulator PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE)
install(FILES ${LogPrinterExe} DESTINATION ${BINARY_INSTALL_DIR} RENAME LogPrinter PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE)

# -- Install libraries
install(TARGETS LMCUtils DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(TARGETS LMCDevice DESTINATION ${CMAKE_INSTALL_LIBDIR})

# -- Install library dict map
##install(FILES DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

#-- Install project headers
install(FILES ${UTILS_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# -- Install share files
file(GLOB_RECURSE TEST_SHARE_FILES ${SHARE_TEST_SRC_DIR}/*)
#MESSAGE(STATUS TEST_SHARE_FILES: ${TEST_SHARE_FILES})
install(FILES ${TEST_SHARE_FILES} DESTINATION ${SHARE_INSTALL_DIR})

# -- Install config files
#MESSAGE(STATUS CONFIG_FILES: ${CONFIG_FILES})
install(FILES ${CONFIG_FILES} DESTINATION ${CONFIG_INSTALL_DIR})

# -- Install script files
#MESSAGE(STATUS SCRIPT_FILES: ${SCRIPT_FILES})
install(FILES ${SCRIPT_FILES} DESTINATION ${SCRIPT_INSTALL_DIR} PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE)

# -- Install ansible files
#MESSAGE(STATUS ANSIBLE_FILES: ${ANSIBLE_FILES})
#install(FILES ${ANSIBLE_FILES} DESTINATION ${ANSIBLE_INSTALL_DIR})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/ansible DESTINATION ${CMAKE_INSTALL_PREFIX} USE_SOURCE_PERMISSIONS)

# -- Install vagrant files
#MESSAGE(STATUS VAGRANT_FILES: ${VAGRANT_FILES})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/vagrant DESTINATION ${CMAKE_INSTALL_PREFIX})
#-----------------------------------------------------------------------------------------------


################################
##       Documentation
################################
#if(DOXYGEN_FOUND)
#	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
#	add_custom_target(doc
#		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
#		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#		COMMENT "Generating API documentation with Doxygen" VERBATIM
#	)
#endif(DOXYGEN_FOUND)

if(BUILD_DOCUMENTATION)
	if(NOT DOXYGEN_FOUND)
  	message(FATAL_ERROR "Doxygen is needed to build the documentation.")
  endif()

	# -- Generate doxygen doc files
  set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen/Doxyfile.in)
  set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/doc/doxygen/Doxyfile)
  configure_file(${doxyfile_in} ${doxyfile} @ONLY)

  add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
	)

	# -- Generate sphinx doc files
	configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/doc/sphinx/conf.py"
    "${CMAKE_CURRENT_BINARY_DIR}/doc/sphinx/conf.py"
    @ONLY)
	configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/doc/sphinx/index.rst"
    "${CMAKE_CURRENT_BINARY_DIR}/doc/sphinx/index.rst"
    @ONLY)

	if(SPHINX_FOUND)
		macro(add_sphinx_target TARGET_NAME BUILDER COMMENT_STR)
    	add_custom_target(${TARGET_NAME}
    	COMMAND sphinx-build -b ${BUILDER} . sphinx/${BUILDER}
			#COMMAND sphinx-build -b ${BUILDER} ${CMAKE_CURRENT_BINARY_DIR}/doc/sphinx sphinx/build/${BUILDER}
    	    #WORKING_DIRECTORY .
					WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/sphinx
    	    DEPENDS doc
    	    COMMENT ${COMMENT_STR}
    	)
		endmacro(add_sphinx_target)
		add_sphinx_target(doc-sphinx html "Build Sphinx html documentation")
	endif()

  # -- Install doc files
  install(
    DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
    DESTINATION ${SHARE_INSTALL_DIR}
  )

endif()




#################################
##        PACKAGING           ###
#################################
## VERSIONING FROM GIT 
#configure_file("${CMAKE_CURRENT_SOURCE_DIR}/git_version.cc.in" "${CMAKE_CURRENT_BINARY_DIR}/git_version.cc" @ONLY)
#list(APPEND SOURCES "${CMAKE_CURRENT_BINARY_DIR}/git_version.cpp" git_version.h)


# all install commands get the same destination. this allows us to use paths
# relative to the executable.
#install(TARGETS example DESTINATION example_destination)
# this is basically a repeat of the file copy instruction that copies the
# resources in the build directory, but here we tell cmake that we want it
# in the package
#install(DIRECTORY resources DESTINATION example_destination)

# now comse everything we need, to create a package
# there are a lot more variables you can set, and some
# you need to set for some package types, but we want to
# be minimal here
set(CPACK_PACKAGE_NAME "DSH_LMC")
set(CPACK_PACKAGE_VERSION "1.0.0")

# we don't want to split our program up into several things
set(CPACK_MONOLITHIC_INSTALL 1)

# This must be last
include(CPack)

