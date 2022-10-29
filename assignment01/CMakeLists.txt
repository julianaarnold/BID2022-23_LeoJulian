cmake_minimum_required(VERSION 3.0)

project(KinectAssignments CXX)

# find OpenCV
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV_LIBS: ${OpenCV_LIBS}")

# find OpenNI2

set(OpenNI_DIR "" CACHE PATH "OpenNI2 base directory. On windows, this should be C:\\Program Files\\OpenNI2.")
set(OpenNI_INCLUDE_DIR ${OpenNI_DIR}/Include)
set(OpenNI_LIB_DIR ${OpenNI_DIR}/Lib)
file(GLOB OpenNI_LIBS ${OpenNI_LIB_DIR}/*)
if(NOT OpenNI_LIBS)
    message(FATAL_ERROR "Could not find any OpenNI libs. Please check the OpenNI_DIR.")
endif()
message(STATUS "OpenNI_LIBS: ${OpenNI_LIBS}")

# framework

set(source_dir src)

set(framework_source_dir ${source_dir}/framework)
file(GLOB framework_sources ${framework_source_dir}/*.cpp)
file(GLOB framework_headers ${framework_source_dir}/*.hpp)

add_library(framework ${framework_headers} ${framework_sources})
target_include_directories(framework PUBLIC ${OpenNI_INCLUDE_DIR})
target_link_libraries(framework ${OpenCV_LIBS} ${OpenNI_LIBS})

# helper macros

macro(get_relative_files files)
    foreach(file ${${files}})
        file(RELATIVE_PATH relative_temp ${CMAKE_SOURCE_DIR} ${file})
        if(NOT files_temp)
            set(files_temp "${relative_temp}")
        else()
            set(files_temp "${files_temp};${relative_temp}")
        endif()
    endforeach()
    set(${files}_relative "${files_temp}")
    unset(files_temp)
    unset(files_relative)
endmacro()

macro(add_additional assignment_name file_type)
    set(${assignment_name}_${file_type}_relative ${${assignment_name}_${file_type}_relative} ${${assignment_name}_additional_${file_type}})
endmacro()

macro(add_files assignment_name file_type glob_pattern)
    file(GLOB ${assignment_name}_${file_type} ${${assignment_name}_source_dir}/${glob_pattern})
    get_relative_files(${assignment_name}_${file_type})
    add_additional(${assignment_name} ${file_type})
endmacro()

# assingment macro

macro(add_assignment assignment_name)
    set(${assignment_name}_source_dir ${source_dir}/${assignment_name})

    add_files(${assignment_name} sources "*.cpp")
    add_files(${assignment_name} headers "*.hpp")
    
    message(STATUS "Adding ${assignment_name}...")
    message(STATUS "  Source dir: ${${assignment_name}_source_dir}")
    message(STATUS "  Headers: ${${assignment_name}_headers_relative}")
    message(STATUS "  Sources: ${${assignment_name}_sources_relative}")

    add_executable(${assignment_name} ${${assignment_name}_headers_relative} ${${assignment_name}_sources_relative})
    target_link_libraries(${assignment_name} framework)
endmacro()

# assingnments

add_assignment("assignment00")
add_assignment("assignment01")
