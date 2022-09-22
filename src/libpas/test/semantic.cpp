#include <libpas/compiler.hpp>

#include <PascalLexer.h>
#include <antlr4-runtime.h>
#include <gtest/gtest.h>

#include <sstream>
#include <string>

namespace pascal::test {

TEST(SemanticSuite, ValidProgram) {
  std::stringstream in(R"(
    {Test Helloworld program}
    Program HelloWorld;
    begin
        Writeln('Hello world!');
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_TRUE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_TRUE(error_stream.str().empty());
}

TEST(SemanticSuite, ValidProgram2) {
  std::stringstream in(R"(
    {Test ArrMin program}
    program ArrMin;
    const 
        n = 10;
    var 
        a: array[1..10] of integer;
        min, i: integer;
    begin
        i := 1;
        while i < n do
        begin
            a[i] := n mod (n - i) + 3;
            i += 1;
        end;
        min := a[1];
        i := 2;
        while i < n do
        begin
            if a[i]<min then
                min := a[i];
            i += 1;
        end;
        writeln('Min: ', min);
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_TRUE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_TRUE(error_stream.str().empty());
}

TEST(SemanticSuite, ValidProgram3) {
  std::stringstream in(R"(
    {Test GCD program}
    program GCD;
    var
        z, i, a, b, m: integer;
    begin
        writeln('Enter a and b');
        readln(a,b);
        if a < b then 
            m := a
        else
            m := b;
        i := 1;
        while i <= m do
        begin
            if (a mod i = 0) then
                if (b mod i = 0) then 
                    z := i;
            i += 1;
        end;
        writeln('GCD: ', z);
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_TRUE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_TRUE(error_stream.str().empty());
}

TEST(SemanticSuite, InvalidProgram) {
  std::stringstream in(R"(
    program TEST;
    const
        a = 42;
        a = 69;
    begin
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(
      error_stream.str(),
      "Error: Repeat declaration of const identifier 'a'\n");
}

TEST(SemanticSuite, InvalidProgram2) {
  std::stringstream in(R"(
    program TEST;
    var
        a : integer;
        a : char;
    begin
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(
      error_stream.str(), "Error: Repeat declaration of identifier 'a'\n");
}

TEST(SemanticSuite, InvalidProgram3) {
  std::stringstream in(R"(
    program TEST;
    const
        a = 42;
    begin
        a := 69;
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(
      error_stream.str(), "Error: Cannot assign new value to constant 'a'\n");
}

TEST(SemanticSuite, InvalidProgram4) {
  std::stringstream in(R"(
    program TEST;
    var
        a : string;
    begin
        a *= 'abc';
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(
      error_stream.str(),
      "Error: Incompatible operation for string expression\n");
}

TEST(SemanticSuite, InvalidProgram5) {
  std::stringstream in(R"(
    program TEST;
    var
        a : char;
    begin
        a += 'a';
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(
      error_stream.str(),
      "Error: Incompatible operation for char expression\n");
}

TEST(SemanticSuite, InvalidProgram6) {
  std::stringstream in(R"(
    program TEST;
    var
        a : char;
    begin
        a += 'aa';
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(
      error_stream.str(),
      "Error: Incompatible operation for char expression\n");
}

TEST(SemanticSuite, ValidProgram4) {
  std::stringstream in(R"(
    program TEST;
    var
        a : string;
        b : char;
    begin
        b := 'b';
        a += b;
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_TRUE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_TRUE(error_stream.str().empty());
}

TEST(SemanticSuite, InvalidProgram7) {
  std::stringstream in(R"(
    program TEST;
    var
        a : char;
    begin
        a += 'abc';
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(
      error_stream.str(),
      "Error: Incompatible operation for char expression\n");
}

TEST(SemanticSuite, InvalidProgram8) {
  std::stringstream in(R"(
    program TEST;
    var
        a : char;
    begin
        readln(a, 42);
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(
      error_stream.str(),
      "Error: Only identifiers or array cells expected in read function "
      "arguments\n");
}

TEST(SemanticSuite, InvalidProgram9) {
  std::stringstream in(R"(
    program TEST;
    begin
        readln(a);
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(error_stream.str(), "Error: Unknown identifier 'a'\n");
}

TEST(SemanticSuite, InvalidProgram10) {
  std::stringstream in(R"(
    program TEST;
    const
        a = 42;
    begin
        readln(a);
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(
      error_stream.str(), "Error: Cannot assign new value to constant 'a'\n");
}

TEST(SemanticSuite, InvalidProgram11) {
  std::stringstream in(R"(
    program TEST;
    const
        a = -'a';
    begin
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(
      error_stream.str(), "Error: Only integer expression can be signed\n");
}

TEST(SemanticSuite, InvalidProgram12) {
  std::stringstream in(R"(
    program TEST;
    var
        a : char;
        b : string;
    begin
        a := a * b;
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(
      error_stream.str(),
      "Error: Incompatible operands types for expression\n");
}

TEST(SemanticSuite, InvalidProgram13) {
  std::stringstream in(R"(
    program TEST;
    var
        a : char;
        b : string;
    begin
        a := b * a;
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(
      error_stream.str(),
      "Error: Incompatible operands types for expression\n");
}

TEST(SemanticSuite, InvalidProgram14) {
  std::stringstream in(R"(
    program TEST;
    var
        a : char;
        b : integer;
    begin
        b := 42 + a;
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(
      error_stream.str(),
      "Error: Incompatible operands types for expression\n");
}

TEST(SemanticSuite, InvalidProgram15) {
  std::stringstream in(R"(
    program TEST;
    var
        a : string;
    begin
        a *= 'abc';
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(
      error_stream.str(),
      "Error: Incompatible operation for string expression\n");
}

TEST(SemanticSuite, InvalidProgram16) {
  std::stringstream in(R"(
    program TEST;
    var
        a : char;
    begin
        a *= 'a';
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(
      error_stream.str(),
      "Error: Incompatible operation for char expression\n");
}

TEST(SemanticSuite, InvalidProgram17) {
  std::stringstream in(R"(
    program TEST;
    begin
        if ('a' < 42) then
            writeln('abc');
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(
      error_stream.str(),
      "Error: Different types of boolean expression operands\n");
}

TEST(SemanticSuite, InvalidProgram18) {
  std::stringstream in(R"(
    program TEST;
    begin
        if ('a' < 'abc') then
            writeln('abc');
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(
      error_stream.str(),
      "Error: Different types of boolean expression operands\n");
}

TEST(SemanticSuite, InvalidProgram19) {
  std::stringstream in(R"(
    program TEST;
    begin
        a[1] := 42;
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(
      error_stream.str(), "Error: Unknown identifier 'a' in array name\n");
}

TEST(SemanticSuite, InvalidProgram20) {
  std::stringstream in(R"(
    program TEST;
    var
        a : integer;
    begin
        a[1] := 42;
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(
      error_stream.str(),
      "Error: Identifier 'a' is not an array or string name\n");
}

TEST(SemanticSuite, InvalidProgram21) {
  std::stringstream in(R"(
    program TEST;
    var
        a : array[0..10] of integer;
    begin
        a['abc'] := 42;
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(error_stream.str(), "Error: Invalid index type of 'a'\n");
}

TEST(SemanticSuite, InvalidProgram22) {
  std::stringstream in(R"(
    program TEST;
    var
        a : integer;
    begin
        a := b;
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(error_stream.str(), "Error: Unknown identifier 'b'\n");
}

TEST(SemanticSuite, InvalidProgram23) {
  std::stringstream in(R"(
    program TEST;
    var
        a : integer;
        b : array[1..10] of integer;
    begin
        a := b;
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_FALSE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  EXPECT_EQ(error_stream.str(), "Error: Identifier 'b' is an array name\n");
}

}  // namespace pascal::test
