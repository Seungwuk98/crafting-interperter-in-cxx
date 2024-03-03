add_executable(ParserTest LexerTests.cpp ParserTests.cpp)

target_link_libraries(ParserTest PRIVATE doctest LoxParser)

target_include_directories(ParserTest PRIVATE
        ${PROJECT_SOURCE_DIR}/third-party
        ${PROJECT_SOURCE_DIR}/include
)