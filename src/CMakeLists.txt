project( Xmas )

add_executable( Xmas WIN32 )

target_link_libraries( Xmas
    PRIVATE gdi32
    PRIVATE user32
    PRIVATE gdiplus
    PRIVATE SAP
    PRIVATE ${OAL_LIB}
    PRIVATE ogg
    PRIVATE vorbis
    PRIVATE vorbisfile
)

target_sources( Xmas
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/wnd/CRandGen.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/wnd/CTimer.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/wnd/IWindowEngine.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/wnd/TrayIcon.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/wnd/WindowController.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/MerryChristmas.rc
)

target_include_directories( Xmas
    PRIVATE ${OAL_INC_DIR}
)