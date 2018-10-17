#ifndef DEF_H_
#define DEF_H_

#define PORT 8888
#define MAXLINE 1024
#define THREAD_SIZE 5
#define MAXEVENTS 1024
#define SERVER_SIZE 1024
#define MAX_THREADS 1024
#define BUFSIZE 1024 * 1024 * 1024 //1M
#define WAIT_TIME 300

#define STATE_PARSE_URI 1
#define STATE_PARSE_HEADERS 2
#define STATE_RECV_BODY 3
#define STATE_ANALYSIS 4
#define STATE_FINISH 5

typedef enum
{
    METHOD_GET = 1,
    METHOD_POST = 2
} ner_method;

typedef enum
{
    HTTP_1_0 = 1,
    HTTP_1_1 = 2
} http_version;

typedef enum
{
    PARSER_AGAIN = -1,
    PARSER_ERROR = -2,
    PARSER_SUCCESS = 0
} ner_parser;

#endif