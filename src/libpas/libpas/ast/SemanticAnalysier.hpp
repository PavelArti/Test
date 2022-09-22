#pragma once

#include <libpas/ast/Ast.hpp>
#include <libpas/ast/SymbolTable.hpp>
#include <libpas/ast/Visitor.hpp>

#include <stdexcept>

namespace pascal::ast {

class SemanticError final : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

class SemanticAnalysier final : public Visitor {
 public:
  static SymbolTable exec(Program& program);

  void visit(Header& member) override;
  void visit(Constdecl& member) override;
  void visit(Constdeclaration& member) override;
  void visit(Vardecl& member) override;
  void visit(Declaration& member) override;
  void visit(Block& statement) override;
  void visit(Assignment& statement) override;
  void visit(While& statement) override;
  void visit(Branch& statement) override;
  void visit(Operation& value) override;
  void visit(Booloperation& value) override;
  void visit(Modification& value) override;
  void visit(Functionname& value) override;
  void visit(Functioncall& statement) override;
  void visit(Expression& member) override;
  void visit(Boolexpr& member) override;
  void visit(Cell& value) override;
  void visit(Id& value) override;
  void visit(Char& value) override;
  void visit(Stringliteral& value) override;
  void visit(Int& value) override;
  void visit(Interval& member) override;
  void visit(Simpletype& vartype) override;
  void visit(Arraytype& vartype) override;

  SymbolTable get_symbol_table() { return symbol_table_; }

 private:
  SymbolTable symbol_table_;
};

}  // namespace pascal::ast
