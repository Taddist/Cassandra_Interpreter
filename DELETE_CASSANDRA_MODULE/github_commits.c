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

    
  json_t *root;
    json_error_t error;

    root = json_load_file(path, 0, &error);
  

    for(i = 0; i < json_array_size(root); i++)
    {
        json_t *data, *sha, *commit, *message,*message2;
/*

JSON_OBJECT/JSON_ARRAY/JSON_STRING/JSON_INTEGER/JSON_REAL/JSON_TRUE/JSON_FALSE/JSON_NULL
*/
        const char *message_text;

        data = json_array_get(root, i);
	/*
	JSON_OBJECT
	*/
        if(!json_is_object(data))
        {
            fprintf(stderr, "error: commit data %d is not an object\n", (int)(i + 1));
            json_decref(root);
            return 1;
        }

        sha = json_object_get(data, "sha");
        if(!json_is_string(sha))
        {
            fprintf(stderr, "error: commit %d: sha is not a string\n", (int)(i + 1));
            return 1;
        }

        commit = json_object_get(data, "commit");
        if(!json_is_object(commit))
        {
            fprintf(stderr, "error: commit %d: commit is not an object\n", (int)(i + 1));
            json_decref(root);
            return 1;
        }
	message = json_object_get(commit, "message");
        if(!json_is_string(message))
        {
            fprintf(stderr, "error: commit %d: message is not a string\n", (int)(i + 1));
            json_decref(root);
            return 1;
        }
	if(!json_is_object(message)){printf("HAHAHAHAHAHAA");}
	

	if(i==0){
		json_string_set(message, "HAHAHAHAHAAHHAHAHA");
		//printf("DONE");
	}

//int json_object_set_new(json_t *json, const char *key, json_t *value)


	

//int json_object_set_new_nocheck(json_t *json, const char *key, json_t *value)
	


//int json_object_del(json_t *json, const char *key)

	//json_object_del(commit,"message");

	message_text = json_string_value(message);
        //printf("%.8s %.*s\n",json_string_value(sha),newline_offset(message_text),message_text);
	
	
        
    }
	FILE *fp = fopen("file4.json", "w");
	json_dumpf(root,fp,JSON_INDENT(1));

	

    json_decref(root);
    return 0;
}
