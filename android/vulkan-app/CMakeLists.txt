# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.4.1)

# Configure project source.
file(GLOB MODULE_SOURCE src/main/cpp/classes/*.cpp)

# Configure all necesarry libraties and includes.
get_filename_component(SHADERC_SRC ${ANDROID_NDK}/sources/third_party/shaderc ABSOLUTE)

set(UTILS_NAME utils_lib)
set(GLSLANG_SPIRV_INCLUDE_DIR ${SHADERC_SRC}/third_party/glslang)

set(INCLUDE_DIR ${INCLUDE_DIR} "${CMAKE_SOURCE_DIR}/src/main/cpp/classes")

include_directories(${GLSLANG_SPIRV_INCLUDE_DIR})

get_filename_component(GLMINC_PREFIX
                        "${CMAKE_SOURCE_DIR}/../../third-party/glm"
                        ABSOLUTE)

# ===========================
# VULKAN ENGINE CONFIGURATION

set(VULKAN_ENGINE_DIR ../..)

file(GLOB VULKAN_ENGINE_UTILS_SOURCE ${VULKAN_ENGINE_DIR}/source/Utils/*.cpp)
file(GLOB VULKAN_ENGINE_PLATFORM_SOURCE ${VULKAN_ENGINE_DIR}/source/Platforms/Android/*.cpp)
file(GLOB VULKAN_ENGINE_RENDERING_SOURCE ${VULKAN_ENGINE_DIR}/source/Rendering/*.cpp)
file(GLOB VULKAN_ENGINE_SOURCE ${VULKAN_ENGINE_DIR}/source/*.cpp)
file(GLOB VULKAN_ENGINE_TEST_SOURCE ${VULKAN_ENGINE_DIR}/tests/*.cpp)
file(GLOB VULKAN_ENGINE_ANDROID_SOURCE ${VULKAN_ENGINE_DIR}/android/utils/*.cpp)

set(VULKAN_ENGINE_INCLUDE_DIR "${VULKAN_ENGINE_DIR}/include")
set(VULKAN_ENGINE_TEST_DIR "${VULKAN_ENGINE_DIR}/tests")

if (ANDROID)
    set(ANDROID_INCLUDE_DIR "${VULKAN_ENGINE_DIR}/android/utils")
endif()

set(SAMPLES_DATA_DIR ${SAMPLES_DATA_DIR} "${VULKAN_ENGINE_DIR}/data")
set(GLSLANG_SPIRV_INCLUDE_DIR ${SHADERC_SRC}/third_party/glslang)

if(SDK_INCLUDE_PATH)
    include_directories( ${SAMPLES_DATA_DIR} ${GLMINC_PREFIX} ${GLSLANG_SPIRV_INCLUDE_DIR} ${SDK_INCLUDE_PATH} )
else()
    include_directories( ${SAMPLES_DATA_DIR} ${GLMINC_PREFIX} ${GLSLANG_SPIRV_INCLUDE_DIR})
endif()

# ===========================

# Import shaderc, gvr and vulkan layer libraries in NDK
add_library(shaderc_lib STATIC IMPORTED)
set_target_properties(shaderc_lib PROPERTIES IMPORTED_LOCATION
                      ${SHADERC_SRC}/libs/${ANDROID_STL}/${ANDROID_ABI}/libshaderc.a)

add_library(lib_gvr SHARED IMPORTED)
set_target_properties(lib_gvr PROPERTIES IMPORTED_LOCATION
                      ${CMAKE_SOURCE_DIR}/../libraries/jni/${ANDROID_ABI}/libgvr.so)

# Build application's shared library
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -Werror -D VK_USE_PLATFORM_ANDROID_KHR -D__ANDROID__ -DVULKAN_ENABLE_DEBUG ${GVR_FLAGS}")

# Force export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

add_library(VulkanAppActivity SHARED
            ${CMAKE_SOURCE_DIR}/src/main/cpp/${MODULE_NAME}.cpp
            ${MODULE_SOURCE}
            ${CMAKE_SOURCE_DIR}/../vulkan_wrapper/vulkan_wrapper.cpp
            ${VULKAN_ENGINE_UTILS_SOURCE} 
            ${VULKAN_ENGINE_PLATFORM_SOURCE} 
            ${VULKAN_ENGINE_RENDERING_SOURCE}
            ${VULKAN_ENGINE_SOURCE}
            ${VULKAN_ENGINE_TEST_SOURCE} 
            ${VULKAN_ENGINE_ANDROID_SOURCE}
)

add_library(native_app_glue STATIC
            ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

target_include_directories(VulkanAppActivity PRIVATE
                            ${INCLUDE_DIR}
                            ${VULKAN_ENGINE_INCLUDE_DIR}
                            ${VULKAN_ENGINE_TEST_DIR}
                            ${ANDROID_INCLUDE_DIR}
                            ${SAMPLES_DATA_DIR}
                            ${GLMINC_PREFIX}
                            ${SHADERC_SRC}/third_party/spirv-tools/include
                            ${SHADERC_SRC}/third_party/spirv-tools/include/spirv-tools
                            ${CMAKE_SOURCE_DIR}/../vulkan_wrapper
                            ${ANDROID_NDK}/sources/android/native_app_glue
                            ${CMAKE_SOURCE_DIR}/../utils
                            ${ANDROID_NDK}/sources/third_party/shaderc/include
                            ${CMAKE_SOURCE_DIR}/../libraries/headers
                            ${CMAKE_SOURCE_DIR}/../libraries/headers/vr/gvr/capi/include
)

target_link_libraries(VulkanAppActivity
                        android
                        log
                        native_app_glue
                        shaderc_lib
                        lib_gvr
)
