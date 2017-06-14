cmake_minimum_required(VERSION 2.8)

project(unit_tests)

find_package(CxxTest REQUIRED)
if(NOT CXXTEST_FOUND)
    message(SEND_ERROR "Failed to find CxxTest")
    return()
endif()
# OS X has some problems
if(APPLE)
    set(CXXTEST_TESTGEN_INTERPRETER bash)
endif()

# Continue if success
include_directories($(CXXTEST_INCLUDE_DIR))
#enable_testing()

# C++ file list
file(GLOB UNITS_SRC "*.hpp") 
set(TEST_SOURCE_LIST ${UNITS_SRC})
#message(STATUS ${TEST_SOURCE_LIST})

#CXXTEST_ADD_TEST(final_test test_starter.cpp ${CMAKE_CURRENT_SOURCE_DIR}/final_test.hpp)
##[[
set(i 0)
foreach(test_file ${TEST_SOURCE_LIST})
    get_filename_component(test_name ${test_file} NAME_WE)
    CXXTEST_ADD_TEST(unit_test_${i}_${test_name} test_starter_${i}.cpp ${test_file})
    target_link_libraries(unit_test_${i}_${test_name} cppexp)
    MATH(EXPR i "${i}+1")
endforeach(test_file)

#]]
