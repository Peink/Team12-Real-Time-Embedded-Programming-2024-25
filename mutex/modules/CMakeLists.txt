file(GLOB dirs RELATIVE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/*)

foreach (subdir ${dirs})
    if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/${subdir}/CMakeLists.txt)
        message(STATUS "Add submodule <${subdir}>")
        add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${subdir})
    endif ()
endforeach ()