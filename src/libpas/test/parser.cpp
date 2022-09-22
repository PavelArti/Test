#include <libpas/compiler.hpp>

#include <PascalLexer.h>
#include <antlr4-runtime.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <exception>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

namespace pascal::test {

static size_t count_indentation(const std::string& str) {
  const auto it = std::find_if_not(
      str.begin(), str.end(), [](char ch) { return std::isspace(ch); });
  return std::distance(str.begin(), it);
}

static std::string dedent(const std::string& str) {
  std::stringstream ss(str);
  std::stringstream out;
  std::string line;

  std::getline(ss, line);

  std::getline(ss, line);
  const auto indentation = count_indentation(line);
  out << line.substr(std::min(indentation, line.size())) << "\n";

  while (std::getline(ss, line)) {
    out << line.substr(std::min(indentation, line.size())) << "\n";
  }

  return out.str();
}

TEST(ParserSuite, ValidProgram) {
  std::stringstream in(R"(
    {Test HelloWorld program}
    Program HelloWorld;
    begin
        Writeln('Hello world!');
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());

  std::stringstream ast_str;
  pascal::dump_ast(parse_result.program_, ast_str);
  EXPECT_EQ(ast_str.str(), dedent(R"(
    <?xml version="1.0"?>
    <pascal>
      <progname>
        <id>helloworld</id>
      </progname>
      <block>
        <functioncall>
          <functionname>writeln</functionname>
          <argument>
            <string>Hello world!</string>
          </argument>
        </functioncall>
      </block>
    </pascal>)"));
}

TEST(ParserSuite, ValidProgram2) {
  std::stringstream in(R"(
    {Test HelloWorld program with variables}
    program HelloWorld;
    var
        a, b, c : integer;
        d : char;
        e : string;
        f : array[5..12] of string;
    begin
        writeln('Hello world!');
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());

  std::stringstream ast_str;
  pascal::dump_ast(parse_result.program_, ast_str);
  EXPECT_EQ(ast_str.str(), dedent(R"(
    <?xml version="1.0"?>
    <pascal>
      <progname>
        <id>helloworld</id>
      </progname>
      <vardecl>
        <declaration>
          <varname>
            <id>a</id>
          </varname>
          <varname>
            <id>b</id>
          </varname>
          <varname>
            <id>c</id>
          </varname>
          <vartype>integer</vartype>
        </declaration>
        <declaration>
          <varname>
            <id>d</id>
          </varname>
          <vartype>char</vartype>
        </declaration>
        <declaration>
          <varname>
            <id>e</id>
          </varname>
          <vartype>string</vartype>
        </declaration>
        <declaration>
          <varname>
            <id>f</id>
          </varname>
          <arraytype>
            <interval>
              <lborder>
                <integer>5</integer>
              </lborder>
              <rborder>
                <integer>12</integer>
              </rborder>
            </interval>
            <vartype>string</vartype>
          </arraytype>
        </declaration>
      </vardecl>
      <block>
        <functioncall>
          <functionname>writeln</functionname>
          <argument>
            <string>Hello world!</string>
          </argument>
        </functioncall>
      </block>
    </pascal>)"));
}

TEST(ParserSuite, ValidProgram3) {
  std::stringstream in(R"(
    {Test HelloWorld program with assignments}
    program HelloWorld;
    const
        a = (1 * 2) + 3;
        b = 4 + (5 - b);
        c = 'str';
        d = 1 mod 2;
    var
        e, f, g : integer;
    begin
        e := (1 * 4);
        a [e + 1] += 5 + 5;
        writeln('Hello world!' + 'a');
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());

  std::stringstream ast_str;
  pascal::dump_ast(parse_result.program_, ast_str);
  EXPECT_EQ(ast_str.str(), dedent(R"(
    <?xml version="1.0"?>
    <pascal>
      <progname>
        <id>helloworld</id>
      </progname>
      <constdecl>
        <constdeclaration>
          <constname>
            <id>a</id>
          </constname>
          <value>
            <braces>
              <integer>1</integer>
              <operation>*</operation>
              <integer>2</integer>
            </braces>
            <operation>+</operation>
            <integer>3</integer>
          </value>
        </constdeclaration>
        <constdeclaration>
          <constname>
            <id>b</id>
          </constname>
          <value>
            <integer>4</integer>
            <operation>+</operation>
            <braces>
              <integer>5</integer>
              <operation>-</operation>
              <id>b</id>
            </braces>
          </value>
        </constdeclaration>
        <constdeclaration>
          <constname>
            <id>c</id>
          </constname>
          <value>
            <string>str</string>
          </value>
        </constdeclaration>
        <constdeclaration>
          <constname>
            <id>d</id>
          </constname>
          <value>
            <integer>1</integer>
            <operation>mod</operation>
            <integer>2</integer>
          </value>
        </constdeclaration>
      </constdecl>
      <vardecl>
        <declaration>
          <varname>
            <id>e</id>
          </varname>
          <varname>
            <id>f</id>
          </varname>
          <varname>
            <id>g</id>
          </varname>
          <vartype>integer</vartype>
        </declaration>
      </vardecl>
      <block>
        <assignment>
          <variable>
            <id>e</id>
          </variable>
          <modification>:=</modification>
          <value>
            <braces>
              <integer>1</integer>
              <operation>*</operation>
              <integer>4</integer>
            </braces>
          </value>
        </assignment>
        <assignment>
          <variable>
            <cell>
              <id>a</id>
              <index>
                <id>e</id>
                <operation>+</operation>
                <integer>1</integer>
              </index>
            </cell>
          </variable>
          <modification>+=</modification>
          <value>
            <integer>5</integer>
            <operation>+</operation>
            <integer>5</integer>
          </value>
        </assignment>
        <functioncall>
          <functionname>writeln</functionname>
          <argument>
            <string>Hello world!</string>
            <operation>+</operation>
            <char>a</char>
          </argument>
        </functioncall>
      </block>
    </pascal>)"));
}

TEST(ParserSuite, ValidProgram4) {
  std::stringstream in(R"(
    {Test HelloWorld program with while loop}
    program HelloWorld;
    var
      n : integer;
    begin
      n := 10;
      while (n + 1 > 5) do
      begin
        n -= 1;
      end;
      writeln ('Hello World');
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());

  std::stringstream ast_str;
  pascal::dump_ast(parse_result.program_, ast_str);
  EXPECT_EQ(ast_str.str(), dedent(R"(
    <?xml version="1.0"?>
    <pascal>
      <progname>
        <id>helloworld</id>
      </progname>
      <vardecl>
        <declaration>
          <varname>
            <id>n</id>
          </varname>
          <vartype>integer</vartype>
        </declaration>
      </vardecl>
      <block>
        <assignment>
          <variable>
            <id>n</id>
          </variable>
          <modification>:=</modification>
          <value>
            <integer>10</integer>
          </value>
        </assignment>
        <whileloop>
          <condition>
            <id>n</id>
            <operation>+</operation>
            <integer>1</integer>
            <booloperation>&gt;</booloperation>
            <integer>5</integer>
          </condition>
          <body>
            <block>
              <assignment>
                <variable>
                  <id>n</id>
                </variable>
                <modification>-=</modification>
                <value>
                  <integer>1</integer>
                </value>
              </assignment>
            </block>
          </body>
        </whileloop>
        <functioncall>
          <functionname>writeln</functionname>
          <argument>
            <string>Hello World</string>
          </argument>
        </functioncall>
      </block>
    </pascal>)"));
}

TEST(ParserSuite, ValidProgram5) {
  std::stringstream in(R"(
    {Test HelloWorld program with branch}
    program HelloWorld;
    var
      a, b, n : integer;
    begin
      a := 1;
      b := 2;
      n := 10;
      if (a > b) then
        begin
          n := 20
        end
      else if b > a then
        begin
          n := 30
        end
      else
        n := 0;
      writeln ('Hello World');
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());

  std::stringstream ast_str;
  pascal::dump_ast(parse_result.program_, ast_str);
  EXPECT_EQ(ast_str.str(), dedent(R"(
    <?xml version="1.0"?>
    <pascal>
      <progname>
        <id>helloworld</id>
      </progname>
      <vardecl>
        <declaration>
          <varname>
            <id>a</id>
          </varname>
          <varname>
            <id>b</id>
          </varname>
          <varname>
            <id>n</id>
          </varname>
          <vartype>integer</vartype>
        </declaration>
      </vardecl>
      <block>
        <assignment>
          <variable>
            <id>a</id>
          </variable>
          <modification>:=</modification>
          <value>
            <integer>1</integer>
          </value>
        </assignment>
        <assignment>
          <variable>
            <id>b</id>
          </variable>
          <modification>:=</modification>
          <value>
            <integer>2</integer>
          </value>
        </assignment>
        <assignment>
          <variable>
            <id>n</id>
          </variable>
          <modification>:=</modification>
          <value>
            <integer>10</integer>
          </value>
        </assignment>
        <branch>
          <condition>
            <id>a</id>
            <booloperation>&gt;</booloperation>
            <id>b</id>
          </condition>
          <body>
            <block>
              <assignment>
                <variable>
                  <id>n</id>
                </variable>
                <modification>:=</modification>
                <value>
                  <integer>20</integer>
                </value>
              </assignment>
            </block>
          </body>
          <alternative>
            <branch>
              <condition>
                <id>b</id>
                <booloperation>&gt;</booloperation>
                <id>a</id>
              </condition>
              <body>
                <block>
                  <assignment>
                    <variable>
                      <id>n</id>
                    </variable>
                    <modification>:=</modification>
                    <value>
                      <integer>30</integer>
                    </value>
                  </assignment>
                </block>
              </body>
              <alternative>
                <assignment>
                  <variable>
                    <id>n</id>
                  </variable>
                  <modification>:=</modification>
                  <value>
                    <integer>0</integer>
                  </value>
                </assignment>
              </alternative>
            </branch>
          </alternative>
        </branch>
        <functioncall>
          <functionname>writeln</functionname>
          <argument>
            <string>Hello World</string>
          </argument>
        </functioncall>
      </block>
    </pascal>)"));
}

TEST(ParserSuite, InvalidProgram) {
  std::stringstream in(R"(
    program HelloWorld;
    begin
        writeln('Hello world!');
    end
    )");
  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);
  auto parse_result = pascal::parse(lexer);

  EXPECT_FALSE(parse_result.errors_.empty());

  std::stringstream errors;
  pascal::dump_errors(parse_result.errors_, errors);
  EXPECT_EQ(errors.str(), "6:4 missing '.' at '<EOF>'\n");
}

TEST(ParserSuite, InvalidProgram2) {
  std::stringstream in(R"(
    program HelloWorld;
    begin
        writeln('Hello world!')
        writeln('Hello world!')
    end.
    )");
  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);
  auto parse_result = pascal::parse(lexer);

  EXPECT_FALSE(parse_result.errors_.empty());

  std::stringstream errors;
  pascal::dump_errors(parse_result.errors_, errors);
  EXPECT_EQ(
      errors.str(), "5:8 mismatched input 'writeln' expecting {'end', ';'}\n");
}

TEST(ParserSuite, InvalidProgram3) {
  std::stringstream in(R"(
    program HelloWorld;
    begin
        writeln('Hello world!')
    end
    var
       a : integer;
    )");
  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);
  auto parse_result = pascal::parse(lexer);

  EXPECT_FALSE(parse_result.errors_.empty());

  std::stringstream errors;
  pascal::dump_errors(parse_result.errors_, errors);
  EXPECT_EQ(errors.str(), "6:4 mismatched input 'var' expecting '.'\n");
}

TEST(ParserSuite, InvalidProgram4) {
  std::stringstream in(R"(
    program HelloWorld;
    const
        a = (3 + 2) +;
    begin
        writeln('Hello world!')
    end
    )");
  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);
  auto parse_result = pascal::parse(lexer);

  EXPECT_FALSE(parse_result.errors_.empty());

  std::stringstream errors;
  pascal::dump_errors(parse_result.errors_, errors);
  EXPECT_EQ(
      errors.str(), "4:21 no viable alternative at input 'consta=(3+2)+;'\n");
}

TEST(ParserSuite, InvalidProgram5) {
  std::stringstream in(R"(
    program HelloWorld;
    const
        a = (3 + 2)
    begin
        writeln('Hello world!')
    end
    )");
  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);
  auto parse_result = pascal::parse(lexer);

  EXPECT_FALSE(parse_result.errors_.empty());

  std::stringstream errors;
  pascal::dump_errors(parse_result.errors_, errors);
  EXPECT_EQ(
      errors.str(), "5:4 no viable alternative at input 'consta=(3+2)begin'\n");
}

TEST(ParserSuite, InvalidProgram6) {
  std::stringstream in(R"(
    program HelloWorld;
    begin
        a[] := 5 + 5;
    end.
    )");
  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);
  auto parse_result = pascal::parse(lexer);

  EXPECT_FALSE(parse_result.errors_.empty());

  std::stringstream errors;
  pascal::dump_errors(parse_result.errors_, errors);
  EXPECT_EQ(
      errors.str(),
      "4:10 mismatched input ']' "
      "expecting {'+', '-', '(', CHARACTER, STRINGLITERAL, INT, ID}\n");
}

TEST(ParserSuite, InvalidProgram7) {
  std::stringstream in(R"(
    program HelloWorld;
    begin
        a = 5 + 5;
    end.
    )");
  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);
  auto parse_result = pascal::parse(lexer);

  EXPECT_FALSE(parse_result.errors_.empty());

  std::stringstream errors;
  pascal::dump_errors(parse_result.errors_, errors);
  EXPECT_EQ(errors.str(), "4:10 no viable alternative at input 'a='\n");
}

}  // namespace pascal::test
