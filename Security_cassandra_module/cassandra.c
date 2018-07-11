#include "cassandra.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include "tabsymb.h"
extern int yylex();
extern char* yytext ;
typetoken token ; 
boolean follow_token = false ;

static roleValue RV;
static auth A;
int main(int argc, char **argv){
	
	token = _lire_token(); 
	  	if (_use_stmt()) 
	  	{
	  		if(check_right())
			{
				printf("Keyspace Changed  \n");
			}
			else 
			{
				// name of role 
				creer_sm_erreur(access_denied,"");
				afficher_sm_erreurs();
			}	
	  	}
		else if (_create_stmt()) 
			{
				creer_file_erreur() ;
				if(nombre_sm_erreurs()==0)
				{
					printf("Exécution Réussite Create  \n");
					printf("Rows created (1)\n");
					check_right();
				}
				else 
				{
					printf("Error!!\n");
					afficher_sm_erreurs();
				}
			}
			
		else if (_alter_stmt()) 
			{
				creer_file_erreur() ;
				if(nombre_sm_erreurs()==0)
				{
					printf("Exécution Réussite Alter\n");
					printf("Values afffected (%d)\n",RV.count_value);
				//afficher_roleChange();
				}
				else 
				{
					printf("Error!!\n");
					afficher_sm_erreurs();
				}
			}
		
		else if (_drop_stmt())
			{	
				creer_file_erreur() ;
				if(nombre_sm_erreurs()==0)
				{
					//afficher_roleDrop();
					printf("Exécution Réussite Drop \n");
					printf("Rows created (1)\n");
				}
				else 
				{
					printf("Error!!\n");
					afficher_sm_erreurs();
				}
				
			}
		else if (_list_stmt())
		{
			if(check_right())
			{

				if(strcmp(RV.name_role,"NULL")==0)
				{
					list_roles();
					printf("Exécution Réussite List \n");
				}
				else 
				{
					list_specific(RV.name_role);
					
				}
			}
			else 
			{
				creer_sm_erreur(access_denied,RV.name_role);
				afficher_sm_erreurs();
			}		
		}
		else if (_grant_stmt())
		{
			creer_file_erreur() ;
			if(check_right())
			{
				printf("Exécution Réussite Grant \n");
			}
			else 
			{
				// name of role 
				creer_sm_erreur(access_denied,"");
				afficher_sm_erreurs();
			}	
		}
		else if (_start_cqlsh())
		{
			
				creer_file_erreur() ;
				if(nombre_sm_erreurs()==0)
				{
					printf("Exécution Réussite Start CQLSH \n");
				}
				else 
				{
					printf("Error!!\n");
					afficher_sm_erreurs();
				}
			
		}
		else printf("erreurs  \n");

    return 0;
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
			token=_lire_token();
			if(token==SEMICOLON) {result=true;}
			}
	}
	return result;
}





boolean _start_cqlsh()
{
	boolean result=false;
	if(token==CQLSH)
	{
		token=_lire_token();
		if(token==U)
		{
			token=_lire_token();
			if(token==IDENTIFICATEUR)
			{
				A.login_name = malloc(strlen(yytext) + 1) ;
				strcpy(A.login_name, yytext);
				token=_lire_token();
				if(token==P)
				{
					token=_lire_token();
					if(token==IDENTIFICATEUR)
					{
						A.password = malloc(strlen(yytext) + 1) ;
						strcpy(A.password, yytext);
						creer_auth(A.login_name,A.password);
						token=_lire_token();
						if(token==SEMICOLON) {result=true;}
					}
				}
			}
		}
	}
	return result;
}





boolean _grant_stmt()
{
	boolean result=false;
	if(token==GRANT_TOKEN)
	{
		token=_lire_token();
		if(_permission_ch())
		{
			token=_lire_token();
			if(token==ON)
			{
				token=_lire_token();
				if(_ressource())
				{
					token=_lire_token();
					if(token==TO_TOKEN)
					{
						token=_lire_token();
						if(_idf())
						{
							token=_lire_token();
							if(token==SEMICOLON){result=true;}
						}
					}
				}
			}
		}
	}
	return result;
}
boolean _permission_ch()
{
	boolean result=false;
	if(token=ALL)
	{
		token=_lire_token();
		if(_permissions_m()) {result=true;}
	}
	else if (_permission())
	{
		token=_lire_token();
		if(_permission_m()) {result=true;}
	}
	return result;
}
boolean _permissions_m()
{
	boolean result=false;
	if(token==ON)
		{
			follow_token = true;
			result = true;
		}
	else if(_permission_ch()){result = true;}
	return result;
}
boolean _permission()
{
	boolean result=false;
	if(token==CREATE) {result = true;}
	else if(token==ALTER_T) {result = true;}
	else if(token==DROP) {result = true;}
	else if(token==SELECT) {result = true;}
	else if(token==MODIFY) {result = true;}
	else if(token==AUTHORIZE) {result = true;}
	else if(token==DESCRIBE_T) {result = true;}
	else if(token==EXECUTE) {result = true;}
	return result;
}
boolean _permission_m()
{
	boolean result=false;
	if(token==ON)
		{
			follow_token = true;
			result = true;
		}
	else if(_permission()){result = true;}
	return result;
}
boolean _ressource()
{
	boolean result=false;
	if(token==ALL_KEYSPACES){result=true;}
	if(token==KEYSPACE)
	{
		token=_lire_token();
		if(_idf()){result=true;}
	}
	if(token==ALL_ROLES){result=true;}
	
	return result;
}








boolean _list_stmt()
{
	boolean result=false;
	if(token==LIST_TOKEN)
	{
		token=_lire_token();
		{
			if(token==ROLES)
			{
				token=_lire_token();
				if(_of_idf())
				{
					token=_lire_token();
					if(_norec())
					{
						token=_lire_token();
						if(token==SEMICOLON) {result=true;}
					}
				}
			}
		}
	}
	return result;
}

boolean _of_idf()
{
	boolean result=false;
	if(token==SEMICOLON)
		{
			RV.name_role = malloc(strlen(yytext) + 1) ;
			strcpy(RV.name_role, "NULL");
			follow_token = true;
			result = true;
		}
	else if(token==NORECURSIVE)
		{
			RV.name_role = malloc(strlen(yytext) + 1) ;
			strcpy(RV.name_role, "NULL");
			follow_token = true;
			result = true;
		}
	else if(token == OF)
		{
			token = _lire_token();
			if(_idf()) {
				RV.name_role = malloc(strlen(yytext) + 1) ;
				strcpy(RV.name_role, yytext);
				result = true;}
		}
	return result;
}
boolean _norec()
{
	boolean result=false;
	if(token==SEMICOLON)
		{
			follow_token = true;
			result = true;
		}
	else if(token==NORECURSIVE)
		{
			
			result = true;
		}
	return result;
}
/*
-----------------DROP-----------------------------------------------------------------------
-------------------------------------------------------------------------------------------
*/
boolean _drop_stmt()
{
	boolean result=false;
    if (token==DROP)
	   	{
	   		token=_lire_token();
			if(_drop_role()) { result=true;}
	   	}
	return result;

}
boolean _drop_role()
{
	boolean result=false;
	if(token==ROLE)
	{
		token=_lire_token();
		if(_if_exists())
		{
			token=_lire_token();
			if(_idf())
			{
				RV.name_role = malloc(strlen(yytext) + 1) ;
				strcpy(RV.name_role, yytext);
				token=_lire_token();
				creer_droprole(RV.name_role,RV.if_ex_value);
				if(token==SEMICOLON){result=true;}
			}
		}
	}
	return result;
}

//IF_EXISTS: if exists | epsilon
boolean _if_exists()
{
	boolean result=false;
	if(token==IDENTIFICATEUR)
		{
			RV.if_ex_value = malloc(strlen(yytext) + 1) ;
			strcpy(RV.if_ex_value,"false");
			follow_token = true;
			result = true;
		}
	else if(token==DOUBLE_QUOTE)
		{
			RV.if_ex_value = malloc(strlen(yytext) + 1) ;
			strcpy(RV.if_ex_value,"false");
			follow_token = true;
			result = true;
		}
	else if(token == IF_TOKEN)
		{
			token = _lire_token();
			if(token == EXISTS_TOKEN) {
				RV.if_ex_value = malloc(strlen(yytext) + 1) ;
				strcpy(RV.if_ex_value,"true");
				result = true;}
		}
	return result;
}

boolean _alter_stmt()
{
	boolean result=false;
	if(token==ALTER_T)
	{
		token=_lire_token();
		if(_alter_role()) {result=true;}
	}
	return result;
}
boolean _alter_role()
{
	boolean result=false;
	if(token==ROLE)
	{	
		RV.typerole =1;
		token=_lire_token();
		if(_idf())
		{
			RV.name_role = malloc(strlen(yytext) + 1) ;
			strcpy(RV.name_role, yytext);
			token=_lire_token();
			if(_list_option_role())
			{
				RV.if_ex_value = malloc(strlen(yytext) + 1) ;
				strcpy(RV.if_ex_value, "NULL");
				creer_roleValue(RV.name_role,RV.password_value,RV.login_value,RV.superuser_value,RV.if_ex_value,RV.count_value,RV.typerole);
				token=_lire_token();
				if(token==SEMICOLON){result=true;}
			}
		}
	}
	return result;
}




/*
------------------CREATE -------------------------------------------------------------------
CREATE_STMT: create (CREATE_KEYSPACE| CREATE_TABLE | CREATE_INDEX| CREATE_ROLE)
--------------------------------------------------------------------------------------------
*/
boolean _create_stmt()
{
	boolean result=false;
    if (token==CREATE)
	   	{
	   		token=_lire_token();
			if(_create_role()) { result=true;}
	   	}
	return result;

}
/*
***************** CREATE ROLE***************************************************************
CREATE_ROLE: role IF_NOT_EX IDF (LIST_OPTION) semicolon
LIST_OPTION_ROLE: with OPTION_B  | epsilone(semicolon)
OPTION_B : OPTION_PLS LIST_OPTION_AUX
LIST_OPTION_AUX : and OPTION_B | epsilone (semicolon)
********************************************************************************************
*/	
boolean _create_role()
{
	boolean result=false;
	if(token==ROLE)
	{
		RV.typerole =0;
		token=_lire_token();
		if(_if_not_ex())
		{

			token=_lire_token();
			if(_idf())
			{
				RV.name_role = malloc(strlen(yytext) + 1) ;
				strcpy(RV.name_role, yytext);
				token=_lire_token();
				if(_list_option_role())
				{

					creer_roleValue(RV.name_role,RV.password_value,RV.login_value,RV.superuser_value,RV.if_ex_value,RV.count_value,RV.typerole);
					token=_lire_token();
					if(token==SEMICOLON){result=true;}
				}
			}

		}
	}
	return result;
}
//LIST_OPTION_ROLE: with OPTION_B  | epsilone(semicolon)
boolean _list_option_role()
{
	boolean result=false;
	if (token==SEMICOLON)
		{
			if(RV.typerole==0)
			{
				RV.password_value = malloc(strlen(yytext) + 1) ;
				strcpy(RV.password_value, "");
				RV.login_value = malloc(strlen(yytext) + 1) ;
				strcpy(RV.login_value, "false");
				RV.superuser_value = malloc(strlen(yytext) + 1) ;
				strcpy(RV.superuser_value, "false");
				RV.count_value=0;
			}
			else if(RV.typerole==1)
			{
				RV.password_value = malloc(strlen(yytext) + 1) ;
				strcpy(RV.password_value, "NULL");
				RV.login_value = malloc(strlen(yytext) + 1) ;
				strcpy(RV.login_value, "NULL");
				RV.superuser_value = malloc(strlen(yytext) + 1) ;
				strcpy(RV.superuser_value, "NULL");
				RV.count_value=0;
			}
			follow_token=true;
			result=true;
		}
	else if (token==WITH)
		{
			token=_lire_token();
			if (_option_b()) {result=true;}	
		}
	return result;
}
//OPTION_B : OPTION_PLS LIST_OPTION_AUX
boolean _option_b()
{
	boolean result=false;
	if(_option_pls())
	{
		token=_lire_token();
		if(_list_option_aux()) {result=true;}
	}
	return result;
}
boolean _option_pls()
{
	boolean result=false;
	if(token==PASSWORD)
	{
		token=_lire_token();
		if(token==EGAL)
		{
			
			token=_lire_token();
			if(token==STRING_TOKEN)
			{
				RV.password_value = malloc(strlen(yytext) + 1) ;
				strcpy(RV.password_value, yytext);
				RV.count_value=1;
				result=true;
			}
		}
		
	}
	else if(token==LOGIN)
	{
		token=_lire_token();
		if(token==EGAL)
		{
			token=_lire_token();
			if(_boolean())
			{
				
					RV.login_value = malloc(strlen(yytext) + 1) ;
					strcpy(RV.login_value, yytext);
					RV.count_value=2;
				
				result=true;
			}
		}
		
	}
	else if(token==SUPERUSER)
	{
		token=_lire_token();
		if(token==EGAL)
		{
			token=_lire_token();
			if(_boolean()){
				
					RV.superuser_value = malloc(strlen(yytext) + 1) ;
					strcpy(RV.superuser_value, yytext);
					RV.count_value=3;
				
				result=true;
			}
		}
	}
	return result;
}
//LIST_OPTION_AUX : and OPTION_B | epsilone (semicolon)
boolean _list_option_aux(){
	boolean result=false;
	if (token==SEMICOLON)
		{
			if(RV.typerole==0)
				{
					if(RV.count_value==1)
					{
						RV.login_value = malloc(strlen(yytext) + 1) ;
						strcpy(RV.login_value, "false");
						RV.superuser_value = malloc(strlen(yytext) + 1) ;
						strcpy(RV.superuser_value, "false");
					}
					else if(RV.count_value==2)
					{
						RV.superuser_value = malloc(strlen(yytext) + 1) ;
						strcpy(RV.superuser_value, "false");
					}
				}
			else if(RV.typerole==1)
				{
					if(RV.count_value==1)
					{
						RV.login_value = malloc(strlen(yytext) + 1) ;
						strcpy(RV.login_value, "NULL");
						RV.superuser_value = malloc(strlen(yytext) + 1) ;
						strcpy(RV.superuser_value, "NULL");
					}
					else if(RV.count_value==2)
					{
						RV.superuser_value = malloc(strlen(yytext) + 1) ;
						strcpy(RV.superuser_value, "NULL");
					}
				}
			follow_token=true;
			result=true;
		}
	else if (token==AND)
		{
			token=_lire_token();
			if (_option_b()) {result=true;}	
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

//IF_NOT_EX : epsilon | if_token not_token exists_token
boolean _if_not_ex()
{
	boolean result=false;
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
	else if (token==IDENTIFICATEUR)
		{
			RV.if_ex_value = malloc(strlen(yytext) + 1) ;
			strcpy(RV.if_ex_value, "false");
			follow_token=true;
			result=true;
		}
	else if (token==DOUBLE_QUOTE)
		{
			RV.if_ex_value = malloc(strlen(yytext) + 1) ;
			strcpy(RV.if_ex_value, "false");
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
							RV.if_ex_value = malloc(strlen(yytext) + 1) ;
							strcpy(RV.if_ex_value, "true");
							result=true;
						}
					}
			}
	return result;
}

// BOOLEAN : true_token |false_token
boolean _boolean()
{
	boolean result=false;
	if (token==TRUE_TOKEN) { result=true ;}
	else if (token==FALSE_TOKEN) {result=true ;}
	return result;
}


// lit le prochaine token s'il n'a pas déjà été lu pRV le prédicat d'un nullable 
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
