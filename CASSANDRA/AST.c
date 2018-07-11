#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AST.h"



// précondition: a<> NULL andest_feuille(a) == false
AST arbre_gauche(AST a){
	return a->noeud.op.expression_gauche;
}

// précondition: a<> NULL andest_feuille(a) == false
AST arbre_droit(AST a){
	return a->noeud.op.expression_droite;
}

// précondition: a<> NULL andest_feuille(a) == false
Type_Op top(AST a){
	return a->noeud.op.top;
}

// précondition: a<> NULL 
boolean est_feuille(AST a){
	return (a->typeexp != OP) ;
}

AST creer_feuille_booleen(boolean b){
	AST result= (AST) malloc(sizeof(struct Exp));
	result->typeexp=BOOLEAN;
	result->typename=BOOL_;
	result->noeud.value= b;
	return result;
}

AST creer_feuille_nombre(double n, type_var type){
	AST result= (AST) malloc(sizeof(struct Exp));
	result->typeexp=NB;
	result->typename=type;
	result->noeud.value= n;
	return result;
}


AST creer_noeud_operation(char op, AST arbre_g, AST arbre_d, type_var type){
	AST result= (AST) malloc(sizeof(struct Exp));
	result->typeexp=OP;
	result->typename=type;
	result->noeud.op.top = ((op=='+')?plus:((op=='-')?moins:((op=='/')?_div:mult)));
	result->noeud.op.expression_gauche= arbre_g;
	result->noeud.op.expression_droite= arbre_d;
	return result;
}

// affichage par parcours infixé de l'arbre abstrait
// précondition: a<> NULL 
void afficher_infixe_arbre(AST ast){
	// if (est_feuille(ast))
	switch(ast->typeexp) {
		case NB : printf(" %lf",ast->noeud.value); break;
		case BOOLEAN : printf(" %s",  (ast->noeud.value==0)?"false":"true" ); break;
		case OP :
			afficher_infixe_arbre(arbre_gauche(ast));
			switch(ast->noeud.op.top){
				case plus : printf(" + "); break;
				case moins : printf(" -"); break;
				case mult: printf(" * "); break;
				case _div: printf(" / "); break;
			}
			afficher_infixe_arbre(arbre_droit(ast));
			break;
	}
}


// affichage par parcours postfixéde l'arbre abstrait
// précondition: a<> NULL 
void afficher_postfixe_arbre(AST ast){
	// if (est_feuille(ast)){
	switch(ast->typeexp) {
		case NB : printf(" %lf",ast->noeud.value); break;
		case BOOLEAN : printf(" %s",  (ast->noeud.value==0)?"false":"true" ); break;
		case OP :
			afficher_postfixe_arbre(arbre_gauche(ast));
			afficher_postfixe_arbre(arbre_droit(ast));
			switch(ast->noeud.op.top){
				case plus : printf(" + "); break;
				case moins : printf(" -"); break;
				case mult: printf(" * "); break;
				case _div: printf(" / "); break;
			}
			break;
	}
}


void free_ast(AST ast){
	// if (est_feuille(ast)){
	switch(ast->typeexp) {
		case NB : free(ast); break;
		case BOOLEAN : free(ast); break;
		case OP :
			free_ast(arbre_gauche(ast));
			free_ast(arbre_droit(ast));
			break;
	}
}



// évalationpar parcours postfixéde l'arbre abstrait
// précondition: a<> NULL 
double evaluer(AST ast){
	double valg, vald;
	//if (est_feuille(ast)){
	switch(ast->typeexp) {
		case NB : return ast->noeud.value; break;
		case OP :
			valg= evaluer(arbre_gauche(ast));
			vald= evaluer(arbre_droit(ast));
			switch(ast->noeud.op.top){
				case plus : return valg+ vald; break;
				case moins : return valg-vald; break;
				case mult: return valg* vald; break;
				case _div: return (valg/vald); break;
			}
			break ;
	}
}
