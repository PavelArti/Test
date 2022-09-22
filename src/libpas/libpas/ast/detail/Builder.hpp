#pragma once

#include <libpas/ast/Ast.hpp>
#include <libpas/compiler.hpp>

#include <PascalBaseVisitor.h>

namespace pascal::ast::detail {

class Builder final : public PascalBaseVisitor {
 public:
  explicit Builder(ast::Program& program) : program_(program) {}

  std::any visitProgram(PascalParser::ProgramContext* context) override;

  std::any visitHeader(PascalParser::HeaderContext* context) override;

  std::any visitConstdecl(PascalParser::ConstdeclContext* context) override;
  std::any visitConstdeclaration(
      PascalParser::ConstdeclarationContext* context) override;
  std::any visitExpression(PascalParser::ExpressionContext* context) override;
  std::any visitBoolexpr(PascalParser::BoolexprContext* context) override;

  std::any visitVardecl(PascalParser::VardeclContext* context) override;
  std::any visitDeclaration(PascalParser::DeclarationContext* context) override;
  std::any visitSimpletype(PascalParser::SimpletypeContext* context) override;
  std::any visitInterval(PascalParser::IntervalContext* context) override;
  std::any visitArraytype(PascalParser::ArraytypeContext* context) override;

  std::any visitBlock(PascalParser::BlockContext* context) override;
  std::any visitFunctioncall(
      PascalParser::FunctioncallContext* context) override;
  std::any visitAssignment(PascalParser::AssignmentContext* context) override;
  std::any visitWhile(PascalParser::WhileContext* context) override;
  std::any visitBranch(PascalParser::BranchContext* context) override;
  std::any visitOperation(PascalParser::OperationContext* context) override;
  std::any visitBooloperation(
      PascalParser::BooloperationContext* context) override;
  std::any visitModification(
      PascalParser::ModificationContext* context) override;
  std::any visitFunctionname(
      PascalParser::FunctionnameContext* context) override;

  std::any visitId(PascalParser::IdContext* context) override;
  std::any visitCell(PascalParser::CellContext* context) override;
  std::any visitChar(PascalParser::CharContext* context) override;
  std::any visitStringliteral(
      PascalParser::StringliteralContext* context) override;
  std::any visitInt(PascalParser::IntContext* context) override;

 private:
  ast::Program& program_;
};

}  // namespace pascal::ast::detail
