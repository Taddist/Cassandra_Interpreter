#ifndef ERROR_H
#define ERROR_H



typedef enum {
    KEYSPACE_EXIST_DEJA,
	KEYSPACE_INEXISTANT,
    TABLE_INEXISTANT,
    TABLE_DEJA_DECLAREE,
    COLONNE_DEJA_DECLAREE,
    COLONNE_INEXISTANT,
    INCOMPATIBLE_TYPE_COLONNE,
    INCOMPATIBLE_TYPE_COMPARAISON,
    FUNCTION_DEJA_DECLARE,
    FUNCTION_INEXISTANT,
    DEPASSEMENT_ARGUMENTS,
    ARGUMENT_INSUFISANT,
    COLONNE_INEXISTANT_IN_SELECT_CL,
    COLONNE_INEXISTANT_IN_WHERE_CL,
    COLONNE_IS_NOT_PRK,
    NO_KSP_USED,
    DOUBLE_PRK_VALUE
 } SemanticErrorType;

// le structure (type) qui defini les erreur semantiques :
typedef struct _smError {
	int num_ligne ;
	char* nom_source ;
	SemanticErrorType type_err ;
} smError;






// les constructeurs :
void creer_file_erreur() ; // cree la structure FIFO globale
void creer_sm_erreur(smError err) ; // cree et ajoute une erreur semantique a la structure FIFO

// les getters :
int nombre_sm_erreurs(); // renvoie le nombre d'erreurs
void afficher_sm_erreurs() ; // affiche les erreurs


#endif