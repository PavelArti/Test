#include <libpas/dump_tokens.hpp>

#include <PascalLexer.h>
#include <antlr4-runtime.h>
#include <gtest/gtest.h>

#include <sstream>
#include <string>

namespace pascal::test {

TEST(LexerSuite, IDTest) {
  std::stringstream in;
  std::stringstream out;

  in << "abc ABC ab_c _abc 123 a123";

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);
  dump_tokens(lexer, out);
  EXPECT_EQ(
      out.str(),
      "Loc=<1:0>\tID 'abc'\n"
      "Loc=<1:4>\tID 'ABC'\n"
      "Loc=<1:8>\tID 'ab_c'\n"
      "Loc=<1:13>\tINVALID '_'\n"
      "Loc=<1:14>\tID 'abc'\n"
      "Loc=<1:18>\tINT '123'\n"
      "Loc=<1:22>\tID 'a123'\n");
}

TEST(LexerSuite, CommentTest) {
  std::stringstream in;
  std::stringstream out;

  in << "(*comment1*) (*comment\n2*) {comment3} {comment\n4}"
        "comment5} {comment6 comment7*) (*comment8";

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);
  dump_tokens(lexer, out);
  EXPECT_EQ(
      out.str(),
      "Loc=<3:2>\tID 'comment5'\n"
      "Loc=<3:10>\tRCURLY '}'\n"
      "Loc=<3:12>\tLCURLY '{'\n"
      "Loc=<3:13>\tID 'comment6'\n"
      "Loc=<3:22>\tID 'comment7'\n"
      "Loc=<3:30>\tSTAR '*'\n"
      "Loc=<3:31>\tRPAREN ')'\n"
      "Loc=<3:33>\tLPAREN '('\n"
      "Loc=<3:34>\tSTAR '*'\n"
      "Loc=<3:35>\tID 'comment8'\n");
}

TEST(LexerSuite, ValueTest) {
  std::stringstream in;
  std::stringstream out;

  in << "0123 4567 'str' 'str2";

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);
  dump_tokens(lexer, out);
  EXPECT_EQ(
      out.str(),
      "Loc=<1:0>\tINT '0'\n"
      "Loc=<1:1>\tINT '123'\n"
      "Loc=<1:5>\tINT '4567'\n"
      "Loc=<1:10>\tSTRINGLITERAL ''str''\n"
      "Loc=<1:16>\tINVALID '''\n"
      "Loc=<1:17>\tID 'str2'\n");
}

TEST(LexerSuite, KeywordTest) {
  std::stringstream in;
  std::stringstream out;

  in << "Program bEgin enD "
        "VAR const integer char string "
        "of readln write writeln";

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);
  dump_tokens(lexer, out);
  EXPECT_EQ(
      out.str(),
      "Loc=<1:0>\tPROGRAM 'Program'\n"
      "Loc=<1:8>\tBEGIN 'bEgin'\n"
      "Loc=<1:14>\tEND 'enD'\n"
      "Loc=<1:18>\tVAR 'VAR'\n"
      "Loc=<1:22>\tCONST 'const'\n"
      "Loc=<1:28>\tINTEGER 'integer'\n"
      "Loc=<1:36>\tCHAR 'char'\n"
      "Loc=<1:41>\tSTRING 'string'\n"
      "Loc=<1:48>\tOF 'of'\n"
      "Loc=<1:51>\tREADLN 'readln'\n"
      "Loc=<1:58>\tWRITE 'write'\n"
      "Loc=<1:64>\tWRITELN 'writeln'\n");
}

TEST(LexerSuite, KeywordTest2) {
  std::stringstream in;
  std::stringstream out;

  in << "+-*div mod if then else "
        ":= += -= *= = > < <> <= >=";

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);
  dump_tokens(lexer, out);
  EXPECT_EQ(
      out.str(),
      "Loc=<1:0>	PLUS '+'\n"
      "Loc=<1:1>	MINUS '-'\n"
      "Loc=<1:2>	STAR '*'\n"
      "Loc=<1:3>	DIV 'div'\n"
      "Loc=<1:7>	MOD 'mod'\n"
      "Loc=<1:11>	IF 'if'\n"
      "Loc=<1:14>	THEN 'then'\n"
      "Loc=<1:19>	ELSE 'else'\n"
      "Loc=<1:24>	ASSIGNMENT ':='\n"
      "Loc=<1:27>	ADD '+='\n"
      "Loc=<1:30>	REDUCE '-='\n"
      "Loc=<1:33>	MULTIPLY '*='\n"
      "Loc=<1:36>	EQUAL '='\n"
      "Loc=<1:38>	MORETHEN '>'\n"
      "Loc=<1:40>	LESSTHEN '<'\n"
      "Loc=<1:42>	NOTEQUAL '<>'\n"
      "Loc=<1:45>	NOTMORE '<='\n"
      "Loc=<1:48>	NOTLESS '>='\n");
}

TEST(LexerSuite, KeywordTest3) {
  std::stringstream in;
  std::stringstream out;

  in << "while do continue array "
        ",:;. )( [] .) (. .. }{";

  antlr4::ANTLRInputStream stream(in);
  PascalLexer lexer(&stream);
  dump_tokens(lexer, out);
  EXPECT_EQ(
      out.str(),
      "Loc=<1:0>\tWHILE 'while'\n"
      "Loc=<1:6>\tDO 'do'\n"
      "Loc=<1:9>\tCONTINUE 'continue'\n"
      "Loc=<1:18>\tARRAY 'array'\n"
      "Loc=<1:24>\tCOMMA ','\n"
      "Loc=<1:25>\tCOLON ':'\n"
      "Loc=<1:26>\tSEMICOLON ';'\n"
      "Loc=<1:27>\tDOT '.'\n"
      "Loc=<1:29>\tRPAREN ')'\n"
      "Loc=<1:30>\tLPAREN '('\n"
      "Loc=<1:32>\tLBRACK '['\n"
      "Loc=<1:33>\tRBRACK ']'\n"
      "Loc=<1:35>\tRBRACK2 '.)'\n"
      "Loc=<1:38>\tLBRACK2 '(.'\n"
      "Loc=<1:41>\tDOTDOT '..'\n"
      "Loc=<1:44>\tRCURLY '}'\n"
      "Loc=<1:45>\tLCURLY '{'\n");
}

}  // namespace pascal::test
