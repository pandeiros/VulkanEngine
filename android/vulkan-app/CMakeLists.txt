# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Build all necesarry libraties.
get_filename_component(SHADERC_SRC
                        ${ANDROID_NDK}/sources/third_party/shaderc
                        ABSOLUTE)

set(UTILS_NAME utils_lib)
set(VULKAN_ENGINE_NAME VulkanEngine)
set(GLSLANG_SPIRV_INCLUDE_DIR ${SHADERC_SRC}/third_party/glslang)

# get_filename_component(PROJECT_SOURCE_DIR
#                         "${CMAKE_SOURCE_DIR}/../.."
#                         ABSOLUTE)

get_filename_component(GLMINC_PREFIX
                        "${CMAKE_SOURCE_DIR}/../../third-party/glm"
                        ABSOLUTE)

# add_subdirectory(${CMAKE_SOURCE_DIR}/../../utils ${CMAKE_BINARY_DIR}/utils)
add_subdirectory(${CMAKE_SOURCE_DIR}/../../ ${CMAKE_BINARY_DIR}/vulkan-engine)

# # Import shaderc library in NDK
# add_library(shaderc_lib STATIC IMPORTED)
# set_target_properties(shaderc_lib PROPERTIES IMPORTED_LOCATION
#                       ${SHADERC_SRC}/libs/${ANDROID_STL}/${ANDROID_ABI}/libshaderc.a)

# add_library(lib_gvr SHARED IMPORTED)
# set_target_properties(lib_gvr PROPERTIES IMPORTED_LOCATION 
#                       ${CMAKE_CURRENT_SOURCE_DIR}/../libraries/jni/${ANDROID_ABI}/libgvr.so)

# Build application's shared library
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -Werror -D VK_USE_PLATFORM_ANDROID_KHR -D__ANDROID__")

# Force export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

add_library(VulkanAppActivity SHARED
            ${CMAKE_SOURCE_DIR}/src/main/cpp/${MODULE_NAME}.cpp
            ${CMAKE_SOURCE_DIR}/../vulkan_wrapper/vulkan_wrapper.cpp
            ${CMAKE_SOURCE_DIR}/../utils/AndroidUtils.cpp)

# add_custom_command(TARGET native-lib POST_BUILD
# COMMAND ${CMAKE_COMMAND} -E copy
# ${CMAKE_CURRENT_SOURCE_DIR}/libs/${ANDROID_ABI}/libSomething.so
# ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libSomething.so
# )

# include_directories( ${CMAKE_SOURCE_DIR}/../utils ${CMAKE_SOURCE_DIR}/../../include ${CMAKE_SOURCE_DIR}/../vulkan_wrapper)

target_include_directories(VulkanAppActivity PRIVATE
                            ${CMAKE_SOURCE_DIR}/../../include
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
                        # ${UTILS_NAME}
                        
                        # shaderc_lib
                        # lib_gvr
)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

# add_library( # Sets the name of the library.
#              native-lib

#              # Sets the library as a shared library.
#              SHARED

#              # Provides a relative path to your source file(s).
#              src/main/cpp/native-lib.cpp )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

# find_library( # Sets the name of the path variable.
#               log-lib

#               # Specifies the name of the NDK library that
#               # you want CMake to locate.
#               log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

# target_link_libraries( # Specifies the target library.
#                        native-lib

#                        # Links the target library to the log library
#                        # included in the NDK.
#                        ${log-lib} )