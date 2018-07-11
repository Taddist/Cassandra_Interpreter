#ifndef AST_H
#define AST_H

#include "cassandra.h" // boolean
#include "operations/tables/op_t.h" // pour type_var

typedef enum {NB=0, BOOLEAN=2, OP=3} Type_Exp ; 


typedef enum {plus=0, moins=1, mult=2, _div=3} Type_Op; // div est un mot réservé

struct Exp ; /* pré déclaration de la structure de stockage d'une expression */

typedef struct Exp* AST;

typedef union {
  double value ;               // pour stocker les feuilles scalaires : nombre, true et false
  struct {
    Type_Op top;
    AST expression_gauche ;
    AST expression_droite ;
  } op;
}ExpValueTypeNode;


typedef struct Exp {
  Type_Exp typeexp ; // NB ou  OP ou BOOLEAN
  type_var    typename; // Int ou Bool ou Double et null si  op
  ExpValueTypeNode noeud ;
}expvalueType;


// précondition : a<> NULL and est_feuille(a) == false
AST arbre_gauche(AST a);
// précondition : a<> NULL and est_feuille(a) == false
AST arbre_droit(AST a);
// précondition : a<> NULL and est_feuille(a) == false
Type_Op top(AST a);
// précondition : a<> NULL
type_var  type(AST a);
// précondition : a<> NULL
boolean est_feuille(AST a);

AST creer_feuille_booleen(boolean b);
AST creer_feuille_nombre(double n, type_var type);
AST creer_noeud_operation(char op, AST arbre_g, AST arbre_d, type_var type);
// affichage par parcours infixé de l'arbre abstrait
// précondition : ast<> NULL
void afficher_infixe_arbre (AST ast);
// affichage par parcours postfixé de l'arbre abstrait
// précondition : ast<> NULL
void afficher_postfixe_arbre (AST ast);

//pour vider l'AST ;
void free_ast(AST ast) ;
// evaluer l'arbre :
double evaluer(AST ast) ;

#endif