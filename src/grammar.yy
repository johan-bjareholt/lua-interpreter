%skeleton "lalr1.cc"
%defines
%define api.value.type variant
%define api.token.constructor
%code requires{
	#include "globals.h"
	#include "node.h"
}
%code{
    #include <string>
    #define YY_DECL yy::parser::symbol_type yylex()

    YY_DECL;

    Node root;
}

%type <Node> block
%type <Node> chunk
%type <Node> chunk2
%type <Node> stat
%type <Node> laststat


%type <Node> if
%type <Node> elseiflist
%type <Node> elseif
%type <Node> else

%type <Node> var
%type <Node> varlist

%type <Node> exp
%type <Node> prefixexp
%type <Node> explist

%type <Node> name
%type <Node> funcname
%type <Node> funcname2
%type <Node> namelist

%type <Node> function
%type <Node> functioncall
%type <Node> funcbody
%type <Node> parlist
%type <Node> args

%type <Node> tableconstructor

%type <Node> field
%type <Node> fieldlist
%type <Node> fieldlist2
%type <Node> optfieldsep
%type <Node> fieldsep

%type <Node> string
%type <Node> binop
%type <Node> unop

%token <std::string> DO
%token <std::string> WHILE
%token <std::string> FOR
%token <std::string> UNTIL
%token <std::string> REPEAT
%token <std::string> END
%token <std::string> IN

%token <std::string> IF
%token <std::string> THEN
%token <std::string> ELSEIF
%token <std::string> ELSE

%token <std::string> LOCAL

%token <std::string> FUNCTION
%token <std::string> RETURN
%token <std::string> BREAK

%token <std::string> NIL
%token <std::string> FALSE
%token <std::string> TRUE
%token <std::string> NUMBER
%token <std::string> STRING
%token <std::string> TDOT
%token <std::string> NAME

%token <std::string> BINOP
%token <std::string> UNOP
%token <std::string> MINUS

%token <std::string> EQUALS
%token <std::string> DOT
%token <std::string> COLON
%token <std::string> COMMA
%token <std::string> SEMICOLON

%token <std::string> BRACES_L
%token <std::string> BRACES_R

%token <std::string> BRACKET_L
%token <std::string> BRACKET_R

%token <std::string> PARANTHESES_L
%token <std::string> PARANTHESES_R

%token QUIT 0 "end of file"

%%

block	: chunk
		{
			$$ = Node("Block","");
			$$.children.push_back($1);
			root = $$;
		}
		;

chunk	: chunk2 laststat {
	  		$$ = $1;
			$$.children.push_back($2);
	  	}
		| chunk2 {
	  		$$ = $1;
		}
		| laststat {
			$$ = Node("Chunk","");
			$$.children.push_back($1);
		}
		;

chunk2	: stat optsemi
	   	{	
			$$ = Node("Chunk","");
			$$.children.push_back($1);
		}
	   	| chunk stat optsemi {
			$$ = $1;
			$$.children.push_back($2);
		}
		;

optsemi	: SEMICOLON {}
		| /* empty */
		;

laststat: RETURN explist optsemi {
			$$ = Node("return","explist");
			$$.children.push_back($2);
		}
		| RETURN optsemi {
			$$ = Node("return","empty");
		}
		| BREAK optsemi {
			$$ = Node("return","break");
			$$.children.push_back(Node("break",""));
		}
		;

stat	: varlist EQUALS explist {
			$$ = Node("varass", "");
			$$.children.push_back($1);
			$$.children.push_back($3);
		}
		| LOCAL namelist EQUALS explist {
			$$ = Node("varass","local");
			$$.children.push_back($2);
			$$.children.push_back($4);
		}
		| LOCAL namelist {
			$$ = Node("vardef","local");
			$$.children.push_back($2);
		}
		| FUNCTION funcname funcbody {
			$$ = Node("funcdef","");
			$$.children.push_back($2);
			$$.children.push_back($3);
		}
		| LOCAL FUNCTION name funcbody {
			$$ = Node("funcdef","local");
			$$.children.push_back($3);
			$$.children.push_back($4);
		}
		| functioncall {
			$$ = $1;
		}
		| DO block END {
			$$ = Node("do", "");
			$$.children.push_back($2);
		}
		| WHILE exp DO block END {
			$$ = Node("while","");
			$$.children.push_back($2);
			$$.children.push_back($4);
		}
		| REPEAT block UNTIL exp {
			$$ = Node("repeat","");
			$$.children.push_back($2);
			$$.children.push_back($4);
		}
		| if elseiflist else END {
			$$ = Node("ifelse","");
			$$.children.push_back($1);
			$$.children.push_back($2);
			$$.children.push_back($3);
		}
		| FOR name EQUALS exp COMMA exp DO block END {
			$$ = Node("for","2var");
			$$.children.push_back($2);
			$$.children.push_back($4);
			$$.children.push_back($6);
			$$.children.push_back($8);
		}
		| FOR name EQUALS exp COMMA exp COMMA exp DO block END {
			$$ = Node("for","3var");
			$$.children.push_back($2);
			$$.children.push_back($4);
			$$.children.push_back($6);
			$$.children.push_back($8);
			$$.children.push_back($10);
		}
		| FOR namelist IN explist DO block END {
			$$ = Node("for","in");
			$$.children.push_back($2);
			$$.children.push_back($4);
			$$.children.push_back($6);
		}
	 	;

if		: IF exp THEN block {
			$$ = Node("if","");
			$$.children.push_back($2);
			$$.children.push_back($4);
		}
		;

elseiflist: elseif {
			$$ = Node("elseiflist","");
			$$.children.push_back($1);
		}
		| elseiflist elseif {
			$$ = $1;
			$$.children.push_back($2);
		}
		| /* empty */ {
			$$ = Node("elseiflist","empty");
		}
		;

elseif	: ELSEIF exp THEN block {
			$$ = Node("elseif","");
			$$.children.push_back($2);
			$$.children.push_back($4);
		}
		;

else	: ELSE block {
	 		$$ = Node("else","");
			$$.children.push_back($2);
	 	}
		| /* empty */ {
			$$ = Node("else","empty");
		}
		;

var		: name {
	 		$$ = Node("var", "name");
			$$.children.push_back($1);
	 	}
		| prefixexp BRACKET_L exp BRACKET_R {
			$$ = Node("var","inbrackets");
			$$.children.push_back($1);
			$$.children.push_back($3);
		}
		| prefixexp DOT name {
			$$ = $1;
			$$.children.push_back($3);
		}
	 	;

varlist	: var {
			$$ = Node("varlist","");
			$$.children.push_back($1);
		}
		| varlist COMMA var {
			$$ = $1;
			$$.children.push_back($3);
		}
		;

name	: NAME {
	 		$$ = Node("name", $1);
	 	}
		;

funcname: funcname2 {
			$$ = $1;
		}
		| funcname2 COLON name {
			$$ = $1;
			$$.children.push_back($3);
		}
		;

funcname2: name {
			$$ = Node("funcname",$1.value);
		}
		| funcname2 DOT name {
			$$ = $1;
			$$.value = $$.value +"."+ $3.value;
		}
		;

namelist: name {
			$$ = Node("namelist","");
			$$.children.push_back($1);
		}
		| namelist COMMA name {
			$$ = $1;
			$$.children.push_back($3);
		}
		;

exp		: NIL {
	 		$$ = Node("nil", $1);
	 	}
	 	| FALSE {
	 		$$ = Node("int", "0");
		}
		| TRUE {
	 		$$ = Node("int", "1");
		}
		| NUMBER {
			$$ = Node("int", $1);
		}
		| string {
			$$ = $1; 
		}
		| TDOT {
			$$ = Node("exp", $1);
		}
		| function {
			$$ = Node("exp","function");
			$$.children.push_back($1);
		}
		| prefixexp {
			$$ = $1;
		}
		| tableconstructor {
			$$ = Node("exp","tableconstructor");
			$$.children.push_back($1);
		}
		| exp binop exp {
			$$ = Node("op", "binop");
			$$.children.push_back($1);
			$$.children.push_back($2);
			$$.children.push_back($3);
		}
		| unop exp {
			$$ = Node("op","unop");
			$$.children.push_back($1);
			$$.children.push_back($2);
		}
		;

explist	: exp {
			$$ = Node("explist", "");
			$$.children.push_back($1);
		}
		| explist COMMA exp {
			$$ = $1;
			$$.children.push_back($3);
		}
		;

prefixexp: var {
			$$ = $1;
		}
		| functioncall {
			$$ = $1;
		}
		| PARANTHESES_L exp PARANTHESES_R {
			$$ = $2;
		}
		;

function: FUNCTION funcbody {
			$$ = Node("function","in-line");
			$$.children.push_back($2);
		}
		;

functioncall: prefixexp args {
			$$ = Node("funccall","");
			$$.children.push_back($1);
			$$.children.push_back($2);
		}
		| prefixexp COLON name args {
			$$ = Node("funccall","2");
			$$.children.push_back($1);
			$$.children.push_back($3);
			$$.children.push_back($4);
		}
		;

funcbody: PARANTHESES_L parlist PARANTHESES_R block END {
			$$ = Node("funcbody","");
			$$.children.push_back($2);
			$$.children.push_back($4);
		}
		| PARANTHESES_L PARANTHESES_R block END {
			$$ = Node("funcbody","");
			$$.children.push_back(Node("parlist","empty"));
			$$.children.push_back($3);
		}
		;

parlist	: namelist {
			$$ = Node("parlist","namelist");
			$$.children.push_back($1);
		}
		| namelist COMMA TDOT {
			$$ = $1;
			$$.children.push_back(Node("argover",""));
		}
		| TDOT {
			$$ = Node("parlist","tdot");
		}
		;

args	: PARANTHESES_L PARANTHESES_R {
	 		$$ = Node("explist","empty");
	 	}
		| PARANTHESES_L explist PARANTHESES_R {
			$$ = $2;
		}
		| tableconstructor {
			$$ = $1;
		}
		| string {
			$$ = $1;
		}
		;

tableconstructor: BRACES_L fieldlist BRACES_R {
			$$ = Node("tableconstructor","");
			$$.children.push_back($2);
		}
		| BRACES_L BRACES_R {
			$$ = Node("tableconstructor","empty");
		}
		;

field	: BRACKET_L exp BRACKET_R EQUALS exp {
	  		$$ = Node("field","bracketequals");
			$$.children.push_back($2);
			$$.children.push_back($5);
	  	}
		| name EQUALS exp {
			$$ = Node("field","equals");
			$$.children.push_back($1);
			$$.children.push_back($3);
		}
		| exp {
			$$ = Node("field", "exp");
			$$.children.push_back($1);
		}
	  	;

fieldlist: fieldlist2 optfieldsep {
		 	$$ = $1;
			$$.children.push_back($1);
		}
		;

fieldlist2: field {
			$$ = Node("fieldlist","");
			$$.children.push_back($1);
		}
		| fieldlist2 fieldsep field {
			$$ = $1;
			$$.children.push_back($3);
		}

optfieldsep: fieldsep { $$ = $1; }
		| /* empty */ {}
		;

fieldsep: COMMA {
			$$ = Node("fieldsep",$1);
		}
		| SEMICOLON {
			$$ = Node("fieldsep",$1);
		}
		;

string	: STRING {
	   		$$ = Node("str", $1.substr(1,$1.length()-2));
	   	}
		;

binop	: BINOP {
	  		$$ = Node("binop", $1);
	  	}
		| MINUS {
			$$ = Node("binop", $1);
		}
		;

unop	: UNOP {
	 		$$ = Node("unop", $1);
	 	}
		| MINUS {
			$$ = Node("unop", $1);
		}
		;