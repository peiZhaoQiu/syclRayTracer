cmake_minimum_required(VERSION 3.22.1)


# option(ENABLE_SYCL "Enable sycl envirnment" OFF)
# option(ENABLE_GPGPU "Enable GPGPU-based tests" ON)
option(ENABLE_DEBUG "Enable Valgrind-based tests" OFF)
# if(ENABLE_SYCL)
# set(CMAKE_C_COMPILER /opt/intel/oneapi/compiler/latest/bin/icx)
# set(CMAKE_CXX_COMPILER /opt/intel/oneapi/compiler/latest/bin/icpx)
# endif()

project(Simulation VERSION 1.0.0 LANGUAGES CXX)





if(ENABLE_DEBUG)
   MESSAGE(STATUS "DEBUG FLAG IS ON")
   ADD_COMPILE_OPTIONS(-g -O0 -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined -fno-sanitize-recover=all  -static-libasan)
endif()

GET_FILENAME_COMPONENT(PARENT_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)

if(ENABLE_DEBUG)
   ADD_SUBDIRECTORY(${PARENT_DIR}/external/tinyobjloader ${CMAKE_SOURCE_DIR}/debugBuild/external/tinyobjloader)
else()
   ADD_SUBDIRECTORY(${PARENT_DIR}/external/tinyobjloader ${CMAKE_SOURCE_DIR}/build/external/tinyobjloader)
endif()




ADD_EXECUTABLE(${PROJECT_NAME} ${CMAKE_SOURCE_DIR} simulation.cpp) #external/tinyobjloader/tiny_obj_loader.cc)


# IF(ENABLE_SYCL)
# TARGET_COMPILE_DEFINITIONS( ${PROJECT_NAME} PRIVATE ENABLE_SYCL=${ENABLE_SYCL})
# IF(ENABLE_GPGPU)
# TARGET_COMPILE_DEFINITIONS( ${PROJECT_NAME} PRIVATE ENABLE_GPGPU=${ENABLE_GPGPU})
# ENDIF()

# 
# TARGET_COMPILE_OPTIONS(${PROJECT_NAME} PUBLIC ${SYCL_FLAGS})
# message(STATUS "SYCL FLAG IS: ${SYCL_FLAGS}")
# message(STATUS "ENABLE_GPGPU FLAG IS: ${ENABLE_GPGPU}")
# ADD_SYCL_TO_TARGET(TARGET ${PROJECT_NAME} SOURCES ${CMAKE_SOURCE_DIR} Test.cpp)
# else()


if(ENABLE_DEBUG)
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/external/tinyobjloader)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC tinyobjloader -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined -fno-sanitize-recover=all  -static-libasan)
else()
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/external/tinyobjloader)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC tinyobjloader)
endif()