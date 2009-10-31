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
        json_t *code_obj;
        json_t *msg_obj;
        json_t *rpc_version = json_string("2.0");
        json_t *error_obj = json_object();
        json_t *data_obj = json_string(err->msg);
        json_t *id_obj = err->id ? json_integer(err->id) : json_null();

        switch (err->errno) {
            case EJ_ERROR_PARSE:
                code_obj = json_integer(-32700);
                msg_obj = json_string("Parse error.");
                break;
            case EJ_ERROR_INVALID_REQUEST:
                code_obj = json_integer(-32600);
                msg_obj = json_string("Invalid request.");
                break;
            case EJ_ERROR_METHOD_NOT_FOUND:
                code_obj = json_integer(-32601);
                msg_obj = json_string("Method not found.");
                break;
            case EJ_ERROR_INVALID_PARAMS:
                code_obj = json_integer(-32602);
                msg_obj = json_string("Invalid params.");
                break;
            case EJ_ERROR_INTERNAL:
                code_obj = json_integer(-32603);
                msg_obj = json_string("Internal error.");
                break;
            case EJ_ERROR_SERVER:
                code_obj = json_integer(-32000);
                msg_obj = json_string("Server error.");
                break;
            default:
                code_obj = json_integer(-32001);
                msg_obj = json_string("Error object not defined.");
                break;
        }

        json_object_set(err->json, "id", id_obj);
        json_object_set(err->json, "jsonrpc", rpc_version);
        json_object_set(err->json, "error", error_obj);
        json_object_set(error_obj, "code", code_obj);
        json_object_set(error_obj, "message", msg_obj);
        json_object_set(error_obj, "data", data_obj);

        json_decref(code_obj);
        json_decref(msg_obj);
        json_decref(error_obj);
        json_decref(rpc_version);
        json_decref(id_obj);
    }
}

ej_error_t*
ej_error_create_impl(unsigned int id, 
                     ej_error_errno_t errno,
                     const char *msg,
                     unsigned int line,
                     const char *file)
{
    ej_error_t *err = malloc(sizeof(*err));
    err->errno = errno;
    err->line = line;
    err->file = file;
    err->id = id;
    err->msg = strdup(msg);
    ej_create_json_error(err);
    return err;
}

ej_error_t*
ej_error_createf_impl(unsigned int id, 
                      ej_error_errno_t errno,
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
    err->id = id;

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

