project(mutex_test)

set(CMAKE_CXX_STANDARD 17)
add_definitions(-Wall)

# execute_process(COMMAND ${CMAKE_CURRENT_LIST_DIR}/scripts/proto_build.sh)

### --------------------------------------------------------------------
### Source code
### --------------------------------------------------------------------
file(GLOB_RECURSE H_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")
file(GLOB_RECURSE C_FILES "${CMAKE_CURRENT_LIST_DIR}/*.c")
file(GLOB_RECURSE CC_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cc")
file(GLOB_RECURSE CPP_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

set(COMPILE_FILES ${H_FILES} ${C_FILES} ${CC_FILES} ${CPP_FILES})


### --------------------------------------------------------------------
### Generate exe
### --------------------------------------------------------------------
# add_library(${PROJECT_NAME} SHARED ${COMPILE_FILES})
add_executable(mutex_test ${COMPILE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/src
        # ${CMAKE_CURRENT_LIST_DIR}/src/components
        # ${CMAKE_CURRENT_LIST_DIR}/src/components/protobuf
        # $ENV{ROS_PATH}/include/
        ${CMAKE_SOURCE_DIR}/lib/3rd/include
)

# if(${BUILD_PLATFORM} STREQUAL "arm")
#         target_compile_definitions(${PROJECT_NAME} PRIVATE -DSPDLOG_FMT_EXTERNAL)
# endif()

target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib/ ${CMAKE_SOURCE_DIR}/lib/3rd/lib  ${CMAKE_SOURCE_DIR}/lib/product/lib)

target_link_libraries(${PROJECT_NAME})
# SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES PREFIX "")

install(TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/lib/product/
        LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/lib/product/
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/lib/product/
        )


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${STRIP_CMD} $<TARGET_FILE:${PROJECT_NAME}>
        COMMAND ${CMAKE_COMMAND} --build . --target install
        COMMENT "Installing and stripping ${PROJECT_NAME}"
        )