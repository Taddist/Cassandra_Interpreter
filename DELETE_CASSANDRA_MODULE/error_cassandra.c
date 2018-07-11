#include <stdio.h>	
#include <string.h>	
#include <stdlib.h>	

#include "error_cassandra.h"
//declaration de tableau global qui est notre file des erreurs :
static smError* file_err ;
static int taille_file_err ;	



void creer_file_erreur(){
	smError* file_err=NULL ;	
	int taille_file_err=0 ;	
}

void creer_sm_erreur(SemanticErrorType _type_err, char* _nom_source){
	smError tmp ;
	tmp.nom_source = malloc(strlen(_nom_source) + 1) ;	strcpy(tmp.nom_source, _nom_source);
	tmp.type_err = _type_err ;
	
	taille_file_err++ ;
	smError* tempo = (smError*)realloc(file_err , taille_file_err*sizeof(smError)) ;
	if(tempo == NULL){
		perror("realloc creer_sm_erreur\n") ;
	}
	file_err = tempo ;
	file_err[taille_file_err-1] = tmp ;
}




int nombre_sm_erreurs(){
	return taille_file_err ;
}
void afficher_sm_erreurs(){
	
	printf("%d erreurs semantiques :\n",taille_file_err );
	for(int i=0 ; i<taille_file_err ; i++){
		char *tp ;
		switch (file_err[i].type_err) {
			case keyspace_not_found :
				tp = malloc(strlen("keyspace not found") + 1) ;	strcpy(tp, "keyspace not found");
				break;
			case table_not_found :
				tp = malloc(strlen("table not found") + 1) ;	strcpy(tp, "table not found");
				break;
			case column_not_found :
				tp = malloc(strlen("column not found") + 1) ;	strcpy(tp, "column not found");
				break;
			case incompatible_affectation :
				tp = malloc(strlen("incompatible affectation") + 1) ;	strcpy(tp, "incompatible affectation");
				break;
			case no_keyspace : 
				tp = malloc(strlen("No keyspace selected ") + 1) ;	strcpy(tp, "No keyspace selected");
				break;

		} // show keyspace selected 
		printf("\t %s : %s \n",file_err[i].nom_source,tp);
	}
}
