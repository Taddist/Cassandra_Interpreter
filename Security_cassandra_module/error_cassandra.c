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
			case role_exists :
				tp = malloc(strlen("Role already exists") + 1) ;	strcpy(tp, "Role already exists ");
				break;
			case role_not_exists:
				tp = malloc(strlen("Role doesn't exist") + 1) ;	strcpy(tp, "Role doesn't exist ");
				break;
			case incorrect_password:
				tp = malloc(strlen("Incorrect Password") + 1) ;	strcpy(tp, "Incorrect Password");
				break;
			case access_denied:
				tp = malloc(strlen("Access Denied") + 1) ;	strcpy(tp, "Access Denied");
				break;
			case file_not_exists:
				tp = malloc(strlen("File doesn't exist") + 1) ;	strcpy(tp, "File doesn't exist");
				break;
		} // show keyspace selected 
		printf("\t %s : %s \n",file_err[i].nom_source,tp);
	}
}
