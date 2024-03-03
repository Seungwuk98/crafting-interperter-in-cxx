file(GLOB_RECURSE InterpreterSources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(LoxInterpreter STATIC ${InterpreterSources})

target_link_libraries(LoxInterpreter PRIVATE LoxParser LoxAST)