#pragma once

#include <libpas/ast/Ast.hpp>
#include <libpas/ast/SymbolTable.hpp>
#include <libpas/ast/Visitor.hpp>

#include <ostream>
#include <sstream>

namespace pascal::ast {

class CodeGenerator final : public Visitor {
 public:
  CodeGenerator(SymbolTable& symbol_table) : symbol_table_(symbol_table) {}
  static void
  exec(Program& program, SymbolTable& symbol_table, std::ostream& out);

  void visit(Header& member) override;
  void visit(Constdecl& member) override;
  void visit(Constdeclaration& member) override;
  void visit(Expression& member) override;
  void visit(Boolexpr& member) override;
  void visit(Vardecl& member) override;
  void visit(Declaration& member) override;
  void visit(Simpletype& vartype) override;
  void visit(Interval& member) override;
  void visit(Arraytype& vartype) override;
  void visit(Block& statement) override;
  void visit(Functioncall& statement) override;
  void visit(Assignment& statement) override;
  void visit(While& statement) override;
  void visit(Branch& statement) override;
  void visit(Operation& value) override;
  void visit(Booloperation& value) override;
  void visit(Modification& value) override;
  void visit(Functionname& value) override;
  void visit(Id& value) override;
  void visit(Cell& value) override;
  void visit(Char& value) override;
  void visit(Stringliteral& value) override;
  void visit(Int& value) override;

 private:
  struct Expr {
    std::vector<size_t> operands;
    std::vector<Op> operations;
  };
  void generate_file(std::ostream& out);
  void write_function(VarType type);
  void writeln_function(VarType type);
  void read_function(VarType type, size_t var);
  void load_variable(VarType type);
  void store_variable(VarType type);
  void add_op(Expr& expr, size_t i, std::string_view operation);
  void parse_stacks(Expr& expr);
  void parse_expression(Expression& expression, Expr& expr);
  VarType get_ptr(Cell& value);
  void convert_to_string();

  SymbolTable& symbol_table_;
  std::stringstream ss_;
  std::stringstream conststrings_;
  bool strings_ = false;
  bool char_convert_ = false;
  bool read_int_ = false;
  bool read_char_ = false;
  bool read_string_ = false;
  bool write_int_ = false;
  bool writeln_int_ = false;
  bool write_char_ = false;
  bool writeln_char_ = false;
  bool write_string_ = false;
  bool writeln_string_ = false;
  size_t vars_ = 0;
};

}  // namespace pascal::ast
