cmake_minimum_required(VERSION 3.22.1)


option(ENABLE_SYCL "Enable sycl envirnment" OFF)
option(ENABLE_VALGRIND "Enable Valgrind-based tests" OFF)
option(ENABLE_GPGPU "Enable GPGPU-based tests" ON)

if(ENABLE_SYCL)
set(CMAKE_C_COMPILER /opt/intel/oneapi/compiler/latest/bin/icx)
set(CMAKE_CXX_COMPILER /opt/intel/oneapi/compiler/latest/bin/icpx)
endif()

project(HELLOEMBREE)

option(ENABLE_DEBUG "Enable Valgrind-based tests" OFF)
if(ENABLE_DEBUG)
   message(STATUS "DEBUG FLAG IS ON")
#    set(CMAKE_EXE_LINKER_FLAGS "-g -O0")
   add_compile_options(-g -O0)
endif()

if(ENABLE_SYCL)
   message(STATUS "SYCL FLAG IS ON")
   # add_compile_options(-fsycl)
   FIND_PACKAGE(IntelSYCL REQUIRED)
   if(ENABLE_GPGPU)
      set(SYCL_FLAGS "-fsycl"
         "-fsycl-targets=nvptx64-nvidia-cuda"
         "-fsycl-unnamed-lambda"
         "-Wno-linker-warnings")
   else()
   add_compile_options(-Wall -g -O1 -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined -fno-sanitize-recover=all  -static-libsan)     
      set(SYCL_FLAGS "-fsycl"
         "-fsycl-targets=spir64_x86_64"
         "-fsycl-unnamed-lambda"
         "-Wno-linker-warnings")
   endif()
endif()




#set(embree_DIR ${CMAKE_SOURCE_DIR}/external/embree-4.2.0/lib/cmake/embree-4.2.0)

ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/external/tinyobjloader)
ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/src)

#target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/external/tinyobjloader)
#make sure to include the model directory so that obj files can be loaded   
#include_directories(${CMAKE_SOURCE_DIR}/Model)
#message(${CMAKE_SOURCE_DIR}/external/tinyobjloader)
#FIND_PACKAGE(embree 4 REQUIRED)
#FIND_PACKAGE(glm REQUIRED)

#include all the files in the src directory
#file(GLOB SOURCES "src/*.cpp")

ADD_EXECUTABLE(${PROJECT_NAME} ${CMAKE_SOURCE_DIR} Test.cpp) #external/tinyobjloader/tiny_obj_loader.cc)

# TARGET_LINK_LIBRARIES(${PROJECT_NAME} embree)
# TARGET_LINK_LIBRARIES(${PROJECT_NAME} glm::glm)

IF(ENABLE_SYCL)
TARGET_COMPILE_DEFINITIONS( ${PROJECT_NAME} PRIVATE ENABLE_SYCL=${ENABLE_SYCL})
IF(ENABLE_GPGPU)
TARGET_COMPILE_DEFINITIONS( ${PROJECT_NAME} PRIVATE ENABLE_GPGPU=${ENABLE_GPGPU})
ENDIF()
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/external/tinyobjloader /opt/intel/oneapi/compiler/latest/linux/include )
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC tinyobjloader rayTracelib sycl ${SYCL_FLAGS} -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined -fno-sanitize-recover=all  -static-libsan)
TARGET_COMPILE_OPTIONS(${PROJECT_NAME} PUBLIC ${SYCL_FLAGS})
message(STATUS "SYCL FLAG IS: ${SYCL_FLAGS}")
message(STATUS "ENABLE_GPGPU FLAG IS: ${ENABLE_GPGPU}")
ADD_SYCL_TO_TARGET(TARGET ${PROJECT_NAME} SOURCES ${CMAKE_SOURCE_DIR} Test.cpp)
else()
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/external/tinyobjloader)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC tinyobjloader rayTracelib)
endif()