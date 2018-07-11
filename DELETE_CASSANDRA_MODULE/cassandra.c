#include <jansson.h>

#include "jansson.h"
#include "hashtable.h"
#include "jansson_private.h"
#include "utf.h"

#include "cassandra.h"
#include "error_cassandra.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tabsymb.h"
extern int yylex();
extern char* yytext ; 
extern int yylineno ;	
static varvalueType* pvvt = NULL ;
typetoken token ; 
static varvalueType tempo ;
static selection t;
static whereClause tmpo;
boolean follow_token = false ;
boolean result=false;


int main(int argc, char *argv[]){
	
    creer_file_idf() ;
    creer_file_selection();
    creer_file_whereClause();
    creer_file_erreur();	
    
	token=_lire_token();
	if(_delete_stmt()){
		printf("\ntrue\n");
		afficher_selection() ;
		afficher_idf() ;
		afficher_whereClause();
		afficher_sm_erreurs();
		
	}
	else{printf("\nfalse\n");}

}
/*
----------------- DELETE -----------------------------------------------------------------------
DELETE_STMT : delete LIST_SELECTION_CH from IDF_P_IDF USING_TIMES_CH where WHERE_CL IF_STMT_CH
			  semicolon 
LIST_SELECTION_CH : epsilone(from) | LIST_SELECTION
LIST_SELECTION : SELECTION LIST_SELECTION_AUX
LIST_SELECTION_AUX : epsilone(from) | semicolon LIST_SELECTION 
SELECTION : IDF  TERM_CH
TERM_CH : accro_ouv TERM accro_fer | epsilone (virgule , from ) 
USING_TIMES_CH: epsilone(where) | using timestamp integer_token
IF_STMT_CH : epsilone(semicolon) | if IF_STMT
IF_STMT: exists | LIST_COND
------------------------------------------------------------------------------------------------
*/
//DELETE_STMT : delete LIST_SELECTION_CH from IDF_P_IDF USING_TIMES_CH where WHERE_CL IF_STMT_CH semicolon 
boolean _delete_stmt()
{
	boolean result=false;
	if(token==DELETE_T){
		token=_lire_token();
		if(_list_selection_ch()){
			token=_lire_token();
			if(token==FROM_T){
				token=_lire_token();
				if(_idf_p_idf()){
					token=_lire_token();
					if(_using_times_ch()){
						token=_lire_token();
						if(token==WHERE){
							token=_lire_token();
							if(_where_cl()){
								token=_lire_token();
								if(_if_stmt_ch()){
									token=_lire_token();
									if(token==SEMICOLON) {result=true;}
								}
							}
						}
					}
				}
			}
		}
	}
	return result;
}
//LIST_SELECTION_CH : epsilone(from) | LIST_SELECTION
boolean _list_selection_ch()
{
	boolean result=false;
	if(token==FROM_T)
		{
			follow_token = true;
			result = true;
		}
	else if(_list_selection()) {result=true;}
	return result;
}
//LIST_SELECTION : SELECTION LIST_SELECTION_AUX
boolean _list_selection()
{
	boolean result=false;
	if(_selection())
	{
		token=_lire_token();
		if(_list_selection_aux()) {result=true;}
	}
	return result;
}
//LIST_SELECTION_AUX : epsilone(from) | virgule LIST_SELECTION 
boolean _list_selection_aux()
{
	boolean result=false;
	if(token==FROM_T)
		{
			follow_token = true;
			result = true;
		}
	else if(token==VIRGULE)
	{

		token=_lire_token();
		if(_list_selection()) {result=true;}
	}
	return result;
}
//SELECTION : IDF  TERM_CH
boolean _selection()
{
	boolean result=false;
	if(_idf())
	{
		t.column_name = malloc(strlen(yytext) + 1) ;	
		strcpy(t.column_name, yytext);
		token=_lire_token();
		if(_term_ch()) {

			creer_selection(t.column_name,t.term,t.ligne_decl) ;
			result=true;}
	}
	return result;
}
//TERM_CH : accro_ouv TERM accro_fer | epsilone (virgule , from )
boolean _term_ch()
{
	boolean result=false;
	if(token==FROM_T)
		{
			t.term = malloc(strlen(yytext) + 1) ;	
			strcpy(t.term, "null");
			t.ligne_decl = yylineno ;
		
			follow_token = true;
			result = true;
		}
	else if(token==VIRGULE)
		{
		t.term = malloc(strlen(yytext) + 1) ;	
		strcpy(t.term, "null");
		t.ligne_decl = yylineno ;
		

			follow_token = true;
			result = true;
		}
	else if(token==CRO_OUV)
	{
		token=_lire_token();
		if(_term())
		{
		t.term = malloc(strlen(yytext) + 1) ;	
		strcpy(t.term, yytext);
		t.ligne_decl = yylineno ;
		
			token=_lire_token();
			if(token==CRO_FER) { result=true;}
		}
	}
	return result;
}
//USING_TIMES_CH: epsilone(where) | using timestamp integer_token
boolean _using_times_ch()
{
	boolean result=false;
	if(token==WHERE)
		{
			tempo.timestamp_value = malloc(strlen(yytext) + 1) ;	
		    strcpy(tempo.timestamp_value, "null");


			follow_token = true;
			result = true;
		}
	else if(token==USING)
	{
		token=_lire_token();
		if(token==TIMESTAMP)
		{
			token=_lire_token();
			if(token==INTEGER_TOKEN) {
				tempo.timestamp_value = malloc(strlen(yytext) + 1) ;	
				strcpy(tempo.timestamp_value, yytext);

				result=true;}
		}
	}
	
	return result;
}
//WHERE_CL : RELATION WHERE_CL_AUX
boolean _where_cl()
{
	boolean result=false;
	if(_relation())
	{
		token=_lire_token();
		if(_where_cl_aux()) {result=true;}
	}
	return result;
}
//WHERE_CL_AUX : and WHERE_CL | epsilone(order,limit,allow,semicolone)
boolean _where_cl_aux()
{
	boolean result=false;
	if(token==ORDER)
		{
			follow_token = true;
			result = true;
		}
	else if(token==LIMIT)
		{
			follow_token = true;
			result = true;
		}
	else if(token==ALLOW)
		{
			follow_token = true;
			result = true;
		}
	else if(token==SEMICOLON)
		{
			follow_token = true;
			result = true;
		}
	else if(token==IF_TOKEN)
		{
			follow_token = true;
			result = true;
		}
	else if(token==AND)
		{
			token=_lire_token();
			if(_where_cl()) {result=true;}
		}
	return result;
}
//IF_STMT_CH : epsilone(semicolon) | if IF_STMT
boolean _if_stmt_ch()
{
	boolean result=false;
	if(token==SEMICOLON)
		{
			tempo.if_exists = malloc(strlen(yytext) + 1) ;	
			strcpy(tempo.if_exists, "false");
			
			follow_token = true;
			result = true;
		}
	else if(token==IF_TOKEN)
	{
		token=_lire_token();
		if(_if_stmt()) {result=true;}
	}
	creer_varvalueType(tempo.keyspace_name,tempo.table_name,tempo.timestamp_value,tempo.if_exists,tempo.ligne_decl);
	return result;
	
}
//IF_STMT: exists | LIST_COND
boolean _if_stmt()
{
	boolean result=false;
	if(token==EXISTS_TOKEN) {
		tempo.if_exists = malloc(strlen(yytext) + 1) ;	
		strcpy(tempo.if_exists, "true");
		
		result=true;}
	else if(_list_cond()) {result=true;}
	return result;
}

//IDF_P_IDF : IDF IDF_P_IDF_AUX
boolean _idf_p_idf(){
	boolean result=false;
	if(_idf())
		{
			tempo.keyspace_name = malloc(strlen(yytext) + 1) ;	strcpy(tempo.keyspace_name, yytext);
			tempo.ligne_decl = yylineno ;
			token=_lire_token();
			if(_idf_p_idf_aux()) {
				
				printf("table name %s\n",tempo.table_name );

				 
				result=true;}
		}
	return result;
}

//IDF_P_IDF_AUX : epsilon| point IDF 
boolean _idf_p_idf_aux()
{
	boolean result=false;
	if (token== PAR_FER)
		{
			follow_token = true;
			result = true;
		}
	else if (token== PAR_OUV)
		{
			follow_token = true;
			result = true;
		}
	else if (token== ACCO_FER)
		{
			follow_token = true;
			result = true;
		}
	else if (token== CRO_FER)
		{
			follow_token = true;
			result = true;
		}
	else if (token== SEMICOLON)
		{
			follow_token = true;
			result = true;
		}
	else if (token== JSON)
		{
			follow_token = true;
			result = true;
		}
	else if (token== WHERE)
		{
			tempo.table_name = malloc(strlen(yytext) + 1) ;	strcpy(tempo.table_name,tempo.keyspace_name);
			tempo.keyspace_name = malloc(strlen(yytext) + 1) ;	strcpy(tempo.keyspace_name, "null");
			follow_token = true;
			result = true;
		}
	else if (token==ORDER)
		{
			follow_token = true;
			result = true;
		}
	else if (token== LIMIT)
		{
			follow_token = true;
			result = true;
		}
	else if (token==ALLOW)
		{
			follow_token = true;
			result = true;
		}
	else if (token==USING)
		{
			tempo.table_name = malloc(strlen(yytext) + 1) ;	strcpy(tempo.table_name,tempo.keyspace_name);
			tempo.keyspace_name = malloc(strlen(yytext) + 1) ;	strcpy(tempo.keyspace_name, "null");
			follow_token = true;
			result = true;
		}
	else if (token==SET_TOKEN)
		{
			follow_token = true;
			result = true;
		}
	else if (token== POINT)
		{
			token=_lire_token();
			if(_idf())
				{
					tempo.table_name = malloc(strlen(yytext) + 1) ;	strcpy(tempo.table_name, yytext);
					result=true;
				}	
		}
	return result;
}
//TERM : : CONSTANT | COLLECTION_LITER | VARIABLE | FUNCTION par_ouv TERM_LIST_X par_fer
boolean _term()
{
	boolean result=false;
	if (_constant()) {result = true;}
	if (_collection_liter()) {result = true;}
	else if (_variable()) {result = true;}
	if (_idf())
		{
			token=_lire_token();
			if (token==PAR_OUV)
				{
					token=_lire_token();
					if (_term_list_x())
						{
							token=_lire_token();
							if (token==PAR_FER)
								{
									result=true;
								}
						}
				}
		}
	return result;
}

//COLLECTION_LITER : LIST_LITERAL | acco_ouv MAP_SET_LITERAL acco_fer
boolean _collection_liter()
{
	boolean result=false;
	if(_list_literal()) {result=true;}
	if(token==ACCO_OUV)
		{
			token=_lire_token();
		    if (_map_set_literal())
		    {
		    	token=_lire_token();
				if (token==ACCO_FER)
					{result=true;}
		    }
		}
	return result;
}

//MAP_SET_LITERAL : epsilon | TERM (MAP_LITERAL_AUX | SET_LITERAL_AUX)
boolean _map_set_literal()
{
	boolean result=false;
	if (token== ACCO_FER)
		{
			follow_token = true;
			result = true;
		}
	else if (_term())
		{
			token=_lire_token();
			if(_map_literal_aux())
			{result=true;}
			if(_set_literal_aux())
				{result=true;}
			
		}
	return result;
}

//SET_LITERAL_AUX: epsilon | virgule MAP_SET_LITERAL
boolean _set_literal_aux()
{
	boolean result=false;
	if (token== ACCO_FER)
		{
			follow_token = true;
			result = true;
		}
	else if (token==VIRGULE)
		{
			token=_lire_token();
			if(_map_set_literal())
				{result=true;}
			
		}
	return result;
}

//MAP_LITERAL_AUX: deux_point TERM TERM_TERM_LIST_AUX
boolean _map_literal_aux()
{
	boolean result=false ;
	if(token==DEUX_POINT)
				{
					token=_lire_token();
					if (_term())
						{
							token=_lire_token();
							if(_term_term_list_aux())
								{
									result=true;
								}
						}
				}
		
	return result;
}

//TERM_TERM_LIST_AUX: epsilon | virgule MAP_SET_LITERAL
boolean _term_term_list_aux()
{
	boolean result=false;
	if (token== ACCO_FER)
		{
			follow_token = true;
			result = true;
		}
	else if (token== VIRGULE)
		{
			token=_lire_token();
			if(_map_set_literal())
				{
					result=true;
				}
		}
	return result;
}

//LIST_LITERAL:cro_ouv LIST_LITERAL_AUX cro_fer
boolean _list_literal()
{
	boolean result=false;
	if(token==	CRO_OUV) 
		{
			token=_lire_token();
		    if (_list_literal_aux())
		    	{
		    		token=_lire_token();
					if (token==CRO_FER)
						{result=true;}
		    	}
		}
	return result;
}

//LIST_LITERAL_AUX:epsilon | TERM_LIST_X
boolean _list_literal_aux()
{
	boolean result=false;
	if (token== CRO_FER)
		{
			follow_token = true;
			result = true;
		}
	else if (_term_list_x())
		{
			result=true;
			
		}
	return result;
}

//TERM_LIST_X: TERM TERM_LIST_AUX
boolean _term_list_x()
{
	boolean result=false ;
	if (_term())
		{
			token=_lire_token();
			if(_term_list_aux())
				{
					result=true;
				}
		}
	return result;
}

//TERM_LIST_AUX : epsilon | virgule TERM_LIST_X
boolean _term_list_aux()
{
	boolean result=false;
	if (token== PAR_FER)
		{
			follow_token = true;
			result = true;
		}
	else if (token== CRO_FER)
		{
			follow_token = true;
			result = true;
		}
	else if (token== ACCO_FER)
		{
			follow_token = true;
			result = true;
		}
	else if (token== VIRGULE)
		{
			token=_lire_token();
			if(_term_list_x())
				{
					result=true;
				}
		}
	return result;
}

//CONSTANT : STRING | NUMBER | BOOLEAN | uuid_token | blob_token | null_token
boolean _constant()
{
	boolean result=false;
	if (token==STRING_TOKEN) {
		tmpo.type_value = malloc(strlen(yytext) + 1) ;	
		strcpy(tmpo.type_value, "string");
		result = true;}
	else if (token==INTEGER_TOKEN) {
		tmpo.type_value = malloc(strlen(yytext) + 1) ;	
		strcpy(tmpo.type_value, "integer");
		result = true;}
	else if (_float()) {result = true;}
	else if (_boolean()) {result = true;}
	else if (token==UUID_TOKEN) {
		tmpo.type_value = malloc(strlen(yytext) + 1) ;	
		strcpy(tmpo.type_value, "uuid");
		result = true;}
	else if (token==BLOB_TOKEN) {
		tmpo.type_value = malloc(strlen(yytext) + 1) ;	
		strcpy(tmpo.type_value, "blob");
		result = true;}
	else if (token==NULL_TOKEN) {
		tmpo.type_value = malloc(strlen(yytext) + 1) ;	
		strcpy(tmpo.type_value, "null");
		result = true;}	
	return result;

}
//FLOAT: float_token | nan | infinity 
boolean _float()
{
	boolean result=false;
	if (token==FLOAT_TOKEN) {
		tmpo.type_value = malloc(strlen(yytext) + 1) ;	
		strcpy(tmpo.type_value, "float");
		result = true;}
	else if (token==NAN) {
		tmpo.type_value = malloc(strlen(yytext) + 1) ;	
		strcpy(tmpo.type_value, "nan");
		result = true;}
	else if (token==INFINITY) {
		tmpo.type_value = malloc(strlen(yytext) + 1) ;	
		strcpy(tmpo.type_value, "infinity");
		result = true;}	
	return result;
}

// BOOLEAN : true_token |false_token
boolean _boolean()
{
	boolean result=false;
	if (token==TRUE_TOKEN) {
		tmpo.type_value = malloc(strlen(yytext) + 1) ;	
		strcpy(tmpo.type_value, "true"); 
		result=true ;}
	else if (token==FALSE_TOKEN) {
		tmpo.type_value = malloc(strlen(yytext) + 1) ;	
		strcpy(tmpo.type_value, "false");
		result=true ;}
	return result;
}

//VARIABLE: point_intero | deux_point IDF
boolean _variable(){
	boolean result;
	if (token==POINT_INTERO)
	{
		tmpo.type_value = malloc(strlen(yytext) + 1) ;	
		strcpy(tmpo.type_value, "point intero");
		result=true;
	}
	else if (token==DEUX_POINT)
	{
		token=_lire_token();
		if (_idf())
		{
			result=true;
		}
		}
	else result=false;

	return result;
}

//IDF : idf | double_quote idf double_quote
boolean _idf()
{
	boolean result=false;
	if (token==IDENTIFICATEUR) {result=true;}
	else if (token==DOUBLE_QUOTE)
			{
				token=_lire_token();
				if(token==IDENTIFICATEUR)
					{
						token=_lire_token();
						if (token==DOUBLE_QUOTE) { result=true;}
					}
			}
	return result;
}

//LIST_COND: CONDITION LIST_COND_AUX
boolean _list_cond()
{
	boolean result=false;
	if(_condition())
	{
		token=_lire_token();
		if(_list_cond_aux()) {result=true;}
	}
	return result;
}
//LIST_COND_AUX : epsilone(semicolone) | and LIST_COND
boolean _list_cond_aux()
{
	boolean result=false;
	if(token==SEMICOLON)
		{
			follow_token = true;
			result = true;
		}
	else if(token==AND)
	{
		token=_lire_token();
		if(_list_cond()) {result=true;}
	}
	return result;
}
//CONDITION : IDF OP TERM
boolean _condition()
{
	boolean result=false;
	if(_idf())
	{
		token=_lire_token();
		if(_op())
		{
			token=_lire_token();
			if(_term()) {result=true;}
		}
	}
	return result;
}

//RELATION : IDF OP TERM | par_ouv IDF_RL par_fer OP TERM_LIST_X | IDF in par_ouv
//			 IDF in par_ouv TERM_LIST_CH par_fer | 
//			 token IDF_RL OP TERM
boolean _relation()
{ 
	boolean result=false;
	if(_idf())
	{
		tmpo.column_name_w = malloc(strlen(yytext) + 1) ;	
		strcpy(tmpo.column_name_w, yytext);
		tmpo.ligne_decl = yylineno ;
		
		token=_lire_token();
		//PROBLEME DANS OP 
		if(_op())
		{
			
			token=_lire_token();
			if(_term()) {
				tmpo.column_value = malloc(strlen(yytext) + 1) ;	
				strcpy(tmpo.column_value, yytext);

				creer_whereClause(tmpo.column_name_w,tmpo.operator,tmpo.column_value,tmpo.type_value,tmpo.ligne_decl,tempo.table_name) ;
				result=true;}
		}
		else if(token==IN)
		{
			token=_lire_token();
			if(token==PAR_OUV)
			{
				token=_lire_token();
				if(_term_list_ch())
				{
					token=_lire_token();
					if(token==PAR_FER) {result=true;}
				}
			}
		}
	}
	else if(token==PAR_OUV)
	{
		token=_lire_token();
		if(_idf_rl())
		{
			token=_lire_token();
			if(token==PAR_FER)
			{
				token=_lire_token();
				if(_op())
				{
					token=_lire_token();
					if(token==PAR_OUV)
					{
						token=_lire_token();
						if(_term_list_x())
						{
							token=_lire_token();
							if(token==PAR_FER) {result=true;}
						}
					}
					 
				}
			}
			
		}
	}
	//token IDF_RL OP TERM
	else if(token==TOKEN)
	{
		token=_lire_token();
		if(token==PAR_OUV)
		{
			token=_lire_token();
			if(_idf_rl())
			{
				token=_lire_token();
				if(_op())
				{
					token=_lire_token();
					if(_term())   {result=true;}
				}
			}
		}
	}

	return result;
}
boolean _term_list_ch()
{
	boolean result=false;
	if(token==PAR_FER)
		{
			follow_token = true;
			result = true;
		}
	else if(_term_list_x()) {result=true;}
	return result;
}
boolean _idf_rl()
{
	boolean result=false;
	if(_idf())
	{
		token=_lire_token();
		if(_idf_rl_aux()) {result=true;}
	}
	return result; 
}
boolean _idf_rl_aux()
{
	boolean result=false;
	if(token==PAR_FER)
		{
			follow_token = true;
			result = true;
		}
	else if(token==VIRGULE)
	{
		token=_lire_token();
		if(_idf_rl()) {result=true;}
	}
	return result;
}
//OP : egale | inf | sup | infe | supe | contains | contains key 
boolean _op()
{
	boolean result=false;
	if(token==EGAL){
		tmpo.operator = (type_op)EGAL_ ;
		result=true;}
	else if(token==INF) {
		tmpo.operator = (type_op)INF_ ;
		result=true;}
	else if(token==SUP) {
		printf("sup\n");
		tmpo.operator = (type_op)SUP_ ;
		result=true;}
	else if(token==INFE) {
		tmpo.operator = (type_op)INFE_ ;
		result=true;}
	else if(token==SUPE) {
		tmpo.operator = (type_op)SUPE_ ;
		result=true;}
	else if(token==CONTAINS) {
		tmpo.operator = (type_op)CONTAINS_ ;
		result=true;}
	else if(token==CONTAINSKEY) {
		tmpo.operator = (type_op)CONTAINSKEY_ ;
		result=true;}
	return result;
}

// lit le prochaine token s'il n'a pas déjà été lu par le prédicat d'un nullable 
typetoken _lire_token()
{
	if (follow_token)
	{
		follow_token = false ;
		return token ;
	}
	else 
	{
		return ((typetoken)yylex());
	}
	
}
