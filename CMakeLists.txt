file(GLOB VULKAN_ENGINE_SOURCE1 source/Utils/*.cpp)
file(GLOB VULKAN_ENGINE_SOURCE2 source/Platforms/Android/*.cpp)
file(GLOB VULKAN_ENGINE_SOURCE3 source/Rendering/*.cpp)
file(GLOB VULKAN_ENGINE_SOURCE4 source/*.cpp)
file(GLOB VULKAN_ENGINE_ANDROID_SOURCE android/utils/*.cpp)

set(INCLUDE_DATA_DIR ${INCLUDE_DATA_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/include") # ${CMAKE_CURRENT_SOURCE_DIR}/include/Rendering ${CMAKE_CURRENT_SOURCE_DIR}/include/Platforms ${CMAKE_CURRENT_SOURCE_DIR}/include/Utils")

if (ANDROID)
    set(ANDROID_INCLUDE_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android/utils")
endif()      

set(SAMPLES_DATA_DIR ${SAMPLES_DATA_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/data")
set(GLSLANG_SPIRV_INCLUDE_DIR ${SHADERC_SRC}/third_party/glslang)

# get_filename_component(GLMINC_PREFIX
#                         "${CMAKE_CURRENT_SOURCE_DIR}/third-party/glm"
#                         ABSOLUTE)

if(SDK_INCLUDE_PATH)
    include_directories( ${SAMPLES_DATA_DIR} ${GLMINC_PREFIX} ${GLSLANG_SPIRV_INCLUDE_DIR} ${SDK_INCLUDE_PATH} )
else()
    include_directories( ${SAMPLES_DATA_DIR} ${GLMINC_PREFIX} ${GLSLANG_SPIRV_INCLUDE_DIR})
endif()

# Import shaderc library in NDK
add_library(shaderc_lib STATIC IMPORTED)
set_target_properties(shaderc_lib PROPERTIES IMPORTED_LOCATION
                      ${SHADERC_SRC}/libs/${ANDROID_STL}/${ANDROID_ABI}/libshaderc.a)

add_library(lib_gvr SHARED IMPORTED)
set_target_properties(lib_gvr PROPERTIES IMPORTED_LOCATION 
                      ${CMAKE_CURRENT_SOURCE_DIR}/android/libraries/jni/${ANDROID_ABI}/libgvr.so)

if (ANDROID)
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -Werror -DVK_USE_PLATFORM_ANDROID_KHR ${GVR_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -Werror -D__ANDROID__ -DVK_USE_PLATFORM_ANDROID_KHR ${GVR_FLAGS}")
elseif(WIN32)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CRT_SECURE_NO_WARNINGS -D_USE_MATH_DEFINES")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS -D_USE_MATH_DEFINES -DVK_USE_PLATFORM_WIN32_KHR")

    # If MSVC, disable some signed/unsigned mismatch warnings.
    if (MSVC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4267")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4267")
    endif()
else()
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare")
endif()

add_library(${VULKAN_ENGINE_NAME} STATIC ${VULKAN_ENGINE_SOURCE1} ${VULKAN_ENGINE_SOURCE2} ${VULKAN_ENGINE_SOURCE3} 
                                        ${VULKAN_ENGINE_SOURCE4} ${VULKAN_ENGINE_ANDROID_SOURCE})

if (ANDROID)
    add_library(native_app_glue STATIC
               ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

    target_include_directories(${VULKAN_ENGINE_NAME} PRIVATE
                                ${INCLUDE_DATA_DIR}
                                ${ANDROID_INCLUDE_DATA_DIR} 
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

