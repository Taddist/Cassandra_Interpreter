#ifndef TABSYMB_H
#define TABSYMB_H
#include "cassandra.h"	

//#include "error.h"
typedef enum {
	EGAL_,INF_,SUP_,INFE_,SUPE_,CONTAINS_,CONTAINSKEY_
 }type_op;




typedef struct varvalueType {
	char* keyspace_name ;
	char* table_name  ;
	char* timestamp_value;
	char* if_exists;
	int ligne_decl ;
} varvalueType;

typedef struct selection{
	char* column_name ;
	char* term ;
	int ligne_decl ;
} selection;

typedef struct whereClause {
	char* column_name_w ;
	char* table_name ;
	type_op operator;
	char* column_value ;
	char* type_value ;
	int ligne_decl ;
} whereClause;

void creer_file_idf() ; 
void creer_file_selection() ;
void creer_file_whereClause() ;
void creer_varvalueType(char* keyspace_name,char* table_name,char* timestamp_value,char* if_exists,int _ligne_decl) ; 
void creer_selection(char* column_name,char* term ,int _ligne_decl) ;
void creer_whereClause(char* column_name_w,type_op operator,char* column_value,char* type_value,int _ligne_decl,char* table_name) ;
// test de declaration des variables :
//varvalueType* exist(char* idf) ;

// les getters :
int nombre_idf(); // renvoie le nombre d'erreurs
void afficher_idf() ; // affiche les erreurs
int nombre_selection();
void afficher_selection() ;
int nombre_whereClause();
void afficher_whereClause() ;
// la fonction de comparaisom des elements de notre file :
static int cmp_idf (const void* a, const void* b);



#endif
