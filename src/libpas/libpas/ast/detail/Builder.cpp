#include <libpas/ast/detail/Builder.hpp>

#include <algorithm>

namespace pascal::ast::detail {

std::any Builder::visitProgram(PascalParser::ProgramContext* context) {
  auto* header = dynamic_cast<Header*>(
      std::any_cast<Member*>(context->header()->accept(this)));
  program_.set_header(header);

  if (context->vardecl() != nullptr) {
    auto* vardecl = dynamic_cast<Vardecl*>(
        std::any_cast<Member*>(context->vardecl()->accept(this)));
    program_.set_vardecl(vardecl);
  }

  if (context->constdecl() != nullptr) {
    auto* constdecl = dynamic_cast<Constdecl*>(
        std::any_cast<Member*>(context->constdecl()->accept(this)));
    program_.set_constdecl(constdecl);
  }

  auto* block = dynamic_cast<Block*>(
      std::any_cast<Member*>(context->block()->accept(this)));
  program_.set_block(block);
  return header;
}

static std::string trim_quotes(const std::string& str) {
  // NOLINTNEXTLINE
  assert(str[0] == '\'' && str[str.size() - 1] == '\'');
  return str.substr(1, str.size() - 2);
}

static std::string normalize_register(const std::string& str) {
  auto text = str;
  std::transform(text.begin(), text.end(), text.begin(), tolower);
  return text;
}

std::any Builder::visitHeader(PascalParser::HeaderContext* context) {
  auto* progname =
      dynamic_cast<Id*>(std::any_cast<Member*>(visit(context->progname())));
  return static_cast<Member*>(program_.create_node<Header>(progname));
}

std::any Builder::visitConstdecl(PascalParser::ConstdeclContext* context) {
  Constdecl::Constdeclarations constdeclarations;
  for (const auto& constdeclaration : context->constdeclaration()) {
    constdeclarations.push_back(dynamic_cast<Constdeclaration*>(
        std::any_cast<Member*>(visit(constdeclaration))));
  }
  return static_cast<Member*>(
      program_.create_node<Constdecl>(constdeclarations));
}

std::any Builder::visitConstdeclaration(
    PascalParser::ConstdeclarationContext* context) {
  auto* constname =
      dynamic_cast<Id*>(std::any_cast<Member*>(visit(context->constname())));
  auto* expression = dynamic_cast<Expression*>(
      std::any_cast<Member*>(visit(context->expression())));
  return static_cast<Member*>(
      program_.create_node<Constdeclaration>(constname, expression));
}

std::any Builder::visitExpression(PascalParser::ExpressionContext* context) {
  Expression::Operands operands;
  Operation* operation = nullptr;
  Expression::Signs signs;
  Value* atom = nullptr;
  bool brackets = context->LPAREN() != nullptr;

  if (context->atom() == nullptr) {
    if (!brackets) {
      operation = dynamic_cast<Operation*>(
          std::any_cast<Member*>(visit(context->operation())));
    }
    for (const auto& operand : context->expression()) {
      operands.push_back(
          dynamic_cast<Expression*>(std::any_cast<Member*>(visit(operand))));
    }
  } else {
    for (const auto& sign : context->sign()) {
      signs.push_back(
          dynamic_cast<Operation*>(std::any_cast<Member*>(static_cast<Member*>(
              program_.create_node<Operation>(sign->getText())))));
    }
    atom = dynamic_cast<Value*>(std::any_cast<Member*>(visit(context->atom())));
  }

  return static_cast<Member*>(program_.create_node<Expression>(
      operands, operation, signs, atom, brackets));
}

std::any Builder::visitBoolexpr(PascalParser::BoolexprContext* context) {
  auto* operand1 = dynamic_cast<Expression*>(
      std::any_cast<Member*>(visit(context->operand1())));
  auto* booloperation = dynamic_cast<Booloperation*>(
      std::any_cast<Member*>(visit(context->booloperation())));
  auto* operand2 = dynamic_cast<Expression*>(
      std::any_cast<Member*>(visit(context->operand2())));
  return static_cast<Member*>(
      program_.create_node<Boolexpr>(operand1, booloperation, operand2));
}

std::any Builder::visitVardecl(PascalParser::VardeclContext* context) {
  Vardecl::Declarations declarations;
  for (const auto& declaration : context->declaration()) {
    declarations.push_back(
        dynamic_cast<Declaration*>(std::any_cast<Member*>(visit(declaration))));
  }
  return static_cast<Member*>(program_.create_node<Vardecl>(declarations));
}

std::any Builder::visitDeclaration(PascalParser::DeclarationContext* context) {
  Declaration::Varnames varnames;
  for (const auto& varname : context->varname()) {
    varnames.push_back(
        dynamic_cast<Id*>(std::any_cast<Member*>(visit(varname))));
  }
  auto* vartype =
      dynamic_cast<Vartype*>(std::any_cast<Member*>(visit(context->vartype())));
  return static_cast<Member*>(
      program_.create_node<Declaration>(varnames, vartype));
}

std::any Builder::visitSimpletype(PascalParser::SimpletypeContext* context) {
  return static_cast<Member*>(
      program_.create_node<Simpletype>(normalize_register(context->getText())));
}

std::any Builder::visitInterval(PascalParser::IntervalContext* context) {
  auto* lborder =
      dynamic_cast<Value*>(std::any_cast<Member*>(visit(context->lborder())));
  auto* rborder =
      dynamic_cast<Value*>(std::any_cast<Member*>(visit(context->rborder())));
  return static_cast<Member*>(program_.create_node<Interval>(lborder, rborder));
}

std::any Builder::visitArraytype(PascalParser::ArraytypeContext* context) {
  auto* interval = dynamic_cast<Interval*>(
      std::any_cast<Member*>(visit(context->interval())));
  auto* simpletype = dynamic_cast<Simpletype*>(
      std::any_cast<Member*>(visit(context->simpletype())));
  return static_cast<Member*>(
      program_.create_node<Arraytype>(interval, simpletype));
}

std::any Builder::visitBlock(PascalParser::BlockContext* context) {
  Block::Components components;
  for (const auto& statement : context->statement()) {
    components.push_back(
        dynamic_cast<Statement*>(std::any_cast<Member*>(visit(statement))));
  }
  return static_cast<Member*>(program_.create_node<Block>(components));
}

std::any Builder::visitFunctioncall(
    PascalParser::FunctioncallContext* context) {
  auto* functionname = dynamic_cast<Functionname*>(
      std::any_cast<Member*>(visit(context->functionname())));

  Functioncall::Variables variables;
  for (const auto& variable : context->variable()) {
    variables.push_back(
        dynamic_cast<Value*>(std::any_cast<Member*>(visit(variable))));
  }
  Functioncall::Arguments arguments;
  for (const auto& argument : context->argument()) {
    arguments.push_back(
        dynamic_cast<Expression*>(std::any_cast<Member*>(visit(argument))));
  }

  return static_cast<Member*>(
      program_.create_node<Functioncall>(functionname, variables, arguments));
}

std::any Builder::visitAssignment(PascalParser::AssignmentContext* context) {
  Cell* cell = nullptr;
  Id* varname = nullptr;
  if (context->cell() != nullptr) {
    cell = dynamic_cast<Cell*>(std::any_cast<Member*>(visit(context->cell())));
  } else {
    varname =
        dynamic_cast<Id*>(std::any_cast<Member*>(visit(context->varname())));
  }
  auto* modification = dynamic_cast<Modification*>(
      std::any_cast<Member*>(visit(context->modification())));
  auto* expression = dynamic_cast<Expression*>(
      std::any_cast<Member*>(visit(context->expression())));
  return static_cast<Member*>(program_.create_node<Assignment>(
      cell, varname, modification, expression));
}

std::any Builder::visitWhile(PascalParser::WhileContext* context) {
  auto* boolexpr = dynamic_cast<Boolexpr*>(
      std::any_cast<Member*>(visit(context->boolexpr())));
  auto* statement = dynamic_cast<Statement*>(
      std::any_cast<Member*>(visit(context->statement())));
  return static_cast<Member*>(program_.create_node<While>(boolexpr, statement));
}

std::any Builder::visitBranch(PascalParser::BranchContext* context) {
  Statement* alternative = nullptr;
  auto* boolexpr = dynamic_cast<Boolexpr*>(
      std::any_cast<Member*>(visit(context->boolexpr())));
  auto* statement = dynamic_cast<Statement*>(
      std::any_cast<Member*>(visit(context->statement())));
  if (context->alternative() != nullptr) {
    alternative = dynamic_cast<Statement*>(
        std::any_cast<Member*>(visit(context->alternative())));
  }
  return static_cast<Member*>(
      program_.create_node<Branch>(boolexpr, statement, alternative));
}

std::any Builder::visitOperation(PascalParser::OperationContext* context) {
  return static_cast<Member*>(
      program_.create_node<Operation>(normalize_register(context->getText())));
}

std::any Builder::visitBooloperation(
    PascalParser::BooloperationContext* context) {
  return static_cast<Member*>(program_.create_node<Booloperation>(
      normalize_register(context->getText())));
}

std::any Builder::visitModification(
    PascalParser::ModificationContext* context) {
  return static_cast<Member*>(
      program_.create_node<Modification>(context->getText()));
}

std::any Builder::visitFunctionname(
    PascalParser::FunctionnameContext* context) {
  return static_cast<Member*>(program_.create_node<Functionname>(
      normalize_register(context->getText())));
}

std::any Builder::visitId(PascalParser::IdContext* context) {
  return static_cast<Member*>(
      program_.create_node<Id>(normalize_register(context->getText())));
}

std::any Builder::visitCell(PascalParser::CellContext* context) {
  auto* varname =
      dynamic_cast<Id*>(std::any_cast<Member*>(visit(context->varname())));
  auto* index = dynamic_cast<Expression*>(
      std::any_cast<Member*>(visit(context->index())));
  return static_cast<Member*>(program_.create_node<Cell>(varname, index));
}

std::any Builder::visitChar(PascalParser::CharContext* context) {
  const auto text = trim_quotes(context->getText());
  return static_cast<Member*>(program_.create_node<Char>(text));
}

std::any Builder::visitStringliteral(
    PascalParser::StringliteralContext* context) {
  const auto text = trim_quotes(context->getText());
  return static_cast<Member*>(program_.create_node<Stringliteral>(text));
}

std::any Builder::visitInt(PascalParser::IntContext* context) {
  return static_cast<Member*>(program_.create_node<Int>(context->getText()));
}

}  // namespace pascal::ast::detail
