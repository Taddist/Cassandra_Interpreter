#include <stdio.h>	// pour le printf true/false
#include <string.h>	// pour les strcpy,strlen...
#include <stdlib.h>	// pour les malloc,realloc
#include "op_k.h"	// notre interface
#include <unistd.h> // pour la fonction access
#include <sys/stat.h> // pour le mkdir
#include <dirent.h>
#include <sys/types.h>


// current keyspace (after use)
static char current_ksp[256] ;

// emplacement de dossier contenant les keyspaces :
static const char* path_ksp = "data/" ;


boolean exist_ksp(ksp_type* ptr_ksp) {

	char path[256];
	strcpy(path, path_ksp);
	strcat(path, ptr_ksp->nom );
	if( access( path , F_OK ) != -1 ) {
    	return true ;
	} else {
    	return false;
	}
}

int create_ksp(ksp_type* ptr_ksp){
	
	
	if(exist_ksp(ptr_ksp)){
		//perror("ksp déja existe\n");
		return -1 ;
		
	}else{
		char path[256];
		strcpy(path, path_ksp);
		strcat(path, ptr_ksp->nom );
		printf("%s\n", path);
		mkdir(path, 0777);
		return 1 ;
	}

}




int remove_ksp(ksp_type* ptr_ksp){

    char name[256];
    strcpy(name, path_ksp);
    strcat(name, ptr_ksp->nom );


    DIR *directory;           /* pointeur de répertoire */
    struct dirent *entry;     /* représente une entrée dans un répertoire. */
    struct stat file_stat;    /* informations sur un fichier. */

    /* Ce tableau servira à stocker le chemin d'accès complet
     * des fichiers et dossiers. Pour simplifier l'exemple,
     * je le définis comme un tableau statique (avec une taille
     * a priori suffisante pour la majorité des situations),
     * mais on pourrait l'allouer dynamiquement pour pouvoir
     * le redimensionner si jamais on tombait sur un chemin
     * d'accès démesurément long. */
    char buffer[1024] = {0};

    /* On ouvre le dossier. */
    directory = opendir(name);
    if ( directory == NULL ) {
        //printf("cannot open directory %s\n", name);
        return -1;
    }

    /* On boucle sur les entrées du dossier. */
    while ( (entry = readdir(directory)) != NULL ) {

        /* On "saute" les répertoires "." et "..". */
        if ( strcmp(entry->d_name, ".") == 0 ||
             strcmp(entry->d_name, "..") == 0 ) {
            continue;
        }

        /* On construit le chemin d'accès du fichier en
         * concaténant son nom avec le nom du dossier
         * parent. On intercale "/" entre les deux.
         * NB: '/' est aussi utilisable sous Windows
         * comme séparateur de dossier. */
        snprintf(buffer, 1024, "%s/%s", name, entry->d_name);

        /* On récupère des infos sur le fichier. */
        stat(buffer, &file_stat);
        /* J'ai la flemme de tester la valeur de retour, mais
         * dans un vrai programme il faudrait le faire :D */

        if ( S_ISREG(file_stat.st_mode) ) {
            /* On est sur un fichier, on le supprime. */
            unlink(buffer);
        }
        else if ( S_ISDIR(file_stat.st_mode) ) {
            /* On est sur un dossier, on appelle cette fonction. */
            rmdir(buffer);
        }
    }

    /* On ferme le dossier. */
    closedir(directory);

    /* Maintenant le dossier doit être vide, on le supprime. */
    remove(name);
    /* J'ai toujours la flemme de tester la valeur de retour... */

    return 1;
} 



int set_current_ksp(ksp_type* ptr_ksp){
    if(!exist_ksp(ptr_ksp)){
        return -1 ;
    }else{
        strcpy(current_ksp,ptr_ksp->nom) ;
        save_current() ;
        return 1 ;
    }
}

void get_current_ksp(char* curr){
    
    load_current() ;
    strcpy(curr , current_ksp) ;
    // printf("sss\n");
}

void load_current(){
    FILE *curr = fopen("current.ksp" , "r+");
    char s[256] ;
    fgets (s, 256, curr);
    strcpy(current_ksp , s) ;
    // printf("current == %s\n", s);
    fclose(curr) ;
}


void save_current(){
    FILE *curr = fopen("current.ksp" , "w+");
    fputs  (current_ksp, curr);
    fclose(curr) ;
}

void free_current(){
    FILE *curr = fopen("current.ksp" , "w+");
    fputs  ("_", curr);
    fclose(curr) ;
}