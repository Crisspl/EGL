cmake_minimum_required(VERSION 3.0.2)

option(EGL_PASSTHROUGH "Build passthrough implementation (loads functions from libEGL.so and passes every call to them)" OFF)
option(EGL_USE_WAYLAND "Build passthrough functions for Wayland platform (has any meaning only if EGL_PASSTHROUGH is ON)" OFF)

if (NOT EGL_PASSTHROUGH)
    if(WIN32)
      set(EGL_PLATFORM_SOURCES
          ${CMAKE_CURRENT_LIST_DIR}/src/egl_windows.cpp ${CMAKE_CURRENT_LIST_DIR}/src/egl.c ${CMAKE_CURRENT_LIST_DIR}/src/egl_common.cpp)
    elseif(UNIX AND NOT APPLE AND NOT ANDROID)
      set(EGL_PLATFORM_SOURCES
          ${CMAKE_CURRENT_LIST_DIR}/src/egl_x11.cpp ${CMAKE_CURRENT_LIST_DIR}/src/egl.c ${CMAKE_CURRENT_LIST_DIR}/src/egl_common.cpp)
    endif()
else()
  set(EGL_PLATFORM_SOURCES
      ${CMAKE_CURRENT_LIST_DIR}/src/egl_passthrough.c)
endif()

set(EGL_SOURCES
    ${EGL_PLATFORM_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/src/egl_internal.h
    ${CMAKE_CURRENT_LIST_DIR}/include/EGL/egl.h
    ${CMAKE_CURRENT_LIST_DIR}/include/EGL/egl.h
    ${CMAKE_CURRENT_LIST_DIR}/include/EGL/eglext.h
    ${CMAKE_CURRENT_LIST_DIR}/include/EGL/eglplatform.h
    ${CMAKE_CURRENT_LIST_DIR}/include/KHR/khrplatform.h)

add_library(egl STATIC
    ${EGL_SOURCES})

target_include_directories(egl PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include)
target_compile_definitions(egl PUBLIC KHRONOS_STATIC) # PUBLIC - make it go down to every target linking egl
add_definitions(-DEGLAPI=)
option(EGL_NO_GLEW "Do not use GLEW" OFF)
if(EGL_NO_GLEW)
  add_definitions(-DEGL_NO_GLEW)
endif()
if(EGL_PASSTHROUGH AND EGL_USE_WAYLAND)
  add_definitions(-DWL_EGL_PLATFORM)
endif()
