/*
   Copyright 2010 Ryan Phillips

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#ifndef OBELISK_ERROR_H_
#define OBELISK_ERROR_H_

struct json_t;

typedef enum {
    OBELISK_ERROR_PARSE,
    OBELISK_ERROR_INVALID_REQUEST,
    OBELISK_ERROR_METHOD_NOT_FOUND,
    OBELISK_ERROR_INVALID_PARAMS,
    OBELISK_ERROR_INTERNAL,
    OBELISK_ERROR_SERVER,
} obelisk_error_errno_t;

typedef struct {
    obelisk_error_errno_t err;
    json_t *json; 
    json_t *id;
    unsigned int line;
    const char *file;
    char *msg;
} obelisk_error_t;

#define OBELISK_SUCCESS (NULL)

#define obelisk_error_create(id, err, msg) obelisk_error_create_impl(id, err,      \
                                                       msg,      \
                                                       __LINE__, \
                                                       __FILE__)

#define obelisk_error_createf(id, err, fmt, ...) obelisk_error_createf_impl(id, err,      \
                                                              __LINE__, \
                                                              __FILE__, \
                                                              fmt,      \
                                                              __VA_ARGS__)

obelisk_error_t*
obelisk_error_create_impl(json_t *id,
                     obelisk_error_errno_t err,
                     const char *msg,
                     unsigned int line,
                     const char *file);

obelisk_error_t*
obelisk_error_createf_impl(json_t *id, 
                      obelisk_error_errno_t err,
                      unsigned int line,
                      const char *file, 
                      const char *fmt,
                      ...);

void
obelisk_error_destroy(obelisk_error_t *err);

#endif
