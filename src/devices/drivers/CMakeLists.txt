add_subdirectory(DSManager)
add_subdirectory(PDUManager)

# DSManager
set (DSManager_SRCS ${DSManager_SRCS} PARENT_SCOPE)
set (DSManager_HEADERS ${DSManager_HEADERS} PARENT_SCOPE)
set (DSManager_HEADERS_WPATH ${DSManager_HEADERS_WPATH} PARENT_SCOPE)
set (Main_DSManager_SRCS ${Main_DSManager_SRCS} PARENT_SCOPE)

# PDUManager
set(PDUManagerExe ${PDUManagerExe} PARENT_SCOPE)

set (INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

