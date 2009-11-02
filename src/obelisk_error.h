/*
 * Copyright (c) 2009 Ryan Phillips
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
    obelisk_error_errno_t errno;
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
                     obelisk_error_errno_t errno,
                     const char *msg,
                     unsigned int line,
                     const char *file);

obelisk_error_t*
obelisk_error_createf_impl(json_t *id, 
                      obelisk_error_errno_t errno,
                      unsigned int line,
                      const char *file, 
                      const char *fmt,
                      ...);

void
obelisk_error_destroy(obelisk_error_t *err);

#endif
