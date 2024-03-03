file(GLOB_RECURSE ParserTestSources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(ParserTest ${ParserTestSources})

target_link_libraries(ParserTest PRIVATE doctest LoxParser)

target_include_directories(ParserTest PRIVATE
        ${PROJECT_SOURCE_DIR}/third-party
        ${PROJECT_SOURCE_DIR}/include
)