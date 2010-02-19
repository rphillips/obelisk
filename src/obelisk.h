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
#ifndef OBELISK_H_
#define OBELISK_H_

#include <jansson.h> /* json */
#include "obelisk_json.h"
#include "obelisk_error.h"

#define OBELISK_DEFAULT_PORT 10351

/* RPC Callbacks */
typedef struct {
    const char *method;
    obelisk_error_t* (*cb)(json_t *params, json_t **response);
} obelisk_rpc_t;

typedef struct {
    unsigned int verbose;
    unsigned int daemonize;
    const char *bindaddr;
    unsigned short port;
} obelisk_settings_t;

typedef struct {
    obelisk_settings_t *settings;
    obelisk_rpc_t *rpc;
    size_t rpc_size;
} obelisk_baton_t;

void
obelisk_init(obelisk_settings_t *settings);

void 
obelisk_run(obelisk_baton_t *baton);

#endif
