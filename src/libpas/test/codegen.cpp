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

TEST(CodegenSuite, HelloWorld) {
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

  std::stringstream llvm_ir_str;
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_TRUE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  pascal::code_generate(parse_result.program_, symbol_table, llvm_ir_str);
  EXPECT_TRUE(error_stream.str().empty());
  EXPECT_EQ(llvm_ir_str.str(), dedent(R"(
    target triple = "x86_64-pc-linux-gnu"
    declare i32 @printf(i8* %format, ...)
    declare i32 @__isoc99_scanf(i8*, ...)


    @.str.strln = constant [4 x i8] c"%s\0A\00"
    define void @writeln_string(i8* %x) {
      call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.str.strln, i64 0, i64 0), i8* %x)
      ret void
    }

    @.str.1 = constant [13 x i8] c"Hello world!\00"

    define i32 @main() {
    start:
      %.2 = getelementptr [13 x i8], [13 x i8]* @.str.1, i64 0, i64 0
      call void @writeln_string(i8* %.2)

      ret i32 0
    })"));
}

TEST(CodegenSuite, GCD) {
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

  std::stringstream llvm_ir_str;
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_TRUE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  pascal::code_generate(parse_result.program_, symbol_table, llvm_ir_str);
  EXPECT_TRUE(error_stream.str().empty());
  EXPECT_EQ(llvm_ir_str.str(), dedent(R"(
    target triple = "x86_64-pc-linux-gnu"
    declare i32 @printf(i8* %format, ...)
    declare i32 @__isoc99_scanf(i8*, ...)

    @.str.int = constant [3 x i8] c"%d\00"
    @.str.str = constant [3 x i8] c"%s\00"

    define void @read_int(i32* %x) {
      call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr  ([3 x i8], [3 x i8]* @.str.int, i64 0, i64 0), i32* %x)
      ret void
    }

    define void @write_string(i8* %x) {
      call i32 (i8*, ...) @printf(i8* getelementptr ([3 x i8], [3 x i8]* @.str.str, i64 0, i64 0), i8* %x)
      ret void
    }

    @.str.intln = constant [4 x i8] c"%d\0A\00"
    define void @writeln_int(i32 %x) {
      call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.str.intln, i64 0, i64 0), i32 %x)
      ret void
    }

    @.str.strln = constant [4 x i8] c"%s\0A\00"
    define void @writeln_string(i8* %x) {
      call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.str.strln, i64 0, i64 0), i8* %x)
      ret void
    }

    @.str.6 = constant [14 x i8] c"Enter a and b\00"
    @.str.45 = constant [6 x i8] c"GCD: \00"

    define i32 @main() {
    start:
      %.1 = alloca i32
      %.2 = alloca i32
      %.3 = alloca i32
      %.4 = alloca i32
      %.5 = alloca i32

      %.7 = getelementptr [14 x i8], [14 x i8]* @.str.6, i64 0, i64 0
      call void @writeln_string(i8* %.7)

      call void @read_int(i32* %.3)
      call void @read_int(i32* %.4)

      %.8 = load i32, i32* %.3

      %.9 = load i32, i32* %.4

      %.10 = icmp slt i32%.8, %.9
      br i1 %.10, label %.11, label %.12

    .11:
      %.14 = load i32, i32* %.3

      store i32 %.14, i32* %.5
      br label %.13

    .12:
      %.15 = load i32, i32* %.4

      store i32 %.15, i32* %.5
      br label %.13

    .13:
      %.16 = alloca i32
      store i32 1, i32* %.16
      %.17 = load i32, i32* %.16

      store i32 %.17, i32* %.2
      br label %.18

    .18:
      %.19 = load i32, i32* %.2

      %.20 = load i32, i32* %.5

      %.21 = icmp sle i32%.19, %.20
      br i1 %.21, label %.22, label %.23

    .22:
      %.24 = load i32, i32* %.3

      %.25 = load i32, i32* %.2

      %.26 = srem i32 %.24, %.25
      %.27 = alloca i32
      store i32 0, i32* %.27
      %.28 = load i32, i32* %.27

      %.29 = icmp eq i32%.26, %.28
      br i1 %.29, label %.30, label %.31

    .30:
      %.32 = load i32, i32* %.4

      %.33 = load i32, i32* %.2

      %.34 = srem i32 %.32, %.33
      %.35 = alloca i32
      store i32 0, i32* %.35
      %.36 = load i32, i32* %.35

      %.37 = icmp eq i32%.34, %.36
      br i1 %.37, label %.38, label %.39

    .38:
      %.40 = load i32, i32* %.2

      store i32 %.40, i32* %.1
      br label %.39

    .39:
      br label %.31

    .31:
      %.41 = alloca i32
      store i32 1, i32* %.41
      %.42 = load i32, i32* %.41

      %.43 = load i32, i32* %.2

      %.44 = add i32 %.43, %.42
      store i32 %.44, i32* %.2
      br label %.18

    .23:
      %.46 = getelementptr [6 x i8], [6 x i8]* @.str.45, i64 0, i64 0
      call void @write_string(i8* %.46)
      %.47 = load i32, i32* %.1

      call void @writeln_int(i32 %.47)

      ret i32 0
    })"));
}

TEST(CodegenSuite, ArrMin) {
  std::stringstream in(R"(
    {Test ArrMin program}
    program ArrMin;
    var 
        a: array[1..100] of integer;
        min, i, n: integer;
    begin
        write('Enter array size: ');
        readln(n);
        i := 1;
        while i <= n do
        begin
           write('Enter ',i,' element: ');
           readln(a[i]);
           i += 1;
        end;
        min := a[1];
        i := 2;
        while i <= n do
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

  std::stringstream llvm_ir_str;
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_TRUE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  pascal::code_generate(parse_result.program_, symbol_table, llvm_ir_str);
  EXPECT_TRUE(error_stream.str().empty());
  EXPECT_EQ(llvm_ir_str.str(), dedent(R"(
    target triple = "x86_64-pc-linux-gnu"
    declare i32 @printf(i8* %format, ...)
    declare i32 @__isoc99_scanf(i8*, ...)

    @.str.int = constant [3 x i8] c"%d\00"
    @.str.str = constant [3 x i8] c"%s\00"

    define void @read_int(i32* %x) {
      call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr  ([3 x i8], [3 x i8]* @.str.int, i64 0, i64 0), i32* %x)
      ret void
    }

    define void @write_int(i32 %x) {
      call i32 (i8*, ...) @printf(i8* getelementptr ([3 x i8], [3 x i8]* @.str.int, i64 0, i64 0), i32 %x)
      ret void
    }

    define void @write_string(i8* %x) {
      call i32 (i8*, ...) @printf(i8* getelementptr ([3 x i8], [3 x i8]* @.str.str, i64 0, i64 0), i8* %x)
      ret void
    }

    @.str.intln = constant [4 x i8] c"%d\0A\00"
    define void @writeln_int(i32 %x) {
      call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.str.intln, i64 0, i64 0), i32 %x)
      ret void
    }

    @.str.5 = constant [19 x i8] c"Enter array size: \00"
    @.str.15 = constant [7 x i8] c"Enter \00"
    @.str.18 = constant [11 x i8] c" element: \00"
    @.str.60 = constant [6 x i8] c"Min: \00"

    define i32 @main() {
    start:
      %.1 = alloca [99 x i32]
      %.2 = alloca i32
      %.3 = alloca i32
      %.4 = alloca i32

      %.6 = getelementptr [19 x i8], [19 x i8]* @.str.5, i64 0, i64 0
      call void @write_string(i8* %.6)

      call void @read_int(i32* %.4)

      %.7 = alloca i32
      store i32 1, i32* %.7
      %.8 = load i32, i32* %.7

      store i32 %.8, i32* %.3
      br label %.9

    .9:
      %.10 = load i32, i32* %.3

      %.11 = load i32, i32* %.4

      %.12 = icmp sle i32%.10, %.11
      br i1 %.12, label %.13, label %.14

    .13:
      %.16 = getelementptr [7 x i8], [7 x i8]* @.str.15, i64 0, i64 0
      call void @write_string(i8* %.16)
      %.17 = load i32, i32* %.3

      call void @write_int(i32 %.17)
      %.19 = getelementptr [11 x i8], [11 x i8]* @.str.18, i64 0, i64 0
      call void @write_string(i8* %.19)

      %.20 = load i32, i32* %.3

      %.21 = sub i32 %.20, 1
      %.22 = sext i32 %.21 to i64
      %.23 = getelementptr [99 x i32], [99 x i32]* %.1, i64 0, i64 %.22
      call void @read_int(i32* %.23)

      %.24 = alloca i32
      store i32 1, i32* %.24
      %.25 = load i32, i32* %.24

      %.26 = load i32, i32* %.3

      %.27 = add i32 %.26, %.25
      store i32 %.27, i32* %.3
      br label %.9

    .14:
      %.28 = alloca i32
      store i32 1, i32* %.28
      %.29 = load i32, i32* %.28

      %.30 = sub i32 %.29, 1
      %.31 = sext i32 %.30 to i64
      %.32 = getelementptr [99 x i32], [99 x i32]* %.1, i64 0, i64 %.31
      %.33 = load i32, i32* %.32
      store i32 %.33, i32* %.2
      %.34 = alloca i32
      store i32 2, i32* %.34
      %.35 = load i32, i32* %.34

      store i32 %.35, i32* %.3
      br label %.36

    .36:
      %.37 = load i32, i32* %.3

      %.38 = load i32, i32* %.4

      %.39 = icmp sle i32%.37, %.38
      br i1 %.39, label %.40, label %.41

    .40:
      %.42 = load i32, i32* %.3

      %.43 = sub i32 %.42, 1
      %.44 = sext i32 %.43 to i64
      %.45 = getelementptr [99 x i32], [99 x i32]* %.1, i64 0, i64 %.44
      %.46 = load i32, i32* %.45
      %.47 = load i32, i32* %.2

      %.48 = icmp slt i32%.46, %.47
      br i1 %.48, label %.49, label %.50

    .49:
      %.51 = load i32, i32* %.3

      %.52 = sub i32 %.51, 1
      %.53 = sext i32 %.52 to i64
      %.54 = getelementptr [99 x i32], [99 x i32]* %.1, i64 0, i64 %.53
      %.55 = load i32, i32* %.54
      store i32 %.55, i32* %.2
      br label %.50

    .50:
      %.56 = alloca i32
      store i32 1, i32* %.56
      %.57 = load i32, i32* %.56

      %.58 = load i32, i32* %.3

      %.59 = add i32 %.58, %.57
      store i32 %.59, i32* %.3
      br label %.36

    .41:
      %.61 = getelementptr [6 x i8], [6 x i8]* @.str.60, i64 0, i64 0
      call void @write_string(i8* %.61)
      %.62 = load i32, i32* %.2

      call void @writeln_int(i32 %.62)

      ret i32 0
    })"));
}

TEST(CodegenSuite, Sort) {
  std::stringstream in(R"(
    {Test BubbleSort program}
    program BubbleSort;
    var
       arr : array[1..100] of integer;
       i, j, buf, N : integer;
    begin
       write('Enter array size: ');
       readln(N);
       i := 1;
       while (i <= N) do
           begin
           write('Enter ',i,' element: ');
           readln(arr[i]);
           i += 1;
           end;
     
       i := 1;
       while (i < N) do
           begin
           j := 1;
           while (j <= N - i) do
                begin
                if (arr[j] > arr[j + 1]) then
                    begin
                    buf := arr[j];
                    arr[j] := arr[j + 1];
                    arr[j + 1] := buf;
                    end;
                j += 1;
                end;
           i += 1;
           end;
     
       writeln('Sorted array:');
       i := 1;
       while (i < N) do
           begin
           write(arr[i],' ');
           i += 1;
           end;
        writeln(arr[i]);
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());

  std::stringstream llvm_ir_str;
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_TRUE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  pascal::code_generate(parse_result.program_, symbol_table, llvm_ir_str);
  EXPECT_TRUE(error_stream.str().empty());
  EXPECT_EQ(llvm_ir_str.str(), dedent(R"(
    target triple = "x86_64-pc-linux-gnu"
    declare i32 @printf(i8* %format, ...)
    declare i32 @__isoc99_scanf(i8*, ...)

    @.str.int = constant [3 x i8] c"%d\00"
    @.str.char = constant [3 x i8] c"%c\00"
    @.str.str = constant [3 x i8] c"%s\00"

    define void @read_int(i32* %x) {
      call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr  ([3 x i8], [3 x i8]* @.str.int, i64 0, i64 0), i32* %x)
      ret void
    }

    define void @write_int(i32 %x) {
      call i32 (i8*, ...) @printf(i8* getelementptr ([3 x i8], [3 x i8]* @.str.int, i64 0, i64 0), i32 %x)
      ret void
    }

    define void @write_char(i32 %x) {
      call i32 (i8*, ...) @printf(i8* getelementptr ([3 x i8], [3 x i8]* @.str.char, i64 0, i64 0), i32 %x)
      ret void
    }

    define void @write_string(i8* %x) {
      call i32 (i8*, ...) @printf(i8* getelementptr ([3 x i8], [3 x i8]* @.str.str, i64 0, i64 0), i8* %x)
      ret void
    }

    @.str.intln = constant [4 x i8] c"%d\0A\00"
    define void @writeln_int(i32 %x) {
      call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.str.intln, i64 0, i64 0), i32 %x)
      ret void
    }

    @.str.strln = constant [4 x i8] c"%s\0A\00"
    define void @writeln_string(i8* %x) {
      call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.str.strln, i64 0, i64 0), i8* %x)
      ret void
    }

    @.str.6 = constant [19 x i8] c"Enter array size: \00"
    @.str.16 = constant [7 x i8] c"Enter \00"
    @.str.19 = constant [11 x i8] c" element: \00"
    @.str.96 = constant [14 x i8] c"Sorted array:\00"

    define i32 @main() {
    start:
      %.1 = alloca [99 x i32]
      %.2 = alloca i32
      %.3 = alloca i32
      %.4 = alloca i32
      %.5 = alloca i32

      %.7 = getelementptr [19 x i8], [19 x i8]* @.str.6, i64 0, i64 0
      call void @write_string(i8* %.7)

      call void @read_int(i32* %.5)

      %.8 = alloca i32
      store i32 1, i32* %.8
      %.9 = load i32, i32* %.8

      store i32 %.9, i32* %.2
      br label %.10

    .10:
      %.11 = load i32, i32* %.2

      %.12 = load i32, i32* %.5

      %.13 = icmp sle i32%.11, %.12
      br i1 %.13, label %.14, label %.15

    .14:
      %.17 = getelementptr [7 x i8], [7 x i8]* @.str.16, i64 0, i64 0
      call void @write_string(i8* %.17)
      %.18 = load i32, i32* %.2

      call void @write_int(i32 %.18)
      %.20 = getelementptr [11 x i8], [11 x i8]* @.str.19, i64 0, i64 0
      call void @write_string(i8* %.20)

      %.21 = load i32, i32* %.2

      %.22 = sub i32 %.21, 1
      %.23 = sext i32 %.22 to i64
      %.24 = getelementptr [99 x i32], [99 x i32]* %.1, i64 0, i64 %.23
      call void @read_int(i32* %.24)

      %.25 = alloca i32
      store i32 1, i32* %.25
      %.26 = load i32, i32* %.25

      %.27 = load i32, i32* %.2

      %.28 = add i32 %.27, %.26
      store i32 %.28, i32* %.2
      br label %.10

    .15:
      %.29 = alloca i32
      store i32 1, i32* %.29
      %.30 = load i32, i32* %.29

      store i32 %.30, i32* %.2
      br label %.31

    .31:
      %.32 = load i32, i32* %.2

      %.33 = load i32, i32* %.5

      %.34 = icmp slt i32%.32, %.33
      br i1 %.34, label %.35, label %.36

    .35:
      %.37 = alloca i32
      store i32 1, i32* %.37
      %.38 = load i32, i32* %.37

      store i32 %.38, i32* %.3
      br label %.39

    .39:
      %.40 = load i32, i32* %.3

      %.41 = load i32, i32* %.5

      %.42 = load i32, i32* %.2

      %.43 = sub i32 %.41, %.42
      %.44 = icmp sle i32%.40, %.43
      br i1 %.44, label %.45, label %.46

    .45:
      %.47 = load i32, i32* %.3

      %.48 = sub i32 %.47, 1
      %.49 = sext i32 %.48 to i64
      %.50 = getelementptr [99 x i32], [99 x i32]* %.1, i64 0, i64 %.49
      %.51 = load i32, i32* %.50
      %.52 = load i32, i32* %.3

      %.53 = alloca i32
      store i32 1, i32* %.53
      %.54 = load i32, i32* %.53

      %.55 = add i32 %.52, %.54
      %.56 = sub i32 %.55, 1
      %.57 = sext i32 %.56 to i64
      %.58 = getelementptr [99 x i32], [99 x i32]* %.1, i64 0, i64 %.57
      %.59 = load i32, i32* %.58
      %.60 = icmp sgt i32%.51, %.59
      br i1 %.60, label %.61, label %.62

    .61:
      %.63 = load i32, i32* %.3

      %.64 = sub i32 %.63, 1
      %.65 = sext i32 %.64 to i64
      %.66 = getelementptr [99 x i32], [99 x i32]* %.1, i64 0, i64 %.65
      %.67 = load i32, i32* %.66
      store i32 %.67, i32* %.4
      %.68 = load i32, i32* %.3

      %.69 = alloca i32
      store i32 1, i32* %.69
      %.70 = load i32, i32* %.69

      %.71 = add i32 %.68, %.70
      %.72 = sub i32 %.71, 1
      %.73 = sext i32 %.72 to i64
      %.74 = getelementptr [99 x i32], [99 x i32]* %.1, i64 0, i64 %.73
      %.75 = load i32, i32* %.74
      %.76 = load i32, i32* %.3

      %.77 = sub i32 %.76, 1
      %.78 = sext i32 %.77 to i64
      %.79 = getelementptr [99 x i32], [99 x i32]* %.1, i64 0, i64 %.78
      store i32 %.75, i32* %.79
      %.80 = load i32, i32* %.4

      %.81 = load i32, i32* %.3

      %.82 = alloca i32
      store i32 1, i32* %.82
      %.83 = load i32, i32* %.82

      %.84 = add i32 %.81, %.83
      %.85 = sub i32 %.84, 1
      %.86 = sext i32 %.85 to i64
      %.87 = getelementptr [99 x i32], [99 x i32]* %.1, i64 0, i64 %.86
      store i32 %.80, i32* %.87
      br label %.62

    .62:
      %.88 = alloca i32
      store i32 1, i32* %.88
      %.89 = load i32, i32* %.88

      %.90 = load i32, i32* %.3

      %.91 = add i32 %.90, %.89
      store i32 %.91, i32* %.3
      br label %.39

    .46:
      %.92 = alloca i32
      store i32 1, i32* %.92
      %.93 = load i32, i32* %.92

      %.94 = load i32, i32* %.2

      %.95 = add i32 %.94, %.93
      store i32 %.95, i32* %.2
      br label %.31

    .36:
      %.97 = getelementptr [14 x i8], [14 x i8]* @.str.96, i64 0, i64 0
      call void @writeln_string(i8* %.97)

      %.98 = alloca i32
      store i32 1, i32* %.98
      %.99 = load i32, i32* %.98

      store i32 %.99, i32* %.2
      br label %.100

    .100:
      %.101 = load i32, i32* %.2

      %.102 = load i32, i32* %.5

      %.103 = icmp slt i32%.101, %.102
      br i1 %.103, label %.104, label %.105

    .104:
      %.106 = load i32, i32* %.2

      %.107 = sub i32 %.106, 1
      %.108 = sext i32 %.107 to i64
      %.109 = getelementptr [99 x i32], [99 x i32]* %.1, i64 0, i64 %.108
      %.110 = load i32, i32* %.109
      call void @write_int(i32 %.110)
      %.111 = alloca i8
      store i8 32, i8* %.111
      %.112 = load i8, i8* %.111

      %.113 = sext i8 %.112 to i32
      call void @write_char(i32 %.113)

      %.114 = alloca i32
      store i32 1, i32* %.114
      %.115 = load i32, i32* %.114

      %.116 = load i32, i32* %.2

      %.117 = add i32 %.116, %.115
      store i32 %.117, i32* %.2
      br label %.100

    .105:
      %.118 = load i32, i32* %.2

      %.119 = sub i32 %.118, 1
      %.120 = sext i32 %.119 to i64
      %.121 = getelementptr [99 x i32], [99 x i32]* %.1, i64 0, i64 %.120
      %.122 = load i32, i32* %.121
      call void @writeln_int(i32 %.122)

      ret i32 0
    })"));
}

TEST(CodegenSuite, Hash) {
  std::stringstream in(R"(
    {Test Hash program}
    program Hash;
    const
        b = 42;
        ten = 10;
    var
        a, res : integer;
    begin
        writeln('Enter value: ');
        readln(a);
        res := b * ((a * 2) - b * ((3 + 4 * ten) div 5) mod 45);
        writeln('Hash: ',res);
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());

  std::stringstream llvm_ir_str;
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_TRUE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  pascal::code_generate(parse_result.program_, symbol_table, llvm_ir_str);
  EXPECT_TRUE(error_stream.str().empty());
  EXPECT_EQ(llvm_ir_str.str(), dedent(R"(
    target triple = "x86_64-pc-linux-gnu"
    declare i32 @printf(i8* %format, ...)
    declare i32 @__isoc99_scanf(i8*, ...)

    @.str.int = constant [3 x i8] c"%d\00"
    @.str.str = constant [3 x i8] c"%s\00"

    define void @read_int(i32* %x) {
      call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr  ([3 x i8], [3 x i8]* @.str.int, i64 0, i64 0), i32* %x)
      ret void
    }

    define void @write_string(i8* %x) {
      call i32 (i8*, ...) @printf(i8* getelementptr ([3 x i8], [3 x i8]* @.str.str, i64 0, i64 0), i8* %x)
      ret void
    }

    @.str.intln = constant [4 x i8] c"%d\0A\00"
    define void @writeln_int(i32 %x) {
      call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.str.intln, i64 0, i64 0), i32 %x)
      ret void
    }

    @.str.strln = constant [4 x i8] c"%s\0A\00"
    define void @writeln_string(i8* %x) {
      call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.str.strln, i64 0, i64 0), i8* %x)
      ret void
    }

    @.str.9 = constant [14 x i8] c"Enter value: \00"
    @.str.33 = constant [7 x i8] c"Hash: \00"

    define i32 @main() {
    start:
      %.1 = alloca i32
      store i32 42, i32* %.1
      %.2 = load i32, i32* %.1

      %.3 = alloca i32
      store i32 %.2, i32* %.3
      %.4 = alloca i32
      store i32 10, i32* %.4
      %.5 = load i32, i32* %.4

      %.6 = alloca i32
      store i32 %.5, i32* %.6
      %.7 = alloca i32
      %.8 = alloca i32

      %.10 = getelementptr [14 x i8], [14 x i8]* @.str.9, i64 0, i64 0
      call void @writeln_string(i8* %.10)

      call void @read_int(i32* %.7)

      %.11 = load i32, i32* %.3

      %.12 = load i32, i32* %.7

      %.13 = alloca i32
      store i32 2, i32* %.13
      %.14 = load i32, i32* %.13

      %.15 = mul i32 %.12, %.14
      %.16 = load i32, i32* %.3

      %.17 = alloca i32
      store i32 3, i32* %.17
      %.18 = load i32, i32* %.17

      %.19 = alloca i32
      store i32 4, i32* %.19
      %.20 = load i32, i32* %.19

      %.21 = load i32, i32* %.6

      %.22 = mul i32 %.20, %.21
      %.23 = add i32 %.18, %.22
      %.24 = alloca i32
      store i32 5, i32* %.24
      %.25 = load i32, i32* %.24

      %.26 = sdiv i32 %.23, %.25
      %.27 = alloca i32
      store i32 45, i32* %.27
      %.28 = load i32, i32* %.27

      %.29 = mul i32 %.16, %.26
      %.30 = srem i32 %.29, %.28
      %.31 = sub i32 %.15, %.30
      %.32 = mul i32 %.11, %.31
      store i32 %.32, i32* %.8
      %.34 = getelementptr [7 x i8], [7 x i8]* @.str.33, i64 0, i64 0
      call void @write_string(i8* %.34)
      %.35 = load i32, i32* %.8

      call void @writeln_int(i32 %.35)

      ret i32 0
    })"));
}

TEST(CodegenSuite, Strings) {
  std::stringstream in(R"(
    {Test Strings program}
    program ArrMin;
    var
        s1, s2, res : string;
        ch : char;
    begin
        writeln('Enter first string: ');
        readln(s1);
        writeln('Enter second string: ');
        readln(s2);
        ch := 'X';
        res := 'a';
        res += s1;
        res += ' ';
        res += s2;
        res += s1;
        writeln(res);
        res := 'abcd';
        res[3] := ch;
        writeln(res);
    end.
    )");

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);

  auto parse_result = pascal::parse(lexer);
  EXPECT_TRUE(parse_result.errors_.empty());

  std::stringstream llvm_ir_str;
  std::stringstream error_stream;
  pascal::ast::SymbolTable symbol_table;
  EXPECT_TRUE(pascal::semantic_analyse(
      parse_result.program_, symbol_table, error_stream));
  pascal::code_generate(parse_result.program_, symbol_table, llvm_ir_str);
  EXPECT_TRUE(error_stream.str().empty());
  EXPECT_EQ(llvm_ir_str.str(), dedent(R"(
    target triple = "x86_64-pc-linux-gnu"
    declare i32 @printf(i8* %format, ...)
    declare i32 @__isoc99_scanf(i8*, ...)
    declare i8* @strcpy(i8* %dst, i8* %src)
    declare i8* @strcat(i8* %dst, i8* %src)

    @.str.empty = constant [1 x i8] c"\00"
    define void @strinit([255 x i8]* %str) {
      %str.ptr = getelementptr [255 x i8], [255 x i8]* %str, i64 0, i64 0
      call i8* @strcpy(i8* %str.ptr, i8* getelementptr ([1 x i8], [1 x i8]* @.str.empty, i64 0, i64 0))
      ret void
    }

    @.str.c = constant [2 x i8] c"*\00"
    define i8* @tostr(i8 %c, [255 x i8]* %str) {
      %str.ptr = getelementptr [255 x i8], [255 x i8]* %str, i64 0, i64 0
      call i8* @strcpy(i8* %str.ptr, i8* getelementptr ([2 x i8], [2 x i8]* @.str.c, i64 0, i64 0))
      store i8 %c, i8* %str.ptr
      ret i8* %str.ptr
    }

    @.str.str = constant [3 x i8] c"%s\00"

    define void @read_string(i8* %x) {
      call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr  ([3 x i8], [3 x i8]* @.str.str, i64 0, i64 0), i8* %x)
      ret void
    }

    @.str.strln = constant [4 x i8] c"%s\0A\00"
    define void @writeln_string(i8* %x) {
      call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.str.strln, i64 0, i64 0), i8* %x)
      ret void
    }

    @.str.5 = constant [21 x i8] c"Enter first string: \00"
    @.str.8 = constant [22 x i8] c"Enter second string: \00"
    @.str.30 = constant [5 x i8] c"abcd\00"

    define i32 @main() {
    start:
      %.1 = alloca [255 x i8]
      call void @strinit([255 x i8]* %.1)
      %.2 = alloca [255 x i8]
      call void @strinit([255 x i8]* %.2)
      %.3 = alloca [255 x i8]
      call void @strinit([255 x i8]* %.3)
      %.4 = alloca i8

      %.6 = getelementptr [21 x i8], [21 x i8]* @.str.5, i64 0, i64 0
      call void @writeln_string(i8* %.6)

      %.7 = getelementptr [255 x i8], [255 x i8]* %.1, i64 0, i64 0

      call void @read_string(i8* %.7)

      %.9 = getelementptr [22 x i8], [22 x i8]* @.str.8, i64 0, i64 0
      call void @writeln_string(i8* %.9)

      %.10 = getelementptr [255 x i8], [255 x i8]* %.2, i64 0, i64 0

      call void @read_string(i8* %.10)

      %.11 = alloca i8
      store i8 88, i8* %.11
      %.12 = load i8, i8* %.11

      store i8 %.12, i8* %.4
      %.13 = alloca i8
      store i8 97, i8* %.13
      %.14 = load i8, i8* %.13

      %.15 = alloca [255 x i8]
      %.16 = call i8* @tostr(i8 %.14, [255 x i8]* %.15)

      %.17 = getelementptr [255 x i8], [255 x i8]* %.3, i64 0, i64 0
      call i8* @strcpy(i8* %.17, i8* %.16)
      %.18 = getelementptr [255 x i8], [255 x i8]* %.1, i64 0, i64 0

      %.19 = getelementptr [255 x i8], [255 x i8]* %.3, i64 0, i64 0
      call i8* @strcat(i8* %.19, i8* %.18)
      %.20 = alloca i8
      store i8 32, i8* %.20
      %.21 = load i8, i8* %.20

      %.22 = alloca [255 x i8]
      %.23 = call i8* @tostr(i8 %.21, [255 x i8]* %.22)

      %.24 = getelementptr [255 x i8], [255 x i8]* %.3, i64 0, i64 0
      call i8* @strcat(i8* %.24, i8* %.23)
      %.25 = getelementptr [255 x i8], [255 x i8]* %.2, i64 0, i64 0

      %.26 = getelementptr [255 x i8], [255 x i8]* %.3, i64 0, i64 0
      call i8* @strcat(i8* %.26, i8* %.25)
      %.27 = getelementptr [255 x i8], [255 x i8]* %.1, i64 0, i64 0

      %.28 = getelementptr [255 x i8], [255 x i8]* %.3, i64 0, i64 0
      call i8* @strcat(i8* %.28, i8* %.27)
      %.29 = getelementptr [255 x i8], [255 x i8]* %.3, i64 0, i64 0

      call void @writeln_string(i8* %.29)

      %.31 = getelementptr [5 x i8], [5 x i8]* @.str.30, i64 0, i64 0
      %.32 = getelementptr [255 x i8], [255 x i8]* %.3, i64 0, i64 0
      call i8* @strcpy(i8* %.32, i8* %.31)
      %.33 = load i8, i8* %.4

      %.34 = alloca i32
      store i32 3, i32* %.34
      %.35 = load i32, i32* %.34

      %.36 = sub nsw i32 %.35, 1
      %.37 = sext i32 %.36 to i64
      %.38 = getelementptr [255 x i8], [255 x i8]* %.3, i64 0, i64 %.37
      store i8 %.33, i8* %.38
      %.39 = getelementptr [255 x i8], [255 x i8]* %.3, i64 0, i64 0

      call void @writeln_string(i8* %.39)

      ret i32 0
    })"));
}

}  // namespace pascal::test
