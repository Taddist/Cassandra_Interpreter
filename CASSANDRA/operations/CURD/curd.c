    #include <stdio.h>  // pour le printf true/false
    #include <string.h> // pour les strcpy,strlen...
    #include <stdlib.h> // pour les malloc,realloc
    #include "curd.h"   // notre interface
    #include <unistd.h> // pour la fonction access
    #include <sys/stat.h> // pour le mkdir
    #include <dirent.h>
    #include <sys/types.h>
    #include <jansson.h>
    // emplacement de dossier contenant les keyspaces :
    static const char* path_tab = "data/" ;
    //list des colonnes de la table courante:
    static column_type* columns_file ;
    static int sa_taille ;
    void init_cols(){
        columns_file=NULL ;    //notre file
        sa_taille=0 ; // sa taille
    }
    int load_cols(table_type* ptr_tab){
        init_cols() ; //initialisation
        // attention elle ne fait pas le test d'existance de la table !!!!!!
        char path[256];
        char curr[256];
        get_current_ksp(curr) ;
        if (!strcmp("_" , curr))
        {
            return -11 ; // no ksp used
        }
        strcpy(path, path_tab);
        strcat(path, curr );
        strcat(path, "/" );
        strcat(path, ptr_tab->nom );
        strcat(path, ".json" );
        //printf("%s\n", path);
        json_error_t err ;
        json_t * root = json_object() ;
        json_t * desc = json_object() ;
        json_t * cols = json_object() ;
        json_t * col = json_object() ;
        json_t * type = json_object() ;
        json_t * prk = json_object() ;
        const char* key ;
        root = json_load_file(path , 0 , &err) ;
        // json_object_foreach(root , key , col ){
        //  printf(" ===== %s\n", key);
        // }
        desc = json_object_get(root , "desc") ;
        cols = json_object_get(desc , "columns_t") ;
        json_object_foreach(cols , key , col ){
            column_type temp_col ;
            // printf("nom === %s\n", key);
            temp_col.nom = malloc(sizeof(key)+1) ;  strcpy(temp_col.nom , key) ;
            
            type = json_object_get(col , "type_c") ;
            prk = json_object_get(col , "prk_c") ;  
            // printf("type === %s\n", json_string_value(type));
            // printf("type === %s\n\n\n", json_boolean_value(prk)? "true" : "false" );
            const char* x = json_string_value(type) ;
            temp_col.type = (!strcmp(x , "int"))? INT_ : (!strcmp(x , "double"))? DOUBLE_ : (!strcmp(x , "bool"))?BOOL_ : STRING_ ;
            temp_col.prk = json_boolean_value(prk) ;
            // :D congratulation !! temp_col created...let's insert it :
            sa_taille++ ;
            columns_file = realloc(columns_file , sa_taille*sizeof(column_type)) ;
            columns_file[sa_taille-1] = temp_col ;
        }
        json_decref(root);
        json_decref(desc);
        json_decref(cols);
        json_decref(col);
        json_decref(type);
        json_decref(prk);
        return 1 ;
    }






    int insert_line_ifnx(table_type* ptr_tab ,int nb_col, column_type *columns,int nb_val, value_type *values ){
        // chargement des colonnes de la table en mémoire:
        if (load_cols( ptr_tab ) == -11) return -11 ;
        if(!exist_tab( ptr_tab )){
            return -1 ; // table non éxistante
        }
        if(nb_col>nb_val){
            return -2 ; // need values
        }else if(nb_col<nb_val){
            return -3 ;  // overdose values
        }
        
        // les indices de clée primaire :
        int prk_col ;
        int prk_array ;

        // nb_col == nb_val
        for (int i=0 ; i<nb_col ; i++){
            for(int j=0 ; j<sa_taille ; j++){
                if(columns_file[j].prk) prk_array = j ;
                if(!strcmp(columns_file[j].nom,columns[i].nom)){
                    columns[i] = columns_file[j] ; // dès maintenant tous les champs sont remplis
                    break ;
                }else if(j==sa_taille-1){
                    return -4;   // colone introuvable ;
                }
            }
            if(columns[i].prk) prk_col = i ;
            if(columns[i].type!=values[i].type){
                return -5 ; // invalide type of value
            }
        }
        //                          ##################################
        //                         ######## interprétation ##########
        //                        ##################################
        char path[256];
        char curr[256];
        get_current_ksp(curr) ;
        strcpy(path, path_tab);
        strcat(path, curr );
        strcat(path, "/" );
        strcat(path, ptr_tab->nom );
        strcat(path, ".json" );
        //printf("%s\n", path);
        json_error_t err ;
        json_t * root = json_object() ;
        json_t * data = json_object() ;
        json_t * desc = json_object() ; // pour incrémenter le nbr de ligne 
        json_t * nbl = json_object() ;
        root = json_load_file(path , 0 , &err) ;
        data = json_object_get(root , "data") ;
        desc = json_object_get(root , "desc") ;
        json_t *array = json_array();


        json_t *arrays = json_array();
        const char* key ;
        boolean flag = false ; // prk n'éxiste pas déja
        json_object_foreach(data , key , arrays ){
            json_t * temp = json_array_get(arrays, prk_array) ;
            int x = json_typeof(temp) ;
            switch (x) {
                case JSON_STRING :
                if(!strcmp(values[prk_col].val , json_string_value(temp) )) flag = true ;
                break;
                case JSON_INTEGER :
                if(atoi(values[prk_col].val) == json_integer_value(temp)) flag = true ;
                break;
                case JSON_REAL :
                if(atof(values[prk_col].val) == json_real_value(temp)) flag = true ;
                break;
                
            }
        }
        if(flag) return -12 ; // DOUBLE_PRK_VALUE




        
        for(int k=0 ;k<sa_taille ; k++){
            int l = -1 ;
            for(int m=0 ; m<nb_col ; m++){
                if(!strcmp(columns_file[k].nom , columns[m].nom)){
                    l=m ;
                }
            }
            if(l==-1){
                json_array_append_new(array, json_null());
            }else{
                type_var x = values[l].type ; 
                json_array_append_new(array, (x==INT_)? json_integer(atoi(values[l].val)) : (x==DOUBLE_)? json_real(atof(values[l].val)) : (x==BOOL_)? json_boolean(atoi(values[l].val)) : json_string(values[l].val));
            }
        }  // ici notre array est bien remplie et pret à etre inseré dans la data
        nbl = json_object_get(desc , "nb_lines_t") ;
        int nb = json_integer_value(nbl);
        nb++ ;
        json_object_set(desc, "nb_lines_t", json_integer(nb));
        char s[256];
        sprintf(s, "%d", nb);
        json_object_set_new(data, s, array );
        json_object_set(root, "data" , data );
        json_object_set(root, "desc" , desc );
        FILE* fichier = fopen( path , "w+") ;
        
        json_dumpf(root , fichier , JSON_INDENT(4)) ;
        fclose(fichier) ;
        // json_decref(root);
        // json_decref(desc);
        // json_decref(data);
        // json_decref(nbl);
        //##############################################
        return 1 ;
    }


    int select_cols(table_type* ptr_tab ,int nb_col, column_type *columns,int nb_cond, condition_type *conditions ){
        // chargement des colonnes de la table en mémoire:
        if (load_cols( ptr_tab ) == -11) return -11 ;
        
        if(!exist_tab( ptr_tab )){
            return -1 ; // table non éxistante
        }
        
        
        // test d'existance des colonnes de select clause si != de star :
        // en meme temps on remplie l'array index par les positions de data correspondante a chaque colonnes :
        int index[nb_col] ;
        int index_cond[nb_cond] ;
        for (int i=0 ; i<nb_col ; i++){
            if(strcmp(columns[i].nom , "star")){ //             !!!!!! n'oublie pas le cas de select * !!!!!!!
                for(int j=0 ; j<sa_taille ; j++){
                    if(!strcmp(columns_file[j].nom,columns[i].nom)){
                        columns[i] = columns_file[j] ; // dès maintenant tous les champs sont remplis
                        index[i] = j ;
                        break ;
                    }else if(j==sa_taille-1){
                        return -2;   // colone introuvable dans sleect clause;
                    }
                }
            }else{
                columns = columns_file ;
                nb_col = sa_taille ;
                for(int j=0 ; j<sa_taille ; j++){
                    index[j] = j ;
                }
                break ;
            }
        }
        // test d'existance des colonnes de where clause :
        for (int i=0 ; i<nb_cond ; i++){
            for(int j=0 ; j<sa_taille ; j++){
                if(!strcmp(columns_file[j].nom , conditions[i].col.nom)){
                    conditions[i].col = columns_file[j] ; // dès maintenant tous les champs sont remplis
                    if (!conditions[i].col.prk)
                    {
                        return -5 ; // colonne de where clause n'est pas primary or indexed key
                    }
                    index_cond[i] = j ;
                    break ;
                }else if(j==sa_taille-1){
                    return -3;   // colone introuvable dans where clause ;
                }
            }
            for(int j=0 ;j<conditions[i].nbr_val ; j++){
                if(conditions[i].col.type != conditions[i].values[j].type){
                    return -4 ; // incompatible types of comparaison
                }
            }

        }
        
        //                          ##################################
        //                         ######## interprétation ##########
        //                        ##################################
        printf("\n\n");
        const char* sep = "----------------------" ;
        for (int i=0 ; i<nb_col ; i++){
            if(i==0){
                printf( "\033[1m\033[36m" " ++%-20s-",sep); 
            }else if(i==nb_col-1){
                printf("-%-20s++\n",sep); 
            }else{
                printf("-%-20s-",sep);
            }
        }
        for (int i=0 ; i<nb_col ; i++){
            if(strcmp(columns[i].nom , "star")){
                if(i==0){
                    printf(" || %-20s |", columns[i].nom);
                }else if(i==nb_col-1){
                    printf("| %-20s || \n", columns[i].nom);
                }else{
                    printf("| %-20s |", columns[i].nom);
                }
            }
        }
        for (int i=0 ; i<nb_col ; i++){
            if(i==0){
                printf(" ||%-20s+",sep); 
            }else if(i==nb_col-1){
                printf("+%-20s||\n",sep); 
            }else{
                printf("+%-20s+",sep);
            }
        }
        //#######################   LECTURE DE DATA   ###############################
        char path[256];
        char curr[256];
        get_current_ksp(curr) ;
        strcpy(path, path_tab);
        strcat(path, curr );
        strcat(path, "/" );
        strcat(path, ptr_tab->nom );
        strcat(path, ".json" );
        json_error_t err ;
        json_t * root = json_object() ;
        json_t * data = json_object() ;
        root = json_load_file(path , 0 , &err) ;
        data = json_object_get(root , "data") ;
        json_t *array = json_array();
        const char* key ;
        // json_object_foreach(root , key , col ){
        //  printf(" ===== %s\n", key);
        // }
        int rows = 0 ;
        json_object_foreach(data , key , array ){
            // flag de test == if true show sinon not show :
            boolean flag = true ;
            boolean tflag = false ;
            

            for (int i = 0; i < nb_cond; ++i)
            {
                json_t * temp = json_array_get(array, index_cond[i]) ;
                int x = json_typeof(temp) ;
                op_type op = conditions[i].op ;
                switch (op) {
                    case EGAL_ :
                    switch (x) {
                        case JSON_STRING :
                        if(strcmp(conditions[i].values[0].val , json_string_value(temp) )) flag = false ;
                        break;
                        case JSON_INTEGER :
                        if(atoi(conditions[i].values[0].val) != json_integer_value(temp)) flag = false ;
                        break;
                        case JSON_REAL :
                        if(atof(conditions[i].values[0].val) != json_real_value(temp)) flag = false ;
                        break;
                        case JSON_TRUE :
                        if(atoi(conditions[i].values[0].val) != 1) flag = false ;
                        break;
                        case JSON_FALSE :
                        if(atoi(conditions[i].values[0].val) != 0) flag = false ;
                        break;
                        case JSON_NULL :
                        if(strcmp(conditions[i].values[0].val , "null" )) flag = false ;
                        break;
                    }
                    break;
                    case INF_ :
                    switch (x) {
                        case JSON_STRING :
                        if(strcmp(conditions[i].values[0].val , json_string_value(temp) ) <= 0 ) flag = false ;
                        break;
                        case JSON_INTEGER :
                        if(atoi(conditions[i].values[0].val) <= json_integer_value(temp)) flag = false ;
                        break;
                        case JSON_REAL :
                        if(atof(conditions[i].values[0].val) <= json_real_value(temp)) flag = false ;
                        break;
                    }
                    break;
                    case SUP_ :
                    switch (x) {
                        case JSON_STRING :
                        if(strcmp(conditions[i].values[0].val , json_string_value(temp) ) >= 0 ) flag = false ;
                        break;
                        case JSON_INTEGER :
                        if(atoi(conditions[i].values[0].val) >= json_integer_value(temp)) flag = false ;
                        break;
                        case JSON_REAL :
                        if(atof(conditions[i].values[0].val) >= json_real_value(temp)) flag = false ;
                        break;
                    }
                    break;
                    case INFE_ :
                    switch (x) {
                        case JSON_STRING :
                        if(strcmp(conditions[i].values[0].val , json_string_value(temp) ) < 0 ) flag = false ;
                        break;
                        case JSON_INTEGER :
                        if(atoi(conditions[i].values[0].val) < json_integer_value(temp)) flag = false ;
                        break;
                        case JSON_REAL :
                        if(atof(conditions[i].values[0].val) < json_real_value(temp)) flag = false ;
                        break;
                    }
                    break;
                    case SUPE_ :
                    switch (x) {
                        case JSON_STRING :
                        if(strcmp(conditions[i].values[0].val , json_string_value(temp) ) > 0 ) flag = false ;
                        break;
                        case JSON_INTEGER :
                        if(atoi(conditions[i].values[0].val) > json_integer_value(temp)) flag = false ;
                        break;
                        case JSON_REAL :
                        if(atof(conditions[i].values[0].val) > json_real_value(temp)) flag = false ;
                        break;
                    }
                    break;
                    case IN_ :
                    switch (x) {
                        case JSON_STRING :
                        for(int j=0 ;j<conditions[i].nbr_val ; j++){
                            if(!strcmp(conditions[i].values[j].val , json_string_value(temp) )){
                                tflag = true ;
                                break;
                            }
                        }
                        flag = tflag ;
                        break;
                        case JSON_INTEGER :
                        for(int j=0 ;j<conditions[i].nbr_val ; j++){
                            if(atoi(conditions[i].values[j].val) == json_integer_value(temp)){
                                tflag = true ;
                                break;
                            }
                        }
                        flag = tflag ;
                        break;
                        case JSON_REAL :
                        for(int j=0 ;j<conditions[i].nbr_val ; j++){
                            if(atof(conditions[i].values[j].val) == json_real_value(temp)){
                                tflag = true ;
                                break;
                            }
                        }
                        flag = tflag ;
                        break;
                        case JSON_TRUE :
                        for(int j=0 ;j<conditions[i].nbr_val ; j++){
                            if(atoi(conditions[i].values[j].val) == 1){
                                tflag = true ;
                                break;
                            }
                        }
                        flag = tflag ;
                        break;
                        case JSON_FALSE :
                        for(int j=0 ;j<conditions[i].nbr_val ; j++){
                            if(atoi(conditions[i].values[j].val) == 0){
                                tflag = true ;
                                break;
                            }
                        }
                        flag = tflag ;
                        break;
                        case JSON_NULL :
                        for(int j=0 ;j<conditions[i].nbr_val ; j++){
                            if(!strcmp(conditions[i].values[0].val , "null" )){
                                tflag = true ;
                                break;
                            }
                        }
                        flag = tflag ;
                        break;
                    }
                }
            }
            if(flag){
            // show row :

                for (int i = 0; i < nb_col; ++i)
                {
                    // printf("############### %d\n", index[i]);
                    json_t * temp = json_array_get(array, index[i]) ;
                    int x = json_typeof(temp) ;
                    char tp[256] ;
                    switch (x) {
                        case JSON_STRING :
                        strcpy(tp , json_string_value(temp) ) ;
                        break;
                        case JSON_INTEGER :
                        sprintf(tp, "%d", (int)json_integer_value(temp) )  ;
                        break;
                        case JSON_REAL :
                        sprintf(tp, "%f", json_real_value(temp) )  ;
                        break;
                        case JSON_TRUE :
                        strcpy(tp , "true" ) ;
                        break;
                        case JSON_FALSE :
                        strcpy(tp , "false" ) ;
                        break;
                        case JSON_NULL :
                        strcpy(tp , "null" ) ;
                        break;
                    }
                    if(i==0){
                        printf(" || %-20s |", tp);
                    }else if(i==nb_col-1){
                        printf("| %-20s || \n", tp);
                    }else{
                        printf("| %-20s |", tp);
                    }
                }
                rows++ ;
            }
        }
        for (int i=0 ; i<nb_col ; i++){
            if(i==0){
                printf(" ++%-20s-",sep); 
            }else if(i==nb_col-1){
                printf("-%-20s++\n",sep); 
            }else{
                printf("-%-20s-",sep);
            }
        }
        printf("\033[1m\033[32m" " ( %-2d Rows )" "\x1b[0m" "\x1b[0m" ,rows);
        json_decref(root);
        json_decref(data);
        //##############################################
        printf("\n\n");
        return 1 ;
    }


