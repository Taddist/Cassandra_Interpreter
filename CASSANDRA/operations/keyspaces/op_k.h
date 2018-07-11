#ifndef OP_K
#define OP_K


#include "../../cassandra.h"	// notre interface d'analyse (just pour les booleans :p )

#include "../../error.h"		// pour reconnaitre les types d'erreurs de keyspace


// pour le moment le keyspace est décrit uniquement par son nom :/ 
typedef struct ksp_type {
	char* nom ;
} ksp_type;



// test de d'existance d'un keyspace :
boolean exist_ksp(ksp_type* ptr_ksp) ;

// creation d'un KSP (KSP=keyspace :) ) -1 au cas d'error
int create_ksp(ksp_type* ptr_ksp) ;

// drop/suppression d'un keyspace -1 au cas d'error :
int remove_ksp(ksp_type* ptr_ksp) ;

// alter

//drop


// gestion de keyspace courant = getter and setters :
int set_current_ksp(ksp_type* ptr_ksp) ;
void get_current_ksp(char* curr) ;

//interaction avec le fichier current.ksp :
void load_current();
void save_current();
void free_current();



#endif
