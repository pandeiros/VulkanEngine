# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.4.1)

# Configure project source.
file(GLOB MODULE_SOURCE src/main/cpp/classes/*.cpp)

# Configure all necesarry libraties and includes.
get_filename_component(SHADERC_SRC ${ANDROID_NDK}/sources/third_party/shaderc ABSOLUTE)

set(UTILS_NAME utils_lib)
set(VULKAN_ENGINE_NAME VulkanEngine)
set(GLSLANG_SPIRV_INCLUDE_DIR ${SHADERC_SRC}/third_party/glslang)

set(INCLUDE_DIR ${INCLUDE_DIR} "${CMAKE_SOURCE_DIR}/src/main/cpp/classes")

include_directories(${GLSLANG_SPIRV_INCLUDE_DIR})

get_filename_component(GLMINC_PREFIX
                        "${CMAKE_SOURCE_DIR}/../../third-party/glm"
                        ABSOLUTE)

add_subdirectory(${CMAKE_SOURCE_DIR}/../../ ${CMAKE_BINARY_DIR}/vulkan-engine)

# Build application's shared library
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -Werror -D VK_USE_PLATFORM_ANDROID_KHR -D__ANDROID__ -DVULKAN_ENABLE_DEBUG -DVULKAN_VR_MODE -DVULKAN_FIX_ANDROID_TRANSPOSE")

# Force export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

add_library(VulkanAppActivity SHARED
            ${CMAKE_SOURCE_DIR}/src/main/cpp/${MODULE_NAME}.cpp
            ${MODULE_SOURCE}
            ${CMAKE_SOURCE_DIR}/../vulkan_wrapper/vulkan_wrapper.cpp
)

target_include_directories(VulkanAppActivity PRIVATE
                            ${INCLUDE_DIR}
                            ${CMAKE_SOURCE_DIR}/../../include
                            ${CMAKE_SOURCE_DIR}/../../tests
                            ${CMAKE_SOURCE_DIR}/../../data
                            ${GLMINC_PREFIX}
                            ${SHADERC_SRC}/third_party/spirv-tools/include
                            ${SHADERC_SRC}/third_party/spirv-tools/include/spirv-tools
                            ${CMAKE_SOURCE_DIR}/../vulkan_wrapper
                            ${ANDROID_NDK}/sources/android/native_app_glue
                            ${CMAKE_SOURCE_DIR}/../utils
                            ${ANDROID_NDK}/sources/third_party/shaderc/include
                            ${CMAKE_CURRENT_SOURCE_DIR}/../libraries/headers
                            ${CMAKE_CURRENT_SOURCE_DIR}/../libraries/headers/vr/gvr/capi/include
)

target_link_libraries(VulkanAppActivity
                        ${VULKAN_ENGINE_NAME}
                        log
                        # lib_vk_layer_core_validation
                        # lib_vk_layer_image
                        # lib_vk_layer_object_tracker
                        # lib_vk_layer_parameter_validation
                        # lib_vk_layer_swapchain
                        # lib_vk_layer_threading
                        # lib_vk_layer_unique_objects
)