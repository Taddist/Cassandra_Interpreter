#include <stdio.h>	// pour le printf true/false
#include <string.h>	// pour les strcpy,strlen...
#include <stdlib.h>	// pour les malloc,realloc
#include "op_t.h"	// notre interface
#include <unistd.h> // pour la fonction access
#include <sys/stat.h> // pour le mkdir
#include <dirent.h>
#include <sys/types.h>
#include <jansson.h>


// emplacement de dossier contenant les keyspaces :
static const char* path_tab = "data/" ;











boolean exist_tab(table_type* ptr_tab) {

    char path[256];
    char curr[256];
    get_current_ksp(curr) ;
    strcpy(path, path_tab);
    strcat(path, curr );
    strcat(path, "/" );
    strcat(path, ptr_tab->nom );
    strcat(path, ".json" );
    if( access( path , F_OK ) != -1 ) {
    	return true ;
    } else {
    	return false;
    }
}


 








int create_tab(table_type* ptr_tab){


	char curr[256];
    get_current_ksp(curr) ;
    if (!strcmp("_" , curr))
    {
        return -11 ; // no ksp used
    }


	if(exist_tab(ptr_tab)){
		//perror("ksp déja existe\n");
		return -1 ;		
	}else{
		char path[256];

        strcpy(path, path_tab);
        strcat(path, curr );
        strcat(path, "/" );
        strcat(path, ptr_tab->nom );
        strcat(path, ".json" );
        // printf("%s\n", path);
        //##############################################


        json_t * root = json_object() ;
        json_t * jdesc = json_object() ;
        json_t * jcolumns = json_object() ;
                // json_t * jnom_c = json_object() ;
        json_t * jdata = json_object() ;


        for(int i=0 ; i< (ptr_tab->nb_columns) ; i++){
            json_t * jnom_c = json_object() ;
        //jnom_c
            type_var x = ptr_tab->columns[i].type ; 
            json_object_set_new(jnom_c, "type_c", json_string((x==INT_) ? "int" : (x==DOUBLE_) ? "double" : (x==BOOL_)? "bool" : "string"));
            json_object_set_new(jnom_c, "prk_c", json_boolean(ptr_tab->columns[i].prk) );

        //jcolumns :
            json_object_set_new(jcolumns , ptr_tab->columns[i].nom , jnom_c) ;
        }

    // jdesc :
        json_object_set_new(jdesc, "nom_t", json_string(ptr_tab->nom) );
        json_object_set_new(jdesc, "nb_lines_t", json_integer(ptr_tab->nb_lines) );
        json_object_set_new(jdesc, "nb_columns_t", json_integer(ptr_tab->nb_columns) );
        json_object_set_new(jdesc, "columns_t", jcolumns );

    // root :
        json_object_set_new(root, "desc", jdesc );
        json_object_set_new(root, "data", jdata );


        FILE* fichier = fopen( path , "w+") ;

        if(!fichier){
            printf("path invalide\n");
        }

        json_dumpf(root , fichier , JSON_INDENT(4)) ;

        fclose(fichier) ;

		//##############################################
        return 1 ;
    }

}






int remove_tab(table_type* ptr_tab){

    char curr[256];
    get_current_ksp(curr) ;
    if (!strcmp("_" , curr))
    {
        return -11 ; // no ksp used
    }

    if(!exist_tab(ptr_tab)){
        //perror("ksp n'existe pas\n");
        return -1 ;
    }else{

        char path[256];
        
        strcpy(path, path_tab);
        strcat(path, curr );
        strcat(path, "/" );
        strcat(path, ptr_tab->nom );
        strcat(path, ".json" );

        // suppression du fichier json = table :
        unlink(path);
        return 1 ;
    }



} 
