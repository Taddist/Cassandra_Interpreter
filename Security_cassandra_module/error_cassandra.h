#ifndef ERROR_CASSANDRA_H
#define ERROR_CASSANDRA_H



typedef enum {
	role_exists,role_not_exists,incorrect_password,access_denied,file_not_exists
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
