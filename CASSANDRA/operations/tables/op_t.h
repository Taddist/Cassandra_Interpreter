#ifndef OP_H
#define OP_H


#include "../../cassandra.h"	// notre interface d'analyse (just pour les booleans :p )

#include "../../error.h"		// pour reconnaitre les types d'erreurs de keyspace

#include "../keyspaces/op_k.h"		// pour reconnaitre le keyspace



typedef enum {
	INT_ , DOUBLE_ , BOOL_ ,STRING_ ,OP__
 }type_var;


typedef struct column_type {
	char* nom ;		//star pour select * 
	type_var type ;
	boolean prk ; //primary key or not 
} column_type;




typedef struct table_type {
	char* nom ;
	int nb_lines ;
	int nb_columns ;
	column_type* columns ;
} table_type;





// test de d'existance d'un keyspace :
boolean exist_tab(table_type* ptr_tab) ;

// creation d'un KSP (KSP=keyspace :) ) -1 au cas d'error
int create_tab(table_type* ptr_tab) ;

// drop/suppression d'un keyspace -1 au cas d'error :
int remove_tab(table_type* ptr_tab) ;

// alter/modification de la table :





#endif
