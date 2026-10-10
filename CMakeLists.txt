cmake_minimum_required(VERSION 3.20)

project(text_editor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(text_editor
    main.cpp
    FileManager.cpp
    FileManager.h
)
