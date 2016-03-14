%top{
    #include "grammar.tab.hh"
    #define YY_DECL yy::parser::symbol_type yylex()

	void log(std::string message, std::string text){
		std::cout << message << ": " << text << std::endl;
	}
}

%option noyywrap nounput batch noinput

%%

 /*
	Reserved keywords
 */
 /* Looping */
do										{ log("do", yytext); return yy::parser::make_DO(yytext); }
while									{ log("while", yytext); return yy::parser::make_WHILE(yytext); }
for										{ log("for", yytext); return yy::parser::make_FOR(yytext); }
until									{ log("until", yytext); return yy::parser::make_UNTIL(yytext); }
repeat									{ log("repeat", yytext); return yy::parser::make_REPEAT(yytext); }
end										{ log("end", yytext); return yy::parser::make_END(yytext); }
in										{ log("in", yytext); return yy::parser::make_IN(yytext); }
 /* if/else statements*/
if										{ log("if", yytext); return yy::parser::make_IF(yytext); }
then									{ log("then", yytext); return yy::parser::make_THEN(yytext); }
elseif									{ log("elseif", yytext); return yy::parser::make_ELSEIF(yytext); }
else									{ log("else", yytext); return yy::parser::make_ELSE(yytext); }

 /*  */
local									{ log("local", yytext); return yy::parser::make_LOCAL(yytext); }

 /* function */
function								{ log("function",yytext); return yy::parser::make_FUNCTION(yytext); }
break									{ log("break",yytext); return yy::parser::make_BREAK(yytext); }


 /* Values */
nil										{ log("nil", yytext); return yy::parser::make_NIL(yytext);}
false									{ log("false", yytext); return yy::parser::make_FALSE(yytext); }
true									{ log("true", yytext); return yy::parser::make_TRUE(yytext);}
[0-9]+									{ log("number",yytext); return yy::parser::make_NUMBER(yytext);}
\"[^\"]*\"								{ log("string",yytext); return yy::parser::make_STRING(yytext);}
\.\.\.									{ log("tdot",yytext); return yy::parser::make_TDOT(yytext);}
[A-Za-z][A-Za-z0-9_]+					{ log("name",yytext); return yy::parser::make_NAME(yytext); }

 /* Token categories */
[,;]									{ log("fieldsep",yytext); return yy::parser::make_FIELDSEP(yytext); }
([-+*/^%<>]|\.\.|<=|>=|==|~=|and|or)	{ log("binop",yytext); return yy::parser::make_BINOP(yytext); }
([-#]|not)								{ log("unop",yytext); return yy::parser::make_UNOP(yytext); }


 /* Single tokens */
=										{ log("equals",yytext); return yy::parser::make_EQUALS(yytext); }

 /* blocks */
\(										{ log("parentheses_l",yytext); }
\)										{ log("parantheses_r",yytext); }
{										{ log("braces_l", yytext); return yy::parser::make_BRACES_L(yytext); }
}										{ log("braces_r", yytext); return yy::parser::make_BRACES_R(yytext); }
[\[]									{ log("bracket_l",yytext); return yy::parser::make_BRACKET_L(yytext); }
[\]]									{ log("bracket_r",yytext); return yy::parser::make_BRACKET_R(yytext); }


<<EOF>>                 				{ log("end", ""); return yy::parser::make_QUIT(); }

%%

