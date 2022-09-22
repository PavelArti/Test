#include <libpas/ast/SemanticAnalysier.hpp>
#include <libpas/ast/SymbolTable.hpp>

#include <string_view>
#include <unordered_map>

namespace pascal::ast {

namespace {

VarType to_var_type(std::string_view text) {
  static const std::unordered_map<std::string_view, VarType> text_to_kind = {
      {"integer", VarType::IntegerType},
      {"char", VarType::CharType},
      {"string", VarType::StringType}};

  return text_to_kind.find(text)->second;
}

}  // namespace

SymbolTable SemanticAnalysier::exec(Program& program) {
  SemanticAnalysier semantic_analysier;
  program.get_header()->accept(semantic_analysier);
  auto* vardecl = program.get_vardecl();
  auto* constdecl = program.get_constdecl();
  if (constdecl != nullptr) {
    constdecl->accept(semantic_analysier);
  }
  if (vardecl != nullptr) {
    vardecl->accept(semantic_analysier);
  }
  program.get_block()->accept(semantic_analysier);
  return semantic_analysier.get_symbol_table();
}

void SemanticAnalysier::visit(Header& member) {
  symbol_table_.insert(std::make_pair(
      member.progname()->text(), Symbol(Form::ProgName, VarType::NoType)));
}

void SemanticAnalysier::visit(Constdecl& member) {
  for (const auto& constant : member.constdeclarations()) {
    constant->accept(*this);
  }
}

void SemanticAnalysier::visit(Constdeclaration& member) {
  const auto constname = member.constname()->text();
  if (symbol_table_.find(constname) != symbol_table_.end()) {
    throw SemanticError(
        "Repeat declaration of const identifier '" + constname + "'");
  }
  member.expression()->accept(*this);
  const auto consttype = member.expression()->type();
  member.constname()->set_type(consttype);
  const auto symbol = Symbol(Form::Constant, consttype);
  symbol_table_.insert(std::make_pair(constname, symbol));
}

void SemanticAnalysier::visit(Vardecl& member) {
  for (const auto& variable : member.declarations()) {
    variable->accept(*this);
  }
}

void SemanticAnalysier::visit(Declaration& member) {
  member.vartype()->accept(*this);
  const auto symbol = member.vartype()->type();
  if (symbol.get_form() == Form::Array &&
      symbol.get_type() == VarType::StringType) {
    throw SemanticError("Incompatible array type of array");
  }
  for (const auto& var : member.varnames()) {
    const auto varname = var->text();
    if (symbol_table_.find(varname) != symbol_table_.end()) {
      throw SemanticError("Repeat declaration of identifier '" + varname + "'");
    }
    var->set_type(symbol.get_type());
    symbol_table_.insert(std::make_pair(varname, symbol));
  }
}

void SemanticAnalysier::visit(Block& statement) {
  for (const auto& component : statement.components()) {
    component->accept(*this);
  }
}

void SemanticAnalysier::visit(Assignment& statement) {
  VarType vartype;
  if (statement.cell() != nullptr) {
    statement.cell()->accept(*this);
    vartype = statement.cell()->type();
  } else {
    const auto it = symbol_table_.find(statement.varname()->text());
    statement.varname()->accept(*this);
    if (it->second.get_form() == Form::Constant) {
      throw SemanticError(
          "Cannot assign new value to constant '" +
          statement.varname()->text() + "'");
    }
    vartype = statement.varname()->type();
  }
  statement.modification()->accept(*this);
  const auto modification = statement.modification()->type();
  if (modification != ModType::Assignment) {
    if (vartype == VarType::StringType && modification != ModType::Add) {
      throw SemanticError("Incompatible operation for string expression");
    }
    if (vartype == VarType::CharType) {
      throw SemanticError("Incompatible operation for char expression");
    }
  }
  statement.expression()->accept(*this);
  if (statement.expression()->type() != vartype &&
      !(vartype == VarType::StringType &&
        statement.expression()->type() == VarType::CharType)) {
    throw SemanticError("Incompatible operands types for assignment");
  }
}

void SemanticAnalysier::visit(While& statement) {
  statement.boolexpr()->accept(*this);
  statement.statement()->accept(*this);
}

void SemanticAnalysier::visit(Branch& statement) {
  statement.boolexpr()->accept(*this);
  statement.statement()->accept(*this);

  auto* alternative = statement.alternative();
  if (alternative != nullptr) {
    alternative->accept(*this);
  }
}

void SemanticAnalysier::visit(Operation& value) {
  const std::string_view text = value.text();
  static const std::unordered_map<std::string_view, Op> text_to_kind = {
      {"+", Op::Plus},
      {"-", Op::Minus},
      {"*", Op::Star},
      {"div", Op::Div},
      {"mod", Op::Mod}};
  value.set_type(text_to_kind.find(text)->second);
}

void SemanticAnalysier::visit(Booloperation& value) {
  const std::string_view text = value.text();
  static const std::unordered_map<std::string_view, BoolOp> text_to_kind = {
      {"=", BoolOp::Equal},
      {">", BoolOp::MoreThen},
      {"<", BoolOp::LessThen},
      {"<>", BoolOp::NotEqual},
      {"<=", BoolOp::NotMore},
      {">=", BoolOp::NotLess}};
  value.set_type(text_to_kind.find(text)->second);
}

void SemanticAnalysier::visit(Modification& value) {
  const std::string_view text = value.text();
  static const std::unordered_map<std::string_view, ModType> text_to_kind = {
      {":=", ModType::Assignment},
      {"+=", ModType::Add},
      {"-=", ModType::Reduce},
      {"*=", ModType::Multiply}};
  value.set_type(text_to_kind.find(text)->second);
}

void SemanticAnalysier::visit(Functionname& value) {
  const std::string_view text = value.text();
  static const std::unordered_map<std::string_view, FuncName> text_to_kind = {
      {"readln", FuncName::Readln},
      {"write", FuncName::Write},
      {"writeln", FuncName::Writeln}};
  value.set_type(text_to_kind.find(text)->second);
}

void SemanticAnalysier::visit(Functioncall& statement) {
  statement.functionname()->accept(*this);
  const auto functionname = statement.functionname()->type();
  if (functionname == FuncName::Readln) {
    if (!statement.arguments().empty()) {
      throw SemanticError(
          "Only identifiers or array cells expected in read function "
          "arguments");
    }
    for (const auto& variable : statement.variables()) {
      variable->accept(*this);
      const auto it = symbol_table_.find(variable->text());
      if (it->second.get_form() == Form::Constant) {
        throw SemanticError(
            "Cannot assign new value to constant '" + variable->text() + "'");
      }
    }
    return;
  }
  for (const auto& variable : statement.variables()) {
    variable->accept(*this);
  }
  for (const auto& argument : statement.arguments()) {
    argument->accept(*this);
  }
}

void SemanticAnalysier::visit(Expression& member) {
  if (member.atom() != nullptr) {
    for (const auto& sign : member.signs()) {
      sign->accept(*this);
    }
    member.atom()->accept(*this);
    const auto type = member.atom()->type();
    if (!member.signs().empty() && (type != VarType::IntegerType)) {
      throw SemanticError("Only integer expression can be signed");
    }
    member.set_type(type);
    return;
  }
  const auto& operands = member.operands();
  auto* operation = member.operation();

  auto operand1_type = VarType::NoType;
  auto operand2_type = VarType::NoType;

  for (const auto& operand : operands) {
    operand->accept(*this);
    if (operand1_type == VarType::NoType) {
      operand1_type = operand->type();
    } else {
      operand2_type = operand->type();
    }
  }
  if (operation != nullptr) {
    if (operand1_type == VarType::CharType ||
        operand1_type == VarType::StringType) {
      throw SemanticError("Incompatible operands types for expression");
    }
    operation->accept(*this);
  } else {
    member.set_type(operand1_type);
    return;
  }
  if (operand1_type != operand2_type) {
    throw SemanticError("Incompatible operands types for expression");
  }

  member.set_type(operand1_type);
}

void SemanticAnalysier::visit(Boolexpr& member) {
  member.operand1()->accept(*this);
  const auto operand1_type = member.operand1()->type();
  member.operand2()->accept(*this);
  const auto operand2_type = member.operand2()->type();
  member.booloperation()->accept(*this);

  if (operand1_type != operand2_type) {
    throw SemanticError("Different types of boolean expression operands");
  }
  member.set_type(operand1_type);
}

void SemanticAnalysier::visit(Cell& value) {
  const auto it = symbol_table_.find(value.varname()->text());
  if (it == symbol_table_.end()) {
    throw SemanticError(
        "Unknown identifier '" + value.text() + "' in array name");
  }
  if (!(it->second.get_form() == Form::Array ||
        it->second.get_type() == VarType::StringType)) {
    throw SemanticError(
        "Identifier '" + value.text() + "' is not an array or string name");
  }
  value.varname()->set_type(it->second.get_type());
  value.index()->accept(*this);
  if (value.index()->type() != VarType::IntegerType) {
    throw SemanticError("Invalid index type of '" + value.text() + "'");
  }
  const auto type = value.varname()->type();
  if (type == VarType::StringType) {
    value.set_type(VarType::CharType);
  } else {
    value.set_type(type);
  }
}

void SemanticAnalysier::visit(Id& value) {
  const auto it = symbol_table_.find(value.text());
  if (it == symbol_table_.end()) {
    throw SemanticError("Unknown identifier '" + value.text() + "'");
  }
  if (it->second.get_form() == Form::Array) {
    throw SemanticError("Identifier '" + value.text() + "' is an array name");
  }
  if (it->second.get_form() == Form::ProgName) {
    throw SemanticError("Identifier '" + value.text() + "' is a program name");
  }
  value.set_type(it->second.get_type());
}

void SemanticAnalysier::visit(Char& value) {
  value.set_type(VarType::CharType);
}

void SemanticAnalysier::visit(Stringliteral& value) {
  value.set_type(VarType::StringType);
}

void SemanticAnalysier::visit(Int& value) {
  value.set_type(VarType::IntegerType);
}

void SemanticAnalysier::visit(Interval& member) {
  member.lborder()->accept(*this);
  member.rborder()->accept(*this);
  member.set_type(VarType::IntegerType);
}

void SemanticAnalysier::visit(Simpletype& vartype) {
  const auto type = to_var_type(vartype.text());
  auto symbol = Symbol(Form::Variable, type);
  vartype.set_type(symbol);
}

void SemanticAnalysier::visit(Arraytype& vartype) {
  vartype.interval()->accept(*this);
  const auto type = to_var_type(vartype.simpletype()->text());
  auto symbol = Symbol(Form::Array, type);
  vartype.set_type(symbol);
}

}  // namespace pascal::ast
