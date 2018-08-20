add_subdirectory(DishManager)
#add_subdirectory(LMCManager)
add_subdirectory(LMCStarter)

# DishManager
set (DishManager_SRCS ${DishManager_SRCS} PARENT_SCOPE)
set (DishManager_HEADERS ${DishManager_HEADERS} PARENT_SCOPE)
set (DishManager_HEADERS_WPATH ${DishManager_HEADERS_WPATH} PARENT_SCOPE)
set (Main_DishManager_SRCS ${Main_DishManager_SRCS} PARENT_SCOPE)

# LMCStarter
set (LMCStarter_SRCS ${LMCStarter_SRCS} PARENT_SCOPE)
set (LMCStarter_HEADERS ${LMCStarter_HEADERS} PARENT_SCOPE)
set (LMCStarter_HEADERS_WPATH ${LMCStarter_HEADERS_WPATH} PARENT_SCOPE)
set (Main_LMCStarter_SRCS ${Main_LMCStarter_SRCS} PARENT_SCOPE)


# LMCManager
#set (LMCManager_SRCS ${LMCManager_SRCS} PARENT_SCOPE)
#set (LMCManager_HEADERS ${LMCManager_HEADERS} PARENT_SCOPE)
#set (LMCManager_HEADERS_WPATH ${LMCManager_HEADERS_WPATH} PARENT_SCOPE)
#set (Main_LMCManager_SRCS ${Main_LMCManager_SRCS} PARENT_SCOPE)

set (INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

