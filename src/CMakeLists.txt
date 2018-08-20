add_subdirectory(utils)
add_subdirectory(devices)

#### Utils ############
set (UTILS_SRCS ${UTILS_SRCS} PARENT_SCOPE)
set (UTILS_HEADERS ${UTILS_HEADERS} PARENT_SCOPE)
set (UTILS_HEADERS_WPATH ${UTILS_HEADERS_WPATH} PARENT_SCOPE)
#######################


####### CORE #########
# LMCDevice
set (LMCDevice_SRCS ${LMCDevice_SRCS} PARENT_SCOPE)
set (LMCDevice_HEADERS ${LMCDevice_HEADERS} PARENT_SCOPE)
set (LMCDevice_HEADERS_WPATH ${LMCDevice_HEADERS_WPATH} PARENT_SCOPE)
set (Main_LMCDevice_SRCS ${Main_LMCDevice_SRCS} PARENT_SCOPE)
#####################

##### SE EMULATORS #########
# SPFEmulator
set (SPFEmulator_SRCS ${SPFEmulator_SRCS} PARENT_SCOPE)
set (SPFEmulator_HEADERS ${SPFEmulator_HEADERS} PARENT_SCOPE)
set (SPFEmulator_HEADERS_WPATH ${SPFEmulator_HEADERS_WPATH} PARENT_SCOPE)
set (Main_SPFEmulator_SRCS ${Main_SPFEmulator_SRCS} PARENT_SCOPE)

# RxEmulator
set (RxEmulator_SRCS ${RxEmulator_SRCS} PARENT_SCOPE)
set (RxEmulator_HEADERS ${RxEmulator_HEADERS} PARENT_SCOPE)
set (RxEmulator_HEADERS_WPATH ${RxEmulator_HEADERS_WPATH} PARENT_SCOPE)
set (Main_RxEmulator_SRCS ${Main_RxEmulator_SRCS} PARENT_SCOPE)

# DSEmulator
set (DSEmulator_SRCS ${DSEmulator_SRCS} PARENT_SCOPE)
set (DSEmulator_HEADERS ${DSEmulator_HEADERS} PARENT_SCOPE)
set (DSEmulator_HEADERS_WPATH ${DSEmulator_HEADERS_WPATH} PARENT_SCOPE)
set (Main_DSEmulator_SRCS ${Main_DSEmulator_SRCS} PARENT_SCOPE)

# TMEmulator
#set (TMEmulator_SRCS ${TMEmulator_SRCS} PARENT_SCOPE)
#set (TMEmulator_HEADERS ${TMEmulator_HEADERS} PARENT_SCOPE)
#set (TMEmulator_HEADERS_WPATH ${TMEmulator_HEADERS_WPATH} PARENT_SCOPE)
#set (Main_TMEmulator_SRCS ${Main_TMEmulator_SRCS} PARENT_SCOPE)
set(TMEmulatorExe ${TMEmulatorExe} PARENT_SCOPE)

# PDUEmulator
set(PDUEmulatorExe ${PDUEmulatorExe} PARENT_SCOPE)
############################

####### LOGGING #########
# ElementLogger
set (ElementLogger_SRCS ${ElementLogger_SRCS} PARENT_SCOPE)
set (ElementLogger_HEADERS ${ElementLogger_HEADERS} PARENT_SCOPE)
set (ElementLogger_HEADERS_WPATH ${ElementLogger_HEADERS_WPATH} PARENT_SCOPE)
set (Main_ElementLogger_SRCS ${Main_ElementLogger_SRCS} PARENT_SCOPE)

# LogPrinter
set(LogPrinterExe ${LogPrinterExe} PARENT_SCOPE)
#####################


####### MONITORING #########
# DishMonitor
##set (DishMonitor_SRCS ${DishMonitor_SRCS} PARENT_SCOPE)
##set (DishMonitor_HEADERS ${DishMonitor_HEADERS} PARENT_SCOPE)
##set (DishMonitor_HEADERS_WPATH ${DishMonitor_HEADERS_WPATH} PARENT_SCOPE)
##set (Main_DishMonitor_SRCS ${Main_DishMonitor_SRCS} PARENT_SCOPE)

# LMCMonitor
set (LMCMonitor_SRCS ${LMCMonitor_SRCS} PARENT_SCOPE)
set (LMCMonitor_HEADERS ${LMCMonitor_HEADERS} PARENT_SCOPE)
set (LMCMonitor_HEADERS_WPATH ${LMCMonitor_HEADERS_WPATH} PARENT_SCOPE)
set (Main_LMCMonitor_SRCS ${Main_LMCMonitor_SRCS} PARENT_SCOPE)
#####################

####### CONTROL DRIVERS #########
# DSManager
set (DSManager_SRCS ${DSManager_SRCS} PARENT_SCOPE)
set (DSManager_HEADERS ${DSManager_HEADERS} PARENT_SCOPE)
set (DSManager_HEADERS_WPATH ${DSManager_HEADERS_WPATH} PARENT_SCOPE)
set (Main_DSManager_SRCS ${Main_DSManager_SRCS} PARENT_SCOPE)

# PDUManager
set(PDUManagerExe ${PDUManagerExe} PARENT_SCOPE)
################################

####### MASTER #########
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
#####################

####### ALARM HANDLING #########
# AlarmHandler
set (AlarmHandler_SRCS ${AlarmHandler_SRCS} PARENT_SCOPE)
set (AlarmHandler_HEADERS ${AlarmHandler_HEADERS} PARENT_SCOPE)
set (AlarmHandler_HEADERS_WPATH ${AlarmHandler_HEADERS_WPATH} PARENT_SCOPE)
set (Main_AlarmHandler_SRCS ${Main_AlarmHandler_SRCS} PARENT_SCOPE)
#####################

####### ARCHIVER #########
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
#####################

set (INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

