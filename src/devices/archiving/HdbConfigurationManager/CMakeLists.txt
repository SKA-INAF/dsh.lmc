## INC & SOURCE DIRS
set(HdbConfigurationManager_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(HdbConfigurationManager_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

## GET SOURCES and HEADERS
set(THIS_SRCS ${THIS_SRCS} 
	${HdbConfigurationManager_SRC_DIR}/HdbConfigurationManagerClass.cpp 
  ${HdbConfigurationManager_SRC_DIR}/HdbConfigurationManager.cpp 
  ${HdbConfigurationManager_SRC_DIR}/HdbConfigurationManagerStateMachine.cpp 
  ${HdbConfigurationManager_SRC_DIR}/ClassFactory.cpp
	${HdbConfigurationManager_SRC_DIR}/HdbConfigurationManagerDynAttrUtils.cpp
)

file(GLOB THIS_HEADERS ${HdbConfigurationManager_INC_DIR}/*.h)

set(THIS_HEADERS_WPATH "")
foreach (_headerFile ${THIS_HEADERS})
	get_filename_component(_basefilename ${_headerFile} NAME)
  list (APPEND THIS_HEADERS_WPATH ${_basefilename})
endforeach()
list(REMOVE_DUPLICATES THIS_HEADERS_WPATH)

## Append srcs and heads to project srcs and heads
list (APPEND HdbConfigurationManager_SRCS ${THIS_SRCS})
list (APPEND HdbConfigurationManager_HEADERS ${THIS_HEADERS})
list (APPEND HdbConfigurationManager_HEADERS_WPATH ${THIS_HEADERS_WPATH})
list (APPEND Main_HdbConfigurationManager_SRCS ${HdbConfigurationManager_SRC_DIR}/main.cpp)

set (HdbConfigurationManager_SRCS ${HdbConfigurationManager_SRCS} PARENT_SCOPE)
set (HdbConfigurationManager_HEADERS ${HdbConfigurationManager_HEADERS} PARENT_SCOPE)
set (HdbConfigurationManager_HEADERS_WPATH ${HdbConfigurationManager_HEADERS_WPATH} PARENT_SCOPE)
set (Main_HdbConfigurationManager_SRCS ${Main_HdbConfigurationManager_SRCS} PARENT_SCOPE)

list (APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
set (INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

