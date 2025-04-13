project(mutex)

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
add_executable(mutex ${COMPILE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_SOURCE_DIR}/include
)

if(${BUILD_PLATFORM} STREQUAL "arm")
        target_compile_definitions(${PROJECT_NAME} PRIVATE -DSPDLOG_FMT_EXTERNAL)
        # target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif()

target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib/3rd/${BUILD_PLATFORM}  ${CMAKE_SOURCE_DIR}/lib/product/${BUILD_PLATFORM})

target_link_libraries(${PROJECT_NAME} rt dl pthread gpiod echo keyBoard serial finger)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/lib/product/${BUILD_PLATFORM}/bin
        )


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        # COMMAND ${STRIP_CMD} $<TARGET_FILE:${PROJECT_NAME}>
        COMMAND ${CMAKE_COMMAND} --build . --target install
        COMMENT "Installing and stripping ${PROJECT_NAME}"
        )