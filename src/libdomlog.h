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

/**
 * @brief Get a Get UCI configuration entry object. 
 * NOTE - the path string must be passed as a char[] array; char * causes seg fault. 
 * More info https://dev.archive.openwrt.org/ticket/5848.html
 * 
 * @param path UCI option path
 * @return char* 
 */
static char *uci_get_config_entry (char *path);

/**
 * @brief Get the running process name
 * 
 * @param process_path Process path /proc/<PID>/comm
 * @return char* Process name string
 */
static char *get_proc_name(char *process_path);

/**
 * @brief Get log tag string value
 * 
 * @param tag tag enum
 * @return char* Log tag string
 */
static char *get_tag_string(int tag);

/**
 * @brief Get the time string value
 * 
 * @return char* 
 */
static char *get_time_string();

/**
 * @brief Fill contents of the log event
 * 
 * @param event Event object
 * @return int 
 */
static int fill_contents(struct domlog_event *event);

/**
 * @brief Check if a read line from a log contains a specified process name
 * 
 * @param line Log entry
 * @param proc_name Process name to be checked against
 * @return int 
 */
static int is_specified_proc(char *line, char *proc_name);

/**
 * @brief Write a log entry to a specified file
 * 
 * @param event Log event
 * @param log_path Log file path
 * @return int 
 */
static int write_log(struct domlog_event event, char *log_path);