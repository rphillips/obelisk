#include <jansson.h> /* json */
#include "ej_error.h"

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
    err->json = json_object();

    {
        json_t *json_str = json_string("error");
        json_object_set(err->json, "type", json_str);
        json_decref(json_str);
    }

    {
        json_t *json_str = json_string(msg);
        json_object_set(err->json, "message", json_str);
        json_decref(json_str);
    }

    return err;
}

ej_error_t*
ej_error_createf_impl(ej_error_errno_t errno,
                      unsigned int line,
                      const char *file, 
                      const char *fmt,
                      ...)
{

}

void
ej_error_destroy(ej_error_t *err)
{
    free(err);
}

