# ########## Project setup ##########
PROJECT(caesar)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set(CMAKE_CXX_STANDARD 17)

# Find Macros
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

INCLUDE(CMakeDependentOption)
INCLUDE(CheckCCompilerFlag)
INCLUDE(CheckCSourceCompiles)
INCLUDE(CheckIncludeFile)
INCLUDE(TestBigEndian)
ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/src/sf2cute-0.2/)

INCLUDE_DIRECTORIES(BEFORE "${CMAKE_SOURCE_DIR}/src/sf2cute-0.2/include")

FILE(GLOB SOURCES "src/*.cpp" "src/libsmfc/*.c")

ADD_EXECUTABLE(caesar ${SOURCES})

TARGET_LINK_LIBRARIES(caesar sf2cute)

if(MSVC)
  TARGET_COMPILE_OPTIONS(caesar PRIVATE /W4 /WX- -D_CRT_SECURE_NO_WARNINGS)
else(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  TARGET_COMPILE_OPTIONS(caesar PRIVATE -Wall -Wconversion -Wsign-conversion -Wno-long-long -pedantic)
endif()
