#ifndef LJSON_H
#define LJSON_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>



#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
   #define FALSE 0
#endif


//
//  Definitions
//

enum _ValueType{
    TYPE_STRING,
    TYPE_NUMBER,
    TYPE_NULL,
    TYPE_OBJECT,
    TYPE_BOOL,
    TYPE_INVALID
};
typedef enum _ValueType ValueType;

typedef struct _JSONValue JSONValue;
typedef struct _KeyValuePair KeyValuePair;

struct _JSONValue{
    ValueType type;
    union{
        double number;
        char* string;
        int boolean;
        struct{
            KeyValuePair* pair;
            int count;
        } object;
    };

};

void JSONValue_free(JSONValue* val);


struct _KeyValuePair{
    char* key;
    JSONValue* value;
};

void key_value_pair_free(KeyValuePair* pair);


struct _JSONDocument{
    JSONValue* root;
};
typedef struct _JSONDocument JSONDocument;


void JSON_document_init(JSONDocument* doc);
void JSON_document_add_obj(JSONDocument* doc);
int JSON_document_load(JSONDocument* doc, char* path);
void JSON_document_free(JSONDocument* doc);

static JSONValue* read_value(const char** text);
static void print_value(JSONValue* val);

//
//  implementation
//



void key_value_pair_free(KeyValuePair* pair){
    free(pair->key);
    JSONValue_free(pair->value);
}


void JSON_document_init(JSONDocument* doc){
    doc->root = NULL;
}

void JSONValue_free(JSONValue* val){
    if(val!=NULL){
        return;
    }

    switch (val->type){
        case TYPE_STRING:
            free(val->string);
            break;
        case TYPE_OBJECT:
            for(int i=0; i< val->object.count; i++){
                free(&val->object.pair[i].key);
                JSONValue_free(val->object.pair[i].value);
            }
            free(val->object.pair);
            break;
        default:
            break;
    }
    free(val);
}


static void skip_spaces(const char** text){
    while(**text == ' '|| **text == '\t' || **text == '\n')
        (*text)++;
}

static char* read_string(const char** text){
    if(**text != '"') return NULL;
    (*text)++;

    const char* start = *text;

    while(**text != '"' && **text != '\0'){
        (*text)++;
    }

    int length = *text - start;

    char* result = (char*) malloc(sizeof(char) * length + 1);

    strncpy(result, start, length);
    result[length] = '\0';

    if(**text == '"') (*text)++;

    return result;
}


static double read_number(const char** text){
    char* end;
    double nbr = strtod(*text, &end);
    *text = end;
    return nbr; 
}

static int read_bool(const char** text){
    if(strncmp(*text, "true", 4) == 0){
        (*text)+=strlen("true");
        return 1;
    }else if(strncmp(*text, "false", 5) == 0){
        (*text)+=strlen("false");
        return 0;
    }
    return 0;
}

static JSONValue* read_object(const char** text){
    JSONValue* obj = (JSONValue*) malloc(sizeof(JSONValue));
    obj->type = TYPE_OBJECT;
    obj->object.pair = (KeyValuePair*) malloc(sizeof(KeyValuePair) * 50);
    obj->object.count = 0;

    (*text)++;
    skip_spaces(text);

    while(**text != '}' && **text != '\0'){
        skip_spaces(text);
        char* key = read_string(text);
        skip_spaces(text);

        if(**text == ':') (*text)++;

        JSONValue* value = read_value(text);

        obj->object.pair[obj->object.count].key = key;
        obj->object.pair[obj->object.count].value = value;
        obj->object.count++;

        skip_spaces(text);

        if(**text == ','){
            (*text)++;
            skip_spaces(text);
        }
    }

    if(**text == '}') (*text)++;

    return obj;
}

static JSONValue* read_value(const char** text){
    skip_spaces(text);
    JSONValue* val = malloc(sizeof(JSONValue));

    if (val == NULL)
        return NULL;

    val->type = TYPE_INVALID;

    if(**text == '"'){
        val->type = TYPE_STRING;
        val->string = read_string(text);        
    }else if(isdigit(**text) || **text == '-'){
        val->type = TYPE_NUMBER;
        val->number = read_number(text);
    }else if(strncmp(*text, "true", 4) == 0 || strncmp(*text, "false", 5) == 0){
        val->type = TYPE_BOOL;
        val->boolean = read_bool(text);
    }else if(strncmp(*text, "null", 4) == 0){
        val->type = TYPE_NULL;
        (*text)+=4;
    }else if(**text == '{'){
        return read_object(text);
    }

    return val;
}

static JSONValue* get_value_by_path(JSONValue* root, const char* path){
    if(root == NULL || root->type != TYPE_OBJECT) return NULL;

    char* pathCopy = strdup(path);
    char* token = strtok(pathCopy, ".");

    JSONValue* curr = root;

    while(token != NULL && curr->type == TYPE_OBJECT){
        int found = 0;

        for(int i=0; i<curr->object.count; i++){
            if(strcmp(curr->object.pair[i].key, token) == 0){
                found = 1;
                curr = curr->object.pair[i].value;
                break;
            }
        }

        if(!found){
            free(pathCopy);
            return NULL;
        }

        token = strtok(NULL, ".");
    }

    free(pathCopy);
    return curr;
}

static void print_object(JSONValue* val) {
    printf("{\n");

    for (int i = 0; i < val->object.count; i++) {
        printf("  %s: ", val->object.pair[i].key);
        print_value(val->object.pair[i].value);
    }

    printf("}\n");
}

static void print_value(JSONValue* val){
    if(!val || val->type == TYPE_INVALID){
        printf("Not Found\n");
        return;
    }

    switch (val->type){
        case TYPE_STRING:
            printf("%s\n", val->string);
            break;
        case TYPE_NUMBER:
            printf("%.2f\n", val->number);
            break;
        case TYPE_BOOL:
            printf("%s\n", val->boolean ? "true" : "false");
            break;
        case TYPE_NULL:
            printf("NULL\n");
            break;
        case TYPE_OBJECT:
            print_object(val);
            break;
    }
}




int JSON_document_load(JSONDocument* doc, char* path){
    //Opening the file and reading its content
    FILE* file = fopen(path, "r");

    if(!file){
        fprintf(stderr, "Cannot open file '%s'\n", path);
        return FALSE;
    }

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buff = (char*) malloc(sizeof(char) * size + 1);

    fread(buff, 1, size, file);
    fclose(file);

    buff[size] = '\0';

     JSON_document_init(doc);

    const char* cursor = buff;

    doc->root = read_value(&cursor);

    free(buff);

    if(doc->root == NULL || doc->root->type == TYPE_INVALID){
        JSON_document_free(doc);
        return FALSE;
    }

    return TRUE;

}

void JSON_document_free(JSONDocument* doc){
    if(doc == NULL){
        return;
    }
    
    JSONValue_free(doc->root);
    doc->root = NULL;
}



#endif