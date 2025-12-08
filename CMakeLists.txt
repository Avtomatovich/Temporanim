cmake_minimum_required(VERSION 3.16)

# Sets project name
project(temporanim LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Sets C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specifies required Qt components
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Gui)
find_package(Qt6 REQUIRED COMPONENTS OpenGL)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
find_package(Qt6 REQUIRED COMPONENTS Xml)

# Sets path for finding ASSIMP binaries
if (APPLE)
    set(CMAKE_PREFIX_PATH /Users/${ENV:USER}/assimp)
elseif(WIN32)
    set(CMAKE_PREFIX_PATH C:/assimp)
endif()

# Specifies required ASSIMP binaries
find_package(assimp REQUIRED)

# Allows you to include files from within those directories, without prefixing their filepaths
include_directories(src)

# Specifies .cpp and .h files to be passed to the compiler
add_executable(${PROJECT_NAME}
    src/main.cpp

    src/realtime.cpp
    src/mainwindow.cpp
    src/settings.cpp
    src/utils/scenefilereader.cpp
    src/utils/sceneparser.cpp

    src/mainwindow.h
    src/realtime.h
    src/settings.h
    src/utils/scenedata.h
    src/utils/scenefilereader.h
    src/utils/sceneparser.h
    src/utils/shaderloader.h
    src/utils/aspectratiowidget/aspectratiowidget.hpp

    src/primitive/cube.h src/primitive/cube.cpp
    src/primitive/sphere.h src/primitive/sphere.cpp
    src/primitive/cone.h src/primitive/cone.cpp
    src/primitive/cylinder.h src/primitive/cylinder.cpp
    src/primitive/primitive.h src/primitive/primitive.cpp

    src/camera/camera.h src/camera/camera.cpp

    src/scene/scene.h src/scene/scene.cpp

    src/geometry/geometry.h src/geometry/geometry.cpp
    src/geometry/mesh.h src/geometry/mesh.cpp
    src/geometry/model.h src/geometry/model.cpp

    src/texture/texture.h src/texture/texture.cpp

    src/utils/debug.h
    src/utils/uniloader.h src/utils/uniloader.cpp
    src/utils/transform.h src/utils/transform.cpp
    src/utils/modelparser.h src/utils/modelparser.cpp

    src/animation/animator.h src/animation/animator.cpp

    src/physics/rigidbody.h src/physics/rigidbody.cpp
)

# GLM: this creates its library and allows you to `#include "glm/..."`
add_subdirectory(glm)

# GLEW: this creates its library and allows you to `#include "GL/glew.h"`
add_library(StaticGLEW STATIC glew/src/glew.c)
include_directories(${PROJECT_NAME} PRIVATE glew/include)

# Specifies libraries to be linked (Qt components, glew, etc)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Core
    Qt::Gui
    Qt::OpenGL
    Qt::OpenGLWidgets
    Qt::Xml
    StaticGLEW
    assimp::assimp
)

# Specifies other files
qt6_add_resources(${PROJECT_NAME} "Resources"
    PREFIX
        "/"
    FILES
        resources/shaders/default.frag
        resources/shaders/default.vert
)

# GLEW: this provides support for Windows (including 64-bit)
if (WIN32)
  add_compile_definitions(GLEW_STATIC)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    opengl32
    glu32
  )
endif()

# Set this flag to silence warnings on Windows
if (MSVC OR MSYS OR MINGW)
  set(CMAKE_CXX_FLAGS "-Wno-volatile")
endif()
# Set this flag to silence warnings on MacOS
if (APPLE)
  set(CMAKE_CXX_FLAGS "-Wno-deprecated-volatile")
endif()
