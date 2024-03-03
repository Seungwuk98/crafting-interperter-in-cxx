file(GLOB_RECURSE InterpreterTests ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(InterpreterTest ${InterpreterTests})

target_link_libraries(InterpreterTest PRIVATE doctest LoxInterpreter)

target_include_directories(InterpreterTest PRIVATE
        ${PROJECT_SOURCE_DIR}/third-party
        ${PROJECT_SOURCE_DIR}/include
)