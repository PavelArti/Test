#pragma once

#include <libpas/ast/Ast.hpp>

namespace pascal::ast {

class Visitor {
 public:
  virtual void visit(Header& member) = 0;
  virtual void visit(Constdecl& member) = 0;
  virtual void visit(Constdeclaration& member) = 0;
  virtual void visit(Expression& member) = 0;
  virtual void visit(Boolexpr& member) = 0;
  virtual void visit(Vardecl& member) = 0;
  virtual void visit(Declaration& member) = 0;
  virtual void visit(Simpletype& vartype) = 0;
  virtual void visit(Interval& member) = 0;
  virtual void visit(Arraytype& vartype) = 0;
  virtual void visit(Block& statement) = 0;
  virtual void visit(Functioncall& statement) = 0;
  virtual void visit(Assignment& statement) = 0;
  virtual void visit(While& statement) = 0;
  virtual void visit(Branch& statement) = 0;
  virtual void visit(Operation& value) = 0;
  virtual void visit(Booloperation& value) = 0;
  virtual void visit(Modification& value) = 0;
  virtual void visit(Functionname& value) = 0;
  virtual void visit(Id& value) = 0;
  virtual void visit(Cell& value) = 0;
  virtual void visit(Char& value) = 0;
  virtual void visit(Stringliteral& value) = 0;
  virtual void visit(Int& value) = 0;
};

}  // namespace pascal::ast
