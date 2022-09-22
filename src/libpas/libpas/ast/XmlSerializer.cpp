#include <libpas/ast/XmlSerializer.hpp>

#include <string>

namespace pascal::ast {

void XmlSerializer::exec(Program& program, std::ostream& out) {
  XmlSerializer xml_serializer;
  xml_serializer.nodes_.push(xml_serializer.doc_.append_child("pascal"));
  program.get_header()->accept(xml_serializer);
  auto* vardecl = program.get_vardecl();
  auto* constdecl = program.get_constdecl();
  if (constdecl != nullptr) {
    constdecl->accept(xml_serializer);
  }
  if (vardecl != nullptr) {
    vardecl->accept(xml_serializer);
  }
  program.get_block()->accept(xml_serializer);
  xml_serializer.doc_.save(out, "  ");
}

void XmlSerializer::visit(Header& member) {
  auto header = append_child("progname");
  nodes_.push(header);
  member.progname()->accept(*this);
  nodes_.pop();
}

void XmlSerializer::visit(Constdecl& member) {
  auto header = append_child("constdecl");
  nodes_.push(header);
  for (const auto& constant : member.constdeclarations()) {
    constant->accept(*this);
  }
  nodes_.pop();
}

void XmlSerializer::visit(Constdeclaration& member) {
  auto header = append_child("constdeclaration");
  nodes_.push(header);

  auto name = append_child("constname");
  nodes_.push(name);
  member.constname()->accept(*this);
  nodes_.pop();

  auto value = append_child("value");
  nodes_.push(value);
  member.expression()->accept(*this);
  nodes_.pop();

  nodes_.pop();
}

void XmlSerializer::visit(Expression& member) {
  if (member.brackets()) {
    auto braces = append_child("braces");
    nodes_.push(braces);
  }
  bool op_visited = false;

  if (member.atom() == nullptr) {
    for (const auto& operand : member.operands()) {
      operand->accept(*this);
      if (!op_visited && member.operation() != nullptr) {
        op_visited = true;
        member.operation()->accept(*this);
      }
    }
  } else {
    for (const auto& sign : member.signs()) {
      sign->accept(*this);
    }
    member.atom()->accept(*this);
  }

  if (member.brackets()) {
    nodes_.pop();
  }
}

void XmlSerializer::visit(Boolexpr& member) {
  auto condition = append_child("condition");
  nodes_.push(condition);

  member.operand1()->accept(*this);
  member.booloperation()->accept(*this);
  member.operand2()->accept(*this);

  nodes_.pop();
}

void XmlSerializer::visit(Vardecl& member) {
  auto header = append_child("vardecl");
  nodes_.push(header);
  for (const auto& variable : member.declarations()) {
    variable->accept(*this);
  }
  nodes_.pop();
}

void XmlSerializer::visit(Declaration& member) {
  auto header = append_child("declaration");
  nodes_.push(header);
  for (const auto& varname : member.varnames()) {
    auto value = append_child("varname");
    nodes_.push(value);
    varname->accept(*this);
    nodes_.pop();
  }
  member.vartype()->accept(*this);
  nodes_.pop();
}

void XmlSerializer::visit(Simpletype& vartype) {
  auto header = append_child("vartype");
  nodes_.push(header);
  append_text(vartype.text().c_str());
  nodes_.pop();
}

void XmlSerializer::visit(Interval& member) {
  auto header = append_child("interval");
  nodes_.push(header);

  auto border1 = append_child("lborder");
  nodes_.push(border1);
  member.lborder()->accept(*this);
  nodes_.pop();

  auto border2 = append_child("rborder");
  nodes_.push(border2);
  member.rborder()->accept(*this);
  nodes_.pop();

  nodes_.pop();
}

void XmlSerializer::visit(Arraytype& vartype) {
  auto header = append_child("arraytype");
  nodes_.push(header);
  vartype.interval()->accept(*this);
  vartype.simpletype()->accept(*this);
  nodes_.pop();
}

void XmlSerializer::visit(Block& statement) {
  auto header = append_child("block");
  nodes_.push(header);
  for (const auto& component : statement.components()) {
    component->accept(*this);
  }
  nodes_.pop();
}

void XmlSerializer::visit(Functioncall& statement) {
  auto header = append_child("functioncall");
  nodes_.push(header);
  statement.functionname()->accept(*this);
  for (const auto& variable : statement.variables()) {
    auto value = append_child("argument");
    nodes_.push(value);
    variable->accept(*this);
    nodes_.pop();
  }
  for (const auto& argument : statement.arguments()) {
    auto value = append_child("argument");
    nodes_.push(value);
    argument->accept(*this);
    nodes_.pop();
  }
  nodes_.pop();
}

void XmlSerializer::visit(Assignment& statement) {
  auto header = append_child("assignment");
  nodes_.push(header);

  auto variable = append_child("variable");
  nodes_.push(variable);
  auto* cell = statement.cell();
  if (cell != nullptr) {
    cell->accept(*this);
  } else {
    statement.varname()->accept(*this);
  }
  nodes_.pop();

  statement.modification()->accept(*this);

  auto value = append_child("value");
  nodes_.push(value);
  statement.expression()->accept(*this);
  nodes_.pop();

  nodes_.pop();
}

void XmlSerializer::visit(While& statement) {
  auto header = append_child("whileloop");
  nodes_.push(header);

  statement.boolexpr()->accept(*this);

  auto body = append_child("body");
  nodes_.push(body);
  statement.statement()->accept(*this);
  nodes_.pop();

  nodes_.pop();
}

void XmlSerializer::visit(Branch& statement) {
  auto header = append_child("branch");
  nodes_.push(header);

  statement.boolexpr()->accept(*this);

  auto body = append_child("body");
  nodes_.push(body);
  statement.statement()->accept(*this);
  nodes_.pop();

  if (statement.alternative() != nullptr) {
    auto alternative = append_child("alternative");
    nodes_.push(alternative);
    statement.alternative()->accept(*this);
    nodes_.pop();
  }
  nodes_.pop();
}

void XmlSerializer::visit(Operation& value) {
  auto header = append_child("operation");
  nodes_.push(header);
  append_text(value.text().c_str());
  nodes_.pop();
}

void XmlSerializer::visit(Booloperation& value) {
  auto header = append_child("booloperation");
  nodes_.push(header);
  append_text(value.text().c_str());
  nodes_.pop();
}

void XmlSerializer::visit(Modification& value) {
  auto header = append_child("modification");
  nodes_.push(header);
  append_text(value.text().c_str());
  nodes_.pop();
}

void XmlSerializer::visit(Functionname& value) {
  auto header = append_child("functionname");
  nodes_.push(header);
  append_text(value.text().c_str());
  nodes_.pop();
}

void XmlSerializer::visit(Id& value) {
  auto header = append_child("id");
  nodes_.push(header);
  append_text(value.text().c_str());
  nodes_.pop();
}

void XmlSerializer::visit(Cell& value) {
  auto header = append_child("cell");
  nodes_.push(header);
  value.varname()->accept(*this);

  auto index = append_child("index");
  nodes_.push(index);
  value.index()->accept(*this);
  nodes_.pop();

  nodes_.pop();
}

void XmlSerializer::visit(Char& value) {
  auto header = append_child("char");
  nodes_.push(header);
  append_text(value.text().c_str());
  nodes_.pop();
}

void XmlSerializer::visit(Stringliteral& value) {
  auto header = append_child("string");
  nodes_.push(header);
  append_text(value.text().c_str());
  nodes_.pop();
}

void XmlSerializer::visit(Int& value) {
  auto header = append_child("integer");
  nodes_.push(header);
  append_text(value.text().c_str());
  nodes_.pop();
}

pugi::xml_node XmlSerializer::append_child(const char* name) {
  return nodes_.top().append_child(name);
}

void XmlSerializer::append_text(const char* text) {
  auto text_node = nodes_.top().append_child(pugi::node_pcdata);
  text_node.set_value(text);
}

}  // namespace pascal::ast
