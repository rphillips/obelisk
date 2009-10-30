#include <jansson.h> /* json */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "ej_error.h"

static void
ej_create_json_error(ej_error_t *err)
{
    err->json = json_object();

    {
        json_t *json_str = json_string("error");
        json_object_set(err->json, "type", json_str);
        json_decref(json_str);
    }

    {
        json_t *json_str = json_string(err->msg);
        json_object_set(err->json, "message", json_str);
        json_decref(json_str);
    }
}

ej_error_t*
ej_error_create_impl(ej_error_errno_t errno,
                     const char *msg,
                     unsigned int line,
                     const char *file)
{
    ej_error_t *err = malloc(sizeof(*err));
    err->errno = errno;
    err->line = line;
    err->file = file;
    err->msg = strdup(msg);
    ej_create_json_error(err);
    return err;
}

ej_error_t*
ej_error_createf_impl(ej_error_errno_t errno,
                      unsigned int line,
                      const char *file, 
                      const char *fmt,
                      ...)
{
    va_list args;
    ej_error_t *err;

    err = malloc(sizeof(*err));
    err->errno = errno;
    err->line = line;
    err->file = file;

    va_start(args, fmt);
    vasprintf(&err->msg, fmt, args);
    va_end(args);

    ej_create_json_error(err);

    return err;
}

void
ej_error_destroy(ej_error_t *err)
{
    if (err && err->json) {
        json_decref(err->json);
    }
    if (err && err->msg) {
        free(err->msg);
    }
    free(err);
}

