set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

add_executable(client



)

set(SRC_FILES
    src/main.cpp
    src/AuthDialog.cpp
    src/CreateGroupChat.cpp
    src/ChatsWindow.cpp
    src/MainWindow.cpp
)

set(INCLUDE_FILES
    include/client/Client.h
    include/client/AuthDialog.h
    include/client/MainWindow.h
    include/client/CommonItem.h
    include/client/ChatsWindow.h
    include/client/CreateGroupChat.h
)

set(UI_FILES
    ui/AuthDialog.ui
    ui/MainWindow.ui
    ui/CreateGroupChat.ui
    ui/ChatsWindow.ui
)

target_include_directories(client PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/ui
)

target_sources(client PRIVATE ${SRC_FILES} ${INCLUDE_FILES} ${UI_FILES})

target_link_libraries(client
	PRIVATE
		common
		Qt5::Widgets
		Qt5::Network
		Qt5::Core
		Qt5::Gui
)

if(WIN32)
    target_link_libraries(client PRIVATE ws2_32)
endif()
