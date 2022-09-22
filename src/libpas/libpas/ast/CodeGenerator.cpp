#include <libpas/ast/CodeGenerator.hpp>

#include <algorithm>
#include <string_view>

namespace pascal::ast {

static std::string_view type_to_string(VarType type) {
  switch (type) {
    case VarType::CharType:
      return "i8";
    case VarType::IntegerType:
      return "i32";
    case VarType::StringType:
      return "[255 x i8]";
    default: /* do nothing */
      break;
  }
  return "";
}

void CodeGenerator::exec(
    Program& program,
    SymbolTable& symbol_table,
    std::ostream& out) {
  CodeGenerator code_generator(symbol_table);
  auto* constdecl = program.get_constdecl();
  if (constdecl != nullptr) {
    constdecl->accept(code_generator);
  }
  auto* vardecl = program.get_vardecl();
  if (vardecl != nullptr) {
    vardecl->accept(code_generator);
  }
  program.get_block()->accept(code_generator);
  code_generator.generate_file(out);
}

void CodeGenerator::generate_file(std::ostream& out) {
  out << "target triple = \"x86_64-pc-linux-gnu\"\n"
         "declare i32 @printf(i8* %format, ...)\n"
         "declare i32 @__isoc99_scanf(i8*, ...)\n";

  if (strings_) {
    out << "declare i8* @strcpy(i8* %dst, i8* %src)\n"
           "declare i8* @strcat(i8* %dst, i8* %src)\n\n"
           "@.str.empty = constant [1 x i8] c\"\\00\"\n"
           "define void @strinit([255 x i8]* %str) {\n"
           "  %str.ptr = getelementptr [255 x i8], [255 x i8]* %str, i64 0, "
           "i64 0\n"
           "  call i8* @strcpy(i8* %str.ptr, i8* getelementptr ([1 x i8], [1 x "
           "i8]* @.str.empty, i64 0, i64 0))\n"
           "  ret void\n}\n\n";
  } else {
    out << "\n";
  }

  if (char_convert_) {
    out << "@.str.c = constant [2 x i8] c\"*\\00\"\n"
           "define i8* @tostr(i8 %c, [255 x i8]* %str) {\n"
           "  %str.ptr = getelementptr [255 x i8], [255 x i8]* %str, i64 0, "
           "i64 0\n"
           "  call i8* @strcpy(i8* %str.ptr, i8* getelementptr ([2 x i8], [2 x "
           "i8]* @.str.c, i64 0, i64 0))\n"
           "  store i8 %c, i8* %str.ptr\n"
           "  ret i8* %str.ptr\n}\n\n";
  }

  if (write_int_ || read_int_) {
    out << "@.str.int = constant [3 x i8] c\"%d\\00\"\n";
  }
  if (write_char_ || read_char_) {
    out << "@.str.char = constant [3 x i8] c\"%c\\00\"\n";
  }
  if (write_string_ || read_string_) {
    out << "@.str.str = constant [3 x i8] c\"%s\\00\"\n";
  }

  out << "\n";

  if (read_int_) {
    out << "define void @read_int(i32* %x) {\n"
           "  call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr  "
           "([3 x i8], [3 x i8]* @.str.int, i64 0, i64 0), i32* %x)\n"
           "  ret void\n"
           "}\n\n";
  }

  if (read_char_) {
    out << "define void @read_char(i8* %x) {\n"
           "  call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr  "
           "([3 x i8], [3 x i8]* @.str.char, i64 0, i64 0), i8* %x)\n"
           "  ret void\n"
           "}\n\n";
  }

  if (read_string_) {
    out << "define void @read_string(i8* %x) {\n"
           "  call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr  "
           "([3 x i8], [3 x i8]* @.str.str, i64 0, i64 0), i8* %x)\n"
           "  ret void\n"
           "}\n\n";
  }

  if (write_int_) {
    out << "define void @write_int(i32 %x) {\n"
           "  call i32 (i8*, ...) @printf(i8* getelementptr ([3 x i8], [3 x "
           "i8]* @.str.int, i64 0, i64 0), i32 %x)\n"
           "  ret void\n"
           "}\n\n";
  }

  if (write_char_) {
    out << "define void @write_char(i32 %x) {\n"
           "  call i32 (i8*, ...) @printf(i8* getelementptr ([3 x i8], [3 x "
           "i8]* @.str.char, i64 0, i64 0), i32 %x)\n"
           "  ret void\n"
           "}\n\n";
  }

  if (write_string_) {
    out << "define void @write_string(i8* %x) {\n"
           "  call i32 (i8*, ...) @printf(i8* getelementptr ([3 x i8], [3 x "
           "i8]* @.str.str, i64 0, i64 0), i8* %x)\n"
           "  ret void\n"
           "}\n\n";
  }

  if (writeln_int_) {
    out << "@.str.intln = constant [4 x i8] c\"%d\\0A\\00\"\n"
           "define void @writeln_int(i32 %x) {\n"
           "  call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x "
           "i8]* @.str.intln, i64 0, i64 0), i32 %x)\n"
           "  ret void\n"
           "}\n\n";
  }

  if (writeln_char_) {
    out << "@.str.charln = constant [4 x i8] c\"%c\\0A\\00\"\n"
           "define void @writeln_char(i32 %x) {\n"
           "  call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x "
           "i8]* @.str.charln, i64 0, i64 0), i32 %x)\n"
           "  ret void\n"
           "}\n\n";
  }

  if (writeln_string_) {
    out << "@.str.strln = constant [4 x i8] c\"%s\\0A\\00\"\n"
           "define void @writeln_string(i8* %x) {\n"
           "  call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x "
           "i8]* @.str.strln, i64 0, i64 0), i8* %x)\n"
           "  ret void\n"
           "}\n\n";
  }

  const auto conststrings = conststrings_.str();
  if (!conststrings.empty()) {
    out << conststrings << "\n";
  }

  out << "define i32 @main() {\n"
         "start:\n"
      << ss_.str() << "  ret i32 0\n}\n";
}

void CodeGenerator::write_function(VarType type) {
  switch (type) {
    case VarType::IntegerType:
      write_int_ = true;
      ss_ << "  call void @write_int(i32 %." << vars_ << ")\n";
      break;
    case VarType::CharType:
      write_char_ = true;
      ++vars_;
      ss_ << "  %." << vars_ << " = sext i8 %." << vars_ - 1
          << " to i32\n"
             "  call void @write_char(i32 %."
          << vars_ << ")\n";
      break;
    case VarType::StringType:
      write_string_ = true;
      ss_ << "  call void @write_string(i8* %." << vars_ << ")\n";
      break;
    default: /* do nothing */
      return;
  }
}

void CodeGenerator::writeln_function(VarType type) {
  switch (type) {
    case VarType::IntegerType:
      writeln_int_ = true;
      ss_ << "  call void @writeln_int(i32 %." << vars_ << ")\n";
      break;
    case VarType::CharType:
      writeln_char_ = true;
      ++vars_;
      ss_ << "  %." << vars_ << " = sext i8 %." << vars_ - 1
          << " to i32\n"
             "  call void @writeln_char(i32 %."
          << vars_ << ")\n";
      break;
    case VarType::StringType:
      writeln_string_ = true;
      ss_ << "  call void @writeln_string(i8* %." << vars_ << ")\n";
      break;
    default: /* do nothing */
      return;
  }
}

void CodeGenerator::read_function(VarType type, size_t var) {
  switch (type) {
    case VarType::IntegerType:
      read_int_ = true;
      ss_ << "  call void @read_int(i32* %." << var << ")\n";
      break;
    case VarType::CharType:
      read_char_ = true;
      ss_ << "  call void @read_char(i8* %." << var << ")\n";
      break;
    case VarType::StringType:
      read_string_ = true;
      ss_ << "  call void @read_string(i8* %." << var << ")\n";
      break;
    default: /* do nothing */
      return;
  }
}

void CodeGenerator::load_variable(VarType type) {
  const auto string_type = type_to_string(type);
  ++vars_;
  ss_ << "  %." << vars_ << " = load " << string_type << ", " << string_type
      << "* %." << vars_ - 1 << "\n";
}

void CodeGenerator::add_op(Expr& expr, size_t i, std::string_view operation) {
  const auto op1 = expr.operands[i];
  const auto op2 = expr.operands[i + 1];
  ++vars_;
  ss_ << "  %." << vars_ << " = " << operation << " i32 %." << op1 << ", %."
      << op2 << "\n";
  expr.operands[i] = vars_;
  expr.operands.erase(expr.operands.begin() + i + 1);
  expr.operations.erase(expr.operations.begin() + i);
}

void CodeGenerator::parse_stacks(Expr& expr) {
  size_t i = 0;
  while (i < expr.operations.size()) {
    if (expr.operations[i] == Op::Star) {
      add_op(expr, i, "mul");
    } else if (expr.operations[i] == Op::Div) {
      add_op(expr, i, "sdiv");
    } else if (expr.operations[i] == Op::Mod) {
      add_op(expr, i, "srem");
    } else {
      ++i;
    }
  }
  while (!expr.operations.empty()) {
    if (expr.operations[0] == Op::Plus) {
      add_op(expr, 0, "add");
    } else {
      add_op(expr, 0, "sub");
    }
  }
}

void CodeGenerator::parse_expression(Expression& expression, Expr& expr) {
  auto* atom = expression.atom();
  if (atom != nullptr) {
    atom->accept(*this);
    const auto& signs = expression.signs();
    if (!signs.empty()) {
      auto minus = static_cast<bool>(
          std::count_if(
              signs.begin(),
              signs.end(),
              [](auto* sign) { return sign->type() == Op::Minus; }) %
          2);
      if (minus) {
        ++vars_;
        ss_ << "  %." << vars_ << " = sub i32 0, %." << vars_ - 1 << "\n";
      }
    }
    expr.operands.push_back(vars_);
    return;
  }

  if (expression.brackets()) {
    Expr subexpr;
    parse_expression(*(expression.operands()[0]), subexpr);
    parse_stacks(subexpr);
    expr.operands.push_back(vars_);
    return;
  }
  parse_expression(*(expression.operands()[0]), expr);
  expr.operations.push_back(expression.operation()->type());
  parse_expression(*(expression.operands()[1]), expr);
}

VarType CodeGenerator::get_ptr(Cell& value) {
  value.index()->accept(*this);
  const auto it = symbol_table_.find(value.text())->second;

  if (it.get_type() == VarType::StringType) {
    vars_ += 3;
    ss_ << "  %." << vars_ - 2 << " = sub nsw i32 %." << vars_ - 3 << ", 1\n"
        << "  %." << vars_ - 1 << " = sext i32 %." << vars_ - 2
        << " to i64\n  %." << vars_
        << " = getelementptr [255 x i8], [255 x i8]* %." << it.get_addr()
        << ", i64 0, i64 %." << vars_ - 1 << "\n";
    return VarType::CharType;
  }

  const auto type = type_to_string(it.get_type());
  if (it.get_min_index() != 0) {
    ++vars_;
    ss_ << "  %." << vars_ << " = sub i32 %." << vars_ - 1 << ", "
        << it.get_min_index() << "\n";
  }
  vars_ += 2;
  ss_ << "  %." << vars_ - 1 << " = sext i32 %." << vars_ - 2 << " to i64\n  %."
      << vars_ << " = getelementptr [" << it.get_size() << " x " << type
      << "], [" << it.get_size() << " x " << type << "]* %." << it.get_addr()
      << ", i64 0, i64 %." << vars_ - 1 << "\n";
  return it.get_type();
}

void CodeGenerator::visit(Header& member) {
  member.accept(*this);
  // not used
}

void CodeGenerator::visit(Constdecl& member) {
  for (const auto& constant : member.constdeclarations()) {
    constant->accept(*this);
  }
}

void CodeGenerator::visit(Constdeclaration& member) {
  member.expression()->accept(*this);
  auto& it = symbol_table_.find(member.constname()->text())->second;
  const auto type = type_to_string(it.get_type());
  ++vars_;
  ss_ << "  %." << vars_ << " = alloca " << type << "\n";
  it.set_addr(vars_);
  if (it.get_type() == VarType::StringType) {
    strings_ = true;
    ss_ << "  call void @strinit([255 x i8]* %." << vars_ << ")\n";
  } else {
    ss_ << "  store " << type << " %." << vars_ - 1 << ", " << type << "* %."
        << vars_ << "\n";
  }
}

void CodeGenerator::visit(Expression& member) {
  if (member.type() == VarType::StringType) {
    member.atom()->accept(*this);
    return;
  }
  Expr expr;
  parse_expression(member, expr);
  if (!expr.operations.empty()) {
    parse_stacks(expr);
  }
}

void CodeGenerator::visit(Boolexpr& member) {
  member.operand1()->accept(*this);
  const auto op1 = vars_;
  member.operand2()->accept(*this);
  const auto op2 = vars_;
  const auto type = type_to_string(member.type());
  std::string operation;

  switch (member.booloperation()->type()) {
    case BoolOp::Equal:
      operation = "eq";
      break;
    case BoolOp::MoreThen:
      operation = "sgt";
      break;
    case BoolOp::LessThen:
      operation = "slt";
      break;
    case BoolOp::NotEqual:
      operation = "ne";
      break;
    case BoolOp::NotMore:
      operation = "sle";
      break;
    case BoolOp::NotLess:
      operation = "sge";
      break;
  }
  ++vars_;
  ss_ << "  %." << vars_ << " = icmp " << operation << " " << type << "%."
      << op1 << ", %." << op2 << "\n";
}

void CodeGenerator::visit(Vardecl& member) {
  for (const auto& variable : member.declarations()) {
    variable->accept(*this);
  }
  ss_ << "\n";
}

void CodeGenerator::visit(Declaration& member) {
  for (const auto& varname : member.varnames()) {
    ++vars_;
    ss_ << "  %." << vars_ << " = alloca ";
    auto& it = symbol_table_.find(varname->text())->second;
    const auto type = type_to_string(it.get_type());
    it.set_addr(vars_);
    if (it.get_form() == Form::Variable) {
      ss_ << type << "\n";
      if (it.get_type() == VarType::StringType) {
        strings_ = true;
        ss_ << "  call void @strinit([255 x i8]* %." << vars_ << ")\n";
      }
    } else {
      auto* array_node = dynamic_cast<Arraytype*>(member.vartype());
      const auto min_index =
          std::stoi(array_node->interval()->lborder()->text());
      const auto size =
          std::stoi(array_node->interval()->rborder()->text()) - min_index;
      ss_ << "[" << size << " x " << type << "]\n";
      it.set_array_data(std::make_pair(min_index, size));
    }
  }
}

void CodeGenerator::visit(Simpletype& vartype) {
  vartype.accept(*this);
  // not used
}

void CodeGenerator::visit(Interval& member) {
  member.accept(*this);
  // not used
}

void CodeGenerator::visit(Arraytype& vartype) {
  vartype.accept(*this);
  // not used
}

void CodeGenerator::visit(Block& statement) {
  for (const auto& component : statement.components()) {
    component->accept(*this);
  }
}

void CodeGenerator::visit(Functioncall& statement) {
  if (statement.functionname()->type() == FuncName::Write) {
    for (const auto& variable : statement.variables()) {
      variable->accept(*this);
      write_function(variable->type());
    }
    for (const auto& argument : statement.arguments()) {
      argument->accept(*this);
      write_function(argument->type());
    }
    ss_ << "\n";
  } else if (statement.functionname()->type() == FuncName::Writeln) {
    auto variable = statement.variables().begin();
    while (variable != statement.variables().end() &&
           variable + 1 != statement.variables().end()) {
      (*variable)->accept(*this);
      write_function((*variable)->type());
      ++variable;
    }
    if (variable != statement.variables().end()) {
      (*variable)->accept(*this);
      writeln_function((*variable)->type());
    }
    auto argument = statement.arguments().begin();
    while (argument != statement.arguments().end() &&
           argument + 1 != statement.arguments().end()) {
      (*argument)->accept(*this);
      write_function((*argument)->type());
      ++argument;
    }
    if (argument != statement.arguments().end()) {
      (*argument)->accept(*this);
      writeln_function((*argument)->type());
    }
    ss_ << "\n";
  } else if (statement.functionname()->type() == FuncName::Readln) {
    for (const auto& variable : statement.variables()) {
      const auto& it = symbol_table_.find(variable->text())->second;
      if (variable->type() == VarType::StringType) {
        variable->accept(*this);
        read_function(variable->type(), vars_);
      } else if (
          it.get_form() == Form::Array ||
          it.get_type() == VarType::StringType) {
        const auto type = get_ptr(*(dynamic_cast<Cell*>(variable)));
        read_function(type, vars_);
      } else {
        read_function(it.get_type(), it.get_addr());
      }
    }
    ss_ << "\n";
  }
}

void CodeGenerator::visit(Assignment& statement) {
  statement.expression()->accept(*this);
  auto* cell = statement.cell();
  auto* varname = statement.varname();
  if (cell == nullptr && varname->type() == VarType::StringType) {
    if (statement.expression()->type() == VarType::CharType) {
      char_convert_ = true;
      vars_ += 2;
      ss_ << "  %." << vars_ - 1
          << " = alloca [255 x i8]\n"
             "  %."
          << vars_ << " = call i8* @tostr(i8 %." << vars_ - 2
          << ", [255 x i8]* %." << vars_ - 1 << ")\n\n";
    }
    const auto& it = symbol_table_.find(varname->text())->second;
    ++vars_;
    ss_ << "  %." << vars_ << " = getelementptr [255 x i8], [255 x i8]* %."
        << it.get_addr() << ", i64 0, i64 0\n";
    if (statement.modification()->type() == ModType::Assignment) {
      ss_ << "  call i8* @strcpy(i8* %." << vars_ << ", i8* %." << vars_ - 1
          << ")\n";
    } else {
      ss_ << "  call i8* @strcat(i8* %." << vars_ << ", i8* %." << vars_ - 1
          << ")\n";
    }
    return;
  }
  const auto rvalue = vars_;
  if (statement.modification()->type() != ModType::Assignment) {
    if (cell != nullptr) {
      cell->accept(*this);
    } else {
      varname->accept(*this);
    }
    std::string operation;
    switch (statement.modification()->type()) {
      case ModType::Add:
        operation = "add";
        break;
      case ModType::Reduce:
        operation = "sub";
        break;
      case ModType::Multiply:
        operation = "mul";
        break;
      default: /* do nothing */
        break;
    }
    ++vars_;
    ss_ << "  %." << vars_ << " = " << operation << " i32 %." << vars_ - 1
        << ", %." << rvalue << "\n";
  }
  if (cell == nullptr) {
    const auto& it = symbol_table_.find(varname->text())->second;
    const auto type = type_to_string(it.get_type());
    ss_ << "  store " << type << " %." << vars_ << ", " << type << "* %."
        << it.get_addr() << "\n";
  } else {
    const auto& it = symbol_table_.find(cell->varname()->text())->second;
    std::string type;
    if (it.get_type() == VarType::StringType) {
      type = "i8";
    } else {
      type = type_to_string(it.get_type());
    }
    if (statement.modification()->type() == ModType::Assignment) {
      get_ptr(*cell);
      ss_ << "  store " << type << " %." << rvalue << ", " << type << "* %."
          << vars_ << "\n";
    } else {
      ss_ << "  store " << type << " %." << vars_ << ", " << type << "* %."
          << vars_ - 2 << "\n";
    }
  }
}

void CodeGenerator::visit(While& statement) {
  ++vars_;
  const auto condition_branch = vars_;
  ss_ << "  br label %." << condition_branch << "\n\n." << condition_branch
      << ":\n";
  statement.boolexpr()->accept(*this);
  const auto branch1 = vars_ + 1;
  const auto branch2 = vars_ + 2;
  ss_ << "  br i1 %." << vars_ << ", label %." << branch1 << ", label %."
      << branch2 << "\n\n." << branch1 << ":\n";
  vars_ += 2;
  statement.statement()->accept(*this);
  ss_ << "  br label %." << condition_branch << "\n\n." << branch2 << ":\n";
}

void CodeGenerator::visit(Branch& statement) {
  statement.boolexpr()->accept(*this);
  const auto branch1 = vars_ + 1;
  const auto branch2 = vars_ + 2;
  ss_ << "  br i1 %." << vars_ << ", label %." << branch1 << ", label %."
      << branch2 << "\n\n." << branch1 << ":\n";

  auto* alternative = statement.alternative();
  if (alternative != nullptr) {
    vars_ += 3;
    const auto branch3 = vars_;
    statement.statement()->accept(*this);
    ss_ << "  br label %." << branch3 << "\n\n." << branch2 << ":\n";
    alternative->accept(*this);
    ss_ << "  br label %." << branch3 << "\n\n." << branch3 << ":\n";
    return;
  }

  vars_ += 2;
  statement.statement()->accept(*this);
  ss_ << "  br label %." << branch2 << "\n\n." << branch2 << ":\n";
}

void CodeGenerator::visit(Operation& value) {
  value.accept(*this);
  // not used
}

void CodeGenerator::visit(Booloperation& value) {
  value.accept(*this);
  // not used
}

void CodeGenerator::visit(Modification& value) {
  value.accept(*this);
  // not used
}

void CodeGenerator::visit(Functionname& value) {
  value.accept(*this);
  // not used
}

void CodeGenerator::visit(Id& value) {
  auto it = symbol_table_.find(value.text())->second;
  ++vars_;
  if (value.type() == VarType::StringType) {
    ss_ << "  %." << vars_ << " = getelementptr [255 x i8], [255 x i8]* %."
        << it.get_addr() << ", i64 0, i64 0\n\n";
    return;
  }
  const auto type = type_to_string(it.get_type());
  ss_ << "  %." << vars_ << " = load " << type << ", " << type << "* %."
      << it.get_addr() << "\n\n";
}

void CodeGenerator::visit(Cell& value) {
  load_variable(get_ptr(value));
}

void CodeGenerator::visit(Char& value) {
  ++vars_;
  ss_ << "  %." << vars_ << " = alloca i8\n"
      << "  store i8 " << static_cast<int>(value.text()[0]) << ", i8* %."
      << vars_ << "\n";
  load_variable(value.type());
  ss_ << "\n";
}

void CodeGenerator::visit(Stringliteral& value) {
  const auto size = value.text().size() + 1;
  vars_ += 2;
  conststrings_ << "@.str." << vars_ - 1 << " = constant [" << size
                << " x i8] c\"" << value.text() << "\\00\"\n";
  ss_ << "  %." << vars_ << " = getelementptr [" << size << " x i8], [" << size
      << " x i8]* @.str." << vars_ - 1 << ", i64 0, i64 0\n";
}

void CodeGenerator::visit(Int& value) {
  ++vars_;
  ss_ << "  %." << vars_ << " = alloca i32\n"
      << "  store i32 " << value.text() << ", i32* %." << vars_ << "\n";
  load_variable(value.type());
  ss_ << "\n";
}

}  // namespace pascal::ast
