%{
   #include <stdio.h>
   #include "cassandra.h"
%}
%option noyywrap
glo "*"
semicolon ";"
div "/"
po "("
pf ")"
ao "{"
af "}"
aco "["
acf "]"
vir ","
egal "="
plus "+"
minus "-"
apo "'" 
diff "!="
sup ">"
supe ">="
inf "<"
infe"<="
point "."
point_intero "?"
deux_point ":"
double_quote "\""
com "--"[ ][^\n]*
comm "//"[ ][^\n]*
commentaire "/*"[ ][^*/]*"*/"

%%

{com} {return COMMENTAIRE;}
{comm} {return COMMENTAIRE;}
{commentaire} {return COMMENTAIRE;}
{point} {return POINT;}
{apo} {return APOSTROPHE;}
{plus} {return PLUS;}
{minus} {return MINUS;}
{div} {return DIV;}
{semicolon} {return SEMICOLON;}
{po} {return PAR_OUV;}
{pf} {return PAR_FER;}
{ao} {return ACCO_OUV;}
{af} {return ACCO_FER;}
{aco} {return CRO_OUV;}
{acf} {return CRO_FER;}
{vir} {return VIRGULE;}
{double_quote} {return DOUBLE_QUOTE;}
{point_intero} {return POINT_INTERO;}
{deux_point} {return DEUX_POINT;}
{egal} {return EGAL;}
{inf} {return INF;}
{sup} {return SUP;}
{infe} {return INFE;}
{supe} {return SUPE;}
{glo} {return ETOILE;}
"begin" {return BEGIN_T;}
"ast" {return AST_TOKEN;}
"exit" {return EXIT;}
"batch" {return BATCH;}
"apply" {return APPLY_T;}
"unlogged" {return UNLOGGED;}
"delete" {return DELETE_T;}
"timestamp" {return TIMESTAMP;}
"and" {return AND;}
"ttl" {return TTL;}
"update" {return UPDATE;}
"select" {return SELECT;}
"token" {return TOKEN;}
"in" {return IN;}
"writetime" {return WRITETIME_T;}
"by" {return BY_T;}
"asc" { return ASC;}
"desc" {return DESC;}
"contains key" {return CONTAINSKEY;}
"contains" {return CONTAINS;}
"where" {return WHERE;}
"limit" {return LIMIT;}
"allow" {return ALLOW;}
"distinct" {return DISTINCT;}
"from" {return FROM_T;}
"as" {return  AS_T;}
"count" {return COUNT_T;}
"use" {return USE;}
"trigger" {return TRIGGER;}
"ascii" {return ASCII_T;}
"bigint" {return BIGINT_T;}
"blob" {return BLOB_T;}
"boolean" {return BOOLEAN_T;}
"counter" {return COUNTER_T;}
"date"	{return DATE_T;}
"decimal" {return DECIMAL_T;}
"double" {return DOUBLE_T;}
"float" {return FLOAT_T;} 
"int" {return INT_T;}
"text" {return TEXT_T;}
"inet" {return INET_T;}
"smallint" {return SMALLINT_T;}
"time" {return TIME_T;}
"timeuuid" {return TIMEUUID_T;}
"tinyint" {return TINYINT_T;}
"uuid" {return UUID_T;}
"varchar" {return VARCHAR_T;}
"varint" {return VARINT_T;}
"on" {return ON;}
"with" {return WITH;}
"if" {return IF_TOKEN;}
"not" {return NOT_TOKEN;}
"exists" {return EXISTS_TOKEN;}
"function" {return FUNCTION;}
"aggregate" {return AGGREGATE;}
"type" {return TYPE;}
"drop" {return DROP;}
"materialized" {return MATERIALIZED;}
"view" {return VIEW;}
"table" {return TABLE;}
"user" {return USER;}
"keyspace" {return  KEYSPACE;}
"role" {return ROLE;}
"index" {return  INDEX;}
"insert" {return INSERT;}
"into" { return INTO;}
"nan" {return NAN;}
"infinity" { return INFINITY;}
"null" {return NULL_TOKEN;}
"true" {return TRUE_TOKEN;}
"false" {return FALSE_TOKEN;}
"values" {return VALUES_TOKEN;}
"json" {return JSON;}
"using" {return USING;}
"create" {return CREATE;}
"map" {return MAP_TOKEN;}
"set" {return SET_TOKEN;}
"list" {return LIST_TOKEN;}
"tuple" {return TUPLE_TOKEN;}
"columnfamily" {return COLUMNFAMILY;}
"static" {return STATIC;}
"primary" {return PRIMARY;}
"key" {return KEY;}
"clustering" {return CLUSTERING;}
"order" {return ORDER;}
"compact" {return COMPACT;}
"storage" {return STORAGE;}
"custom" {return CUSTOM;}
"keys" {return KEYS;}
"options" {return OPTIONS;}
"filtering" {return FILTERING;}
[-+]?[0-9]+											{return INTEGER_TOKEN;}
[-+]?[0-9]+(\.[0-9]*)?([eE][+-]?[0-9+])?			{return FLOAT_TOKEN;}
[0-9a-fA-F]											{return HEX_TOKEN;} 			
[0-9a-fA-F]{8}[-][0-9a-fA-F]{4}[-][0-9a-fA-F]{4}[-][0-9a-fA-F]{4}[-][0-9a-fA-F]{12}		{return UUID_TOKEN;}
[0][xX]([0-9a-fA-F])+								{return BLOB_TOKEN;}
([\']([^\'])*[\'])									{return STRING_TOKEN;}
"$$"([^("$$")])*"$$"								{return STRING_TOKEN;}
[a-zA-Z_][a-zA-Z0-9_]*	{return IDENTIFICATEUR;} 
[ \t]+
. {return ERREUR;}

%%