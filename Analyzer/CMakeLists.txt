cmake_minimum_required(VERSION 3.10)

project(DOPL-Ass4 VERSION 1.0 LANGUAGES CXX)

add_executable(DOPL-Ass4
	src/main.cpp
)

target_include_directories(
	DOPL-Ass4
	PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(
    DOPL-Ass4
    PRIVATE
    cxx_std_17
)