#pragma once

#include <libpas/ast/SymbolTable.hpp>

#include <memory>
#include <string>
#include <vector>

namespace pascal::ast {

class Member;
class Header;
class Constdecl;
class Vardecl;
class Block;
class Visitor;

class Program final {
 public:
  template <class T, class... Args>
  T* create_node(Args&&... args) {
    static_assert(std::is_base_of_v<Member, T>);
    members_.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
    return dynamic_cast<T*>(members_.back().get());
  }

  void set_header(Header* header) { header_ = header; }
  void set_constdecl(Constdecl* constdecl) { constdecl_ = constdecl; }
  void set_vardecl(Vardecl* vardecl) { vardecl_ = vardecl; }
  void set_block(Block* block) { block_ = block; }
  Header* get_header() { return header_; }
  Constdecl* get_constdecl() { return constdecl_; }
  Vardecl* get_vardecl() { return vardecl_; }
  Block* get_block() { return block_; }

 private:
  std::vector<std::unique_ptr<Member>> members_;
  Header* header_ = nullptr;
  Constdecl* constdecl_ = nullptr;
  Vardecl* vardecl_ = nullptr;
  Block* block_ = nullptr;
};

class Member {
 public:
  virtual ~Member() = default;
  virtual void accept(Visitor& visitor) = 0;
};

class Statement : public Member {
 public:
  virtual ~Statement() = default;
  virtual void accept(Visitor& visitor) = 0;
};

class Value : public Member {
 public:
  virtual ~Value() = default;
  virtual void accept(Visitor& visitor) = 0;
  virtual const std::string& text() const = 0;
  VarType type() const { return type_; }
  void set_type(VarType type) { type_ = type; }

 protected:
  VarType type_;
};

class Vartype : public Member {
 public:
  virtual ~Vartype() = default;
  virtual void accept(Visitor& visitor) = 0;
  const Symbol& type() const { return type_; }
  void set_type(Symbol& type) { type_ = std::move(type); }

 protected:
  Symbol type_;
};

class Operation final : public Member {
 public:
  explicit Operation(std::string text) : text_(std::move(text)) {}
  const std::string& text() const { return text_; }
  void accept(Visitor& visitor) override;
  Op type() const { return type_; }
  void set_type(Op type) { type_ = type; }

 private:
  std::string text_;
  Op type_;
};

class Booloperation final : public Member {
 public:
  explicit Booloperation(std::string text) : text_(std::move(text)) {}
  const std::string& text() const { return text_; }
  void accept(Visitor& visitor) override;
  BoolOp type() const { return type_; }
  void set_type(BoolOp type) { type_ = type; }

 private:
  std::string text_;
  BoolOp type_;
};

class Modification final : public Member {
 public:
  explicit Modification(std::string text) : text_(std::move(text)) {}
  const std::string& text() const { return text_; }
  void accept(Visitor& visitor) override;
  ModType type() const { return type_; }
  void set_type(ModType type) { type_ = type; }

 private:
  std::string text_;
  ModType type_;
};

class Expression final : public Member {
 public:
  using Operands = std::vector<ast::Expression*>;
  using Signs = std::vector<ast::Operation*>;
  Expression(
      Operands operands,
      Operation* operation,
      Signs signs,
      Value* atom,
      bool brackets)
      : operands_(std::move(operands)),
        operation_(std::move(operation)),
        signs_(std::move(signs)),
        atom_(std::move(atom)),
        brackets_(brackets) {}
  const Operands& operands() const { return operands_; }
  Operation* operation() { return operation_; }
  const Signs& signs() const { return signs_; }
  Value* atom() { return atom_; }
  bool brackets() const { return brackets_; }
  void accept(Visitor& visitor) override;
  VarType type() const { return type_; }
  void set_type(VarType type) { type_ = type; }
  const std::string& text() const { return atom_->text(); }

 private:
  Operands operands_;
  Operation* operation_;
  Signs signs_;
  Value* atom_;
  bool brackets_;
  VarType type_;
};

class Boolexpr final : public Member {
 public:
  Boolexpr(
      Expression* operand1,
      Booloperation* booloperation,
      Expression* operand2)
      : operand1_(std::move(operand1)),
        booloperation_(std::move(booloperation)),
        operand2_(std::move(operand2)) {}
  Expression* operand1() { return operand1_; }
  Booloperation* booloperation() { return booloperation_; }
  Expression* operand2() { return operand2_; }
  void accept(Visitor& visitor) override;
  VarType type() const { return type_; }
  void set_type(VarType type) { type_ = type; }

 private:
  Expression* operand1_;
  Booloperation* booloperation_;
  Expression* operand2_;
  VarType type_;
};

class Simpletype final : public Vartype {
 public:
  explicit Simpletype(std::string text) : text_(std::move(text)) {}
  const std::string& text() const { return text_; }
  void accept(Visitor& visitor) override;

 private:
  std::string text_;
};

class Interval final : public Member {
 public:
  Interval(Value* lborder, Value* rborder)
      : lborder_(std::move(lborder)), rborder_(std::move(rborder)) {}
  Value* lborder() { return lborder_; }
  Value* rborder() { return rborder_; }
  void accept(Visitor& visitor) override;
  VarType type() const { return type_; }
  void set_type(VarType type) { type_ = type; }

 private:
  Value* lborder_;
  Value* rborder_;
  VarType type_;
};

class Arraytype final : public Vartype {
 public:
  Arraytype(Interval* interval, Simpletype* simpletype)
      : interval_(std::move(interval)), simpletype_(std::move(simpletype)) {}
  Interval* interval() { return interval_; }
  Simpletype* simpletype() { return simpletype_; }
  void accept(Visitor& visitor) override;

 private:
  Interval* interval_;
  Simpletype* simpletype_;
};

class Functionname final : public Member {
 public:
  explicit Functionname(std::string text) : text_(std::move(text)) {}
  const std::string& text() const { return text_; }
  void accept(Visitor& visitor) override;
  FuncName type() const { return type_; }
  void set_type(FuncName type) { type_ = type; }

 private:
  std::string text_;
  FuncName type_;
};

class Id final : public Value {
 public:
  explicit Id(std::string text) : text_(std::move(text)) {}
  virtual const std::string& text() const override { return text_; }
  void accept(Visitor& visitor) override;

 private:
  std::string text_;
};

class Cell final : public Value {
 public:
  Cell(Id* varname, Expression* index)
      : varname_(std::move(varname)), index_(std::move(index)) {}
  Id* varname() { return varname_; }
  Expression* index() { return index_; }
  virtual const std::string& text() const override { return varname_->text(); }
  void accept(Visitor& visitor) override;

 private:
  Id* varname_;
  Expression* index_;
};

class Constdeclaration final : public Member {
 public:
  Constdeclaration(Id* constname, Expression* expression)
      : constname_(std::move(constname)), expression_(std::move(expression)) {}
  Id* constname() { return constname_; }
  Expression* expression() { return expression_; }
  void accept(Visitor& visitor) override;

 private:
  Id* constname_;
  Expression* expression_;
};

class Declaration final : public Member {
 public:
  using Varnames = std::vector<ast::Id*>;

  Declaration(Varnames varnames, Vartype* vartype)
      : varnames_(std::move(varnames)), vartype_(std::move(vartype)) {}
  const Varnames& varnames() const { return varnames_; }
  Vartype* vartype() { return vartype_; }
  void accept(Visitor& visitor) override;

 private:
  Varnames varnames_;
  Vartype* vartype_;
};

class Char final : public Value {
 public:
  explicit Char(std::string text) : text_(std::move(text)) {}
  virtual const std::string& text() const override { return text_; }
  void accept(Visitor& visitor) override;

 private:
  std::string text_;
};

class Stringliteral final : public Value {
 public:
  explicit Stringliteral(std::string text) : text_(std::move(text)) {}
  virtual const std::string& text() const override { return text_; }
  void accept(Visitor& visitor) override;

 private:
  std::string text_;
};

class Int final : public Value {
 public:
  explicit Int(std::string text) : text_(std::move(text)) {}
  virtual const std::string& text() const override { return text_; }
  void accept(Visitor& visitor) override;

 private:
  std::string text_;
};

class Header final : public Member {
 public:
  explicit Header(Id* progname) : progname_(std::move(progname)) {}
  Id* progname() { return progname_; }
  void accept(Visitor& visitor) override;

 private:
  Id* progname_;
};

class Constdecl final : public Member {
 public:
  using Constdeclarations = std::vector<ast::Constdeclaration*>;

  explicit Constdecl(Constdeclarations constdeclarations)
      : constdeclarations_(std::move(constdeclarations)) {}
  const Constdeclarations& constdeclarations() const {
    return constdeclarations_;
  }
  void accept(Visitor& visitor) override;

 private:
  Constdeclarations constdeclarations_;
};

class Vardecl final : public Member {
 public:
  using Declarations = std::vector<ast::Declaration*>;

  explicit Vardecl(Declarations declarations)
      : declarations_(std::move(declarations)) {}
  const Declarations& declarations() const { return declarations_; }
  void accept(Visitor& visitor) override;

 private:
  Declarations declarations_;
};

class Block final : public Statement {
 public:
  using Components = std::vector<ast::Statement*>;

  explicit Block(Components components) : components_(std::move(components)) {}
  const Components& components() const { return components_; }
  void accept(Visitor& visitor) override;

 private:
  Components components_;
};

class Functioncall final : public Statement {
 public:
  using Variables = std::vector<ast::Value*>;
  using Arguments = std::vector<ast::Expression*>;

  Functioncall(
      Functionname* function_name,
      Variables variables,
      Arguments arguments)
      : function_name_(std::move(function_name)),
        variables_(std::move(variables)),
        arguments_(std::move(arguments)) {}
  Functionname* functionname() { return function_name_; }
  const Variables& variables() const { return variables_; }
  const Arguments& arguments() const { return arguments_; }
  void accept(Visitor& visitor) override;

 private:
  Functionname* function_name_;
  Variables variables_;
  Arguments arguments_;
};

class Assignment final : public Statement {
 public:
  Assignment(
      Cell* cell,
      Id* varname,
      Modification* modification,
      Expression* expression)
      : cell_(std::move(cell)),
        varname_(std::move(varname)),
        modification_(std::move(modification)),
        expression_(std::move(expression)) {}
  Cell* cell() { return cell_; }
  Id* varname() { return varname_; }
  Modification* modification() { return modification_; }
  Expression* expression() { return expression_; }
  void accept(Visitor& visitor) override;

 private:
  Cell* cell_;
  Id* varname_;
  Modification* modification_;
  Expression* expression_;
};

class While final : public Statement {
 public:
  While(Boolexpr* boolexpr, Statement* statement)
      : boolexpr_(std::move(boolexpr)), statement_(std::move(statement)) {}
  Boolexpr* boolexpr() { return boolexpr_; }
  Statement* statement() { return statement_; }
  void accept(Visitor& visitor) override;

 private:
  Boolexpr* boolexpr_;
  Statement* statement_;
};

class Branch final : public Statement {
 public:
  Branch(Boolexpr* boolexpr, Statement* statement, Statement* alternative)
      : boolexpr_(std::move(boolexpr)),
        statement_(std::move(statement)),
        alternative_(std::move(alternative)) {}
  Boolexpr* boolexpr() { return boolexpr_; }
  Statement* statement() { return statement_; }
  Statement* alternative() { return alternative_; }
  void accept(Visitor& visitor) override;

 private:
  Boolexpr* boolexpr_;
  Statement* statement_;
  Statement* alternative_;
};

}  // namespace pascal::ast
