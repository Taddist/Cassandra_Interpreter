#ifndef ERROR_H
#define ERROR_H



typedef enum {
	variable_deja_declaree , variable_mal_initialisee  , var_non_decl , incomp_aff
 } SemanticErrorType;

// le structure (type) qui defini les erreur semantiques :
typedef struct _smError {
	int num_ligne ;
	char* nom_source ;
	SemanticErrorType type_err ;
} smError;



#endif


// les constructeurs :
void creer_file_erreur() ; // cree la structure FIFO globale
void creer_sm_erreur(SemanticErrorType _type_err, int _num_ligne, char* _nom_source) ; // cree et ajoute une erreur semantique a la structure FIFO

// les getters :
int nombre_sm_erreurs(); // renvoie le nombre d'erreurs
void afficher_sm_erreurs() ; // affiche les erreurs