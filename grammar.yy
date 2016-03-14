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


%type <Node> field

%type <Node> var
%type <Node> varlist

%type <Node> exp
%type <Node> explist

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
	 	;

/*
field	: BRACKET_L exp BRACKET_R EQUALS exp
	  	;
*/

varlist	: var {
			$$ = Node("varlist","");
			$$.children.push_back($1);
		}
		| varlist FIELDSEP var {
			$$ = $1;
			$$.children.push_back($3);
		}
		;

var		: NAME {
	 		$$ = Node("var", $1);
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
			$$ = Node("exp", "");
			$$.children.push_back($1);
			$$.children.push_back($2);
			$$.children.push_back($3);
		}
		;
/*
function: FUNCTION funcbody {
		}
		;

funcbody: PARANTHESES_L parlist PARANTHESES_R block END {
			$$ = Node("funcbody","");
			$$.children.push_back($2);
			$$.children.push_back($4);
		}
		;

parlist	: {
		$$ = Node("","");
		}
*/

binop	: BINOP {
	  		$$ = Node("binop", $1);
	  	}
		;
