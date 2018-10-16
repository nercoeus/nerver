#ifndef HTTP_H_
#define HTTP_H_

#include <string>
#include <map>
using namespace std;

typedef map<string, string>   ner_http_header;

typedef struct{
    string method;
    string url;
    string version;

    ner_http_header header;
    string body;
}ner_http;

ner_http* ner_http_init();
int http_request(string http_str, ner_http* http_data);
void ner_http_free(ner_http* http_data);

#endif