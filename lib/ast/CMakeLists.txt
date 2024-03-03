
file(GLOB_RECURSE ASTSources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(LoxAST STATIC ${ASTSources})



