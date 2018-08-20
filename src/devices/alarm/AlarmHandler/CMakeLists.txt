## INC & SOURCE DIRS
set(AlarmHandler_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(AlarmHandler_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

## GET SOURCES and HEADERS
set(THIS_SRCS ${THIS_SRCS} 
  ${AlarmHandler_SRC_DIR}/AlarmHandlerClass.cpp 
  ${AlarmHandler_SRC_DIR}/AlarmHandler.cpp 
  ${AlarmHandler_SRC_DIR}/AlarmHandlerStateMachine.cpp 
  ${AlarmHandler_SRC_DIR}/ClassFactory.cpp
	${AlarmHandler_SRC_DIR}/AlarmHandlerDynAttrUtils.cpp
	${AlarmHandler_SRC_DIR}/alarm_table.cpp
	${AlarmHandler_SRC_DIR}/alarm-thread.cpp
	${AlarmHandler_SRC_DIR}/cmd_thread.cpp
	${AlarmHandler_SRC_DIR}/event_table.cpp
	${AlarmHandler_SRC_DIR}/SubscribeThread.cpp
	${AlarmHandler_SRC_DIR}/update-thread.cpp
)

file(GLOB THIS_HEADERS ${AlarmHandler_INC_DIR}/*.h)

set(THIS_HEADERS_WPATH "")
foreach (_headerFile ${THIS_HEADERS})
	get_filename_component(_basefilename ${_headerFile} NAME)
  list (APPEND THIS_HEADERS_WPATH ${_basefilename})
endforeach()
list(REMOVE_DUPLICATES THIS_HEADERS_WPATH)


## Append srcs and heads to project srcs and heads
list (APPEND AlarmHandler_SRCS ${THIS_SRCS})
list (APPEND AlarmHandler_HEADERS ${THIS_HEADERS})
list (APPEND AlarmHandler_HEADERS_WPATH ${THIS_HEADERS_WPATH})
list (APPEND Main_AlarmHandler_SRCS ${AlarmHandler_SRC_DIR}/main.cpp)

set (AlarmHandler_SRCS ${AlarmHandler_SRCS} PARENT_SCOPE)
set (AlarmHandler_HEADERS ${AlarmHandler_HEADERS} PARENT_SCOPE)
set (AlarmHandler_HEADERS_WPATH ${AlarmHandler_HEADERS_WPATH} PARENT_SCOPE)
set (Main_AlarmHandler_SRCS ${Main_AlarmHandler_SRCS} PARENT_SCOPE)

list (APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
set (INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

