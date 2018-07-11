/*
 * Copyright (c) 2009-2014 Petri Lehtinen <petri@digip.org>
 *
 * Jansson is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */

#include <stdlib.h>
#include <string.h>

#include <jansson.h>

#include "jansson.h"
#include "hashtable.h"
#include "jansson_private.h"
#include "utf.h"


#define BUFFER_SIZE  (256 * 1024)  /* 256 KB */

#define URL_FORMAT   "https://api.github.com/repos/%s/%s/commits"
#define URL_SIZE     256

/* Return the offset of the first newline in text or the length of
   text if there's no newline */
static int newline_offset(const char *text)
{
    const char *newline = strchr(text, '\n');
    if(!newline)
        return strlen(text);
    else
        return (int)(newline - text);
}

struct write_result
{
    char *data;
    int pos;
};

static size_t write_response(void *ptr, size_t size, size_t nmemb, void *stream)
{
    struct write_result *result = (struct write_result *)stream;

    if(result->pos + size * nmemb >= BUFFER_SIZE - 1)
    {
        fprintf(stderr, "error: too small buffer\n");
        return 0;
    }

    memcpy(result->data + result->pos, ptr, size * nmemb);
    result->pos += size * nmemb;

    return size * nmemb;
}




int main(int argc, char *argv[])
{
    size_t i;
    char *text;


#define PATH_SIZE     256
size_t size = 0;

	char path[PATH_SIZE];
    	snprintf(path, PATH_SIZE, "./%s.json", "emp");

    json_t *root;
    json_error_t error;


    root = json_load_file(path, 0, &error);
  for(i = 0; i < json_array_size(root); i++)
    {
        json_t *data, *sha, *commit, *message;
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
            printf("KEY NOT FOUND\n");
            json_decref(root);
            return 1;
        }else{
		printf("HAHAHAHAHAHAHA VIIIRTUAL WAS HERE\n");
		return 12;
	}


//int json_object_del(json_t *json, const char *key)

	json_object_del(commit, "message");
        
	
        
    }

	FILE *fp = fopen("file4.json", "w");
	json_dumpf(root,fp,JSON_SORT_KEYS);



    

    json_decref(root);
    return 0;
}
