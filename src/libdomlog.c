#include "libdomlog.h"

static char *uci_get_config_entry (char *path) {
    struct uci_context *c;
    struct uci_ptr ptr;
    char *result;

    c = uci_alloc_context ();
    if (uci_lookup_ptr (c, &ptr, path, true) != UCI_OK) {
        uci_perror (c, "get_config_entry Error");
        return 1;
    }

    switch(ptr.o->type) {
        case UCI_TYPE_STRING:
            result = strdup(ptr.o->v.string);
            break;
        default:
            break;
        }

    uci_free_context (c);
    return result;
}

static char *get_proc_name(char *process_path) {
    char *proc_name = malloc(PROCNAME_STRING_SIZE);
    FILE *fp;
    if((fp = fopen(process_path, "r")) == NULL) {
        fprintf(stderr, "Cannot open %s\n", process_path);
        return -1;
    }

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, fp)) != -1) {
        line[strcspn(line, "\n")] = 0; // wonderful function to remove tailing newline
        strncpy(proc_name, line, strlen(line));
    }

    fclose(fp);
    if (line)
        free(line);
    return proc_name;
}

static char *get_tag_string(int tag) {
    char *tag_string = malloc(TAG_STRING_SIZE);
    switch(tag) {
        case DOMLOG_INFO: 
            strcpy(tag_string, "INFO");
            break;
        case DOMLOG_WARNING: 
            strcpy(tag_string, "WARNING");
            break;
        case DOMLOG_ERROR: 
            strcpy(tag_string, "ERROR");
            break;
        case DOMLOG_DEBUG: 
            strcpy(tag_string, "DEBUG");
            break;
        default: 
            strcpy(tag_string, NULL);
    }
    return tag_string;
}

static char *get_time_string() {
    char *time_string = malloc(TIME_STRING_SIZE);
    time_t t = time(NULL);
    struct tm * p = localtime(&t);
    strftime(time_string, TIME_STRING_SIZE, "%A, %B %d %H:%M:%S %Y", p);
    return time_string;
}

static int fill_contents(struct domlog_event *event) {
    pid_t pid = getpid();
    char pid_string[32];
    char process_path[256];
    char *time_string = get_time_string();
    char *tag_string = get_tag_string(event->tag_enum);

    snprintf(pid_string, sizeof(pid_string), "%d", pid);
    snprintf(process_path, sizeof(process_path), "/proc/%d/comm", pid);

    char *process_name = get_proc_name(process_path);

    event->proc_name = malloc(strlen(process_name) + 1);
    event->pid = malloc(strlen(pid_string) + 1);
    event->tag_string = malloc(strlen(tag_string) + 1);
    event->time_string = malloc(strlen(time_string) + 1);

    strncpy(event->proc_name, process_name, strlen(process_name) + 1);
    strncpy(event->pid, pid_string, strlen(pid_string) + 1);
    strncpy(event->tag_string, tag_string, strlen(tag_string) + 1);
    strncpy(event->time_string, time_string, strlen(time_string) + 1);

    free(time_string);
    free(process_name);
    free(tag_string);
    return 0;
}

static int is_specified_proc(char *line, char *proc_name) {
    char *token;
    char *token_pointer;
    token_pointer = strdup(line);

    for(int i = 0; i < TOKEN_ITERATIONS; i++) {
        if ((token = strtok_r(token_pointer, " ", &token_pointer)) == NULL) { return -1; }
    }

    if(strcmp(token, proc_name) == 0) {
        return 1;
    }
    return 0;
}

static int write_log(struct domlog_event event, char *log_path) {
    FILE *fp;
    if((fp = fopen(log_path, "a")) == NULL) {
        fprintf(stderr, "Cannot open %s\n", log_path);
        return -1;
    }
    fprintf(fp, "%s %s %s [%s]: %s\n", event.time_string, 
    event.tag_string, event.proc_name, event.pid, event.message);
    fclose(fp);
    return 0;
}

int domlog_log(int tag, char *message) {
    if (strlen(message) > 300) {
        return -1;
    }

    char log_path_command[] = "domlog.config.log_path";
    char *log_path = uci_get_config_entry(log_path_command);

    struct domlog_event event;
    event.tag_enum = tag;
    event.message = message;
    fill_contents(&event);

    if(write_log(event, log_path)) {
        return -1;
    }

    free(event.proc_name);
    free(event.pid);
    free(event.tag_string);
    free(event.time_string);
    return 0;
}

int domlog_print(char *proc_name) {
    char log_path_command[] = "domlog.config.log_path";
    char *log_path = uci_get_config_entry(log_path_command);

    FILE *fp;
    if((fp = fopen(log_path, "r")) == NULL) {
        fprintf(stderr, "Cannot open %s\n", log_path);
        return -1;
    }

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, fp)) != -1) {
        if(proc_name != NULL) {
            if(is_specified_proc(line, proc_name) == 1) {
                fprintf(stdout, "%s", line);
            }
        } else {
            fprintf(stdout, "%s", line);
        }
    }

    fclose(fp);
    if (line)
        free(line);
    return 0;
}