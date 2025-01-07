grammar Spade;

sep: ';'*;          // Separator

module: importStmt* declaration* EOF;

importStmt: IMPORT ('.' '.'?)? reference (AS IDENTIFIER|'.' '*')? sep;

reference: IDENTIFIER ('.' IDENTIFIER)*;

// Declarations
declaration: modifiers (
                 varDecl
               | functionDecl
               | compoundDecl
            );

constraintList: constraint (',' constraint)* ','?
constraint: IDENTIFIER ':' type

parentList: parent (',' parent)* ','?;
parent: reference ('[' typeArgs ']')?;

typeParams: typeParam (',' typeParam)* ','?;
typeParam: (OUT | IN)? IDENTIFIER ('=' type)?;

compoundDecl: (CLASS | INTERFACE | ANNOTATION) IDENTIFIER 
              ('[' typeParams ']' ('where' constaintList)?)? 
              (':' parentList)? 
          ('{' enumList memberDecl* '}')?;

enumList: enumerator (',' enumerator)* ','?;
enumerator: IDENTIFIER ('=' expr | '(' args? ')')?;

// Member declaration
memberDecl: modifiers (
                varDecl
              | functionDecl
              | initDecl
              | compoundDecl except ANNOTATION
           ) sep;

modifiers: (ABSTRACT | FINAL | STATIC | PRIVATE | INTERNAL | PROTECTED | PUBLIC)*;

initDecl: INIT '(' params? ')' definition;

varDecl: (VAR | CONST) (IDENTIFIER (':' type)? | destructDecl) ('=' expr)?;

functionDecl: FUN IDENTIFIER 
                  (('[' typeParams ']') ('where' constaintList)?)? 
                  '(' params? ')' 
                  ('->' type)? 
              definition?;
params: paramList ('*' paramList)? ('/' paramList)?;
paramList: param (',' param)* ','?;
param: CONST? '*'? (IDENTIFIER | '_') (':' type)? ('=' expr)?;

definition: '=' stmt sep | block;

// Statement
stmts: stmt sep | block;
stmt
    : IF expr body (ELSE body)?                                  # ifStmt
    | WHILE expr body (ELSE body)?                               # whileStmt
    | DO block WHILE expr (ELSE body)?                           # doStmt
    | FOR destruct IN postfix body (ELSE body)?                # forStmt
    | MATCH expr '{'
        matchCase*
        (ELSE '->' stmts)?
      '}'                                                   # matchStmt
    | TRY body (catchStmt+ finallyStmt? | finallyStmt)     # tryStmt
    | CONTINUE                                              # continueStmt
    | BREAK                                                 # breakStmt
    | THROW expr                                            # throwStmt
    | RETURN expr                                          # returnStmt
    | YIELD expr                                            # yieldStmt
    | '...'                                                 # noStmt
    | expr                                                  # exprStmt
    ;

block: '{' (block | declaration sep | stmt sep)* '}';

matchCase: WHEN items (IF expr)? '->' stmts;

catchStmt: CATCH refList (AS IDENTIFIER)? body;
finallyStmt: FINALLY body;

body: ':' stmt sep | block;

// Expression
expr
    : op=(BANG|TILDE|DASH|PLUS)? postfix                            # unaryExpr
    | expr '??' expr                                        # elvisExpr
    | expr (AS safe='?'? type)*                                   # castExpr
    | <assoc=right> expr '**' expr                          # powerExpr
    | expr op=(STAR|SLASH|MODULUS) expr                               # factorExpr
    | expr op=(PLUS|DASH) expr                                   # termExpr
    | expr ('<''<'|'>''>'|'>''>''>') expr                   # shiftExpr
    | expr '&' expr                                         # bitAndExpr
    | expr '^' expr                                         # bitXorExpr
    | expr '|' expr                                         # bitOrExpr
    | expr op=(LT|LE|EQ|NE|GE|GT) expr               # relationalExpr
    | expr (IS NOT?|NOT? IN) expr                   # conditionalExpr
    | NOT expr                                            # notExpr
    | expr AND expr                                       # andExpr
    | expr OR expr                                        # orExpr
    | expr if expr else expr                                # ternaryExpr
    | (param | '(' params? ')') '->' type? (block | expr)   # lambdaExpr
    | assigneeList assignOperator items                     # assignExpr
    ;

assigneeList: assignee (',' assignee)* ','?;
assignee: postfix | destructDecl;
assignOperator: '='|'+='|'-='|'*='|'/='|'%='|'**='|'<<='|'>>='|'>>>='|'&='|'|='|'^='|'??=';

destructDecl: '[' destruct (',' destruct)* ','? ']';
destruct: '*'? (IDENTIFIER | '_');

// Postfix expression
postfix: primary                            # postfixPrimary
    | postfix '?'? '.' (IDENTIFIER | INIT)  # postfixDot
    | postfix '(' args? ')'                 # postfixCall
    | postfix '[' indexer ']'               # postfixIndexer
    | postfix '[' typeArgs ']'              # postfixGeneric
    | postfix block                         # postfixBlock
    ;

args: arg (',' arg)* ','?;
arg: (IDENTIFIER ':')? (expr | '_');

indexer: slice (',' slice)* ','?;
slice: expr
     | expr? ':' expr? (':' expr?)?;

// Primary expressions
primary: TRUE | FALSE | NULL | INTEGER | FLOAT | STRING | IDENTIFIER                       # constantExpr
       | objectBuilder                  # builderExpr
       | SUPER ('[' reference ']')?     # superExpr
       | SELF                           # thisExpr
       | '(' (expr)  ')'                # groupExpr
       | '(' items? ')'                 # tupleExpr
       | '{' items '}'                  # setExpr
       | '{' entries? '}'               # mapExpr
       | '[' items? ']'                 # listExpr
       | type                           # typeExpr
       ;

objectBuilder: OBJECT (':' type)? '{' memberDecl* '}';

items: expr (',' expr)* ','?;

entries: entry (',' entry)* ','?;
entry: primary ':' expr;

// Kind expressions
type: type '|' type                         # unionType
    | type '&' type                         # intersectionType
    | type '?'                              # nullableType
    | primaryType;

primaryType: | reference ('[' typeArgs ']')?              # referenceType
             | TYPEOF '(' expr ')'                 # typeofType
             | TYPE                                # literalType
             | '(' paramTypes? ')' '->' type          # functionType
             | '(' type ')' # groupedType
             | OBJECT ('{' memberTypeList? '}')?  # objectType
    ;

typeArgs: type (',' type)* ','?;

paramTypes: paramType (',' paramType)* ','?;
paramType: '*'? type;

typeList: type (',' type)* ','?;

memberTypeList: memberType (',' memberType)* ','?;
memberType: (IDENTIFIER | INIT) (':' type)?;

// Keywords

// Heading
PACKAGE: 'package';
IMPORT: 'import';
// Clauses
EXTENDS: 'extends';
IMPLEMENTS: 'implements';
// Declarations
ENUM: 'enum';
CLASS: 'class';
INTERFACE: 'interface';
ANNOTATION: 'annotation';
INIT: 'init';
FUN: 'fun';
CONST: 'const';
VAR: 'var';
// Modifiers
ABSTRACT: 'abstract';
FINAL: 'final';
STATIC: 'static';
// Accessors
PRIVATE: 'private';
PROTECTED: 'protected';
INTERNAL: 'internal';
PUBLIC: 'public';
// Statements
IF: 'if';
ELSE: 'else';
WHILE: 'while';
DO: 'do';
FOR: 'for';
IN: 'in';
MATCH: 'match';
WHEN: 'when';
THROW: 'throw';
TRY: 'try';
CATCH: 'catch';
FINALLY: 'finally';
CONTINUE: 'continue';
BREAK: 'break';
RETURN: 'return';
YIELD: 'yield';
// Operators
GT : '>' ;
GE : '>=' ;
NE : '!=' ;
EQ : '==' ;
LE : '<=' ;
LT : '<' ;
MODULUS : '%' ;
SLASH : '/' ;
STAR : '*' ;
PLUS : '+' ;
DASH : '-' ;
TILDE : '~' ;
BANG : '!' ;
AS: 'as';
IS: 'is';
NOT: 'not';
AND: 'and';
OR: 'or';
// Primary expressions
SUPER: 'super';
SELF: 'self';
// Literals
TRUE: 'true';
FALSE: 'false';
NULL: 'null';
// Special keywords
OBJECT: 'object';
TYPE: 'varKind';
TYPEOF: 'typeof';

// Contextual keywords
OUT: 'out';
REF: 'ref';

IDENTIFIER: [a-zA-Z_][a-zA-Z0-9_]*
          | REF | OUT;

INTEGER: DECIMAL
       | ('0b'|'0B') BINARY
       | '0' OCTAL
       | ('0x'|'0X') HEX
       ;

FLOAT: DECIMAL '.' DECIMAL? (('e'|'E') EXP)?
     | ('0x'|'0X') HEX '.' HEX? (('p'|'P') EXP)?;

STRING: '\'' (ESC|~[\n])*? '\''
      | '"' (ESC|.)*? '"'
      | '`' (ESC|.)*? '`'
      ;

fragment ESC: '\\"' | '\\\\';
fragment EXP: ('+'|'-') DECIMAL;
fragment DECIMAL: (('0'..'9') '_'?)+;
fragment BINARY: (('0'..'1') '_'?)+;
fragment OCTAL: (('0'..'7') '_'?)+;
fragment HEX: ([0-9a-fA-F] '_'?)+;

WS: [ \t\f]+ -> skip;
NEWLINE: '\r'? '\n' -> channel(HIDDEN);

BLOCK_COMMENT
    : '/*' .*? '*/' -> skip
    ;
LINE_COMMENT
    : '//' ~[\r\n]* -> skip
    ;