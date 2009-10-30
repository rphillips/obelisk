#ifndef EJ_ERROR_H_
#define EJ_ERROR_H_

struct json_t;

typedef enum {
    EJ_ERROR_GENERAL,    
} ej_error_errno_t;

typedef struct {
    ej_error_errno_t errno;
    json_t *json; 
    unsigned int line;
    const char *file;
} ej_error_t;

#define ej_error_create(err, msg) ej_error_create_impl(err,      \
                                                       msg,      \
                                                       __LINE__, \
                                                       __FILE__)

#define ej_error_createf(err, fmt, ...) ej_error_createf_impl(err,      \
                                                              __LINE__, \
                                                              __FILE__, \
                                                              fmt,      \
                                                              __VA_ARGS__)

ej_error_t*
ej_error_create_impl(ej_error_errno_t errno,
                     const char *msg,
                     unsigned int line,
                     const char *file);

ej_error_t*
ej_error_createf_impl(ej_error_errno_t errno,
                      unsigned int line,
                      const char *file, 
                      const char *fmt,
                      ...);

void
ej_error_destroy(ej_error_t *err);

#endif
