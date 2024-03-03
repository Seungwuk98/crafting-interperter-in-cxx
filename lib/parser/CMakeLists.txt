file(GLOB_RECURSE ParserSources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(LoxParser STATIC ${ParserSources})


target_link_libraries(LoxParser PUBLIC ${LLVM_AVAILABLE_LIBS} LoxAST)
