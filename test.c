#include <stdio.h>
#include "ljson.h"

int main(void)
{
    JSONDocument doc;

    JSON_document_init(&doc);

    if (!JSON_document_load(&doc, "test.json")) {
        printf("Failed to load JSON\n");
        return 1;
    }

    printf("JSON loaded successfully!\n\n");



    JSONValue* name = get_value_by_path(doc.root, "name");

    printf("name: ");
    print_value(name);


    JSONValue* age = get_value_by_path(doc.root, "age");

    printf("age: ");
    print_value(age);


    JSONValue* student = get_value_by_path(doc.root, "student");

    printf("student: ");
    print_value(student);


    JSONValue* city = get_value_by_path(doc.root, "city");

    printf("city: ");
    print_value(city);


    JSONValue* nothing = get_value_by_path(doc.root, "nothing");

    printf("nothing: ");
    print_value(nothing);


    JSONValue* country = get_value_by_path(doc.root, "address.country");

    printf("country: ");
    print_value(country);


    JSONValue* street = get_value_by_path(doc.root, "address.street");

    printf("street: ");
    print_value(street);



    JSONValue* unknown = get_value_by_path(doc.root, "address.zipcode");

    printf("zipcode: ");
    print_value(unknown);


    JSON_document_free(&doc);

    return 0;
}