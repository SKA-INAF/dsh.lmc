## INC & SOURCE DIRS
set(LMCMonitor_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(LMCMonitor_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

## GET SOURCES and HEADERS
set(THIS_SRCS ${THIS_SRCS} 
  ${LMCMonitor_SRC_DIR}/LMCMonitorClass.cpp 
  ${LMCMonitor_SRC_DIR}/LMCMonitor.cpp 
  ${LMCMonitor_SRC_DIR}/LMCMonitorStateMachine.cpp 
  ${LMCMonitor_SRC_DIR}/ClassFactory.cpp
	${LMCMonitor_SRC_DIR}/NagiosListenerThread.cpp
)

file(GLOB THIS_HEADERS ${LMCMonitor_INC_DIR}/*.h)

set(THIS_HEADERS_WPATH "")
foreach (_headerFile ${THIS_HEADERS})
	get_filename_component(_basefilename ${_headerFile} NAME)
  list (APPEND THIS_HEADERS_WPATH ${_basefilename})
endforeach()
list(REMOVE_DUPLICATES THIS_HEADERS_WPATH)


## Append srcs and heads to project srcs and heads
list (APPEND LMCMonitor_SRCS ${THIS_SRCS})
list (APPEND LMCMonitor_HEADERS ${THIS_HEADERS})
list (APPEND LMCMonitor_HEADERS_WPATH ${THIS_HEADERS_WPATH})
list (APPEND Main_LMCMonitor_SRCS ${LMCMonitor_SRC_DIR}/main.cpp)

set (LMCMonitor_SRCS ${LMCMonitor_SRCS} PARENT_SCOPE)
set (LMCMonitor_HEADERS ${LMCMonitor_HEADERS} PARENT_SCOPE)
set (LMCMonitor_HEADERS_WPATH ${LMCMonitor_HEADERS_WPATH} PARENT_SCOPE)
set (Main_LMCMonitor_SRCS ${Main_LMCMonitor_SRCS} PARENT_SCOPE)

list (APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
set (INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

