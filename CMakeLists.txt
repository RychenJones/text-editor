cmake_minimum_required(VERSION 3.20)

project(text_editor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(text_editor
    main.cpp
    src/Document.cpp
    src/FileStorage.cpp
    src/KeyReader.cpp
    src/TerminalRenderer.cpp
    src/TextEditorUI.cpp
    include/CursorPosition.h
    include/Document.h
    include/FileStorage.h
    include/KeyReader.h
    include/TerminalRenderer.h
    include/TextEditorUI.h
    include/Viewport.h
)

target_include_directories(text_editor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

enable_testing()

add_executable(document_tests
    tests/DocumentTests.cpp
    src/Document.cpp
)

target_include_directories(document_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_test(NAME document_tests COMMAND document_tests)

add_executable(terminal_renderer_tests
    tests/TerminalRendererTests.cpp
    src/Document.cpp
    src/TerminalRenderer.cpp
)

target_include_directories(terminal_renderer_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_test(NAME terminal_renderer_tests COMMAND terminal_renderer_tests)

add_executable(file_storage_tests
    tests/FileStorageTests.cpp
    src/Document.cpp
    src/FileStorage.cpp
)

target_include_directories(file_storage_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_test(NAME file_storage_tests COMMAND file_storage_tests)
