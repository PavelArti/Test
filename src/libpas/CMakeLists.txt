set(lib_name pascal)

add_library(${lib_name})

include(CompileOptions)
pascal_target_set_compile_options(${lib_name})

target_include_directories(${lib_name} PUBLIC .)

target_sources(
  ${lib_name}
  PUBLIC
    libpas/ast/Ast.hpp
    libpas/ast/SymbolTable.hpp
    libpas/ast/Visitor.hpp
    libpas/ast/XmlSerializer.hpp
    libpas/ast/SemanticAnalysier.hpp
    libpas/ast/CodeGenerator.hpp
    libpas/dump_tokens.hpp
    libpas/compiler.hpp
  PRIVATE
    libpas/ast/detail/Builder.cpp
    libpas/ast/detail/Builder.hpp
    libpas/ast/Ast.cpp
    libpas/ast/XmlSerializer.cpp
    libpas/ast/SemanticAnalysier.cpp
    libpas/ast/CodeGenerator.cpp
    libpas/dump_tokens.cpp
    libpas/compiler.cpp
)

target_link_libraries(
  ${lib_name}
  PUBLIC
    Pascal
  PRIVATE
    fmt
    pugixml
)

set(test_name libpas_test)

add_executable(${test_name})

add_test(NAME ${test_name} COMMAND ${test_name})

pascal_target_set_compile_options(${test_name})

target_sources(
  ${test_name}
  PRIVATE
    test/lexer.cpp
    test/parser.cpp
    test/semantic.cpp
    test/codegen.cpp
)

target_link_libraries(
  ${test_name}
  PRIVATE
    ${lib_name}
    gtest
    gtest_main
)
