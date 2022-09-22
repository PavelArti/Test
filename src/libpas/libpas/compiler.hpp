#pragma once

#include <libpas/ast/Ast.hpp>
#include <libpas/ast/SymbolTable.hpp>

#include <PascalLexer.h>

#include <iosfwd>

namespace pascal {

struct Error {
  size_t line_;
  size_t column_;
  std::string message_;
};

using Errors = std::vector<Error>;

struct ParseResult {
  static ParseResult program(ast::Program program) {
    ParseResult result;
    result.program_ = std::move(program);
    return result;
  }

  static ParseResult errors(Errors errors) {
    ParseResult result;
    result.errors_ = std::move(errors);
    return result;
  }

  ast::Program program_;
  Errors errors_;
};

ParseResult parse(PascalLexer& lexer);

void dump_ast(ast::Program& program, std::ostream& out);
bool semantic_analyse(
    ast::Program& program,
    ast::SymbolTable& symbol_table,
    std::ostream& out);
void code_generate(
    ast::Program& program,
    ast::SymbolTable& symbol_table,
    std::ostream& out);
void exec_generate(std::string_view input_file, std::string_view output_file);
void dump_errors(
    const Errors& errors,
    std::ostream& out /*, std::istream& in*/);

}  // namespace pascal
