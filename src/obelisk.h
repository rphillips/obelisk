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
#ifndef OBELISK_H_
#define OBELISK_H_

#include <jansson.h> /* json */
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
