#include <libpas/compiler.hpp>
#include <libpas/dump_tokens.hpp>

#include <PascalLexer.h>
#include <antlr4-runtime.h>
#include <cxxopts.hpp>

#include <fstream>
#include <iostream>
#include <regex>
#include <string>

const char* const file_path_opt = "file-path";
const char* const dump_tokens_opt = "dump-tokens";
const char* const dump_ast_opt = "dump-ast";
const char* const dump_asm_opt = "dump-asm";

int main(int argc, char** argv) {
  cxxopts::Options options("pascal-compiler", "ANTLR4 Pascal compiler");

  options.positional_help("<file-path>");

  try {
    // clang-format off
    options.add_options()
        (file_path_opt, "", cxxopts::value<std::string>())
        (dump_tokens_opt, "")
        (dump_ast_opt, "")
        (dump_asm_opt, "")
        ("h,help", "Print help");
    // clang-format on
  } catch (const cxxopts::OptionSpecException& e) {
    std::cerr << e.what() << "\n";
    return 1;
  }

  options.parse_positional({file_path_opt});

  try {
    const auto result = options.parse(argc, argv);

    if (result.count("help") > 0 || result.count(file_path_opt) != 1) {
      std::cout << options.help() << "\n";
      return 0;
    }
    const auto progname = result[file_path_opt].as<std::string>();
    std::ifstream input_stream(progname);

    if (!input_stream.good()) {
      std::cerr << "Unable to read stream\n";
      return 1;
    }

    antlr4::ANTLRInputStream stream(input_stream);
    PascalLexer lexer(&stream);

    if (result.count(dump_tokens_opt) > 0) {
      pascal::dump_tokens(lexer, std::cout);
    } else {
      auto parser_result = pascal::parse(lexer);
      if (!parser_result.errors_.empty()) {
        pascal::dump_errors(parser_result.errors_, std::cerr);
      } else if (result.count(dump_ast_opt) > 0) {
        pascal::dump_ast(parser_result.program_, std::cout);
      } else {
        pascal::ast::SymbolTable symbol_table;
        if (pascal::semantic_analyse(
                parser_result.program_, symbol_table, std::cerr)) {
          std::regex target(".pas");
          const auto filename =
              std::regex_replace(progname, target, std::string{});
          std::ofstream output_stream(filename + ".ll");
          pascal::code_generate(
              parser_result.program_, symbol_table, output_stream);
          output_stream.close();
          if (result.count(dump_asm_opt) == 0) {
            pascal::exec_generate(filename + ".ll", filename);
          }
        }
      }
    }

  } catch (const cxxopts::OptionException& e) {
    std::cerr << e.what() << "\n";
    return 1;
  }
}
