## INC & SOURCE DIRS
set(LMCDevice_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(LMCDevice_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
##message (STATUS "LMCDevice_INC_DIR: ${LMCDevice_INC_DIR}")

## GET SOURCES and HEADERS
#file(GLOB_RECURSE THIS_SRCS ${BaseDevice_SRC_DIR}/*.cpp)
set(THIS_SRCS ${THIS_SRCS} 
  ${LMCDevice_SRC_DIR}/LMCDeviceClass.cpp 
  ${LMCDevice_SRC_DIR}/LMCDevice.cpp 
  ${LMCDevice_SRC_DIR}/LMCDeviceStateMachine.cpp 
	${LMCDevice_SRC_DIR}/LMCDeviceDynAttrUtils.cpp 
  ${LMCDevice_SRC_DIR}/ClassFactory.cpp
	${LMCDevice_SRC_DIR}/EventCallBack.cpp
	${LMCDevice_SRC_DIR}/ProxyMonitorTask.cpp
	${LMCDevice_SRC_DIR}/EventHandlerTask.cpp
	${LMCDevice_SRC_DIR}/TaskThread.cpp
	${LMCDevice_SRC_DIR}/AttrRandomizerThread.cpp
)

file(GLOB THIS_HEADERS ${LMCDevice_INC_DIR}/*.h)

set(THIS_HEADERS_WPATH "")
foreach (_headerFile ${THIS_HEADERS})
	get_filename_component(_basefilename ${_headerFile} NAME)
  list (APPEND THIS_HEADERS_WPATH ${_basefilename})
endforeach()
list(REMOVE_DUPLICATES THIS_HEADERS_WPATH)

## Append srcs and heads to project srcs and heads
list (APPEND LMCDevice_SRCS ${THIS_SRCS})
list (APPEND LMCDevice_HEADERS ${THIS_HEADERS})
list (APPEND LMCDevice_HEADERS_WPATH ${THIS_HEADERS_WPATH})
list (APPEND Main_LMCDevice_SRCS ${LMCDevice_SRC_DIR}/main.cpp)

set (LMCDevice_SRCS ${LMCDevice_SRCS} PARENT_SCOPE)
set (LMCDevice_HEADERS ${LMCDevice_HEADERS} PARENT_SCOPE)
set (LMCDevice_HEADERS_WPATH ${LMCDevice_HEADERS_WPATH} PARENT_SCOPE)
set (Main_LMCDevice_SRCS ${Main_LMCDevice_SRCS} PARENT_SCOPE)

list (APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
set (INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

