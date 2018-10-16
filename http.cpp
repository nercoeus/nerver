#include "http.h"
#include <stdlib.h>

ner_http* ner_http_init(){
    ner_http * res = (ner_http*)new ner_http;
    if(res == NULL){
        exit(-1);
    }
    return res;
}


int http_request(string http_str, ner_http* http_data){
    return 1;
}

void ner_http_free(ner_http* http_data){
    if(http_data == NULL){
        return ;
    }
    delete http_data;
}