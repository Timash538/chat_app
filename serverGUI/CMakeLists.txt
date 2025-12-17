set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

add_executable(serverGUI
)

set(SRC_FILES
    src/main.cpp
    src/ServerGUIWindow.cpp
)

set(INCLUDE_FILES
    include/serverGUI/ServerGUIWindow.h
    include/serverGUI/UserInfoItem.h
    include/serverGUI/ChatInfoItem.h
)

set(UI_FILES
    ui/ServerGUIWindow.ui
)

target_include_directories(serverGUI PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/ui
)

target_sources(serverGUI PRIVATE ${SRC_FILES} ${INCLUDE_FILES} ${UI_FILES})

if(QT_NO_DEBUG_OUTPUT)
    target_compile_definitions(serverGUI PRIVATE QT_NO_DEBUG_OUTPUT)
endif()

target_link_libraries(serverGUI
	PRIVATE
		common
		Qt5::Widgets
		Qt5::Network
		Qt5::Core
		Qt5::Gui
)

if(WIN32)
    target_link_libraries(serverGUI PRIVATE ws2_32)
endif()
