#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace pascal::ast {

enum class Form { Constant, Variable, Array, ProgName, NoForm };

enum class VarType { IntegerType, CharType, StringType, NoType };

enum class Op { Plus, Minus, Star, Div, Mod };

enum class BoolOp { Equal, MoreThen, LessThen, NotEqual, NotMore, NotLess };

enum class ModType { Assignment, Add, Reduce, Multiply };

enum class FuncName { Readln, Write, Writeln };

class Symbol {
 public:
  using ArrayData = std::optional<std::pair<size_t, size_t>>;
  Symbol(
      Form form = Form::NoForm,
      VarType type = VarType::NoType,
      size_t addr = 0,
      ArrayData array_data = {})
      : form_(form), type_(type), addr_(addr), array_data_(array_data) {}
  Form get_form() const { return form_; }
  VarType get_type() const { return type_; }
  size_t get_addr() const { return addr_; }
  size_t get_min_index() const { return array_data_->first; }
  size_t get_size() const { return array_data_->second; }
  void set_addr(size_t addr) { addr_ = addr; }
  void set_array_data(ArrayData array_data) { array_data_ = array_data; }

 private:
  Form form_;
  VarType type_;
  size_t addr_;
  ArrayData array_data_;
};

using SymbolTable = std::unordered_map<std::string, Symbol>;

}  // namespace pascal::ast
