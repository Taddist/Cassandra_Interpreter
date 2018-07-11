#include <stdio.h>	
#include <string.h>	
#include <stdlib.h>	
#include "tabsymb.h"	
#include <dirent.h>
#include <jansson.h>

#include "jansson.h"
#include "hashtable.h"
#include "jansson_private.h"
#include "utf.h"

#define PATH_FORMAT   "%s.json"
#define PATH_SIZE     256

static roleValue RV;	
static auth A;

void creer_roleValue(char* _name_role,char* _password_value,char* _login_value,char* _superuser_value,char* _if_ex_value,int _count_value,int _typerole){
	RV.name_role = malloc(strlen(_name_role) + 1) ;
	strcpy(RV.name_role, _name_role);

	RV.password_value = malloc(strlen(_password_value) + 1) ;
	strcpy(RV.password_value, _password_value);

	RV.login_value = malloc(strlen(_login_value) + 1) ;
	strcpy(RV.login_value, _login_value);

	RV.superuser_value = malloc(strlen(_superuser_value) + 1) ;
	strcpy(RV.superuser_value, _superuser_value);

	RV.if_ex_value = malloc(strlen(_if_ex_value) + 1) ;
	strcpy(RV.if_ex_value, _if_ex_value);

	RV.count_value= _count_value;
	RV.typerole=_typerole;
	// case CREATE 
	if(RV.typerole==0)
	{
		if(check_if_role_exist(RV.name_role))
		{
			if(strcmp(RV.if_ex_value,"false")==0)
			{
				creer_sm_erreur(role_exists,RV.name_role );
			}
		}
		else 
		{
			if(check_right())
			{

				write_role(RV.name_role);
				//afficher_roleValue();
			}
			else 
			{
				creer_sm_erreur(access_denied,RV.name_role);
			}

			
			
			
		}
	}
	//case ALTER
	else if (RV.typerole==1)
	{
		if(check_if_role_exist(RV.name_role))
		{
			alter_role(RV.name_role,RV.password_value,RV.login_value,RV.superuser_value);
		}
		else 
		{
			creer_sm_erreur(role_not_exists,RV.name_role);
		}
	}
	  

}
/*

json_t *root_tmp  ;
	json_error_t error;
	size_t current_keyspace;
	root_tmp = json_load_file("./file5.json", 0, &error);

	current_keyspace=json_array_size(root_tmp)+1;

	printf("%lu\n",current_keyspace );

	*/
void write_role(const char * role_name)
{
	char path[PATH_SIZE];
    snprintf(path, PATH_SIZE, "./roles/%s.json", role_name);

	memmove(RV.password_value,RV.password_value+1, strlen(RV.password_value));
	RV.password_value[strlen(RV.password_value)-1] = '\0';
	json_error_t error;
    json_t *root = json_array();

    
	json_t *password = json_string(RV.password_value);
	json_t *login =json_string(RV.login_value);
	json_t *superuser = json_string(RV.superuser_value);

    json_t *role = json_object();
	json_t *roles = json_object();


	json_object_set(role,"password", password);
	json_object_set(role,"login", login);
	json_object_set(role,"superuser", superuser);

	json_object_set_new(roles, RV.name_role, role);
	json_array_insert(root,0,roles);

    json_t *empty = json_array();

    json_t *permissions = json_object();
	json_object_set_new(permissions,"CREATE", empty);
	json_object_set_new(permissions,"ALTER", empty);
	json_object_set_new(permissions,"DROP", empty);
	json_object_set_new(permissions,"SELECT", empty);
	json_object_set_new(permissions,"MODIFY", empty);
	json_object_set_new(permissions,"AUTHORIZE", empty);
	json_object_set_new(permissions,"DESCRIBE", empty);
	json_object_set_new(permissions,"EXECUTE", empty);
	json_array_insert(root,1,permissions);

	
    
	FILE *fp = fopen(path, "w");
	json_dumpf(root,fp,JSON_INDENT(1));

    json_decref(root);
}
/*
boolean  write_superuser(const char * role_name){

	char path[PATH_SIZE];
    snprintf(path, PATH_SIZE, "./roles/superuser_current.json");

	json_t *root,*list ,*log_pass;
    json_error_t error;
    const char * val ;

	root = json_load_file(path, 0, &error);
	list=json_array_get(root,0);
	log_pass=json_object_get(list,role_name);
	val=json_string_value(log_pass);

		
	if((strcmp(val,"125"))==0){
		return true ; 
	}

	else return false ;

	
}

*/
boolean check_if_role_exist(const char * role_name){


	char path[PATH_SIZE];
    snprintf(path, PATH_SIZE, "./roles/%s.json", role_name);
	FILE *file;
	    if ((file = fopen(path, "r"))){
		fclose(file);
		return true;
	    }
		else return false;
}

void creer_droprole(char* _name_role,char* _if_ex_value)
{
	RV.name_role = malloc(strlen(_name_role) + 1) ;
	strcpy(RV.name_role, _name_role);

	RV.if_ex_value = malloc(strlen(_if_ex_value) + 1) ;
	strcpy(RV.if_ex_value, _if_ex_value);

	if(check_if_role_exist(RV.name_role) )
	{
		if(check_right())
			{

				char path[PATH_SIZE];
		    	snprintf(path, PATH_SIZE, "./roles/%s.json", _name_role);
				remove(path);
			}
			else 
			{
				creer_sm_erreur(access_denied,RV.name_role);
			}
		
	}
	else 
	{	
		if (strcmp(RV.if_ex_value,"false")==0)
		{
				creer_sm_erreur(role_not_exists,RV.name_role );
		}

	}  

}

void alter_role(const char * name_role,const char *password_value,const char *login_value,const char *superuser_value)
{
	json_error_t error;
	json_t *root,*role ,*property ,*role_char;
	char path[PATH_SIZE];
	snprintf(path, PATH_SIZE, "./roles/%s.json", name_role);
	root = json_load_file(path, 0, &error);
	role=json_array_get(root,0);
	role_char=json_object_get(role,name_role);

	memmove(RV.password_value,RV.password_value+1, strlen(RV.password_value));
	RV.password_value[strlen(RV.password_value)-1] = '\0';



	if(RV.count_value==1)
	{
		if(check_right())
			{

				json_t * text =json_object_get(role_char,"password");
				json_string_set(text, password_value);
			}
			else 
			{
				creer_sm_erreur(access_denied,RV.name_role);
			}

		
	}
	else if(RV.count_value==2)
	{
		if(check_right())
			{

				json_t * text =json_object_get(role_char,"password");
				json_string_set(text, password_value);

				json_t * text1 =json_object_get(role_char,"login");
				json_string_set(text1, login_value);
			}
			else 
			{
				creer_sm_erreur(access_denied,RV.name_role);
			}
		
	}
	else if(RV.count_value==3)
	{
		if(check_right())
			{

				json_t * text =json_object_get(role_char,"password");
				json_string_set(text, password_value);

				json_t * text1 =json_object_get(role_char,"login");
				json_string_set(text1, login_value);

				json_t * text2 =json_object_get(role_char,"superuser");
				json_string_set(text2, superuser_value);
			}
			else 
			{
				creer_sm_erreur(access_denied,RV.name_role);
			}
		
	}
	FILE *fp = fopen(path, "w");
		json_dumpf(root,fp,JSON_INDENT(4));
   
}

void afficher_roleValue(){
	printf("Name Role :%s\n\t\tIf Exists: %s\n\t\tPassword: %s\n\t\tLogIn : %s\n\t\tSuperUser: %s\n\t\tNumber of parameters:%d\n",RV.name_role,RV.if_ex_value,RV.password_value,RV.login_value,RV.superuser_value,RV.count_value);	
}
 void afficher_roleChange(){
 	printf("Name Role :%s\n\t\tPassword: %s\n\t\tLogIn : %s\n\t\tSuperUser: %s\n\t\tNumber of parameters:%d\n",RV.name_role,RV.password_value,RV.login_value,RV.superuser_value,RV.count_value);
 }
 void afficher_roleDrop(){
 	printf("Name Role :%s\n\t\tIf Exists: %s\n",RV.name_role,RV.if_ex_value);
 }

void list_roles()
{
	DIR *d;
	const char *y , *y1;
    struct dirent *dir;
	d = opendir("./roles");
	json_t *root,*role ,*property ,*role_char , *x , *text,*x1;
	json_error_t error;
	int i=0 ; 
	char path[PATH_SIZE];
	printf("Role\t|Login\t|Superuser\t\n");
	printf("--------+-------+---------\n");
			if (d)
			{
				while ((dir = readdir(d)) != NULL)
            	{
					if(strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0 && strcmp(dir->d_name,"superuser_current.json")!=0)
					{
						char *token=strtok(dir->d_name,".");
						snprintf(path, PATH_SIZE, "./roles/%s.json",token);
						root = json_load_file(path, 0, &error);
						role=json_array_get(root,0);
						role_char=json_object_get(role,token);
						x=json_object_get(role_char,"login");
						y =json_string_value(x);
						x1=json_object_get(role_char,"superuser");
						y1 =json_string_value(x1);
						printf("%s\t|", token);
						printf("%s\t|",y);
		 				printf("%s\t\n",y1);
		 				i+=1;
					}	  
        		}
        		closedir(d);    
    		}
    printf("(%d) Roles Listed \n",i );
}

void list_specific(const char * role_name){

			if(check_if_role_exist(role_name))
			{
				const char *y , *y1;
				json_t *root,*role ,*property ,*role_char , *x , *text,*x1;
				json_error_t error;
				char path[PATH_SIZE];
				printf("Role\t|Login\t|Superuser\t\n");
				printf("--------+-------+---------\n");
				snprintf(path, PATH_SIZE, "./roles/%s.json",role_name);
				root = json_load_file(path, 0, &error);
				role=json_array_get(root,0);
				role_char=json_object_get(role,role_name);
				x=json_object_get(role_char,"login");
				y =json_string_value(x);
				x1=json_object_get(role_char,"superuser");
				y1 =json_string_value(x1);
				printf("%s\t|", role_name);
				printf("%s\t|",y);
		 		printf("%s\t\n",y1);
			}
			else 
			{
				creer_sm_erreur(role_not_exists,RV.name_role);
				afficher_sm_erreurs();
			}
}

void creer_auth(char* _login_name,char* _password)
{
	A.login_name = malloc(strlen(_login_name) + 1) ;
	strcpy(A.login_name, _login_name);

	A.password = malloc(strlen(_password) + 1) ;
	strcpy(A.password, _password);

	if(strcmp(A.login_name,"cassandra")==0)
	{
		if(strcmp(A.password,"cassandra")==0)
		{
			write_current_login(A.login_name);
		}
	}
	else 
	{
		if(check_if_role_exist(A.login_name))
		{

				const char *y , *y1 , *y2;
				json_t *root,*role ,*role_char , *x ,*x1,*x2;
				json_error_t error;

				char path[PATH_SIZE];
				snprintf(path, PATH_SIZE, "./roles/%s.json",_login_name);

				root = json_load_file(path, 0, &error);
				role=json_array_get(root,0);
				role_char=json_object_get(role,_login_name);
				
				x2=json_object_get(role_char,"password");
				y2 =json_string_value(x2);
				

				x=json_object_get(role_char,"login");
				y =json_string_value(x);
				

				if(strcmp(A.password ,y2)==0)
				{
						if(strcmp( y,"true")==0)
						{
							write_current_login(A.login_name);
						}
						else 
						{
							creer_sm_erreur(access_denied,A.login_name);
						}
				}
				else 
				{
					creer_sm_erreur(incorrect_password,A.password);
				}
				
		}
		else creer_sm_erreur(role_not_exists,A.login_name);
	}
}

void write_current_login(char* _login_name)
{
		json_t *root,*current_Login_name;
    	json_error_t error;
    	char path[PATH_SIZE];
    	snprintf(path, PATH_SIZE, "./roles/superuser_current.json");
    	root = json_array();
		current_Login_name=json_string(A.login_name);
		json_array_insert(root,0,current_Login_name);
		FILE *fp = fopen(path, "w");
		json_dumpf(root,fp,JSON_INDENT(1));
    	json_decref(root);
}
boolean check_if_file_exist(){


	char path[PATH_SIZE];
    snprintf(path, PATH_SIZE, "./roles/superuser_current.json");
	FILE *file;
	    if ((file = fopen(path, "r"))){
		fclose(file);
		return true;
	    }
	return false;
}


boolean check_right()
{
		const char  *name;
		json_t *root,*current_Login_name ;
    	json_error_t error;
    	char path[PATH_SIZE];
    	const char *y ;
		json_t *root_role,*role ,*role_char , *x;
		char path_role[PATH_SIZE];

    	snprintf(path, PATH_SIZE, "./roles/superuser_current.json");
    	if(check_if_file_exist())
    	{
    		root = json_load_file(path, 0, &error);
    		current_Login_name=json_array_get(root,0);
			name=json_string_value(current_Login_name);

			if(strcmp(name,"cassandra")!=0)
			{
				snprintf(path_role, PATH_SIZE, "./roles/%s.json",name);
				root_role = json_load_file(path_role, 0, &error);
				role=json_array_get(root_role,0);
				role_char=json_object_get(role,name);
				x=json_object_get(role_char,"superuser");
				y =json_string_value(x);
				//printf("%s\n", y);
				if(strcmp(y,"true")==0)
				{
					return true;
				}
			}
			else 
			{
				return true;
			}
			
    	}
    	else
    	{
    	 creer_sm_erreur(file_not_exists,"superuser_current.json");
    	}
		

return false;


}
