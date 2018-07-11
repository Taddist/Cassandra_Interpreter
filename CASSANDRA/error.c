#include <stdio.h>	// pour le printf true/false
#include <string.h>	// pour les strcpy,strlen...
#include <stdlib.h>	// pour les malloc,realloc
#include "error.h"	// notre interface


//declaration de tableau global qui est notre file des erreurs :
static smError* file_err ;
static int taille_file_err ;	// sa taille


 // cree la structure FIFO globale
void creer_file_erreur(){
	smError* file_err=NULL ;	//notre file
	int taille_file_err=0 ;	// sa taille
}


// cree et ajoute une erreur semantique a la structure FIFO
void creer_sm_erreur(smError err) {
	// append :
	taille_file_err++ ;
	smError* tempo = (smError*)realloc(file_err , taille_file_err*sizeof(smError)) ;
	// attention realloc fait le free(file_err) implicitement
	// tout ce qu'on a pour le moment c'est le tempo
	if(tempo == NULL){
		printf("\nExiting!! :realloc creer_sm_erreur\n");
    	exit(0);
	}
	file_err = tempo ;
	//free(tempo) ; attention ici l'affectation est faite par pointer cad si on free temp c'est free(file_err)
	file_err[taille_file_err-1] = err ;
}



// les getters :
int nombre_sm_erreurs(){
	return taille_file_err ;
}
void afficher_sm_erreurs(){
	
	if(!taille_file_err){
		printf( "\033[1m\033[32m" "semantiquement correcte\n" "\x1b[0m");
	}else{
		printf( "\033[1m\033[31m" "%d erreurs semantiques :\n" "\x1b[0m" ,taille_file_err );
	}


	
	char tp[256] ;
	for(int i=0 ; i<taille_file_err ; i++){
		switch (file_err[i].type_err) {
			case KEYSPACE_EXIST_DEJA :
				strcpy(tp , "keyspace (schem) existe déja !!") ;
				break;
			case KEYSPACE_INEXISTANT :
				strcpy(tp , "keyspace (schem) inexistant !!") ;
				break;
			case TABLE_INEXISTANT :
				strcpy(tp , "table inexistante !!") ;
				break;
			case TABLE_DEJA_DECLAREE :
				strcpy(tp , "table existe déja !!") ;
				break;
			case DEPASSEMENT_ARGUMENTS :
				strcpy(tp , "arguments insuffisant pour values !!") ;
				break;
			case ARGUMENT_INSUFISANT :
				strcpy(tp , "dépassement d'arguments pour values !!") ;
				break;
			case COLONNE_INEXISTANT :
				strcpy(tp , "colonne inexistante !!") ;
				break;
			case INCOMPATIBLE_TYPE_COLONNE :
				strcpy(tp , "type de valeur incompatible !!") ;
				break;
			case COLONNE_INEXISTANT_IN_WHERE_CL :
				strcpy(tp , "colonne (dans where clause) inexistante !!") ;
				break;
			case COLONNE_INEXISTANT_IN_SELECT_CL :
				strcpy(tp , "colonne (dans select clause) inexistante !!") ;
				break;
			case INCOMPATIBLE_TYPE_COMPARAISON :
				strcpy(tp , "comparaison de type incompatible (dans where clause) !!") ;
				break;
			case COLONNE_IS_NOT_PRK :
				strcpy(tp , "colonne (dans where clause) n'est pas primary_key !!") ;
				break;
			case NO_KSP_USED :
				strcpy(tp , "aucun keyspace n'est utilisé !!") ;
				break;
			case DOUBLE_PRK_VALUE :
				strcpy(tp , "valeur de la clée primaire déja existe !!") ;
				break;

		}
		printf("\033[1m\033[31m" "\tligne %d : %s ---> %s\n" "\x1b[0m",file_err[i].num_ligne,file_err[i].nom_source,tp  );
	}
	printf("\n\n");
}