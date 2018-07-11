/*
 * Copyright (c) 2009-2016 Petri Lehtinen <petri@digip.org>
 *
 * Jansson is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifdef HAVE_CONFIG_H
#include <jansson_private_config.h>
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include "jansson.h"
#include "hashtable.h"
#include "jansson_private.h"
#include "utf.h"


static int newline_offset(const char *text)
{
    const char *newline = strchr(text, '\n');
    if(!newline)
        return strlen(text);
    else
        return (int)(newline - text);
}
int main(int argc, char *argv[])
{
    size_t i;
    char *text;


#define PATH_SIZE     256
size_t size = 0;

	char path[PATH_SIZE];
    	snprintf(path, PATH_SIZE, "./file.json");

    

    	json_error_t error;

    	json_t *root = json_array();
	json_t *desc = json_object();
	json_array_insert(root,0,desc);
	


    	
	json_t *desc_table = json_object();
	json_t *columns = json_object();
	json_object_set_new(desc_table,"columns_t",columns);
    	json_t *table = json_object();
	json_t *tables = json_object();

	
	
	json_object_set_new(table, "desc_table", desc_table);
	json_object_set_new(tables, "table1", table);
	json_array_insert(root,1,tables);
	

  

    
	FILE *fp = fopen("file4.json", "w");
	json_dumpf(root,fp,JSON_INDENT(1));

	

    json_decref(root);
    return 0;
}   
