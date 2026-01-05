cmake_minimum_required(VERSION 3.16)

project(Pngyu VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Set UI file directory for proper generation
set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/preview_window
    ${CMAKE_SOURCE_DIR}/src/preferences_dialog
    ${CMAKE_SOURCE_DIR}/src/imageoptim_integration_question_dialog
)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/preview_window
    ${CMAKE_SOURCE_DIR}/src/preferences_dialog
    ${CMAKE_SOURCE_DIR}/src/imageoptim_integration_question_dialog
)

# Source files
set(PROJECT_SOURCES
    src/main.cpp
    src/pngyumainwindow.cpp
    src/pngyumainwindow.h
    src/pngyumainwindow.ui
    src/pngyu_execute_pngquant_command.cpp
    src/pngyu_execute_pngquant_command.h
    src/pngyu_util.h
    src/pngyu_pngquant_option.h
    src/pngyu_defines.h
    src/pngyu_custom_tablewidget_item.h
    src/spinnerwidget.h
    src/executecompressthread.cpp
    src/executecompressthread.h
    src/executecompressworkerthread.cpp
    src/executecompressworkerthread.h
    src/preview_window/basicimageview.cpp
    src/preview_window/basicimageview.h
    src/preview_window/pngyupreviewwindow.cpp
    src/preview_window/pngyupreviewwindow.h
    src/preview_window/pngyupreviewwindow.ui
    src/preferences_dialog/pngyupreferencesdialog.cpp
    src/preferences_dialog/pngyupreferencesdialog.h
    src/preferences_dialog/pngyupreferencesdialog.ui
    src/imageoptim_integration_question_dialog/pngyuimageoptimintegrationquestiondialog.cpp
    src/imageoptim_integration_question_dialog/pngyuimageoptimintegrationquestiondialog.h
    src/imageoptim_integration_question_dialog/pngyuimageoptimintegrationquestiondialog.ui
)

# Resource files
set(PROJECT_RESOURCES
    resource/resource.qrc
)

# Create executable
if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE icon.icns)
    set(MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/resource/Info.plist)
    
    qt_add_executable(Pngyu MACOSX_BUNDLE
        ${PROJECT_SOURCES}
        ${PROJECT_RESOURCES}
        resource/icon.icns
    )
    
    set_source_files_properties(resource/icon.icns PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
    
    # Copy pngquant binary to app bundle
    add_custom_command(TARGET Pngyu POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_BUNDLE_CONTENT_DIR:Pngyu>/Resources
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/pngquant_bin/mac/pngquant
            $<TARGET_BUNDLE_CONTENT_DIR:Pngyu>/Resources/pngquant
        COMMAND chmod +x $<TARGET_BUNDLE_CONTENT_DIR:Pngyu>/Resources/pngquant
    )
elseif(WIN32)
    qt_add_executable(Pngyu WIN32
        ${PROJECT_SOURCES}
        ${PROJECT_RESOURCES}
        resource/resource.rc
    )
    
    # Copy pngquant binary to pngquant subdirectory
    add_custom_command(TARGET Pngyu POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:Pngyu>/pngquant
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/pngquant_bin/win/pngquant.exe
            $<TARGET_FILE_DIR:Pngyu>/pngquant/pngquant.exe
    )
else()
    qt_add_executable(Pngyu
        ${PROJECT_SOURCES}
        ${PROJECT_RESOURCES}
    )
endif()

# Link Qt libraries
target_link_libraries(Pngyu PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Installation
if(APPLE)
    install(TARGETS Pngyu
        BUNDLE DESTINATION .
        RUNTIME DESTINATION bin
    )
elseif(WIN32)
    install(TARGETS Pngyu
        RUNTIME DESTINATION bin
    )
else()
    install(TARGETS Pngyu
        RUNTIME DESTINATION bin
    )
endif()
