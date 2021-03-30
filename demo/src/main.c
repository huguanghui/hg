#include <stdio.h>
#include <stdlib.h>
#include "gh_log.h"

AML_LOG_DEFINE(test);
#define AML_LOG_DEFAULT AML_LOG_GET_CAT(test)

int main (int argc, char *argv[]) {
    const char *debuglevel = getenv("AML_DEBUG");
    if (!debuglevel)
        debuglevel = ".*:3";
    aml_ipc_log_set_from_string(debuglevel);
    const char *tracelevel = getenv("AML_TRACE");
    if (!tracelevel)
       tracelevel = ".*:3";
    aml_ipc_trace_set_from_string(tracelevel);
    FILE *ftrace = NULL; 
    const char *tracefile = getenv("AML_TRACE_FILE");
    if (tracefile)
    {
        ftrace = fopen(tracefile, "wb");
        aml_ipc_trace_set_json_output(ftrace);
    }

    AML_LOGVVV("hello world!");

    if (ftrace)
        fclose(ftrace);

    return EXIT_SUCCESS;
}

