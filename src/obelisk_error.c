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
#include <jansson.h> /* json */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "obelisk_error.h"

static void
obelisk_create_json_error(obelisk_error_t *err)
{
    err->json = json_object();

    {
        json_t *code_obj;
        json_t *msg_obj;
        json_t *rpc_version = json_string("2.0");
        json_t *error_obj = json_object();
        json_t *data_obj = json_string(err->msg);
        json_t *id_obj = err->id ? err->id : json_null();

        switch (err->err) {
            case OBELISK_ERROR_PARSE:
                code_obj = json_integer(-32700);
                msg_obj = json_string("Parse error.");
                break;
            case OBELISK_ERROR_INVALID_REQUEST:
                code_obj = json_integer(-32600);
                msg_obj = json_string("Invalid request.");
                break;
            case OBELISK_ERROR_METHOD_NOT_FOUND:
                code_obj = json_integer(-32601);
                msg_obj = json_string("Method not found.");
                break;
            case OBELISK_ERROR_INVALID_PARAMS:
                code_obj = json_integer(-32602);
                msg_obj = json_string("Invalid params.");
                break;
            case OBELISK_ERROR_INTERNAL:
                code_obj = json_integer(-32603);
                msg_obj = json_string("Internal error.");
                break;
            case OBELISK_ERROR_SERVER:
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

obelisk_error_t*
obelisk_error_create_impl(json_t *id, 
                     obelisk_error_errno_t e,
                     const char *msg,
                     unsigned int line,
                     const char *file)
{
    obelisk_error_t *err = malloc(sizeof(obelisk_error_t));
    err->err = e;
    err->line = line;
    err->file = file;
    err->id = id;
    err->msg = strdup(msg);
    obelisk_create_json_error(err);
    return err;
}

obelisk_error_t*
obelisk_error_createf_impl(json_t *id, 
                      obelisk_error_errno_t e,
                      unsigned int line,
                      const char *file, 
                      const char *fmt,
                      ...)
{
    va_list args;
    obelisk_error_t *err;

    err = malloc(sizeof(*err));
    err->err = e;
    err->line = line;
    err->file = file;
    err->id = id;

    va_start(args, fmt);
    vasprintf(&err->msg, fmt, args);
    va_end(args);

    obelisk_create_json_error(err);

    return err;
}

void
obelisk_error_destroy(obelisk_error_t *err)
{
    if (err && err->json) {
        json_decref(err->json);
    }
    if (err && err->msg) {
        free(err->msg);
    }
    free(err);
}

