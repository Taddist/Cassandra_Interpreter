#ifndef ERROR_CASSANDRA_H
#define ERROR_CASSANDRA_H



typedef enum {
	keyspace_not_found,
	table_not_found ,
 	column_not_found ,
	incompatible_affectation ,
	no_keyspace ,
  	incomp_aff ,
	keyspace_use_table_not_found
 } SemanticErrorType;

typedef struct _smError {
	char* nom_source ;
	SemanticErrorType type_err ;
} smError;



#endif


void creer_file_erreur() ; 
void creer_sm_erreur(SemanticErrorType _type_err, char* _nom_source) ; 


int nombre_sm_erreurs();
void afficher_sm_erreurs() ; 
