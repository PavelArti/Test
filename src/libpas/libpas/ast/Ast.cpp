#include <libpas/ast/Ast.hpp>

#include <libpas/ast/Visitor.hpp>

namespace pascal::ast {

void Operation::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void Booloperation::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void Modification::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void Expression::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void Boolexpr::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void Simpletype::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void Interval::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void Arraytype::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void Functionname::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void Id::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void Cell::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void Constdeclaration::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void Declaration::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void Char::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void Stringliteral::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void Int::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void Header::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void Constdecl::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void Vardecl::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void Block::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void Functioncall::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void Assignment::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void While::accept(Visitor& visitor) {
  visitor.visit(*this);
}

void Branch::accept(Visitor& visitor) {
  visitor.visit(*this);
}

}  // namespace pascal::ast
