#pragma once

#include <libpas/ast/Visitor.hpp>

#include <pugixml.hpp>

#include <ostream>
#include <stack>

namespace pascal::ast {

class XmlSerializer final : public Visitor {
 public:
  static void exec(Program& program, std::ostream& out);

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
  pugi::xml_node append_child(const char* name);
  void append_text(const char* text);

  pugi::xml_document doc_;
  std::stack<pugi::xml_node> nodes_;
};

}  // namespace pascal::ast
