##add_subdirectory(AlarmHandler)
add_subdirectory(AlarmHandler_last)

# AlarmHandler
set (AlarmHandler_SRCS ${AlarmHandler_SRCS} PARENT_SCOPE)
set (AlarmHandler_HEADERS ${AlarmHandler_HEADERS} PARENT_SCOPE)
set (AlarmHandler_HEADERS_WPATH ${AlarmHandler_HEADERS_WPATH} PARENT_SCOPE)
set (Main_AlarmHandler_SRCS ${Main_AlarmHandler_SRCS} PARENT_SCOPE)

set (INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

