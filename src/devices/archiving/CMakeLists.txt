add_subdirectory(HdbEventSubscriber)
add_subdirectory(HdbConfigurationManager)

# HdbEventSubscriber
set (HdbEventSubscriber_SRCS ${HdbEventSubscriber_SRCS} PARENT_SCOPE)
set (HdbEventSubscriber_HEADERS ${HdbEventSubscriber_HEADERS} PARENT_SCOPE)
set (HdbEventSubscriber_HEADERS_WPATH ${HdbEventSubscriber_HEADERS_WPATH} PARENT_SCOPE)
set (Main_HdbEventSubscriber_SRCS ${Main_HdbEventSubscriber_SRCS} PARENT_SCOPE)

# HdbConfigurationManager
set (HdbConfigurationManager_SRCS ${HdbConfigurationManager_SRCS} PARENT_SCOPE)
set (HdbConfigurationManager_HEADERS ${HdbConfigurationManager_HEADERS} PARENT_SCOPE)
set (HdbConfigurationManager_HEADERS_WPATH ${HdbConfigurationManager_HEADERS_WPATH} PARENT_SCOPE)
set (Main_HdbConfigurationManager_SRCS ${Main_HdbConfigurationManager_SRCS} PARENT_SCOPE)

set (INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

