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
#include <event2/event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "obelisk.h"

obelisk_error_t* 
time_cb(json_t *params, json_t **result)
{
    time_t current_time = time(NULL);
    *result = json_integer(current_time);
    return OBELISK_SUCCESS;
}

obelisk_rpc_t rpc_callbacks[] = {
    {"time", time_cb}
};

int 
main(int argc, char **argv)
{
    int ch;
    const char *listen_addr = "0.0.0.0";
    unsigned short port = 9009;
    struct event_base *base = event_base_new();
    struct evhttp *http = evhttp_new(base);
    obelisk_settings_t settings;
    obelisk_baton_t baton;

    obelisk_init(&settings);
    baton.settings = &settings;
    baton.rpc = &rpc_callbacks;
    baton.rpc_size = sizeof(rpc_callbacks);

    while (-1 != (ch = getopt(argc, argv,
                              "p:"
                              "l:"
                              "v"
                             ))) {
        switch (ch) {
            case 'p':
                port = atoi(optarg);
                break;
            case 'l':
                listen_addr = optarg;
                break;
            case 'v':
                settings.verbose++;
                break;
        }
    }

    if (settings.verbose) {
        fprintf(stderr, "%s\n", "ej");
        fprintf(stderr, "Listening on %s:%i\n", listen_addr, port);
    }

    evhttp_set_cb(http, "/api", obelisk_api_cb, &baton);
    evhttp_bind_socket(http, listen_addr, port);

    event_base_dispatch(base);
    return 0;
}
