#ifndef CURD_H
#define CURD_H


#include "../../cassandra.h"	// notre interface d'analyse (just pour les booleans :p )

#include "../../error.h"		// pour reconnaitre les types d'erreurs de keyspace

#include "../tables/op_t.h"		
#include "../keyspaces/op_k.h"		// pour reconnaitre le keyspace


// pour definir le type de comparaison dens les conditions :
typedef enum {EGAL_=0,INF_=1,SUP_=2,INFE_=3,SUPE_=4,IN_=5} op_type;


typedef struct value_type {
	type_var type ;
	char* val ;	// !!!! 0 ou 1 pour le boolean
} value_type;





typedef struct condition_type {
	column_type col ;
	op_type op ;
	int nbr_val ;	// toujours =1 sauf le cas de IN on aura plusieurs valeurs
	value_type* values ;	//c'est un array de valeurs ( plus qu'1 element au cas de IN )
} condition_type;





// insertion d'une ligne (-1 au cas d'erreur)
int insert_line_ifnx(table_type* ptr_tab ,int nb_col, column_type *columns,int nb_val, value_type *values ) ;

// suppression d'une ligne (-1 au cas d'error) :
// int remove_tab(table_type* ptr_tab) ;

// fonctions de test :
int load_cols(table_type* ptr_tab) ;

// chargement de data en mémoire :
// void load_data(table_type* ptr_tab) ;

// fonction d'affichage (select) :
int select_cols(table_type* ptr_tab ,int nb_col, column_type *columns,int nb_cond, condition_type *conditions ) ;


#endif
