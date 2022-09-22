grammar Pascal;

options { caseInsensitive = true; }

program
    : header 
    ((constdecl? vardecl?) | (vardecl? constdecl?))
    block DOT
    ;



header
    : PROGRAM progname SEMICOLON 
    ;
    
progname
    : id
    ;
    
    
    
constdecl
    : CONST (constdeclaration SEMICOLON)+
    ;
    
constdeclaration
    : constname EQUAL expression
    ;    
    
constname
    : id
    ;
    
expression
    : expression operation expression
    | LPAREN expression RPAREN
    | sign* atom
    ;

sign
    : PLUS
    | MINUS
    ;
    
atom
    : value
    ;
    
operation
    : PLUS
    | MINUS
    | STAR
    | DIV
    | MOD
    ;
    
boolexpr
    : LPAREN operand1 booloperation operand2 RPAREN
    | operand1 booloperation operand2
    ;
    
operand1
    : expression
    ;
    
operand2
    : expression
    ;

booloperation
    :
    | EQUAL
    | MORETHEN
    | LESSTHEN
    | NOTEQUAL
    | NOTMORE
    | NOTLESS
    ;    
    
vardecl
    : VAR declaration*
    ;
    
declaration
    : varname (COMMA varname)* COLON vartype SEMICOLON
    ;
    
varname
    : id
    ;
    
vartype
    : simpletype
    | arraytype
    ;
    
simpletype
    : INTEGER
    | CHAR
    | STRING
    ;
    
arraytype
    : ARRAY (LBRACK | LBRACK2) interval (RBRACK | RBRACK2) OF simpletype
    ;
    
interval
    : lborder DOTDOT rborder
    ;
    
lborder
    : int
    ;
    
rborder
    : int
    ;
    
    
block
    : BEGIN END
    | BEGIN statement (SEMICOLON (statement SEMICOLON)*)? END
    ;
    
statement
    : block
    | functioncall
    | assignment
    | while
    | branch
    ;
    
functioncall
    : functionname LPAREN
    ( (variable (COMMA variable)*) | (argument (COMMA argument)*) ) 
    RPAREN
    ;
    
variable
    : varname
    | cell
    ;
    
functionname
    : READLN
    | WRITE
    | WRITELN
    ;
    
argument
    : expression
    ;
    
assignment
    : (cell | varname) modification expression
    ;  
    
    
    
while
    : WHILE boolexpr DO statement
    ;
    
branch
    : IF boolexpr THEN statement (ELSE alternative)?
    ;
    
alternative
    : statement
    ;
    
modification
    : ASSIGNMENT
    | ADD
    | REDUCE
    | MULTIPLY
    ;

value
    : id
    | int
    | char
    | stringliteral
    | cell
    ;
    
id
    : ID
    ;
    
int
    : INT
    ;
    
char
    : CHARACTER
    ;
    
stringliteral
    : STRINGLITERAL
    ;

function
    : functioncall
    ;    

cell
    : varname (LBRACK | LBRACK2) index (RBRACK | RBRACK2)
    ;
    
index
    : expression
    ;

PROGRAM
    : 'program'
    ;

BEGIN
    : 'begin'
    ;

END
    : 'end'
    ;

VAR
    : 'var'
    ;

CONST
    : 'const'
    ;

INTEGER
    : 'integer'
    ;

CHAR
    : 'char'
    ;

STRING
    : 'string'
    ;

OF
    : 'of'
    ;

READLN
    : 'readln'
    ;

WRITE
    : 'write'
    ;

WRITELN
    : 'writeln'
    ;

PLUS
    : '+'
    ;

MINUS
    : '-'
    ;

STAR
    : '*'
    ;

DIV
    : 'div'
    ;
    
MOD
    : 'mod'
    ;

IF
    : 'if'
    ;

THEN
    : 'then'
    ;

ELSE
    : 'else'
    ;

NOT
    : 'not'
    ;

AND
    : 'and'
    ;

OR
    : 'or'
    ;

XOR
    : 'xor'
    ;
    
ASSIGNMENT
    : ':='
    ;
    
ADD
    : '+='
    ;
    
REDUCE
    : '-='
    ;
    
MULTIPLY
    : '*='
    ;

EQUAL
    : '='
    ;

MORETHEN
    : '>'
    ;

LESSTHEN
    : '<'
    ;

NOTEQUAL
    : '<>'
    ;

NOTMORE
    : '<='
    ;

NOTLESS
    : '>='
    ;

WHILE
    : 'while'
    ;

DO
    : 'do'
    ;

BREAK
    : 'break'
    ;

CONTINUE
    : 'continue'
    ;

ARRAY
    : 'array'
    ;

COMMA
    : ','
    ;

COLON
    : ':'
    ;

SEMICOLON
    : ';'
    ;

DOT
    : '.'
    ;

LPAREN
    : '('
    ;

RPAREN
    : ')'
    ;

LBRACK
    : '['
    ;

LBRACK2
    : '(.'
    ;

RBRACK
    : ']'
    ;

RBRACK2
    : '.)'
    ;

DOTDOT
    : '..'
    ;

LCURLY
    : '{'
    ;

RCURLY
    : '}'
    ;

WS
    : [ \t\r\n] -> skip
    ;

COMMENT_1
    : '(*' .*? '*)' -> skip
    ;

COMMENT_2
    : '{' .*? '}' -> skip
    ;
    
CHARACTER
    : '\'' ('\'\'' | ~ ('\'')) '\''
    ;

STRINGLITERAL
    : '\'' ('\'\'' | ~ ('\''))* '\''
    ;

INT
    : '0'
    | [1-9] [0-9]*
    ;

ID
    : ('a' .. 'z') ('a' .. 'z' | '0' .. '9' | '_')*
    ;

INVALID
    : .
    ;
