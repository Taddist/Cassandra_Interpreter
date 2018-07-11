#include <stdio.h>	
#include <string.h>	
#include <stdlib.h>	
#include "tabsymb.h"



#include <jansson.h>

#include "jansson.h"
#include "hashtable.h"
#include "jansson_private.h"
#include "utf.h"

#include "error_cassandra.h"	

#define PATH_FORMAT   "%s.json"
#define PATH_SIZE     256





static varvalueType* file_idf ;	
static int taille_file_idf ;	
static selection* file_selection ;	
static int taille_file_selection ;
static whereClause* file_whereClause ;	
static int taille_file_whereClause ;

static varvalueType tempo;
static selection tmp ;
static whereClause tm ;
static smError tmp_sem ;
static int nbr_query_affected=0;


//PASTE ERROR_CASSANDRA.C

static smError* file_err ;
static int taille_file_err ;	



void creer_file_erreur(){
	smError* file_err=NULL ;	
	int taille_file_err=0 ;	
}

void creer_sm_erreur(SemanticErrorType _type_err, char* _nom_source){

	tmp_sem.nom_source = malloc(strlen(_nom_source) + 1) ;	strcpy(tmp_sem.nom_source, _nom_source);
	tmp_sem.type_err = _type_err ;
	
	taille_file_err++ ;
	smError* tempo = (smError*)realloc(file_err , taille_file_err*sizeof(smError)) ;
	if(tempo == NULL){
		perror("realloc creer_sm_erreur\n") ;
	}
	file_err = tempo ;
	file_err[taille_file_err-1] = tmp_sem ;
}




int nombre_sm_erreurs(){
	return taille_file_err ;
}
void afficher_sm_erreurs(){
	
	printf("%d erreurs semantiques :\n",taille_file_err );
	for(int i=0 ; i<taille_file_err ; i++){
		char *tp_sem ;
		switch (file_err[i].type_err) {
			case keyspace_not_found :
				tp_sem = malloc(strlen("keyspace not found") + 1) ;	strcpy(tp_sem, "keyspace not found");
				break;
			case table_not_found :
				tp_sem = malloc(strlen("table not found") + 1) ;	strcpy(tp_sem, "table not found");
				break;
			case column_not_found :
				tp_sem = malloc(strlen("column not found") + 1) ;	strcpy(tp_sem, "column not found");
				break;
			case incompatible_affectation :
				tp_sem = malloc(strlen("incompatible affectation") + 1) ;	strcpy(tp_sem, "incompatible affectation");
				break;
			case no_keyspace : 
				tp_sem = malloc(strlen("No keyspace selected ") + 1) ;	strcpy(tp_sem, "No keyspace selected");
				break;
			case incomp_aff : 
				tp_sem = malloc(strlen("Imcompatible affecation ") + 1) ;	strcpy(tp_sem, "Imcompatible affecation ");
				break;
			case keyspace_use_table_not_found : 
				tp_sem = malloc(strlen("TABLE NOT FOUND IN CURRENT KEYSPACE ") + 1) ;	strcpy(tp_sem, "TABLE NOT FOUND IN CURRENT KEYSPACE ");
				break;

		} // show keyspace selected 
		printf("\t %s : %s \n",file_err[i].nom_source,tp_sem);
	}

	// do delete
	if(taille_file_err==0 && taille_file_selection==0){
		char path[PATH_SIZE];
		if(strcmp(tempo.keyspace_name,"null")==0){
			const char * current_keyspace_char;
			json_t *root,*root_tmp,*table_list,*current_keyspace,*table,*desc_table,*columns_t,*column_name_w,*type;


		    	json_error_t error;
			root_tmp = json_load_file("./keyspaces/current_keyspace.json", 0, &error);
			current_keyspace=json_array_get(root_tmp,0);
			current_keyspace_char=json_string_value(current_keyspace);
			snprintf(path, PATH_SIZE, "./keyspaces/%s.json",current_keyspace_char);	
		}else{
		    	snprintf(path, PATH_SIZE, "./keyspaces/%s.json",tempo.keyspace_name);

		}
		json_t *root,*table_list,*table,*data;
		json_error_t error;
		root = json_load_file(path, 0, &error);
		table_list=json_array_get(root,1);
		table=json_object_get(table_list,tempo.table_name);
		data=json_object_get(table,"data");
		const char *key_row;
		json_t *value_row;
		json_object_foreach(data, key_row, value_row) {
			boolean delete = true;	
				// haha
				int j;
				for(j=0;j<taille_file_whereClause;j++){
					json_t *column_value=json_object_get(value_row,file_whereClause[j].column_name_w);
					const char * column_value_string = json_string_value(column_value);
					if(strcmp(column_value_string,file_whereClause[j].column_value)!=0){delete=false;}			
				}
				if(delete){json_object_del(data, key_row);nbr_query_affected++;}
				
				
		}
		FILE *fp = fopen(path, "w");
		json_dumpf(root,fp,JSON_INDENT(4));
		printf("QUERY OK ! , %i row affected \n",nbr_query_affected);

		

		
		
	}

	if(taille_file_err==0 && taille_file_selection!=0){

	
		char path[PATH_SIZE];
		if(strcmp(tempo.keyspace_name,"null")==0){
			const char * current_keyspace_char;
			json_t *root,*root_tmp,*table_list,*current_keyspace,*table,*desc_table,*columns_t,*column_name_w,*type;


		    	json_error_t error;
			root_tmp = json_load_file("./keyspaces/current_keyspace.json", 0, &error);
			current_keyspace=json_array_get(root_tmp,0);
			current_keyspace_char=json_string_value(current_keyspace);
			snprintf(path, PATH_SIZE, "./keyspaces/%s.json",current_keyspace_char);	
		}else{
		    	snprintf(path, PATH_SIZE, "./keyspaces/%s.json",tempo.keyspace_name);

		}
		json_t *root,*table_list,*table,*data;
		json_error_t error;
		root = json_load_file(path, 0, &error);
		table_list=json_array_get(root,1);
		table=json_object_get(table_list,tempo.table_name);
		data=json_object_get(table,"data");
		const char *key_row;
		json_t *value_row;
		json_object_foreach(data, key_row, value_row) {
			boolean update = true;	
				// haha
				int j;
				for(j=0;j<taille_file_whereClause;j++){
					json_t *column_value=json_object_get(value_row,file_whereClause[j].column_name_w);
					const char * column_value_string = json_string_value(column_value);
					if(strcmp(column_value_string,file_whereClause[j].column_value)!=0){update=false;}			
				}
				if(update){
					
	
					for(int k=0 ; k<taille_file_selection ; k++){
						json_t *text=json_object_get(value_row,file_selection[k].column_name);

						json_string_set(text, "null");	
					}
					nbr_query_affected++;
				}
				
				
		}
		FILE *fp = fopen(path, "w");
		json_dumpf(root,fp,JSON_INDENT(4));
		printf("QUERY OK ! , %i row affected \n",nbr_query_affected);		





	
	
	
	}
	
}

//FIN PASTE ERROR_CASSANDRA.C

void creer_file_idf(){
	varvalueType* file_idf=NULL ;	
	int taille_file_idf=0 ;	
}


void creer_file_selection(){
	selection* file_selection=NULL ;	
	int taille_file_selection=0 ;	
}
void creer_file_whereClause(){
	whereClause* file_whereClause=NULL ;	
	int taille_file_whereClause=0 ;	
}

void do_delete(const char * key,const char * keyspace,const char * table_name){

	char path[PATH_SIZE];
    	snprintf(path, PATH_SIZE, "./keyspaces/%s.json", keyspace);
	int i;


        json_t *root,*table_list,*table,*data;



    	json_error_t error;
	root = json_load_file(path, 0, &error);
	table_list=json_array_get(root,1);
	table=json_object_get(table_list,table_name);
	data=json_object_get(table,"data");
	json_object_del(data,key);

	FILE *fp = fopen("./keyspaces/file3.json", "w");
	json_dumpf(root,fp,JSON_SORT_KEYS);



    

   	 json_decref(root);


}
boolean check_keyspace_in_use( const char * keyspace_name){
	const char * current_keyspace_char;

	json_t *root,*current_keyspace,*table;
    	json_error_t error;
	root = json_load_file("./keyspaces/current_keyspace.json", 0, &error);
	current_keyspace=json_array_get(root,0);
	current_keyspace_char=json_string_value(current_keyspace);


	
	if(strcmp(current_keyspace_char,keyspace_name)!=0){

		return false;
	}

	return true;
}

boolean check_if_table_exist(char* keyspace_name,const char * table_name){


	char path[PATH_SIZE];
    	snprintf(path, PATH_SIZE, "./keyspaces/%s.json",keyspace_name);
	int i;


	json_t *root,*table_list,*table;
    	json_error_t error;
	root = json_load_file(path, 0, &error);
	table_list=json_array_get(root,1);
	table=json_object_get(table_list,table_name);
		
	if(json_is_object(table)){
		return true;
	}

	return false;	
	
}



boolean check_if_keyspace_exist(const char * keyspace_name){


	char path[PATH_SIZE];
    	snprintf(path, PATH_SIZE, "./keyspaces/%s.json", keyspace_name);
	FILE *file;
	    if ((file = fopen(path, "r"))){
		fclose(file);
		return true;
	    }
	return false;
}
boolean check_if_column_exists(char* _column_name_w,char* table_name){


	
	const char * current_keyspace_char;
	json_t *root,*root_tmp,*table_list,*current_keyspace,*table,*desc_table,*columns_t,*column_name_w,*type;


    	json_error_t error;
	root_tmp = json_load_file("./keyspaces/current_keyspace.json", 0, &error);
	current_keyspace=json_array_get(root_tmp,0);
	current_keyspace_char=json_string_value(current_keyspace);


	char path[PATH_SIZE];
    	snprintf(path, PATH_SIZE, "./keyspaces/%s.json",current_keyspace_char);







	root = json_load_file(path, 0, &error);
	table_list=json_array_get(root,1);//get all tables :D VIIRTUAL WAAAS HERE
	table=json_object_get(table_list,table_name);
	desc_table=json_object_get(table,"desc_table");
	columns_t=json_object_get(desc_table,"columns_t");
	column_name_w=json_object_get(columns_t,_column_name_w);
	if(json_is_object(column_name_w)){
		return true;
	}

	return false;	
	
}

void creer_varvalueType(char* _keyspace_name,char* _table_name,char* _timestamp_value,char* _if_exists,int _ligne_decl){
	
	
	//NEW

	if(strcmp(_keyspace_name,"null")!=0){
		if(check_keyspace_in_use(_keyspace_name)){
				
			if(check_if_keyspace_exist(_keyspace_name)){
				if(check_if_table_exist(_keyspace_name,_table_name)){
					for(int i=0 ; i<taille_file_selection ; i++){
						if(!check_if_column_exists(file_selection[i].column_name,_table_name)){
							creer_sm_erreur(column_not_found,file_selection[i].column_name );					
						}	
					}

					tempo.keyspace_name = malloc(strlen(_keyspace_name ) + 1) ;	
					strcpy(tempo.keyspace_name , _keyspace_name );
					tempo.table_name = malloc(strlen(_table_name) + 1) ;	
					strcpy(tempo.table_name, _table_name);
					tempo.timestamp_value = malloc(strlen(_timestamp_value) + 1) ;	
					strcpy(tempo.timestamp_value, _timestamp_value);
					tempo.if_exists = malloc(strlen(_if_exists) + 1) ;	
					strcpy(tempo.if_exists, _if_exists);
					tempo.ligne_decl = _ligne_decl ;	
					
				}else{creer_sm_erreur(table_not_found,_table_name );}
			}else{
				creer_sm_erreur(keyspace_not_found,_keyspace_name );
				//should return and not complete
			}
		}else{  creer_sm_erreur(no_keyspace,_keyspace_name);}
	}else{
			const char * current_keyspace_char;

			json_t *root,*current_keyspace,*table;
		    	json_error_t error;
			root = json_load_file("./keyspaces/current_keyspace.json", 0, &error);
			current_keyspace=json_array_get(root,0);
			current_keyspace_char=json_string_value(current_keyspace);
			if(!check_if_table_exist((char*)current_keyspace_char,_table_name)){	

			creer_sm_erreur(keyspace_use_table_not_found,(char*)current_keyspace_char);
			
		}
		
						tempo.keyspace_name = malloc(strlen(_keyspace_name ) + 1) ;
						strcpy(tempo.keyspace_name , current_keyspace_char );
						tempo.table_name = malloc(strlen(_table_name) + 1) ;	
						strcpy(tempo.table_name, _table_name);
						tempo.timestamp_value = malloc(strlen(_timestamp_value) + 1) ;	
						strcpy(tempo.timestamp_value, _timestamp_value);
						tempo.if_exists = malloc(strlen(_if_exists) + 1) ;	
						strcpy(tempo.if_exists, _if_exists);
						tempo.ligne_decl = _ligne_decl ;
	}
	

			
}





void creer_selection(char* _column_name,char* _term,int _ligne_decl){

	tmp.column_name = malloc(strlen(_column_name ) + 1) ;	
	strcpy(tmp.column_name , _column_name );
	tmp.term = malloc(strlen(_term ) + 1) ;	
	strcpy(tmp.term , _term );
	tmp.ligne_decl = _ligne_decl ;		
	taille_file_selection++ ;
	selection* t = (selection*)realloc(file_selection , taille_file_selection*sizeof(selection)) ;
	if(t == NULL){
		perror("realloc creer_selection\n") ;
	}
	file_selection = t ;
	file_selection[taille_file_selection-1] = tmp ;
}


boolean check_type_column(char* _column_name_w,const char * _type_value,char* table_name){


	
	const char * current_keyspace_char;
	json_t *root,*root_tmp,*table_list,*current_keyspace,*table,*desc_table,*columns_t,*column_name_w,*type;


    	json_error_t error;
	root_tmp = json_load_file("./keyspaces/current_keyspace.json", 0, &error);
	current_keyspace=json_array_get(root_tmp,0);
	current_keyspace_char=json_string_value(current_keyspace);


	char path[PATH_SIZE];
    	snprintf(path, PATH_SIZE, "./keyspaces/%s.json",current_keyspace_char);







	root = json_load_file(path, 0, &error);
	table_list=json_array_get(root,1);//get all tables :D VIIRTUAL WAAAS HERE
	table=json_object_get(table_list,table_name);
	desc_table=json_object_get(table,"desc_table");
	columns_t=json_object_get(desc_table,"columns_t");
	column_name_w=json_object_get(columns_t,_column_name_w);
	type=json_object_get(column_name_w,"type_c");
	const char *type_text;
	type_text=json_string_value(type); 
	
	if(strcmp(type_text,_type_value)==0){
		return true;
	}

	return false;	
	
}



void creer_whereClause(char* _column_name_w,type_op _operator ,char* _column_value,char* _type_value,int _ligne_decl,char* table_name){

	if(check_if_column_exists(_column_name_w,table_name)){
		if(check_type_column(_column_name_w,_type_value,table_name)){
					tm.column_name_w = malloc(strlen(_column_name_w ) + 1) ;	
					strcpy(tm.column_name_w , _column_name_w );
					tm.table_name = malloc(strlen(table_name ) + 1) ;	
					strcpy(tm.table_name , table_name );
					tm.operator=_operator ;
					tm.column_value = malloc(strlen(_column_value ) + 1) ;	
					strcpy(tm.column_value , _column_value );
					tm.type_value = malloc(strlen(_type_value ) + 1) ;	
					strcpy(tm.type_value , _type_value );
					tm.ligne_decl = _ligne_decl ;
					taille_file_whereClause++ ;
					whereClause* wc = (whereClause*)realloc(file_whereClause , taille_file_whereClause*sizeof(whereClause)) ;
					if(wc == NULL){
						perror("realloc creer_selection\n") ;
					}
					file_whereClause = wc ;
					file_whereClause[taille_file_whereClause-1] = tm ;	
					
			
		}else{creer_sm_erreur(incomp_aff,_column_name_w );}
	}else{creer_sm_erreur(column_not_found,_column_name_w );}

	
	
		
}

// les getters :
int nombre_idf(){
	return taille_file_idf ;
}

int nombre_selection(){
	return taille_file_selection;
}
void afficher_selection(){
	printf("%d taille de la pile Selection  :\n",taille_file_selection );
	for(int i=0 ; i<taille_file_selection ; i++){

	printf("\tcolumn name  :%s\n\t term :%s\n\t ligne de declaration : %d\n",file_selection[i].column_name,file_selection[i].term,file_selection[i].ligne_decl);
	
	}
}
void afficher_idf(){
	printf("Name keyspace et Table \n");
	printf("\tkeyspace name  :%s\n\tTable name : %s\n\t Timestamp: %s\n\t if exists: %s\n\t  ligne de declaration : %d\n",tempo.keyspace_name,tempo.table_name,tempo.timestamp_value,tempo.if_exists,tempo.ligne_decl);
	// code to delete the object 

}

int nombre_whereClause(){
	return taille_file_whereClause;
}
void afficher_whereClause(){
	printf("%d taille de la pile Where   :\n",taille_file_whereClause );
	for(int i=0 ; i<taille_file_whereClause ; i++){
		if(strcmp(file_whereClause[i].type_value,"string")== 0)
	{
		memmove(file_whereClause[i].column_value, file_whereClause[i].column_value+1, strlen(file_whereClause[i].column_value));
		file_whereClause[i].column_value[strlen(file_whereClause[i].column_value)-1] = '\0';
	}
		char *tp ;
		switch (file_whereClause[i].operator) {
			case EGAL_ :
				tp = malloc(strlen("egal") + 1) ;	strcpy(tp, "egal");
				break;
			case INF_ :
				tp = malloc(strlen("inferieur") + 1) ;	strcpy(tp, "inferieur");
				break;
			case SUP_ :
				tp = malloc(strlen("superieur") + 1) ;	strcpy(tp, "superieur");
				break;
			case INFE_ :
				tp = malloc(strlen("inferieur ou egale") + 1) ;	strcpy(tp, "inferieur ou egale");
				break;
			case SUPE_ :
				tp = malloc(strlen("superieur ou egale") + 1) ;	strcpy(tp, "superieur ou egale");
				break;	
			case CONTAINS_ :
				tp = malloc(strlen("contains") + 1) ;	strcpy(tp, "contains");
				break;
			case CONTAINSKEY_ :
				tp = malloc(strlen("contains key ") + 1) ;	strcpy(tp, "contains key");
				break;		
		}
		printf("\ttable name  :%s\n\tcolumn name  :%s\n\t OPERATOR  :%s\n\tVALUE:%s\n\t Type value: %s\n",file_whereClause[i].table_name,file_whereClause[i].column_name_w,(char*)tp,file_whereClause[i].column_value,file_whereClause[i].type_value);
	//void do_delete(const char * key,const char * table_name) //

    		
		//do_delete(file_whereClause[0].column_value,tempo.table_name);		 
		//printf("%s - %s",tempo.table_name,file_whereClause[0].column_value);

			
		
	}
}
static int cmp_idf(const void* a, const void* b){
	varvalueType* s1 = (varvalueType*)a;
    	varvalueType* s2 = (varvalueType*)b;
 
    return strcmp(s1->keyspace_name, s2->keyspace_name);
}

varvalueType* exist(char* idf){
	varvalueType* pvvt = bsearch(&idf, file_idf, taille_file_idf, sizeof *file_idf, cmp_idf);

}
