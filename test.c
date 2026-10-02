#include "ljson.h"

int main(){

    JSONDocument doc;

    if(JSON_document_load(&doc, "test.json")){
        puts("Hello if : \n");
        for(int i=0; i<doc.size; i++){
            printf("Hello i %d: \n", i);
            for(int j=0; j<doc.objects[i].size; j++){
                printf("Hello j %d: \n", j);
                printf("%s = %s\n", doc.objects[i].key_value_pairs[j].key, doc.objects[i].key_value_pairs[j].value);
            }
        }
    }else{
        puts("false");
    }

    return 0;
}