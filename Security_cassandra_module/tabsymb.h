#ifndef TABSYMB_H
#define TABSYMB_H
#include "cassandra.h"	

#include "error_cassandra.h"




typedef struct roleValue {
	char* name_role ;
	char* password_value ;
	char* login_value; 
	char* superuser_value ;
	char* if_ex_value;
	int count_value;
	int typerole;
} roleValue;


typedef struct auth {
	char* login_name ;
	char* password ;
} auth;


void creer_roleValue(char* _name_role,char* _password_value,char* _login_value,char* _superuser_value,char* _if_ex_value,int _count_value,int _typerole) ; 
void afficher_roleValue() ; 

void afficher_roleChange();

void creer_droprole(char* _name_role,char* _if_ex_value);
void afficher_roleDrop();

void write_role();
boolean check_if_role_exist(const char * role_name);
 
void alter_role(const char * name_role,const char *password_value,const char *login_value,const char *superuser_value);


void list_roles();
void list_specific(const char * role_name);

void creer_auth(char* _login_name,char* _password); 
void write_current_login(char* _login_name);
boolean check_if_file_exist();
boolean check_right();
void afficher_auth() ; 
//boolean write_superuser();
#endif
