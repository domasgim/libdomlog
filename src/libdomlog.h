#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uci.h>

#define TIME_STRING_SIZE 128
#define PROCNAME_STRING_SIZE 128
#define TAG_STRING_SIZE 128
#define TOKEN_ITERATIONS 7

typedef enum { 
    DOMLOG_INFO, DOMLOG_WARNING, DOMLOG_ERROR, DOMLOG_DEBUG 
    } domlog_tag;

struct domlog_event {
    struct tm *time;
    char *time_string;
    char *pid;
    char *proc_name;
    int tag_enum;
    char *tag_string;
    char *message;
};

/**
 * @brief Write a log entry to a file
 * 
 * @param tag Message tag
 * @param message Message contents
 * @return int 
 */
int domlog_log(int tag, char *message);

/**
 * @brief Print all log entries
 * 
 * @param proc_name Log entry from a specific process. Leave NULL to print all entries
 * @return int 
 */
int domlog_print(char *proc_name);