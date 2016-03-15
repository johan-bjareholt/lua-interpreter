%skeleton "lalr1.cc"
%defines
%define api.value.type variant
%define api.token.constructor
%code requires{
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
%type <Node> stat

%type <Node> if
%type <Node> elseiflist
%type <Node> elseif
%type <Node> else

%type <Node> field
%type <Node> fieldlist

%type <Node> var
%type <Node> varlist

%type <Node> exp
%type <Node> explist

%type <Node> name
%type <Node> funcname
%type <Node> namelist

%type <Node> function
%type <Node> funcbody
%type <Node> parlist

%type <Node> binop

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
%token <std::string> BREAK

%token <std::string> NIL
%token <std::string> FALSE
%token <std::string> TRUE
%token <std::string> NUMBER
%token <std::string> STRING
%token <std::string> TDOT
%token <std::string> NAME

%token <std::string> FIELDSEP
%token <std::string> BINOP
%token <std::string> UNOP

%token <std::string> EQUALS
%token <std::string> DOT

%token <std::string> BRACES_L
%token <std::string> BRACES_R

%token <std::string> BRACKET_L
%token <std::string> BRACKET_R

%token <std::string> PARANTHESES_L
%token <std::string> PARANTHESES_R

%token QUIT 0 "end of file"

%%

chunk	: block
		{
			$$ = Node("Chunk","");
			$$.children.push_back($1);
			root = $$;
		}
		;

block	: stat
	   	{	
			$$ = Node("Block","");
			$$.children.push_back($1);
		}
	   	| block stat {
			$$ = $1;
			$$.children.push_back($2);
		}
		;

stat	: varlist EQUALS explist {
			$$ = Node("stat", "assignment");
			$$.children.push_back($1);
			$$.children.push_back($3);
		}
		| DO block END {
			$$ = Node("stat", "do-end block");
			$$.children.push_back($2);
		}
		| WHILE exp DO block END {
			$$ = Node("stat","while-do-end block");
			$$.children.push_back($2);
			$$.children.push_back($4);
		}
		| REPEAT block UNTIL exp {
			$$ = Node("stat","repear-until block");
			$$.children.push_back($2);
			$$.children.push_back($4);
		}
		| if elseiflist else END {
			$$ = Node("stat","else-elseif-else");
			$$.children.push_back($1);
			$$.children.push_back($2);
			$$.children.push_back($3);
		}
		| FOR name EQUALS exp FIELDSEP exp DO block END {
			$$ = Node("stat","for, 2var");
			$$.children.push_back($2);
			$$.children.push_back($4);
			$$.children.push_back($6);
			$$.children.push_back($8);
		}
		| FOR name EQUALS exp FIELDSEP exp FIELDSEP exp DO block END {
			$$ = Node("stat","for, 3var");
			$$.children.push_back($2);
			$$.children.push_back($4);
			$$.children.push_back($6);
			$$.children.push_back($8);
			$$.children.push_back($10);
		}
		| FUNCTION funcname funcbody {
			$$ = Node("stat","function");
			$$.children.push_back($2);
			$$.children.push_back($3);
		}
		| LOCAL FUNCTION name funcbody {
			$$ = Node("stat","local function");
			$$.children.push_back($3);
			$$.children.push_back($4);
		}
		| LOCAL namelist {
			$$ = Node("stat","undefied local variable");
			$$.children.push_back($2);
		}
		| LOCAL namelist EQUALS explist {
			$$ = Node("stat","local variable");
			$$.children.push_back($2);
			$$.children.push_back($4);
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

fieldlist: field {
			$$ = Node("fieldlist","");
			$$.children.push_back($1);
		}
		| fieldlist FIELDSEP field {
			$$ = $1;
			$$.children.push_back($3);
		}

var		: name {
	 		$$ = Node("var", "name");
			$$.children.push_back($1);
	 	}
	 	;

varlist	: var {
			$$ = Node("varlist","");
			$$.children.push_back($1);
		}
		| varlist FIELDSEP var {
			$$ = $1;
			$$.children.push_back($3);
		}
		;

name	: NAME {
	 		$$ = Node("name", $1);
	 	}

funcname: name {
			$$ = Node("funcname","");
			$$.children.push_back($1);
		}
		| funcname DOT name {
			$$ = $1;
			$$.children.push_back($3);
		}
		;

namelist: name {
			$$ = Node("namelist","");
			$$.children.push_back($1);
		}
		| namelist FIELDSEP name {
			$$ = $1;
			$$.children.push_back($3);
		}
		;

exp		: NIL {
	 		$$ = Node("exp", $1);
	 	}
	 	| FALSE {
	 		$$ = Node("exp", $1);
		}
		| TRUE {
	 		$$ = Node("exp", $1);
		}
		| NUMBER {
			$$ = Node("exp", $1);
		}
		| STRING {
			$$ = Node("exp", $1);
		}
		| TDOT {
			$$ = Node("exp", $1);
		}
		| exp binop exp {
			$$ = Node("exp", "binoperation");
			$$.children.push_back($1);
			$$.children.push_back($2);
			$$.children.push_back($3);
		}
		| function {
			$$ = Node("exp","in-line function");
			$$.children.push_back($1);
		}
		;

explist	: exp {
			$$ = Node("explist", "");
			$$.children.push_back($1);
		}
		| explist FIELDSEP exp {
			$$ = $1;
			$$.children.push_back($3);
		}
		;

function: FUNCTION funcbody {
			$$ = Node("function","in-line");
			$$.children.push_back($2);
		}
		;

funcbody: PARANTHESES_L parlist PARANTHESES_R block END {
			$$ = Node("funcbody","");
			$$.children.push_back($2);
			$$.children.push_back($4);
		}
		;

parlist	: namelist {
			$$ = Node("parlist","namelist");
			$$.children.push_back($1);
		}
		| TDOT {
			$$ = Node("parlist","tdot");
		}

binop	: BINOP {
	  		$$ = Node("binop", $1);
	  	}
		;
