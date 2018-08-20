## INC & SOURCE DIRS
set(AlarmSystem_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(AlarmSystem_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

## GET SOURCES and HEADERS
set(THIS_SRCS ${THIS_SRCS} 
  ${AlarmSystem_SRC_DIR}/AlarmClass.cpp 
  ${AlarmSystem_SRC_DIR}/Alarm.cpp 
  ${AlarmSystem_SRC_DIR}/AlarmStateMachine.cpp 
  ${AlarmSystem_SRC_DIR}/ClassFactory.cpp
	${AlarmSystem_SRC_DIR}/alarm_table.cpp
	${AlarmSystem_SRC_DIR}/alarm-thread.cpp
	${AlarmSystem_SRC_DIR}/cmd_thread.cpp
	${AlarmSystem_SRC_DIR}/event_table.cpp
	${AlarmSystem_SRC_DIR}/log_thread.cpp
	${AlarmSystem_SRC_DIR}/update-thread.cpp
)

file(GLOB THIS_HEADERS ${AlarmSystem_INC_DIR}/*.h)

set(THIS_HEADERS_WPATH "")
foreach (_headerFile ${THIS_HEADERS})
	get_filename_component(_basefilename ${_headerFile} NAME)
  list (APPEND THIS_HEADERS_WPATH ${_basefilename})
endforeach()
list(REMOVE_DUPLICATES THIS_HEADERS_WPATH)


## Append srcs and heads to project srcs and heads
list (APPEND AlarmSystem_SRCS ${THIS_SRCS})
list (APPEND AlarmSystem_HEADERS ${THIS_HEADERS})
list (APPEND AlarmSystem_HEADERS_WPATH ${THIS_HEADERS_WPATH})
list (APPEND Main_AlarmSystem_SRCS ${AlarmSystem_SRC_DIR}/main.cpp)

set (AlarmSystem_SRCS ${AlarmSystem_SRCS} PARENT_SCOPE)
set (AlarmSystem_HEADERS ${AlarmSystem_HEADERS} PARENT_SCOPE)
set (AlarmSystem_HEADERS_WPATH ${AlarmSystem_HEADERS_WPATH} PARENT_SCOPE)
set (Main_AlarmSystem_SRCS ${Main_AlarmSystem_SRCS} PARENT_SCOPE)

list (APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
set (INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

