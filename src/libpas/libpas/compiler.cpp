#include <libpas/ast/CodeGenerator.hpp>
#include <libpas/ast/SemanticAnalysier.hpp>
#include <libpas/ast/XmlSerializer.hpp>
#include <libpas/ast/detail/Builder.hpp>
#include <libpas/compiler.hpp>

#include <PascalLexer.h>
#include <PascalParser.h>

#include <fmt/format.h>

#include <iostream>

namespace pascal {

namespace {

class StreamErrorListener : public antlr4::BaseErrorListener {
 public:
  void syntaxError(
      antlr4::Recognizer* /*recognizer*/,
      antlr4::Token* /*offendingSymbol*/,
      size_t line,
      size_t column,
      const std::string& message,
      std::exception_ptr /*e*/) override {
    errors_.emplace_back(Error{line, column, message});
  }

  const Errors& errors() const { return errors_; }

 private:
  Errors errors_;
};

}  // namespace

ParseResult parse(PascalLexer& lexer) {
  antlr4::CommonTokenStream tokens(&lexer);
  PascalParser parser(&tokens);

  StreamErrorListener error_listener;
  parser.removeErrorListeners();
  parser.addErrorListener(&error_listener);

  auto* program_parse_tree = parser.program();

  const auto& errors = error_listener.errors();
  if (!errors.empty()) {
    return ParseResult::errors(errors);
  }

  ast::Program program;
  ast::detail::Builder builder(program);
  builder.visit(program_parse_tree);

  return ParseResult::program(std::move(program));
}

void dump_ast(ast::Program& program, std::ostream& out) {
  ast::XmlSerializer::exec(program, out);
}

bool semantic_analyse(
    ast::Program& program,
    ast::SymbolTable& symbol_table,
    std::ostream& out) {
  try {
    symbol_table = ast::SemanticAnalysier::exec(program);
  } catch (const ast::SemanticError& e) {
    out << fmt::format("Error: {}\n", e.what());
    return false;
  }
  return true;
}

void code_generate(
    ast::Program& program,
    ast::SymbolTable& symbol_table,
    std::ostream& out) {
  ast::CodeGenerator::exec(program, symbol_table, out);
}

void exec_generate(std::string_view input_file, std::string_view output_file) {
  std::stringstream ss;
  ss << "clang " << input_file << " -o " << output_file;
  const auto str = ss.str();
  // NOLINTNEXTLINE
  system(str.c_str());
}

void dump_errors(const Errors& errors, std::ostream& out) {
  for (const auto& error : errors) {
    out << fmt::format(
        "{}:{} {}\n", error.line_, error.column_, error.message_);
  }
}

}  // namespace pascal
