cmake_minimum_required(VERSION 3.20)
project(hdmi-capture-viewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Encontrar Qt6
find_package(Qt6 COMPONENTS 
    Core 
    Gui 
    Widgets 
    Multimedia 
    OpenGL 
    OpenGLWidgets 
    REQUIRED)

# Encontrar FFmpeg
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
    libavcodec
    libavformat
    libavutil
    libswscale
)

# Encontrar V4L2
pkg_check_modules(V4L2 REQUIRED IMPORTED_TARGET libv4l2)

# Archivos fuente
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/devicedetector.cpp
    src/videocapture.cpp
    src/videowidget.cpp
)

set(HEADERS
    src/mainwindow.h
    src/devicedetector.h
    src/videocapture.h
    src/videowidget.h
)

# Crear ejecutable
add_executable(hdmi-capture-viewer ${SOURCES} ${HEADERS})

# Enlazar librerías
target_link_libraries(hdmi-capture-viewer
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    PkgConfig::FFMPEG
    PkgConfig::V4L2
)

# Incluir directorios
target_include_directories(hdmi-capture-viewer PRIVATE src/)
