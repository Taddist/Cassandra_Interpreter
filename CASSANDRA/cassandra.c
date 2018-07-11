#include "cassandra.h"
#include "error.h"
#include "operations/tables/op_t.h"
#include "operations/keyspaces/op_k.h"
#include "operations/CURD/curd.h"
#include "AST.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h> // pour tuer le process corant -- exit function


// mon AST :
AST mon_ast = NULL ;
AST tmp_ast = NULL ;



// les variables temporaires (Pointeurs) :
ksp_type* temp_ksp = NULL ;
table_type* temp_tab = NULL ;
column_type* temp_col = NULL ;
column_type* temp_cols = NULL ;
smError* temp_err = NULL ;
value_type* temp_val = NULL ;
value_type* temp_vals = NULL ;
condition_type* temp_cond = NULL ;
condition_type* temp_conds = NULL ;


typetoken op_on ;
int count=0 ;
int count2=0 ;


// le token :
extern int yylex() ;	// l'élément courant
extern char* yytext ; 	// le texte courant
extern int yylineno ;	// numero de ligne courante




typetoken token ; 
boolean follow_token = false ;
boolean result=false;


int main(int argc, char *argv[]){


	// creation de la file d'erreurs :
	creer_file_erreur() ;

	// char xx[256] ;
	// get_current_ksp(xx) ;
	// printf("curr = %s\n", xx);

	token = _lire_token(); 
	boolean flag = false ;  
	if (_use_stmt())  {flag=true;}
	else if (_drop_stmt()) {flag=true;}
	else if (_insert_stmt()) {flag=true;}
	else if (_create_stmt()) {flag=true;}
	else if(_select_stmt()) {flag=true;}
	else if(_update_stmt()) {flag=true;}
	else if (_delete_stmt()) {flag=true;}
	else if(_batch_stmt()) {flag=true;}
	else if(_exit_()) {flag=true;}// Voire _exit function == kill all + free current keyspace
	
	if(flag){
		printf( "\033[1m\033[32m" "Syntaxiquement correcte \n" "\x1b[0m" );
		afficher_sm_erreurs();	// affichage des erreurs
	}else{
		printf( "\033[1m\033[31m" "Incorrecte syntaxiquement\n" "\x1b[0m" );
	}


	return 0;
}
/*
------------------CREATE -------------------------------------------------------------------
CLOSE THE PROGRAMME
--------------------------------------------------------------------------------------------
*/
boolean _exit_(){
	boolean result=false;
	if (token==EXIT)
	{
		result = true ;
		free_current() ; 
		printf("\033[1m\033[33m"  "exiting CASSANDRA interpreter...\n");
		kill(0, SIGKILL) ;
	}
	return result;
}




/*
------------------CREATE -------------------------------------------------------------------
CREATE_STMT: create (CREATE_KEYSPACE| CREATE_TABLE | CREATE_INDEX)
--------------------------------------------------------------------------------------------
*/
boolean _create_stmt()
{
	boolean result=false;
	if (token==CREATE)
	{
		token=_lire_token();
		if(_create_keyspace()) { result=true;}
		else if(_create_table()) { result=true;}
		else if(_create_index()) { result=true;}
	}
	return result;
}
/*
*******************CREATE KEYSPACE**********************************************************
CREATE_KEYSPACE: keyspace IF_NOT_EX IDF with PROPERTIES semicolon
PROPERTIES : PROPERTY PROPERTIES_AUX
PROPERTIES_AUX : and PROPERTIES | epsilon
PROPERTY : IDF egale PROPERTY_AUX
PROPERTY_AUX : IDF | CONST | MAP_LIT
MAP_LIT : acc_ouv MAP_LIT_AUX acco_fer
MAP_LIT_AUX: TERM deux_point TERM MAP_LIT_X
MAP_LIT_X: epsilon | virgule MAP_LIT_AUX
********************************************************************************************
*/
boolean _create_keyspace()
{
	boolean result=false;
	if(token==KEYSPACE)
	{
		token=_lire_token();
		if(_if_not_ex())
		{
			token=_lire_token();
			if(_idf())
			{

				// préparation des donnée temporaire :	
				temp_ksp = malloc(sizeof *temp_ksp);
				temp_ksp->nom = malloc(strlen(yytext) + 1) ;	strcpy(temp_ksp->nom, yytext);

				temp_err = malloc(sizeof *temp_err);
				temp_err->num_ligne = (int)yylineno ;
				temp_err->nom_source = malloc(strlen(yytext) + 1) ;	strcpy(temp_err->nom_source, yytext);
				temp_err->type_err = KEYSPACE_EXIST_DEJA ;


				token=_lire_token();
				if(token==WITH)
				{
					token=_lire_token();
					if(_properties())
					{
						token=_lire_token();
						if(token==SEMICOLON) 
						{
							result=true;
							// test d'existance puis creation :
							
							if(create_ksp(temp_ksp) != -1){
								printf( "\033[1m\033[32m" "keyspace %s created \n" "\x1b[0m" , temp_ksp->nom);
								free(temp_ksp);	free(temp_err);
							}else{
								creer_sm_erreur(*temp_err) ;
								free(temp_ksp);	free(temp_err);
							}
						}
					}
				}
			}
		}
	}
	return result;
}

//PROPERTIES : PROPERTY PROPERTIES_AUX
boolean _properties(){
	boolean result=false;
	if (_property())
	{
		token=_lire_token();
		if (_properties_aux()) {result=true;}
	}
	return result;
}

//PROPERTIES_AUX : and PROPERTIES | epsilon
boolean _properties_aux()
{
	boolean result=false;
	if (token==SEMICOLON)
	{
		follow_token=true;
		result=true;
	}
	else if (token==AND)
	{
		token=_lire_token();
		if (_properties())
		{
			result=true;
		}
	}
	return result;
}
//PROPERTY : IDF egale PROPERTY_AUX
boolean _property()
{
	boolean result=false ;
	if (_idf())
	{
		token=_lire_token();
		if (token==EGAL)
		{
			token=_lire_token();
			if (_property_aux())
			{
				result=true;
			}
		}
	}
	return result ;
}

//PROPERTY_AUX : IDF | CONSTANT| MAP_LITER 
boolean _property_aux()
{
	boolean result=false;
	if (_idf()) { result=true;}
	else if (_constant()) { result=true;}
	else if(_map_lit()) { result=true;}
	return result;
}
//MAP_LIT : acc_ouv MAP_LIT_AUX acco_fer
boolean _map_lit()
{
	boolean result=false;
	if(token==ACCO_OUV) 
	{
		token=_lire_token();
		if (_map_lit_aux())
		{
			token=_lire_token();
			if (token==ACCO_FER) {result=true;}
		}
	}
	return result;
}
//MAP_LIT_AUX: TERM deux_point TERM MAP_LIT_X
boolean _map_lit_aux()
{
	boolean result=false ;
	if(_term())
	{
		token=_lire_token();
		if(token==DEUX_POINT)
		{
			token=_lire_token();
			if (_term())
			{
				token=_lire_token();
				if(_map_lit_x()){result=true;}
			}
		}
	}
	return result;
}

//MAP_LIT_X: epsilon | virgule MAP_LIT_AUX
boolean _map_lit_x()
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
		if(_map_lit_aux())
		{
			result=true;
		}
	}
	return result;
}
/*
****************** CREATE TABLE ***********************************************************
CREATE_TABLE: TAB_COL IF_NOT_EX IDF_P_IDF  par_ouv LIST_COLUMN_DF par_fer with LIST_OPTION semicolone
TAB_COL : table | columnfamily 
LIST_COLUMN_DF :  COLUMN_DF LIST_COLUMN_DF_AUX 
LIST_COLUMN_DF_AUX : epsilone | virgule LIST_COLUMN_DF
COLUMN_DF : COLUMN_DF_I | COLUMN_DF_P
COLUMN_DF_I : IDF TYPE STATIC PRIMARY_KEY 
COLUMN_DF_P : PRIMARY_KEY  LIST_PART_KEY
STATIC :epsilone | STATIC 
PRIMARY_KEY: epsilone | Primary key 
TYPE: NATIVE_TYPE | COLLECTION_TYPE | TUPLE_TYPE | STRING 
NATIVE_TYPE : ascii_t | bigint_t | blob_t| boolean_t | counter_t| date_t | decimal_t | 
double_t | float_t | int_t | text_t | inet_t | smallint_t | time_t| timeuuid_t| tinyint_t 
uuid_t | varchar_t | varint_t
COLLECTION_TYPE: LIST_N | SET_N | MAP_N
LIST_N: list inf NATIVE_TYPE sup 
SET_N : set inf NATIVE_TYPE sup 
MAP_N : map  inf NATIVE_TYPE virgule NATIVE_TYPE  sup
TUPLE_TYPE : tuple inf TUPLE_TYPE_AUX sup 
TUPLE_TYPE_AUX : TYPE TUPLE_TYPE_X
TUPLE_TYPE_X : epsilone | virgule TUPLE_TYPE_AUX
LIST_PART_KEY : par_ouv  PART_KEY LIST_ID par_fer 
PART_KEY : IDF | par_ouv  PART_KEY_AUX par_fer 
PART_KEY_AUX : IDF PART_KEY_X 
PART_KEY_X : virgule PART_KEY_AUX
LIST_ID : epsilone | virgule IDF LIST_ID
LIST_OPTION : PROPERTY | COMPACT_STORAGE | CLUSTERING ORDER 
COMPACT_STORAGE : compact storage 
CLUSTERING ORDER : clustering order 
*******************************************************************************************
*/
//CREATE_TABLE: TAB_COL IF_NOT_EX IDF_P_IDF  par_ouv LIST_COLUMN_DF par_fer LIST_OPTION semicolone
boolean _create_table ()
{
	boolean result=false;
	if (_tab_col())
	{
		token=_lire_token();
		if(_if_not_ex())
		{
			token=_lire_token();
			if(_idf_p_idf())
			{

				


				token=_lire_token();
				if(token==PAR_OUV)
				{
					token=_lire_token();
					if(_list_column_df())
					{
						token=_lire_token();
						if(token==PAR_FER)
						{	

							// ici on a fin de saisie de collones == nbcolumns est préte à l'emploie :
							temp_tab->nb_columns = count ; // c'est fini avec temp_tab
							count = 0 ;


							// dernière detection :
							// printf("####################\n");

							token=_lire_token();
							if(token==WITH)
							{


								token=_lire_token();
								if(_list_option())
								{
									token=_lire_token();

									if(token==SEMICOLON){

										// test d'existance puis creation de la table :
										int i = create_tab(temp_tab);
										if(i==1){
											printf( "\033[1m\033[32m" "table %s created \n" "\x1b[0m" , temp_tab->nom);
											free(temp_tab);	free(temp_err);
										}else if(i==-1){
											creer_sm_erreur(*temp_err) ;
											free(temp_tab);	free(temp_err);
										}else if(i==-11){
											
											temp_err->type_err = NO_KSP_USED ;
											creer_sm_erreur(*temp_err) ;
											free(temp_tab);	free(temp_err);
										}

										result=true;
									}
								}
							}
						}
					}	
				}
			}
		}
	}
	return result ;
}
//TAB_COL : table | columnfamily
boolean _tab_col()
{
	boolean result=false;
	if(token==TABLE) { result=true;}
	else if (token==COLUMNFAMILY) { result=true;}
}
//LIST_COLUMN_DF :  COLUMN_DF LIST_COLUMN_DF_AUX
boolean _list_column_df()
{
	boolean result=false;
	if(_column_df())
	{
		token=_lire_token();
		if(_list_column_df_aux()) {result=true;}
	}
}
//LIST_COLUMN_DF_AUX : epsilone | virgule LIST_COLUMN_DF
boolean _list_column_df_aux()
{
	boolean result=false;
	if (token== PAR_FER)
	{
		follow_token = true;
		result = true;
	}
	else if (token== VIRGULE)
	{
		token=_lire_token();
		if(_list_column_df()) {result=true;}	
	}
	return result;
}
//COLUMN_DF : COLUMN_DF_I | COLUMN_DF_P
boolean _column_df()
{
	boolean result=false;
	if(_column_df_i()) {result=true;}
	else if(_column_df_p()) {result=true;}
}
//COLUMN_DF_I : IDF TYPE STATIC PRIMARY_KEY 
boolean _column_df_i() 
{
	boolean result=false;
	if(_idf())
	{

		// preparation de la colonne temporaire  = récupération du nom :
		temp_col = malloc(sizeof *temp_col);
		temp_col->nom = malloc(strlen(yytext) + 1) ;	strcpy(temp_col->nom, yytext);



		token=_lire_token();
		if(_arg_type())
		{

			// recupération de type de colonne :
			typetoken x = token ;
			temp_col->type = ((x==BIGINT_T || x==INT_T || x==SMALLINT_T || x==VARINT_T)? INT_ :  (x==DECIMAL_T || x==DOUBLE_T || x==FLOAT_T)? DOUBLE_ :  (x==BOOLEAN_T )? BOOL_ :   STRING_  ) ;
			// (x==TEXT_T || x==VARCHAR_T)?

			token=_lire_token();
			if(_static())
			{
				token=_lire_token();
				if(_primary_key()) {
					result=true;
					count++ ;	// ici on a une colonne compléte est préte a l'add:
					temp_tab->columns = (column_type*)realloc(temp_tab->columns , count*sizeof(column_type)) ;
					temp_tab->columns[count-1] = *temp_col ;
					free(temp_col);	
					
				}
			}
		}
	}
	return result ;
}
//COLUMN_DF_P : PRIMARY_KEY  LIST_PART_KEY
boolean _column_df_p()
{
	boolean result=false;
	if(_primary_key())
	{
		token=_lire_token();
		if(_list_part_key()) {result=true;}
	}
	return result ;
}
//STATIC :epsilone | STATIC 
boolean _static()
{
	boolean result=false;
	if (token==PRIMARY)
	{
		follow_token = true;
		result = true;
	}
	else if (token==VIRGULE)
	{
		follow_token = true;
		result = true;
	}
	else if (token==PAR_FER)
	{
		follow_token = true;
		result = true;
	}
	else if (token== STATIC) {result=true;}	
	return result;
}

//PRIMARY_KEY: epsilone | Primary key 
boolean _primary_key()
{
	boolean result=false;
	if (token==VIRGULE)
	{
		temp_col->prk = false ;
		follow_token = true;
		result = true;
	}
	else if (token==PAR_FER)
	{
		temp_col->prk = false ;
		follow_token = true;
		result = true;
	}
	else if (token==PRIMARY) 
	{
		token=_lire_token();
		if(token==KEY) {

			result=true;
			temp_col->prk = true ;
		}	
	}
	return result;
}
//LIST_PART_KEY : par_ouv  PART_KEY LIST_ID par_fer 
boolean _list_part_key()
{
	boolean result=false;
	if(token==PAR_OUV)
	{
		token=_lire_token();
		if(_part_key())
		{
			token=_lire_token();
			if(_list_id())
			{
				token=_lire_token();
				if(token==PAR_FER) {result=true;}
			}
		}
	}
	return result;
}
//PART_KEY : IDF | par_ouv  PART_KEY_AUX par_fer 
boolean _part_key()
{
	boolean result=false;
	if(_idf()){ result=true;}
	else if (token==PAR_OUV)
	{
		token=_lire_token();
		if(_part_key_aux())
		{
			token=_lire_token();
			{
				if(token==PAR_FER){result=true;}
			}
		}
	}
	return result;
}
//PART_KEY_AUX : IDF PART_KEY_X 
boolean _part_key_aux()
{
	boolean result=false;
	if(_idf())
	{
		token=_lire_token();
		if(_part_key_x()) { result=true;}
	}
	return result;
}
//PART_KEY_X : virgule PART_KEY_AUX
boolean _part_key_x()
{
	boolean result=false;
	if(token==VIRGULE)
	{
		token=_lire_token();
		if(_part_key_aux()) {result=true;}
	}
	return result;
}
//LIST_ID : epsilone | virgule IDF LIST_ID
boolean _list_id()
{
	boolean result=false;
	if (token==PAR_FER)
	{
		follow_token = true;
		result = true;
	}
	else if (token==VIRGULE) 
	{
		token=_lire_token();
		if(_idf())
		{
			token=_lire_token();
			if(_list_id()){result=true;}	
		}
	}
	return result;

}
//LIST_OPTION : PROPERTY | COMPACT_STORAGE | CLUSTERING ORDER 
boolean _list_option()
{
	boolean result=false;
	if(_property()) {result=true;}
	else if(_compact_storage()) {result=true;}
	else if(_clustering_order()) {result=true;}
	return result;
}
//COMPACT_STORAGE : compact storage 
boolean _compact_storage()
{
	boolean result=false;
	if(token== COMPACT)
	{
		token=_lire_token();
		if(token==STORAGE) {result=true;}
	}
	return result;
}
//CLUSTERING ORDER : clustering order 
boolean _clustering_order()
{
	boolean result=false;
	if(token== CLUSTERING)
	{
		token=_lire_token();
		if(token==ORDER) {result=true;}
	}
	return result;
}

/*
***************** CREATE INDEX *************************************************************
CREATE_INDEX : create CUSTOM index IF_NOT_EX INDEX_N on IDF_P_IDF par ouv INDEX_IDF
			   par fer USING_STMT semicolone
CUSTUM : epsilone(index)| custom 
INDEX_N :epsilone(on) | identificateur
INDEX_IDF : IDF | keys par ouv IDF par_fer
USING_STMT : using string_token WITH_STMT | epsilone(semicolon)
WITH_STMT : with options egale MAP_LIT | epsilone (semicolon)
********************************************************************************************
*/
boolean _create_index()
{
	boolean result=false;
	if(_custom())
	{
		token=_lire_token();
		if(token==INDEX)
		{
			token=_lire_token();
			if(_if_not_ex())
			{
				token=_lire_token();
				if(_index_n())
				{
					token=_lire_token();
					if(token==ON)
					{
						token=_lire_token();
						if(_idf_p_idf())
						{
							token=_lire_token();
							if(token=PAR_OUV)
							{
								token=_lire_token();
								if(_index_idf())
								{
									token=_lire_token();
									if(token==PAR_FER)
									{
										token=_lire_token();
										if(_using_stmt())
										{
											token=_lire_token();
											if(token==SEMICOLON)  {result=true;}
										}
									}

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
// bla custom makatekhdemch mochekil f follow 
//CUSTUM : epsilone(index)| custom 
boolean _custom()
{

	boolean result=false;
	if (token==INDEX)
	{
		follow_token=true;
		result=true;
	}
	else if (token==CUSTOM) {result=true;}
	return result;
}
//blach identif makatekhdemch mochekil f follow
//INDEX_N :epsilone(on) | identificateur
boolean _index_n()
{
	boolean result=false;
	if (token==ON)
	{
		follow_token = true;
		result = true;
	}
	else if (token==IDENTIFICATEUR) {result=true;}	
	return result;
}
//INDEX_IDF : IDF | keys par ouv IDF par_fer
boolean _index_idf()
{
	boolean result=false;
	if(_idf()) {result=true;}
	else if(token==KEYS)
	{
		token=_lire_token();
		if(token==PAR_OUV)
		{
			token=_lire_token();
			if(_idf())
			{
				token=_lire_token();
				if(token==PAR_FER) {result=true;}
			}
		}
	}
	return result;
}
//USING_STMT : using string_token WITH_STMT | epsilone(semicolon)
boolean _using_stmt()
{
	boolean result=false;
	if (token==SEMICOLON)
	{
		follow_token = true;
		result = true;
	}
	else if(token==USING)
	{
		token=_lire_token();
		if(token==STRING_TOKEN)
		{
			token=_lire_token();
			if(_with_stmt()) {result=true;}
		}
	}
	return result;
}
//WITH_STMT : with options egale MAP_LIT | epsilone (semicolon)
boolean _with_stmt()
{
	boolean result=false;
	if (token==SEMICOLON)
	{
		follow_token = true;
		result = true;
	}
	else if(token==WITH)
	{
		token=_lire_token();
		if(token==OPTIONS)
		{
			token=_lire_token();
			if(token==EGAL)
			{
				token=_lire_token();
				if(_map_lit()) {result=true;}
			}		
		}
	}
	return result;
}
/*
------------------ USE ---------------------------------------------------------------------
USE_STMT : use IDF
IDF : idf | double_quote idf double_quote
--------------------------------------------------------------------------------------------
*/
//USE_STMT : use IDF
boolean _use_stmt()
{
	boolean result=false;
	if (token==USE)
	{
		token=_lire_token();
		if(_idf()) { 

			// préparation des donnée temporaire :	
			temp_ksp = malloc(sizeof *temp_ksp);
			temp_ksp->nom = malloc(strlen(yytext) + 1) ;	strcpy(temp_ksp->nom, yytext);

			temp_err = malloc(sizeof *temp_err);
			temp_err->num_ligne = (int)yylineno ;
			temp_err->nom_source = malloc(strlen(yytext) + 1) ;	strcpy(temp_err->nom_source, yytext);
			temp_err->type_err = KEYSPACE_INEXISTANT ;




			token=_lire_token();
			if(token==SEMICOLON) {

				if(set_current_ksp(temp_ksp)!=-1){			
					printf( "\033[1m\033[32m" "keyspace %s changed \n" "\x1b[0m" , temp_ksp->nom);
					// char *xx = calloc(256 , sizeof(char)) ;
					// get_current_ksp(xx);
					// printf("current == %s\n", xx);
					free(temp_ksp);	free(temp_err);
				}else{
					creer_sm_erreur(*temp_err) ;
					free(temp_ksp);	free(temp_err);
				}


				result=true;}
			}
		}
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

/*
------------------ SELECT -------------------------------------------------------------------
SELECT_STMT: select JSON SELECT_CLAUSE from IDF_P_IDF  WHERE_CH ORDER_BY_CH LIMIT_CH ALLOW_CH
			 semicolone
JSON : epsilone(semicolon) | json 
SELECT_CLAUSE: DISTINCT_CH SELECT_LIST | count par ouv ET_UN par_fer AS_IDF_CH
ET_UN :  etoile | un 
DISTINCT_CH : epsilone (selector) | distinct
SELECT_LIST: SELECTOR_LIST | etoile
SELECTOR_LIST : SELECTOR AS_IDF_CH SELECTOR_LIST_AUX
SELECTOR_LIST_AUX : epsilone (from) |  virgule SELECTOR_LIST
SELECTOR: IDF | writetime par_ouv IDF par_fer |TTL par_ouv IDF par_fer | 
		  IDF_P_IDF par_ouv LIST_SELECTOR 
LIST_SELECTOR : SELECTOR LIST_SELECTOR_AUX
LIST_SELECTOR_AUX : epsilone(par_fer) | virgule LIST_SELECTOR
AS_IDF_CH epsilone (from,virgule)|as IDF
WHERE_CH : where WHERE_CL | epsilone(order,limit,allow,semicolone)
WHERE_CL : RELATION WHERE_CL_AUX
WHERE_CL_AUX : and WHERE_CL
ORDER_BY_CH : epsilone(allow,semicolone,limit) | order by ORDER_BY
OP : egale | inf | sup | infe | supe | contains | contains key 
ORDER_BY : ORDERING ORDER_BY_AUX
ORDER_BY_AUX : virgule ORDERING
ORDERING : IDF ORDERING_AUX
ORDERING_AUX : asc | desc 
LIMIT_CH :epsilone(allow,semicolone) |  limit integer_token 
ALLOW_CH :epsilone(semicolone) |  allow filtering 
---------------------------------------------------------------------------------------------
*/
//SELECT_STMT: select JSON SELECT_CLAUSE from IDF_P_IDF  WHERE_CH ORDER_BY_CH LIMIT_CH ALLOW_CH
//             semicolone
	boolean _select_stmt()
	{
		boolean result=false;
		if(token==SELECT)
		{
			
			token=_lire_token();
			if(_json())
			{
				token=_lire_token();
				if(_select_clause())
				{
					token=_lire_token();
					if(token=FROM_T)
					{
						token=_lire_token();
						if(_idf_p_idf())
						{
							token=_lire_token();
							if(_where_ch())
							{
								token=_lire_token();
								if(_order_by_ch())
								{
									token=_lire_token();
									if(_limit_ch())
									{
										token=_lire_token();
										if(_allow_ch())
										{
											token=_lire_token();
											if(token==SEMICOLON){

												// ici fin de commande alors debut de traitement :
												int i = select_cols(temp_tab,count,temp_cols,count2, temp_conds );
												if(i == -1){
													temp_err->type_err = TABLE_INEXISTANT ;
													creer_sm_erreur(*temp_err) ;
													free(temp_err);free(temp_tab);free(temp_cols);free(temp_conds);free(temp_vals);count=0;count2=0;

												}else if(i == -2){
													temp_err->type_err = COLONNE_INEXISTANT_IN_SELECT_CL ;
													creer_sm_erreur(*temp_err) ;
													free(temp_err);free(temp_tab);free(temp_cols);free(temp_conds);free(temp_vals);count=0;count2=0;
												}else if(i == -3){
													temp_err->type_err = COLONNE_INEXISTANT_IN_WHERE_CL ;
													creer_sm_erreur(*temp_err) ;
													free(temp_err);free(temp_tab);free(temp_cols);free(temp_conds);free(temp_vals);count=0;count2=0;
												}else if(i == -4){
													temp_err->type_err = INCOMPATIBLE_TYPE_COMPARAISON ;
													creer_sm_erreur(*temp_err) ;
													free(temp_err);free(temp_tab);free(temp_cols);free(temp_conds);free(temp_vals);count=0;count2=0;
												}else if(i == -5){
													temp_err->type_err = COLONNE_IS_NOT_PRK ;
													creer_sm_erreur(*temp_err) ;
													free(temp_err);free(temp_tab);free(temp_cols);free(temp_conds);free(temp_vals);count=0;count2=0;
												}else if(i == -11){
													temp_err->type_err = NO_KSP_USED ;
													creer_sm_erreur(*temp_err) ;
													free(temp_err);free(temp_tab);free(temp_cols);free(temp_conds);free(temp_vals);count=0;count2=0;
												}else{
													free(temp_err);free(temp_tab);free(temp_cols);free(temp_conds);free(temp_vals);count=0;count2=0;
												}


												result=true;
											}
										}
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
//FOLLOW DIAL JSON 
//JSON : epsilone(semicolon) | json 
	boolean _json()
	{
		boolean result=false;
		if (token==COUNT_T)
		{
			follow_token = true;
			result = true;
		}
		else if (token==IDENTIFICATEUR)
		{
			follow_token = true;
			result = true;
		}else if (token==ETOILE)
		{
			follow_token = true;
			result = true;
		}
		else if(token==DISTINCT)
		{
			follow_token = true;
			result = true;
		}
		else if(token==JSON){result=true;}
		return result;
	}
//SELECT_CLAUSE: DISTINCT_CH SELECT_LIST | count par ouv ( etoile | un )par_fer AS_IDF_CH
	boolean _select_clause()
	{

		boolean result=false;
		if(token==COUNT_T)
		{
			token=_lire_token();
			if(token==PAR_OUV)
			{
				token=_lire_token();
				if(_et_un())
				{
					token=_lire_token();
					if(token==PAR_FER)
					{
						token=_lire_token();
						if(_as_idf_ch()) { 
							result=true;
						}
					}
				}
			}
		}
		else if(_distinct_ch())
		{
			token=_lire_token();
			if(_select_list()) {result=true;}
		}
		return result;
	}
//ET_UN :  etoile | un 
//string token should be equal to 1 
	boolean _et_un()
	{
		boolean result=false;
		if(token==ETOILE) {result=true;}
		else if(token==STRING_TOKEN) {result=true;}
		return result;
	}
//DISTINCT_CH : epsilone (selector) | distinct
	boolean _distinct_ch()
	{ 
		boolean result=false;
		if(token==IDENTIFICATEUR)
		{
			follow_token = true;
			result = true;
		}
		else if(token==WRITETIME_T)
		{
			follow_token = true;
			result = true;
		}else if(token==ETOILE)
		{
			follow_token = true;
			result = true;
		}
		else if(token==TTL)
		{
			follow_token = true;
			result = true;
		}
		else if(token==DISTINCT) {result=true;}
		return result;
	}
//SELECT_LIST: SELECTOR_LIST | etoile
	boolean _select_list()
	{

		boolean result=false;
		if(_selector_list()) {result=true;}
		else if (token==ETOILE) {
			result=true;
			// traitement :
			temp_col = malloc(sizeof *temp_col);
			temp_col->nom = malloc(strlen("star") + 1) ;	strcpy(temp_col->nom, "star");

			count++ ;	
			temp_cols = (column_type*)realloc(temp_cols , count*sizeof(column_type)) ;
			temp_cols[count-1] = *temp_col ;
			free(temp_col);
		}
		return result;
	}
//SELECTOR_LIST : SELECTOR AS_IDF_CH SELECTOR_LIST_AUX
	boolean _selector_list()
	{
		boolean result=false;
		if(_selector())
		{
			token=_lire_token();
			if(_as_idf_ch())
			{
				token=_lire_token();
				if(_selector_list_aux()) {result=true;}
			}
		}
		return result;
	}
//SELECTOR_LIST_AUX : epsilone (from) |  virgule SELECTOR_LIST
	boolean _selector_list_aux()
	{
		boolean result=false;
		if(token==FROM_T)
		{
			follow_token = true;
			result = true;
		}
		else if (token==VIRGULE)
		{
			token=_lire_token();
			if(_selector_list()) {result=true;}
		}
		return result;
	}
//SELECTOR: IDF | writetime par_ouv IDF par_fer |TTL par_ouv IDF par_fer | 
		 // IDF_P_IDF par_ouv LIST_SELECTOR par_fer
	boolean _selector()
	{
		boolean result=false;
		if(_idf()) {

			// traitement :
			temp_col = malloc(sizeof *temp_col);
			temp_col->nom = malloc(strlen(yytext) + 1) ;	strcpy(temp_col->nom, yytext);

			count++ ;	
			temp_cols = (column_type*)realloc(temp_cols , count*sizeof(column_type)) ;
			temp_cols[count-1] = *temp_col ;
			free(temp_col);



			result=true;
		}
		else if (token==WRITETIME_T)
		{
			token=_lire_token();
			if(token==PAR_OUV)
			{
				token=_lire_token();
				if(_idf())
				{
					token=_lire_token();
					if(token==PAR_FER) {result=true;}
				}
			}
		}
	//  PROBLEME F cassandra.l :'( '
		else if (token==TTL)
		{
			token=_lire_token();
			if(token==PAR_OUV)
			{
				token=_lire_token();
				if(_idf())
				{
					token=_lire_token();
					if(token==PAR_FER) {result=true;}
				}
			}
		}
		else if(_idf_p_idf())
		{
			token=_lire_token();
			if(token==PAR_OUV)
			{
				token=_lire_token();
				if(_list_selector())
				{
					token=_lire_token();
					if(token==PAR_FER) {result=true;}
				}
			}
		}
		return result;
	}
//LIST_SELECTOR : SELECTOR LIST_SELECTOR_AUX
	boolean _list_selector()
	{
		boolean result=false;
		if(_selector())
		{
			token=_lire_token();
			if(_list_selector_aux()) {result=true;}
		}
		return result; 
	}
//LIST_SELECTOR_AUX : epsilone(par_fer) | virgule LIST_SELECTOR
	boolean _list_selector_aux()
	{
		boolean result=false;
		if(token==PAR_OUV)
		{
			follow_token = true;
			result = true;
		}
		else if (token==VIRGULE)
		{
			token=_lire_token();
			if(_list_selector()) {result=true;}
		}
		return result;
	}
//AS_IDF_CH epsilone (from,virgule)|as IDF
	boolean _as_idf_ch()
	{
		boolean result=false;
		if(token==FROM_T)
		{
			follow_token = true;
			result = true;
		}
		else if(token==VIRGULE)
		{
			follow_token = true;
			result = true;
		}
		else if(token==AS_T)
		{
			token=_lire_token();
			if(_idf()) {result=true;}
		}
		return result;
	}
//WHERE_CH : where WHERE_CL | epsilone(order,limit,allow,semicolone)
	boolean _where_ch()
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
		else if(token==WHERE)
		{
			token=_lire_token();
			if(_where_cl()) {result=true;}
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
		else if(token==AND)
		{
			token=_lire_token();
			if(_where_cl()) {result=true;}
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
			temp_cond = malloc(sizeof *temp_cond);
			temp_cond->nbr_val=0 ;
			temp_cond->col.nom = malloc(strlen(yytext) + 1) ;	strcpy(temp_cond->col.nom , yytext);

			token=_lire_token();
			if(_op())
			{

				// voire op :

				token=_lire_token();
				if(_term()) {

					(temp_cond->nbr_val)++ ; 
					temp_cond->values = (value_type*)realloc(temp_cond->values , (temp_cond->nbr_val)*sizeof(value_type)) ;
					temp_cond->values[temp_cond->nbr_val-1] = *temp_val ;
					free(temp_val);	

					count2++ ;	
					temp_conds = realloc(temp_conds , count2*sizeof(*temp_conds)) ;
					temp_conds[count2-1] = *temp_cond ;
					free(temp_cond);

					result=true;
				}
			}
			else if(token==IN)
			{


				temp_cond->op = IN_ ;
				token=_lire_token();
				if(token==PAR_OUV)
				{
					token=_lire_token();
					if(_term_list_ch())
					{
						token=_lire_token();
						if(token==PAR_FER) {
							result=true;

							count2++ ;	
							temp_conds = realloc(temp_conds , count2*sizeof(*temp_conds)) ;
							temp_conds[count2-1] = *temp_cond ;
							free(temp_cond);
						}
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
		if(token==EGAL) {result=true; temp_cond->op = EGAL_ ;}
		else if(token==INF) {result=true; temp_cond->op = INF_ ;}
		else if(token==SUP) {result=true; temp_cond->op = SUP_ ;}
		else if(token==INFE) {result=true; temp_cond->op = INFE_ ;}
		else if(token==SUPE) {result=true; temp_cond->op = SUPE_ ;}
		else if(token==CONTAINS) {result=true;}
		else if(token==CONTAINSKEY) {result=true;}
		return result;
	}
//ORDER_BY_CH : epsilone(allow,semicolone,limit) | order by ORDER_BY
	boolean _order_by_ch()
	{
		boolean result=false;
		if(token==LIMIT)
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
		else if(token==ORDER)
		{
			token=_lire_token();
			if(token==BY_T)
			{
				token=_lire_token();
				if(_order_by()) {result=true;}
			}

		}
		return result;
	}
//ORDER_BY : ORDERING ORDER_BY_AUX
	boolean _order_by()
	{
		boolean result=false;
		if(_ordering())
		{
			token=_lire_token();
			if(_order_by_aux()) {result=true;}
		}
		return result;
	}
//ORDER_BY_AUX : virgule ORDERING
	boolean _order_by_aux()
	{
		boolean result=false;
		if(token==LIMIT)
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
		if(token==VIRGULE)
		{
			token=_lire_token();
			if(_ordering()) {result=true;}
		}
		return result;
	}
//ORDERING : IDF ORDERING_AUX
	boolean _ordering()
	{
		boolean result=false;
		if(_idf())
		{
			printf("idf\n");
			token=_lire_token();
			if(_ordering_aux()) {result=true;}
		}
		return result;
	}
//ORDERING_AUX : asc | desc
	boolean _ordering_aux(){
		boolean result=false;
		if(token==VIRGULE)
		{
			follow_token = true;
			result = true;
		}
		else if(token==SEMICOLON)
		{
			follow_token = true;
			result = true;
		}
		else if(token==ASC) {printf("asc\n");
		result=true;}
		else if (token==DESC) {result=true;}
		return result;
	}
//LIMIT_CH :epsilone(allow,semicolone) |  limit integer_token 
	boolean _limit_ch()
	{
		boolean result=false;
		if(token==SEMICOLON)
		{
			follow_token = true;
			result = true;
		}
		else if(token==ALLOW)
		{
			follow_token = true;
			result = true;
		}
		if(token==LIMIT)
		{
			token=_lire_token();
			if(token==INTEGER_TOKEN)  {result=true;}
		}
		return result;
	}
//ALLOW_CH :epsilone(semicolone) |  allow filtering 
	boolean _allow_ch()
	{
		boolean result=false;
		if(token==SEMICOLON)
		{
			follow_token = true;
			result = true;
		}
		else if (token==ALLOW)
		{
			token=_lire_token();
			if(token==FILTERING)  {result=true;}
		}
		return result;
	}
/*
------------------ UPDATE ---------------------------------------------------------------------
UPDATE_STMT: update idf_p_idf USING_CH set LIST_ASSIGNMENT where WHERE_CL IF_CH
USING_CH : using LIST_OPTION | epsilone (set)
LIST_OPTIONS : OPTION LIST_OPTION_AUX
LIST_OPTION_AUX : epsilone(set) | and LIST_OPTIONS
OPTION : timestamp integer_token | ttl integer_token
LIST_ASSIGNMENT : ASSIGNMENT LIST_ASSIGNMENT_AUX
LIST_ASSIGNMENT_AUX : epslone(where) | virgule LIST_ASSIGNMENT
ASSIGNMENT : IDF egal TERM | 
IF_CH : epsilone(semicolone) | if LIST_COND
LIST_COND: CONDITION LIST_COND_AUX
LIST_COND_AUX : epsilone(epsilone) | and LIST_COND
CONDITION : IDF OP TERM
-----------------------------------------------------------------------------------------------
*/
//UPDATE_STMT: update idf_p_idf USING_CH set LIST_ASSIGNMENT where WHERE_CL IF_CH
	boolean _update_stmt()
	{
		boolean result=false;
		if(token==UPDATE)
		{
			token=_lire_token();
			if(_idf_p_idf())
			{
				token=_lire_token();
				if(_using_ch())
				{
					token=_lire_token();
					if(token==SET_TOKEN)
					{
						token=_lire_token();
						if(_list_assignment())
						{
							token=_lire_token();
							if(token==WHERE)
							{
								token=_lire_token();
								if(_where_cl())
								{
									token=_lire_token();
									if(_if_ch())
									{
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
		return result ;
	}
//USING_CH : using LIST_OPTION | epsilone (set)
	boolean _using_ch()
	{
		boolean result=false;
		if(token==SET_TOKEN)
		{
			follow_token = true;
			result = true;
		}
		else if(token==	USING)
		{
			token=_lire_token();
			if(_list_options()) {result=true;}
		}
		return result;
	}
//LIST_OPTIONS: OPTION LIST_OPTION_AUX
	boolean _list_options()
	{
		boolean result=false;
		if(_option())
		{

			token=_lire_token();
			if(_list_option_aux()) {result=true;}
		}
		return result;
	}
//LIST_OPTION_AUX : epsilone(set) | and LIST_OPTIONS
	boolean _list_option_aux()
	{	boolean result=false;
		if(token==SET_TOKEN)
		{
			follow_token = true;
			result = true;
		}
		else if(token==AND)
		{
			token=_lire_token();
			if(_list_options()) {result=true;}
		}
		return result;

	}
//LIST_ASSIGNMENT : ASSIGNMENT LIST_ASSIGNMENT_AUX
	boolean _list_assignment()
	{
		boolean result=false;
		if(_assignment())
		{
			token=_lire_token();
			if(_list_assignment_aux()) {result=true;}
		}
		return result;
	}
//ASSIGNMENT : IDF egal TERM | 
	boolean _assignment() 
	{
		boolean result=false;
		if(_idf())
		{
			token=_lire_token();
			if(token==EGAL)
			{
				token=_lire_token();
				if(_term()) {result=true;}
			}
		}
		return result;

	}
//LIST_ASSIGNMENT_AUX : epslone(where) | virgule LIST_ASSIGNMENT
	boolean _list_assignment_aux()
	{
		boolean result=false;
		if(token==WHERE)
		{
			follow_token = true;
			result = true;
		}
		else if(token==VIRGULE)
		{
			token=_lire_token();
			if(_list_assignment()) {result=true;}
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
//IF_CH : epsilone(semicolone) | if LIST_COND
	boolean _if_ch()
	{
		boolean result=false;
		if(token==SEMICOLON)
		{
			follow_token = true;
			result = true;
		}
		else if(token==IF_TOKEN)
		{
			token=_lire_token();
			if(_list_cond()) {result=true;}
		}
		return result;
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
//DELETE_STMT : delete LIST_SELECTION_CH from IDF_P_IDF USING_TIMES_CH where WHERE_CL IF_STMT_CH
			 // semicolon 
	boolean _delete_stmt()
	{
		boolean result=false;
		if(token==DELETE_T)
		{
			printf("1\n");
			token=_lire_token();
			if(_list_selection_ch())
			{
				printf("2\n");
				token=_lire_token();
				if(token==FROM_T)
				{
					printf("3\n");
					token=_lire_token();
					if(_idf_p_idf())
					{
						printf("4\n");
						token=_lire_token();
						if(_using_times_ch())
						{
							printf("5\n");
							token=_lire_token();
							if(token==WHERE)
							{
								printf("6\n");
								token=_lire_token();
								if(_where_cl())
								{
									printf("7\n");
									token=_lire_token();
									if(_if_stmt_ch())
									{
										printf("8\n");
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
//LIST_SELECTION_AUX : epsilone(from) | semicolon LIST_SELECTION 
	boolean _list_selection_aux()
	{
		boolean result=false;
		if(token==FROM_T)
		{
			follow_token = true;
			result = true;
		}
		else if(token==SEMICOLON)
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
			token=_lire_token();
			if(_term_ch()) {result=true;}
		}
		return result;
	}
//TERM_CH : accro_ouv TERM accro_fer | epsilone (virgule , from )
	boolean _term_ch()
	{
		boolean result=false;
		if(token==FROM_T)
		{
			follow_token = true;
			result = true;
		}
		else if(token==VIRGULE)
		{
			follow_token = true;
			result = true;
		}
		else if(token==CRO_OUV)
		{
			token=_lire_token();
			if(_term())
			{
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
			follow_token = true;
			result = true;
		}
		else if(token==USING)
		{
			token=_lire_token();
			if(token==TIMESTAMP)
			{
				token=_lire_token();
				if(token==INTEGER_TOKEN) {result=true;}
			}
		}
		return result;
	}
//IF_STMT_CH : epsilone(semicolon) | if IF_STMT
	boolean _if_stmt_ch()
	{
		boolean result=false;
		if(token==SEMICOLON)
		{
			follow_token = true;
			result = true;
		}
		else if(token==IF_TOKEN)
		{
			token=_lire_token();
			if(_if_stmt()) {result=true;}
		}
		return result;
	}
//IF_STMT: exists | LIST_COND
	boolean _if_stmt()
	{
		boolean result=false;
		if(token==EXISTS_TOKEN) {result=true;}
		else if(_list_cond()) {result=true;}
		return result;
	}
/*
------------------ DROP ------------------------------------------------------------------------
DROP_STMT :drop (DROP_AGGREGATE | DROP_FUNCTION |DROP | DROP_IDFP | DROP_TRIGGER)
------------------------------------------------------------------------------------------------
*/
//DROP_STMT :drop (DROP_AGGREGATE | DROP_FUNCTION |DROP | DROP_IDFP | DROP_TRIGGER)
	boolean _drop_stmt()
	{
		boolean result=false;
		if (token==DROP)
		{
	   		//printf("drp\n");
			token=_lire_token();
			if(_drop_function_aggregate()) { result=true;}
			else if(_drop()) {result=true;}
			else if(_drop_idfp()) { result=true;}
			else if(_drop_trigger()) { result=true;}
		}

		return result;
	}
/*
*******************DROP FUNCTION  && DROP AGGREGATE****************************************
DROP_FUNCTION_AGGREGATE: FCT_AGG IF_EXISTS IDF_P_IDF DROP_LIST semicolon
DROP_LIST :par_ouv FUNCTION_AUX par_fer |epsilon
FUNCTION_AUX: epsilon | ARG_SIGN
ARG_SIGN: ARG_TYPE ARG_SIGN_AUX
ARG_SIGN_AUX : epsilon | virgule ARG_SIGN
ARG_TYPE: INT | TEXT
IF_EXISTS: if exists | epsilon
IDF_P_IDF :  idf IDF_P_IDF_AUX
IDF_P_IDF_AUX :  point idf | epsilon
IDF : idf | double_quote idf double_quote
********************************************************************************************
*/
//DROP_FUNCTION: FCT_AGG IF_EXISTS IDF_P_IDF DROP_LIST semicolon
	boolean _drop_function_aggregate()
	{
		boolean result=false;
		if (_fct_agg())
		{
			printf("func\n");
			token=_lire_token();
			if(_if_exists())
			{
				token=_lire_token();
				if(_idf_p_idf())
				{
					token=_lire_token();
					if(_drop_list())
					{
						token=_lire_token();
						if (token==SEMICOLON)
						{
							result=true;
						}
					}
				}
			}
		}
		return result;
	}
	boolean _fct_agg()
	{
		boolean result=false;
		if(token==FUNCTION){result=true;}
		else if(token==AGGREGATE) {result=true;}
		return result;
	}
//DROP_LIST :par_ouv FUNCTION_AUX par_fer |epsilon
	boolean _drop_list()
	{
		boolean result=false;
		if(token==SEMICOLON)
		{
			follow_token = true;
			result = true;
		}
		else if(token==PAR_OUV)
		{
			token=_lire_token();
			if(_function_aux())
			{
				token=_lire_token();
				if(token==PAR_FER)
				{
					result=true;
				}
			}
		}
		return result;
	}
//FUNCTION_AUX: epsilon | ARG_SIGN
	boolean _function_aux()
	{
		boolean result=false;
		if(token==PAR_FER)
		{
			follow_token = true;
			result = true;
		}
		else if(_arg_sign())
		{
			result = true;
		}
		return result;

	}
//ARG_SIGN: ARG_TYPE ARG_SIGN_AUX
	boolean _arg_sign()
	{
		boolean result=false;
		if (_arg_type())
		{
			token=_lire_token();
			if (_arg_sign_aux()) {result=true;}
		}
		return result;
	}
//ARG_SIGN_AUX : epsilon | virgule ARG_SIGN 
	boolean _arg_sign_aux()
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
			if (_arg_sign()) {result=true;}
		}
		return result;
	}             
//ARG_TYPE:NATIVE_TYPE| COLLECTION_TYPE | IDF_P_IDF | tuple_type | string_token
	boolean _arg_type()
	{
		boolean result=false;
		if(_native_type()){result=true;}
		else if (_collection_type()) {result=true;}
		else if (_idf_p_idf()) {result=true;}
		else if (_tuple_type()) {result=true;}
		else if (token==STRING_TOKEN) {result=true;}
		return result;
	}
//TUPLE_TYPE :  tuple inf TUPLE_TYPE_AUX sup
	boolean _tuple_type() 
	{
		boolean result=false;
		if(token==TUPLE_TOKEN)
		{
			token=_lire_token();
			if (token==INF)
			{
				token=_lire_token();
				if (_tuple_type_aux())
				{
					token=_lire_token();
					if(token=SUP){result=true;}
				}		
			}
		}
		return result;
	}

//TUPLE_TYPE_AUX : ARG_TYPE  TUPLE_TYPE_X
//TUPLE_TYPE_X : virgule TUPLE_TYPE_AUX | epsilon
	boolean _tuple_type_aux()
	{
		boolean result=false;
		if (_arg_type())
		{
			token=_lire_token();
			if (_tuple_type_x()) {result=true;}
		}
		return result;
	}

	boolean _tuple_type_x()
	{
		boolean result=false;
		if(token==SUP)
		{
			follow_token = true;
			result = true;
		}
		else if(token==VIRGULE)
		{
			token=_lire_token();
			if (_tuple_type_aux()) {result=true;}
		}
		return result;
	}
// COLLECTION_TYPE : map inf ARG_TYPE virgule ARG_TYPE sup | set inf ARG_TYPE sup | list inf ARG_TYPE sup
	boolean _collection_type()
	{
		boolean result=false;
		if(token==MAP_TOKEN) 
		{
			token=_lire_token();
			if(token==INF)
			{
				token=_lire_token();
				if(_arg_type())
				{
					token=_lire_token();
					if(token==VIRGULE)
					{
						token=_lire_token();
						if(_arg_type())
						{
							token=_lire_token();
							if(token==SUP) {result=true;}
						}
					}
				}
			}
		}
		if(token==SET_TOKEN)
		{
			token=_lire_token();
			if(token==INF)
			{
				token=_lire_token();
				if(_arg_type())
				{
					token=_lire_token();
					if(token==SUP) {result=true;}
				}
			}
		}	
		if(token==LIST_TOKEN)
		{
			token=_lire_token();
			if(token==INF)
			{
				token=_lire_token();
				if(_arg_type())
				{
					token=_lire_token();
					if(token==SUP) {result=true;}
				}
			}
		}	

		return result;
	}

//NATIVE_TYPE: ascii | bigint | blob | boolean | counter | date | decimal | double | float | int | text | inet | smallint | time | timeuuid | tinyint | uuid | varchar | varint
	boolean _native_type()
	{
		boolean result=false;
		if(token ==ASCII_T) {result=true;}
		else if (token==BIGINT_T) {result=true;}
		else if (token==BLOB_T) {result=true;}
		else if (token==BOOLEAN_T) {result=true;}
		else if (token==COUNTER_T) {result=true;}
		else if (token==DATE_T) {result=true;}
		else if (token==DECIMAL_T) {result=true;}
		else if (token==DOUBLE_T) {result=true;}
		else if (token==FLOAT_T) {result=true;}
		else if (token==INT_T) {result=true;}
		else if (token==TEXT_T) {result=true;}
		else if (token==INET_T) {result=true;}
		else if (token==SMALLINT_T) {result=true;}
		else if (token==TIME_T) {result=true;}
		else if (token==TIMEUUID_T) {result=true;}
		else if (token==TINYINT_T) {result=true;}
		else if (token==UUID_T) {result=true;}
		else if (token==VARCHAR_T) {result=true;}
		else if (token==VARINT_T) {result=true;}
		return result;
	}
	boolean _user_defined_type();
	boolean _custom_type();
//IF_EXISTS: if exists | epsilon
	boolean _if_exists()
	{
		boolean result=false;
		if(token==IDENTIFICATEUR)
		{
			follow_token = true;
			result = true;
		}
		if(token==DOUBLE_QUOTE)
		{
			follow_token = true;
			result = true;
		}
		else if(token == IF_TOKEN)
		{
			token = _lire_token();
			if(token == EXISTS_TOKEN) {result = true;}
		}
		return result;
	}

/*
********************DROP KEYSPACE && DROP ROLE && DROP USER***************************************
DROP: DROP_TYPE IF_EXISTS  IDF semicolon
DROP_TYPE : keyspace | role  | user 
IF_EXISTS: if exists | epsilon
IDF : idf | double_quote idf double_quote
***************************************************************************************************
*/
//DROP: DROP_TYPE IF_EXISTS  DROP_AUX semicolon
	boolean _drop(){
		boolean result=false;
		if (_drop_type())
		{
			token=_lire_token();
			if (_if_exists())
			{
				token=_lire_token();
				if (_idf())
				{

									// drop what ?? :
					if(op_on==KEYSPACE){
										// préparation des donnée temporaire :	
						temp_ksp = malloc(sizeof *temp_ksp);
						temp_ksp->nom = malloc(strlen(yytext) + 1) ;	strcpy(temp_ksp->nom, yytext);

						temp_err = malloc(sizeof *temp_err);
						temp_err->num_ligne = (int)yylineno ;
						temp_err->nom_source = malloc(strlen(yytext) + 1) ;	strcpy(temp_err->nom_source, yytext);
						temp_err->type_err = KEYSPACE_INEXISTANT ;
					}

					token=_lire_token();
					if (token==SEMICOLON)
					{	

						if(remove_ksp(temp_ksp) != -1){
							printf( "\033[1m\033[32m" "keyspace %s dropped \n" "\x1b[0m" , temp_ksp->nom);
							free(temp_ksp);	free(temp_err);
						}else{
							creer_sm_erreur(*temp_err) ;
							free(temp_ksp);	free(temp_err);
						}

						result=true;
					}
				}					
			}	
		}
		return result;

	}

// DROP_TYPE : keyspace | role  | user 
	boolean _drop_type()
	{ 
		boolean result= false;
		if (token == KEYSPACE) {result = true;	op_on = KEYSPACE ;	}
		else if (token == ROLE) {result = true;	op_on = ROLE ;	}
		else if (token == USER) {result = true;	op_on = USER ;	}
		return result;
	}
/*
********************DROP TABLE && DROP TYPE && DROP VIEW && DROP INDEX**************************
DROP_IDFP: TYPE_T IF_EXISTS IDF_P_IDF semicolon
TYPE T : table | type | VIEW | index
IF_EXISTS: if exists | epsilon
IDF_P_IDF :  idf IDF_P_IDF_AUX
IDF_P_IDF_AUX :  point idf | epsilon
IDF : idf | double_quote idf double_quote
******************************************************************************************
*/
//DROP_IDFP: TYPE_T IF_EXISTS IDF_P_IDF semicolon
	boolean _drop_idfp(){
		boolean result=false;
		if (_type_t())
		{
			token=_lire_token();
			if (_if_exists())
			{
				token=_lire_token();
				if (_idf_p_idf())
				{
					token=_lire_token();
					if (token==SEMICOLON)
					{
						int i = remove_tab(temp_tab) ;
						if(i == 1){
							printf( "\033[1m\033[32m" "table %s dropped \n" "\x1b[0m" , temp_tab->nom);
							free(temp_tab);	free(temp_err);
						}else if (i==-1){
							temp_err->type_err = TABLE_INEXISTANT ;
							creer_sm_erreur(*temp_err) ;
							free(temp_tab);	free(temp_err);
						}else if (i==-11){
							temp_err->type_err = NO_KSP_USED ;
							creer_sm_erreur(*temp_err) ;
							free(temp_tab);	free(temp_err);
						}

						result=true;
					}
				}					
			}	
		}
		return result;

	}
//TYPE T : table | type | VIEW | index
	boolean _type_t()
	{
		boolean result=false;
		if (token==TABLE){result=true; op_on = TABLE;}
		else if (token==TYPE) {result=true;}
		else if (_view()) {result=true;}
		else if (token==INDEX) {result=true;}
		return result;
	}

//VIEW :  materialized view  
	boolean _view()
	{
		if (token==MATERIALIZED)
		{ 
			token=_lire_token();
			if (token== VIEW)
			{ 
				result= true;
			}
		}
		return result;
	}
/*
********************DROP TRIGGER******************************************************************
DROP_TRIGGER: trigger IF_EXISTS IDF ON IDF_P_IDF semicolon
IF_EXISTS: if exists | epsilon
IDF_P_IDF :  idf IDF_P_IDF_AUX
IDF_P_IDF_AUX :  point idf | epsilon
IDF : idf | double_quote idf double_quote
*********************************************************************************************
*/
//DROP_TRIGGER: trigger IF_EXISTS IDF ON IDF_P_IDF semicolon
	boolean _drop_trigger()
	{
		boolean result=false;
		if (token==TRIGGER)
		{
			printf("tri\n");
			token=_lire_token();
			if(_if_exists())
			{
				printf("if ex\n");
				token=_lire_token();
				if(_idf())
				{
					printf("idf\n");
					token=_lire_token();
					if (token==ON)
					{
						printf("on\n");
						token=_lire_token();
						if(_idf_p_idf())
						{
							printf("idf p \n");
							token=_lire_token();
							if(token==SEMICOLON){result=true;}
						}
					}
				}
			}
		}
		return result;
	}

/*
--------------------- INSERT ---------------------------------------------------------------------------
INSERT : insert into IDF_P_IDF LIST_JSON IF_NOT_EX OPTION_X  semicolon
LIST_JSON : NAME_LIST values_token VALUE_LIST | json IDF
NAME_LIST : par_ouv LIST_IDF par_fer
LIST_IDF : IDF LIST_IDF_AUX
LIST_IDF_AUX : virg LIST_IDF | epsilon
VALUE_LIST : par_ouv LIST_TERM_OR_LITER par_fer
LIST_TERM_OR_LITER :  TERM LIST_TERM_OR_LITER_AUX
LIST_IDF_AUX : virg LIST_TERM_OR_LITER | epsilon
TERM : : CONSTANT | COLLECTION_LITER | VARIABLE | FUNCTION par_ouv TERM_LIST_X par_fer
COLLECTION_LITER : LIST_LITERAL | acco_ouv MAP_SET_LITERAL acco_fer
MAP_SET_LITERAL : epsilon | TERM (MAP_LITERAL_AUX | SET_LITERAL_AUX)
SET_LITERAL_AUX: epsilon | virgule MAP_SET_LITERAL
MAP_LITERAL_AUX: deux_point TERM TERM_TERM_LIST_AUX
TERM_TERM_LIST_AUX: epsilon | virgule MAP_SET_LITERAL
LIST_LITERAL:cro_ouv LIST_LITERAL_AUX cro_fer
LIST_LITERAL_AUX:epsilon | TERM_LIST_X
TERM_LIST_X: TERM TERM_LIST_AUX
TERM_LIST_AUX : epsilon | virgule TERM_LIST_X
IDF_P_IDF : IDF IDF_P_IDF_AUX
IDF_P_IDF_AUX : epsilon| point IDF 
IDF : idf | double_quote idf double_quote
CONSTANT : STRING | NUMBER | BOOLEAN | uuid_token | blob_token | null_token
FLOAT: float_token | nan | infinity
BOOLEAN : true_token |false_token
VARIABLE: point_intero | deux_point IDF
IF_NOT_EX : epsilon | if_token not_token exists_token
OPTION_X : epsilon | using OPTION_LIST
OPTION_LIST : OPTION OPTION_LIST_AUX
OPTION_LIST_AUX : and OPTION_LIST | epsilon
OPTION : timestamp int | ttl int
-----------------------------------------------------------------------------------------------------
*/
//INSERT : insert into IDF_P_IDF LIST_JSON IF_NOT_EX OPTION_X  semicolon
	boolean _insert_stmt()
	{
		boolean result=false;
		if (token==INSERT)
		{
			token=_lire_token();
			if (token==INTO)
			{
				token=_lire_token();
				if (_idf_p_idf())
				{
					token=_lire_token();
					if (_list_json())
					{
						token=_lire_token();
						if (_if_not_ex())
						{
							token=_lire_token();
							if (_option_x())
							{
								token=_lire_token();
								if(token==SEMICOLON)
								{

									// traitement (temp_table,temp_error,temp_cols et temp_vals et leurs tailles sont déja crées n'oublie pas l'initialisation)
									int i = insert_line_ifnx(temp_tab,count,temp_cols,count2,temp_vals);
									if(i == -1){
										temp_err->type_err = TABLE_INEXISTANT ;
										creer_sm_erreur(*temp_err) ;
										free(temp_err);free(temp_tab);free(temp_cols);free(temp_vals);count=0;count2=0;
									}else if(i == -2){
										temp_err->type_err = DEPASSEMENT_ARGUMENTS ;
										creer_sm_erreur(*temp_err) ;
										free(temp_err);free(temp_tab);free(temp_cols);free(temp_vals);count=0;count2=0;
									}else if(i == -3){
										temp_err->type_err = ARGUMENT_INSUFISANT ;
										creer_sm_erreur(*temp_err) ;
										free(temp_err);free(temp_tab);free(temp_cols);free(temp_vals);count=0;count2=0;
									}else if(i == -4){
										temp_err->type_err = COLONNE_INEXISTANT ;
										creer_sm_erreur(*temp_err) ;
										free(temp_err);free(temp_tab);free(temp_cols);free(temp_vals);count=0;count2=0;
									}else if(i == -5){
										temp_err->type_err = INCOMPATIBLE_TYPE_COLONNE ;
										creer_sm_erreur(*temp_err) ;
										free(temp_err);free(temp_tab);free(temp_cols);free(temp_vals);count=0;count2=0;
									}else if(i == -11){
										temp_err->type_err = NO_KSP_USED ;
										creer_sm_erreur(*temp_err) ;
										free(temp_err);free(temp_tab);free(temp_cols);free(temp_vals);count=0;count2=0;
									}else if(i == -12){
										temp_err->type_err = DOUBLE_PRK_VALUE ;
										creer_sm_erreur(*temp_err) ;
										free(temp_err);free(temp_tab);free(temp_cols);free(temp_vals);count=0;count2=0;
									}else{
										printf( "\033[1m\033[32m" "ligne inserée\n" "\x1b[0m" );
										free(temp_err);free(temp_tab);free(temp_cols);free(temp_vals);count=0;count2=0;
									}

									result=true;
								}
							}
						}		
					}
				}
			}
		}
		return result;
	}
//OPTION_X : epsilon | using OPTION_LIST	
	boolean	_option_x()
	{
		boolean result;
		if(token==SEMICOLON)
		{
			follow_token = true;
			result = true;
		}
		else if(token==USING)
		{
			token = _lire_token();
			if(_option_list())
			{
				result = true;
			}
			else result = false;
		}
		else result = false;
		return result;
	}

//OPTION_LIST : OPTION OPTION_LIST_AUX
//OPTION_LIST_AUX : and OPTION_LIST | epsilon	
	boolean _option_list()
	{
		boolean result;
		if (_option())
		{
			token = _lire_token();
			if (_option_list_aux())
			{
				result=true;
			}	
			else result=false;
		}
		else result=false;
		return result;
	}

	boolean _option_list_aux()
	{
		boolean result;
		if(token==SEMICOLON)
		{
			follow_token = true;
			result = true;
		}
		else if(token==AND)
		{
			token = _lire_token();
			if(_option_list())
			{
				result = true;
			}
			else
			{
				result = false;
			}
		}
		else
		{
			result = false;
		}
		return result;
	}

//OPTION : timestamp int | ttl int
	boolean _option()
	{
		boolean result=false ; 
		if (token==TIMESTAMP)
		{
			token=_lire_token();
			if (token==INTEGER_TOKEN) {result=true;}
		}
		else if (token==TTL)
		{
			token=_lire_token();
			if (token==INTEGER_TOKEN) {result=true;}
		}
		return result;
	}
//IF_NOT_EX : epsilon | if_token not_token exists_token
	boolean _if_not_ex()
	{
		boolean result;
		if (token==USING)
		{
			follow_token=true;
			result=true;
		}
		else if (token==SEMICOLON)
		{
			follow_token=true;
			result=true;
		}
		else if (_idf())
		{
			follow_token=true;
			result=true;
		}
		else if (token==IF_TOKEN)
		{
			token=_lire_token();
			if (token==NOT_TOKEN)
			{
				token=_lire_token();
				if (token==EXISTS_TOKEN)
				{
					result=true;
				}
				else result=false ;
			}
			else result=false ;
		}
		else result=false ;
		return result;
	}
//LIST_JSON : NAME_LIST values_token VALUE_LIST | json IDF
	boolean _list_json()
	{
		boolean result=false;
		if (_name_list())
		{
			token=_lire_token();
			if (token==VALUES_TOKEN)
			{
				token=_lire_token();
				if (_value_list())
				{
					result=true;
				}
			}
		}
		else if (token==JSON)
		{
			token=_lire_token();
			if (token==STRING_TOKEN)
			{
				result=true;
			}
		}
		return result;
	}

//NAME_LIST : par_ouv LIST_IDF par_fer
	boolean _name_list()
	{
		boolean result;
		if (token==PAR_OUV)
		{
			token=_lire_token();
			if(_list_idf())
			{
				token=_lire_token();
				if (token==PAR_FER)
				{
					result=true;
				}
				else result=false;	
			}
			else result=false;
		}
		else result=false;
		return result;
	}

//LIST_IDF : IDF LIST_IDF_AUX
//LIST_IDF_AUX : virg LIST_IDF | epsilon
	boolean _list_idf()
	{
		boolean result ;
		if (_idf())
		{

			// traitement :
			temp_col = malloc(sizeof *temp_col);
			temp_col->nom = malloc(strlen(yytext) + 1) ;	strcpy(temp_col->nom, yytext);

			count++ ;	
			temp_cols = (column_type*)realloc(temp_cols , count*sizeof(column_type)) ;
			temp_cols[count-1] = *temp_col ;
			free(temp_col);


			token=_lire_token();
			if(_list_idf_aux())
			{
				result=true;
			}
			else result=false;
		}
		else result=false;
		return result;
	}

	boolean _list_idf_aux()
	{
		boolean result;
		if (token== PAR_FER)
		{
			follow_token = true;
			result = true;
		}
		else if (token== VIRGULE)
		{
			token=_lire_token();
			if(_list_idf())
			{
				result=true;
			}
			else result=false;	
		}
		else result=false;
		return result;
	}

//VALUE_LIST : par_ouv LIST_TERM_OR_LITER par_fer
	boolean _value_list()
	{
		boolean result=false;
		if (token==PAR_OUV)
		{
			token=_lire_token();
			if (_list_term_or_liter())
			{	
				token=_lire_token();
				if (token==PAR_FER)
				{	
					result=true;
				}
			}
		}
		return result;
	}
//LIST_TERM_OR_LITER :  TERM LIST_TERM_OR_LITER_AUX
//LIST_IDF_AUX : virg LIST_TERM_OR_LITER | epsilon
	boolean _list_term_or_liter()
	{
		boolean result ;
		if (_term())
		{

			// traitement (sachant qu'une temp_val est déja crée lors de l'appel de term() )

			count2++ ;	
			temp_vals = (value_type*)realloc(temp_vals , count2*sizeof(value_type)) ;
			temp_vals[count2-1] = *temp_val ;
			free(temp_val);


			token=_lire_token();
			if(_list_term_or_liter_aux())
			{
				result=true;
			}
			else result=false;
		}
		else result=false;
		return result;
	}

	boolean _list_term_or_liter_aux()
	{
		boolean result;
		if (token== PAR_FER)
		{
			follow_token = true;
			result = true;
		}
		else if (token== VIRGULE)
		{
			token=_lire_token();
			if(_list_term_or_liter())
			{
				result=true;
			}
			else result=false;	
		}
		else result=false;
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


			temp_cond->nbr_val++ ; 
			temp_cond->values = (value_type*)realloc(temp_cond->values , temp_cond->nbr_val*sizeof(value_type)) ;
			temp_cond->values[temp_cond->nbr_val-1] = *temp_val ;
			free(temp_val);	

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

//IDF_P_IDF : IDF IDF_P_IDF_AUX
	boolean _idf_p_idf()
	{
		boolean result=false;
		if(_idf())
		{

			// préparation des donnée temporaire :	
			temp_tab = malloc(sizeof *temp_tab);
			temp_tab->nom = malloc(strlen(yytext) + 1) ;	strcpy(temp_tab->nom, yytext);
			temp_tab->nb_lines = 0 ;

			temp_err = malloc(sizeof *temp_err);
			temp_err->num_ligne = (int)yylineno ;
			temp_err->nom_source = malloc(strlen(yytext) + 1) ;	strcpy(temp_err->nom_source, yytext);
			temp_err->type_err = TABLE_DEJA_DECLAREE ;

			token=_lire_token();
			if(_idf_p_idf_aux()) {result=true;}
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

			temp_val = malloc(sizeof *temp_val);
			temp_val->val = malloc(strlen(yytext) + 1) ;	strcpy(temp_val->val, yytext);
			temp_val->type = STRING_ ;


			result = true;
		}
		else if (token==INTEGER_TOKEN) {

			temp_val = malloc(sizeof *temp_val);
			temp_val->val = malloc(strlen(yytext) + 1) ;	strcpy(temp_val->val, yytext);
			temp_val->type = INT_ ;

			result = true;
		}
		else if (_float()) {

			temp_val = malloc(sizeof *temp_val);
			temp_val->val = malloc(strlen(yytext) + 1) ;	strcpy(temp_val->val, yytext);
			temp_val->type = DOUBLE_ ;

			result = true;
		}
		else if (_boolean()) {

			//voire boolean :

			result = true;
		}
		else if (token==UUID_TOKEN) {result = true;}
		else if (token==BLOB_TOKEN) {result = true;}
		else if (token==NULL_TOKEN) {result = true;}	
		else if(token == AST_TOKEN){
			token = _lire_token() ;
			if(token == CRO_OUV){
				token = _lire_token() ;
				if(_addsub()){
					token = _lire_token() ;
					if(token == CRO_FER){

						// fin d'instruction :
						char s[256];
						sprintf(s, "%d", (int)evaluer(mon_ast));
						// int i = (int)evaluer(mon_ast) ;
						printf("\neval == %s\n", s);
						printf("AST postfixe : ");
						afficher_postfixe_arbre(mon_ast) ;
						printf("\n");
						printf("AST infixe : ");
						afficher_infixe_arbre(mon_ast) ;
						free_ast(mon_ast);
						mon_ast = NULL ;
						
						temp_val = malloc(sizeof *temp_val);
						temp_val->val = malloc(strlen(s) + 1) ;	strcpy(temp_val->val, s);
						temp_val->type = INT_ ;

						result = true;
					}
				}
			}
		}	
		return result;

	}

	// les prédicats de calcules des expressions :
	// addsub :
	boolean _addsub(){
		boolean result=false ;
		if(_multdiv()){
			token = _lire_token() ;
			if(_addsubaux()){
				result = true ;
			}
		}
		return result ;
	}


	// multdiv :
	boolean _multdiv(){
		boolean result = false ;
		if(_aux()){
			token=_lire_token() ;
			if(_multdivaux()){
				result = true ;
			}
		}
		return result ;
	}

	// aux :
	boolean _aux(){
		boolean result=false ;
		if(token==INTEGER_TOKEN){
			// ajout dans l'AST de l'int
			tmp_ast = creer_feuille_nombre(strtod(yytext,NULL), INT_ );
			if(mon_ast==NULL){
				mon_ast = tmp_ast ;
			}else{
				mon_ast->noeud.op.expression_droite = tmp_ast ;
			}
			
			result=true ;
		}else if(token==PAR_OUV){

			token=_lire_token();
			if(_addsub()){
				token=_lire_token() ;
				if(token==PAR_FER){
					result=true ;
				}
			}
		}
		return result ;
	}


	// multdivaux :
	boolean _multdivaux(){
		boolean result = false;
		if(token==PLUS ){
			result=true ;
			follow_token = true ;
		}else if(token==CRO_FER ){
			result=true ;
			follow_token = true ;
		}else if(token==PAR_FER ){
			result=true ;
			follow_token = true ;
		}else if( token==MINUS){
			result=true ;
			follow_token = true ;
		}
		else if(token==ETOILE){
			// ajout dans l'AST de l'opérateur mult :
			tmp_ast = creer_noeud_operation('*', mon_ast, NULL , OP__);
			mon_ast = tmp_ast ;
			
			token=_lire_token() ;
			if(_aux()){
				token=_lire_token() ;
				if(_multdivaux()){
					result=true ;
				}
			}
		}else if(token==DIV){

			// ajout dans l'AST de l'opérateur mult :
			tmp_ast = creer_noeud_operation('/', mon_ast, NULL , OP__);
			mon_ast = tmp_ast ;
			

			token=_lire_token() ;
			if(_aux()){
				token=_lire_token() ;
				if(_multdivaux()){
					result=true ;
				}
			}
		}
		return result ;

	}

	


	// addsubaux() :
	boolean _addsubaux(){
		boolean result=false ;
		if(token==PAR_FER){
			follow_token = true ;
			result=true ;
		}else if(token==CRO_FER){
			follow_token = true ;
			result=true ;
		}else if(token == PLUS){
			// ajout dans l'AST de l'opérateur plus :
			tmp_ast = creer_noeud_operation('+', mon_ast, NULL , OP__);
			mon_ast = tmp_ast ;
			
			

			token = _lire_token() ;
			if(_multdiv()){
				token=_lire_token() ;
				if(_addsubaux()){
					result=true ;
				}
			}
		}else if(token == MINUS){

			// ajout dans l'AST de l'opérateur minus :
			tmp_ast = creer_noeud_operation('-', mon_ast, NULL , OP__);
			mon_ast = tmp_ast ;
			

			token = _lire_token() ;
			if(_multdiv()){
				token=_lire_token() ;
				if(_addsubaux()){
					result=true ;
				}
			}
		}
		return result ;
	}





//FLOAT: float_token | nan | infinity 
	boolean _float()
	{
		boolean result=false;
		if (token==FLOAT_TOKEN) {result = true;}
		else if (token==NAN) {result = true;}
		else if (token==INFINITY) {result = true;}	
		return result;
	}

// BOOLEAN : true_token |false_token
	boolean _boolean()
	{
		boolean result=false;
		if (token==TRUE_TOKEN) {
			
			temp_val = malloc(sizeof *temp_val);
			temp_val->val = malloc(strlen("1") + 1) ;	strcpy(temp_val->val, "1");
			temp_val->type = BOOL_ ;

			result=true ;
		}
		else if (token==FALSE_TOKEN) {
			
			temp_val = malloc(sizeof *temp_val);
			temp_val->val = malloc(strlen("0") + 1) ;	strcpy(temp_val->val, "0");
			temp_val->type = BOOL_ ;

			result=true ;
		}
		return result;
	}

//VARIABLE: point_intero | deux_point IDF
	boolean _variable(){
		boolean result;
		if (token==POINT_INTERO)
		{
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
/*
-------------- BATCH -----------------------------------------------------------------------
BATCH_STMT : begin UNLO_COUN batch USING_OPT_LIST MODIF_STMT_LIST apply batch semicolon
UNLO_COUN : unlogged | counter
USING_OPT_LIST :epsilone(insert,update , delete ) | using OPT_LIST
OPT_LIST: OPTION_TIMESTAMP OPT_LIST_AUX
OPTION_TIMESTAMP : timestamp integer_token
OPT_LIST_AUX : epsilone(insert,update , delete) | and OPT_LIST
MODIF_STMT_LIST: MODIF_STMT MODIF_STMT_LIST_AUX
MODIF_STMT_LIST_AUX : epsilone(apply) | semicolone MODIF_STMT_LIST
MODIF_STMT : INSERT_STMT | UPDATE_STMT | DELETE_STMT 
---------------------------------------------------------------------------------------------
*/
//BATCH_STMT : begin UNLO_COUN batch USING_OPT_LIST MODIF_STMT_LIST apply batch semicolon
	boolean _batch_stmt()
	{
		boolean result=false; 
		if(token==BEGIN_T)
		{
			token=_lire_token();
			if(_unlo_coun())
			{
				token=_lire_token();
				if(token==BATCH)
				{
					token=_lire_token();
					if(_using_opt_list())
					{
						token=_lire_token();
						if(_modif_stmt_list())
						{
							token=_lire_token();
							if(token==APPLY_T)
							{
								token=_lire_token();
								if(token==BATCH)
								{
									token=_lire_token();
									if(token==SEMICOLON) {result=true;}							
								}
							}	
						}
					}
				}
			}
		}
		return result;
	}
//UNLO_COUN : unlogged | counter
	boolean _unlo_coun()
	{
		boolean result=false; 
		if (token==	BATCH)
		{
			follow_token = true;
			result = true;
		}
		else if(token==UNLOGGED) {result=true;}
		else if(token==COUNTER_T) {result=true;}
		return result;
	}
//USING_OPT_LIST :epsilone(insert,update , delete ) | using OPT_LIST
	boolean _using_opt_list()
	{
		boolean result=false;
		if (token==INSERT)
		{
			follow_token = true;
			result = true;
		}
		else if (token==UPDATE)
		{
			follow_token = true;
			result = true;
		}
		else if (token==DELETE_T)
		{
			follow_token = true;
			result = true;
		}
		else if(token==USING)
		{
			token=_lire_token();
			if(_opt_list()) {result=true;}					
		}
		return result;
	}
//OPT_LIST: OPTION_TIMESTAMP OPT_LIST_AUX
	boolean _opt_list()
	{
		boolean result=false;
		if(_option_timestamp())
		{
			token=_lire_token();
			if(_opt_list_aux()) {result=true;}	
		}
		return result;
	}
//OPTION_TIMESTAMP : timestamp integer_token
	boolean _option_timestamp()
	{
		boolean result=false;
		if(token==TIMESTAMP)
		{
			token=_lire_token();
			if(token==INTEGER_TOKEN) {result=true;}	
		}
		return result;
	}
//OPT_LIST_AUX : epsilone(insert,update , delete) | and OPT_LIST
	boolean _opt_list_aux()
	{
		boolean result=false;
		if (token==INSERT)
		{
			follow_token = true;
			result = true;
		}
		else if (token==UPDATE)
		{
			follow_token = true;
			result = true;
		}
		else if (token==DELETE)
		{
			follow_token = true;
			result = true;
		}
		else if (token==AND)
		{
			token=_lire_token();
			if(_opt_list()) {result=true;}	
		}
		return result;
	}
//MODIF_STMT_LIST: MODIF_STMT MODIF_STMT_LIST_AUX
	boolean _modif_stmt_list()
	{
		boolean result=false;
		if(_modif_stmt())
		{
			token=_lire_token();
			if(_modif_stmt_list_aux()){result=true;}
		}
		return result;
	}
//MODIF_STMT_LIST_AUX : epsilone(apply) | semicolone MODIF_STMT_LIST
	boolean _modif_stmt_list_aux()
	{
		boolean result=false;
		if (token==APPLY_T)
		{
			follow_token = true;
			result = true;
		}
		else if (_modif_stmt_list()){result=true;}
		return result;
	}
//MODIF_STMT : INSERT_STMT | UPDATE_STMT | DELETE_STMT 
	boolean _modif_stmt()
	{
		boolean result=false;
		if (_insert_stmt()) {result=true;}
		else if(_update_stmt()) {result=true;}
		else if (_delete_stmt()) {result=true;}
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
