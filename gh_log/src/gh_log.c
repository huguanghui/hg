#include <pthread.h>
#include <regex.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define _GNU_SOURCE
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/prctl.h>

#include "gh_log.h"

__thread int sc_level__ = 0;
static __thread int trace_seq = 0;
static int global_trace_seq = 0;
static pthread_mutex_t g_aml_log_lock = PTHREAD_MUTEX_INITIALIZER;
static struct AmlIPCLogCat AML_LOG_LAST = {NULL, 0, 0, NULL};
struct AmlIPCLogCat AML_LOG_DEFAULT = {"default", 0, 0, &AML_LOG_LAST};
static struct AmlIPCLogCat *g_aml_log_cat_list = &AML_LOG_DEFAULT;

struct log_setting_node {
    struct log_setting_node *next;
    int level;
    regex_t pattern;
};

static struct log_setting_node *g_log_setting = NULL;
static struct log_setting_node *g_trace_setting = NULL;

// locked
static int get_log_level(const char *name) {
    struct log_setting_node *node = g_log_setting;
    for (; node != NULL; node = node->next) {
        if (regexec(&node->pattern, name, 0, NULL, 0) == 0)
            return node->level;
    }
    return 0;
}
// locked
static int get_trace_level(const char *name) {
    struct log_setting_node *node = g_trace_setting;
    for (; node != NULL; node = node->next) {
        if (regexec(&node->pattern, name, 0, NULL, 0) == 0)
            return node->level;
    }
    return 0;
}

static void get_setting_from_string(struct log_setting_node **head, const char *str)
{
    const char *key = str;
    const char *val = NULL;
    char buf[256];
    struct log_setting_node *newhead = NULL;
    struct log_setting_node *tail = NULL;
    struct log_setting_node *node;
    for (const char *p = str;; ++p) {
        if (key) {
            if (*p == ':') {
                snprintf(buf, sizeof(buf), "%.*s", (int)(p - key), key);
                key = NULL;
                val = p + 1;
            }
        } else {
            if (*p == ',' || *p == '\0') {
                node = malloc(sizeof(*node));
                if (regcomp(&node->pattern, buf, REG_NOSUB) != 0) {
                    printf("failed to compile regex '%s'\n", buf);
                    free(node);
                } else {
                    snprintf(buf, sizeof(buf), "%.*s", (int)(p - val), val);
                    node->level = atoi(buf);
                    node->next = NULL;
                    if (newhead) {
                        tail->next = node;
                    } else {
                        newhead = node;
                    }
                    tail = node;
                }
                key = p + 1;
            }
        }
        if (*p == '\0')
            break;
    }
    tail = *head;
    pthread_mutex_lock(&g_aml_log_lock);
    while (tail) {
        node = tail;
        tail = node->next;
        regfree(&node->pattern);
        free(node);
    }
    *head = newhead;
    struct AmlIPCLogCat *cat = g_aml_log_cat_list;
    for (; cat != NULL; cat = cat->next) {
        if (cat->name) {
            cat->log_level = get_log_level(cat->name);
            cat->trace_level = get_trace_level(cat->name);
        }
    }
    pthread_mutex_unlock(&g_aml_log_lock);
}

void aml_ipc_log_set_from_string(const char *str)
{
    get_setting_from_string(&g_log_setting, str);
}

void aml_ipc_trace_set_from_string(const char *str)
{
    get_setting_from_string(&g_trace_setting, str);
}

static const char *log_level_name[] = {"E", "W", "I", "D", "V",
                                       "VV",  "VVV",  "",     "",      ""};

static FILE *ipc_log_fp = NULL;
void aml_ipc_log_set_output_file(FILE *fp)
{
    ipc_log_fp = fp;
}

void aml_ipc_log_msg(struct AmlIPCLogCat *cat,
                     pid_t tid,
                     int level, const char *file, const char *function,
                     int lineno, const char *fmt, ...) {
    if (cat->next == NULL) {
        pthread_mutex_lock(&g_aml_log_lock);
        cat->next = g_aml_log_cat_list;
        g_aml_log_cat_list = cat;
        cat->log_level = get_log_level(cat->name);
        cat->trace_level = get_trace_level(cat->name);
        pthread_mutex_unlock(&g_aml_log_lock);
        if (!AML_LOG_CAT_ENABLE(cat, level))
            return;
    }
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    char buf[4096];
    size_t len = sprintf(buf, "%ld.%06ld|%5d|%s/%s|%s@%03d| ",
                         ts.tv_sec, (ts.tv_nsec / 1000) % 1000000,
                         tid, cat->name, log_level_name[level], function, lineno);
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(&buf[len], sizeof(buf) - len, fmt, ap);
    va_end(ap);
    fprintf(ipc_log_fp ?: stdout, "%s\n", buf);
}

static FILE *trace_json_fp = NULL;
void aml_ipc_trace_set_json_output(FILE *fp)
{
    if (fp != NULL)
        fprintf(fp, "[");
    ++global_trace_seq;
    trace_json_fp = fp;
}

static void trace_thread_name(FILE * fp)
{
    char tname[32];
    prctl(PR_GET_NAME, tname, NULL, NULL, NULL);
    fprintf(fp,
            "{\"name\":\"thread_name\",\"ph\":\"M\",\"pid\":%ld,\"tid\":%ld,"
            "\"args\":{\"name\":\"%s\"}},\n",
            syscall(SYS_getpid), syscall(SYS_gettid), tname);
}

void stop_scope_timer__(struct AmlIPCScopeTimer **pptimer) {
    --sc_level__;
    if (*pptimer == NULL)
        return;
    struct AmlIPCScopeTimer *timer = *pptimer;
    struct AmlIPCLogCat *cat = timer->cat;
    if (cat->next == NULL) {
        pthread_mutex_lock(&g_aml_log_lock);
        cat->next = g_aml_log_cat_list;
        g_aml_log_cat_list = cat;
        cat->log_level = get_log_level(cat->name);
        cat->trace_level = get_trace_level(cat->name);
        pthread_mutex_unlock(&g_aml_log_lock);
    }
    if (!AML_TRACE_CAT_ENABLE(cat, timer->level))
        return;
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    int64_t time_ns = (ts.tv_sec - timer->start_time.tv_sec) * 1000000000LL +
                      (ts.tv_nsec - timer->start_time.tv_nsec);
    if (trace_json_fp) {
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);
        int64_t ttime_ns = (ts.tv_sec - timer->start_ttime.tv_sec) * 1000000000LL +
                           (ts.tv_nsec - timer->start_ttime.tv_nsec);
        if (global_trace_seq != trace_seq) {
            trace_seq = global_trace_seq;
            trace_thread_name(trace_json_fp);
        }
        fprintf(trace_json_fp,
                "{\"name\":\"%s\",\"cat\":\"%s\",\"ph\":\"X\",\"ts\":%lld,\"tts\":%lld,\"dur\":%d,"
                "\"tdur\":%d,\"pid\":%ld,\"tid\":%ld,\"args\":{\"text\":\"%s\"}},\n",
                timer->func, cat->name,
                timer->start_time.tv_sec * 1000000LL + timer->start_time.tv_nsec / 1000,
                timer->start_ttime.tv_sec * 1000000LL + timer->start_ttime.tv_nsec / 1000,
                (int)(time_ns / 1000), (int)(ttime_ns / 1000), syscall(SYS_getpid),
                syscall(SYS_gettid), timer->buf);
        return;
    }
    fprintf(ipc_log_fp ?: stdout, "%ld %ld.%06ld %*d %s %s %s:%d %s %d.%06d ms %s\n",
            syscall(SYS_gettid), ts.tv_sec, (ts.tv_nsec / 1000) % 1000000, sc_level__ + 2,
            sc_level__, cat->name, log_level_name[timer->level], timer->file, timer->line,
            timer->func, (int)(time_ns / 1000000LL), (int)(time_ns % 1000000LL), timer->buf);
}

void aml_ipc_trace_log(struct AmlIPCLogCat *cat, int level, const char *evcat, const char *evname,
                       int evph, void *evid, const char *fmt, ...) {
    if (cat->next == NULL) {
        pthread_mutex_lock(&g_aml_log_lock);
        cat->next = g_aml_log_cat_list;
        g_aml_log_cat_list = cat;
        cat->log_level = get_log_level(cat->name);
        cat->trace_level = get_trace_level(cat->name);
        pthread_mutex_unlock(&g_aml_log_lock);
        if (!AML_TRACE_CAT_ENABLE(cat, level))
            return;
    }
    if (trace_json_fp) {
        if (global_trace_seq != trace_seq) {
            trace_seq = global_trace_seq;
            trace_thread_name(trace_json_fp);
        }
        struct timespec ts,tts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tts);
        char buf[4096];
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(buf, sizeof(buf), fmt, ap);
        va_end(ap);
        fprintf(trace_json_fp,
                "{\"name\":\"%s\",\"cat\":\"%s\",\"ph\":\"%c\",\"ts\":%lld,\"tts\":%lld,\"pid\":%"
                "ld,\"tid\":%ld,\"id\":\"%p\",\"args\":{\"text\":\"%s\"}},\n",
                evname, evcat, evph, ts.tv_sec * 1000000LL + ts.tv_nsec / 1000,
                tts.tv_sec * 1000000LL + tts.tv_nsec / 1000, syscall(SYS_getpid),
                syscall(SYS_gettid), evid, buf);
    }
}
