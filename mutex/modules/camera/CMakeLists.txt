project(camera)

set(CMAKE_CXX_STANDARD 17)
add_definitions(-Wall)

set(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_BUILD_TYPE "Release")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
endif()

### --------------------------------------------------------------------
### Source code
### --------------------------------------------------------------------
file(GLOB_RECURSE H_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")
file(GLOB_RECURSE C_FILES "${CMAKE_CURRENT_LIST_DIR}/*.c")
file(GLOB_RECURSE CC_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cc")
file(GLOB_RECURSE CPP_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

set(COMPILE_FILES ${H_FILES} ${C_FILES} ${CC_FILES} ${CPP_FILES})

### --------------------------------------------------------------------
### Generate lib
### --------------------------------------------------------------------
add_library(${PROJECT_NAME} SHARED ${COMPILE_FILES})

add_definitions(-DUSE_ARM)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/src
        /usr/include/opencv4
)

target_link_directories(${PROJECT_NAME} PRIVATE)

set(LIB_COMMON zmq opencv_core opencv_imgproc opencv_imgcodecs rt pthread)

set(LIB_ARM ${LIB_COMMON})

target_link_libraries(${PROJECT_NAME} ${LIB_ARM})

install(TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/lib/product/${BUILD_PLATFORM}/
        LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/lib/product/${BUILD_PLATFORM}/
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/lib/product/
        )


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        # COMMAND ${STRIP_CMD} $<TARGET_FILE:${PROJECT_NAME}>
        COMMAND ${CMAKE_COMMAND} --build . --target install
        COMMENT "Installing and stripping ${PROJECT_NAME}"
        )