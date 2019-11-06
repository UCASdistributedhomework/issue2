cmake_minimum_required(VERSION 3.5)

PROJECT (helloWord)
SET(CMAKE_CXX_FLAGS "-g  -std=c++11" )


INCLUDE_DIRECTORIES(./gen-cpp)

SET( LIB_SOURCE  
    gen-cpp/Locker.cpp
    gen-cpp/lock_types.cpp
    gen-cpp/lock_constants.cpp
    )

SET( SERVER_FILES LockServer.cpp)
ADD_EXECUTABLE(server  ${SERVER_FILES} ${LIB_SOURCE} ) 
TARGET_LINK_LIBRARIES(server -lthrift)

SET( CLIENT_FILES LockClient.cpp)
ADD_EXECUTABLE(client  ${CLIENT_FILES} ${LIB_SOURCE} ) 
TARGET_LINK_LIBRARIES(client -lthrift)
