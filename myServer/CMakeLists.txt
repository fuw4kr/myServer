cmake_minimum_required(VERSION 3.16)
project(myServerApp LANGUAGES C CXX)

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
    message(FATAL_ERROR " PostgreSQL (libpq) not found! Install via vcpkg: vcpkg install libpq:x64-windows")
endif()

add_executable(${PROJECT_NAME}
    myServer.cpp
    myServer.h
    src/filters/AuthFilter.cpp
    src/filters/AuthFilter.h
    src/controllers/AuthController.h
    src/controllers/AuthController.cpp
    src/controllers/TableControllerBase.h
    src/controllers/TableControllerBase.cpp
    src/controllers/PersonController.h
    src/controllers/PersonController.cpp
    src/controllers/CameraController.h
    src/controllers/CameraController.cpp
    src/controllers/EventController.h
    src/controllers/EventController.cpp
    src/controllers/AlertController.h
    src/controllers/AlertController.cpp
    src/controllers/SystemLogController.h
    src/controllers/SystemLogController.cpp
    src/controllers/EmbeddingController.h
    src/controllers/EmbeddingController.cpp
    src/controllers/AllController.h
    src/controllers/AllController.cpp
    src/controllers/RootController.h
    src/controllers/RootController.cpp
    src/repositories/TableRepository.h
    src/repositories/TableRepository.cpp
    src/repositories/UserRepository.h
    src/repositories/UserRepository.cpp
    src/security/PasswordHasher.cpp
    src/security/PasswordHasher.h
    src/security/bcrypt/bcrypt.c
    src/security/bcrypt/bcrypt_wrapper.h
    src/security/bcrypt/crypt_blowfish/crypt_blowfish.c
    src/security/bcrypt/crypt_blowfish/crypt_blowfish.h
    src/security/bcrypt/crypt_blowfish/crypt_gensalt.c
    src/security/bcrypt/crypt_blowfish/crypt_gensalt.h
    src/security/bcrypt/crypt_blowfish/ow-crypt.h
    src/security/bcrypt/crypt_blowfish/wrapper.c)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/security
        ${CMAKE_CURRENT_SOURCE_DIR}/src/security/bcrypt
        ${CMAKE_CURRENT_SOURCE_DIR}/src/security/bcrypt/crypt_blowfish
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Drogon::Drogon
        PostgreSQL::PostgreSQL
)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE Bcrypt)
endif ()

message(STATUS " Drogon version: ${DROGON_VERSION}")
message(STATUS " PostgreSQL include dir: ${PostgreSQL_INCLUDE_DIRS}")
message(STATUS " PostgreSQL library: ${PostgreSQL_LIBRARIES}")
