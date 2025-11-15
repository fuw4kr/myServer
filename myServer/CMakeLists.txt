cmake_minimum_required(VERSION 3.16)
project(myServerApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Using default vcpkg toolchain path...")
    set(CMAKE_TOOLCHAIN_FILE "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

find_package(Drogon CONFIG REQUIRED)

find_package(PostgreSQL REQUIRED)
if(NOT PostgreSQL_FOUND)
    message(FATAL_ERROR " PostgreSQL (libpq) не знайдено! Встанови через vcpkg: vcpkg install libpq:x64-windows")
endif()

add_executable(${PROJECT_NAME}
    myServer.cpp
    myServer.h
    AuthFilter.cpp
    AuthFilter.h
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Drogon::Drogon
        PostgreSQL::PostgreSQL
)

message(STATUS " Drogon version: ${DROGON_VERSION}")
message(STATUS " PostgreSQL include dir: ${PostgreSQL_INCLUDE_DIRS}")
message(STATUS " PostgreSQL library: ${PostgreSQL_LIBRARIES}")

