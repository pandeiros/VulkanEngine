file(GLOB VULKAN_ENGINE_UTILS_SOURCE source/Utils/*.cpp)
file(GLOB VULKAN_ENGINE_PLATFORM_SOURCE source/Platforms/Android/*.cpp)
file(GLOB VULKAN_ENGINE_RENDERING_SOURCE source/Rendering/*.cpp)
file(GLOB VULKAN_ENGINE_SOURCE source/*.cpp)
file(GLOB VULKAN_ENGINE_TEST_SOURCE tests/*.cpp)
file(GLOB VULKAN_ENGINE_ANDROID_SOURCE android/utils/*.cpp)

set(INCLUDE_DIR ${INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")

set(ANDROID_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android/utils")

set(SAMPLES_DATA_DIR ${SAMPLES_DATA_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/data")
set(GLSLANG_SPIRV_INCLUDE_DIR ${SHADERC_SRC}/third_party/glslang)

if(SDK_INCLUDE_PATH)
    include_directories( ${SAMPLES_DATA_DIR} ${GLMINC_PREFIX} ${GLSLANG_SPIRV_INCLUDE_DIR} ${SDK_INCLUDE_PATH} )
else()
    include_directories( ${SAMPLES_DATA_DIR} ${GLMINC_PREFIX} ${GLSLANG_SPIRV_INCLUDE_DIR})
endif()

# Import shaderc, gvr and vulkan layer libraries in NDK
add_library(shaderc_lib STATIC IMPORTED)
set_target_properties(shaderc_lib PROPERTIES IMPORTED_LOCATION
                      ${SHADERC_SRC}/libs/${ANDROID_STL}/${ANDROID_ABI}/libshaderc.a)

add_library(lib_gvr SHARED IMPORTED)
set_target_properties(lib_gvr PROPERTIES IMPORTED_LOCATION
                      ${CMAKE_CURRENT_SOURCE_DIR}/android/libraries/jni/${ANDROID_ABI}/libgvr.so)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -Werror -D__ANDROID__ -DVK_USE_PLATFORM_ANDROID_KHR -DVULKAN_ENABLE_DEBUG -DVULKAN_VR_MODE ${GVR_FLAGS}")

add_library(${VULKAN_ENGINE_NAME} STATIC ${VULKAN_ENGINE_UTILS_SOURCE} ${VULKAN_ENGINE_PLATFORM_SOURCE} ${VULKAN_ENGINE_RENDERING_SOURCE}
                                        ${VULKAN_ENGINE_SOURCE} ${VULKAN_ENGINE_ANDROID_SOURCE})

if (ANDROID)
    add_library(native_app_glue STATIC
               ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

    target_include_directories(${VULKAN_ENGINE_NAME} PRIVATE
                                ${INCLUDE_DIR}
                                ${ANDROID_INCLUDE_DIR}
                                ${ANDROID_NDK}/sources/android/native_app_glue
                                ${CMAKE_CURRENT_SOURCE_DIR}/android/vulkan_wrapper
                                ${CMAKE_CURRENT_SOURCE_DIR}/data
                                ${GLMINC_PREFIX}
                                ${ANDROID_NDK}/sources/third_party/shaderc/include
                                ${CMAKE_CURRENT_SOURCE_DIR}/android/libraries/headers
                                ${CMAKE_CURRENT_SOURCE_DIR}/android/libraries/headers/vr/gvr/capi/include
                                ${SHADERC_SRC}/third_party/spirv-tools/include
                                ${SHADERC_SRC}/third_party/spirv-tools/include/spirv-tools
    )

    target_link_libraries(${VULKAN_ENGINE_NAME}
                            android
                            log
                            native_app_glue
                            shaderc_lib
                            lib_gvr
    )

endif()
