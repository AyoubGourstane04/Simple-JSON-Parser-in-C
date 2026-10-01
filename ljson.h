#ifndef LJSON_H
#define LJSON_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>



#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
   #define FALSE 0
#endif


//
//  Definitions
//

struct _KeyValuePair{
    char* key;
    char* value;
};
typedef struct _KeyValuePair KeyValuePair;

void key_value_pair_free(KeyValuePair* pair);

struct _JSONObject{
    int size;
    int heap_size;
    KeyValuePair* key_value_pairs;
};
typedef struct _JSONObject JSONObject;

void JSON_object_init(JSONObject* obj);
void JSON_object_add(JSONObject* obj, KeyValuePair* pair);
void JSON_object_free(JSONObject* obj);

struct _JSONDocument{
    JSONObject* objects;
};
typedef struct _JSONDocument JSONDocument;


int load_JSON_document(JSONDocument* doc, char* path);
void free_JSON_document(JSONDocument* doc);


//
//  implementation
//



void key_value_pair_free(KeyValuePair* pair){
    free(pair->key);
    free(pair->value);
}


void JSON_object_init(JSONObject* obj){
    obj->heap_size = 1;
    obj->size=0;
    obj->key_value_pairs = (KeyValuePair*) malloc(sizeof(KeyValuePair) * obj->heap_size);
}

void JSON_object_add(JSONObject* obj, KeyValuePair* pair){
    if(obj->size >= obj->heap_size){
        obj->heap_size *= 2;
        obj->key_value_pairs = realloc(obj->key_value_pairs, sizeof(KeyValuePair) * obj->heap_size);
    }

    obj->key_value_pairs[obj->size++] = *pair;
}

void JSON_object_free(JSONObject* obj){
    for(int i=0; i<obj->size; i++){
        key_value_pair_free(&obj->key_value_pairs[i]);
    }
}



int load_JSON_document(JSONDocument* doc, char* path){
    //Opening the file and reading its content
    FILE* file = fopen(path, "r");

    if(!file){
        fprintf(stderr, "Cannot open file %s\n", path);
        return FALSE;
    }

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buff = (char*) malloc(sizeof(char) * size + 1);

    fread(buff, 1, size, file);
    fclose(file);










}








#endif