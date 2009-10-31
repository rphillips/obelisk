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
#include <jansson.h> /* json */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ej_error.h"

typedef struct {
    unsigned int verbose;
} ej_settings_t;

/** 
 * @brief Run the JSON-RPC Handler
 * @param request The entire request object
 * @param response response object, freed by caller
 * @return EJ_SUCCESS on success
 */
static ej_error_t*
ej_run_handle(json_t *request, json_t **response)
{
    /* Create Response */
    *response = json_object();
    {
        json_t *json_str = json_string("Hello World");
        json_object_set(*response, "universe", json_str);
        json_decref(json_str);
    }
    return EJ_SUCCESS;
}

static void
ej_api_cb(struct evhttp_request *req, void *arg)
{
    ej_error_t *ej_err = NULL;
    json_error_t js_err;
    json_t *js_rsp;
    json_t *js_req;
    unsigned char *json_request;
    unsigned char *json_response;
    struct evbuffer *evb = evbuffer_new();
    struct evbuffer *evr = evhttp_request_get_input_buffer(req);
    ev_ssize_t request_length = evbuffer_get_length(evr);
    ej_settings_t *settings = (ej_settings_t*) arg;

    /* Check for POST */
    if (req->type != EVHTTP_REQ_POST) {
        ej_err = ej_error_create(0, EJ_ERROR_INVALID_REQUEST, "Empty Request");
        js_rsp = ej_err->json;
        json_incref(js_rsp);
        goto done;
    }

    /* Check for an empty request */
    if (request_length == 0) {
        ej_err = ej_error_create(0, EJ_ERROR_INVALID_REQUEST, "Empty Request");
        js_rsp = ej_err->json;
        json_incref(js_rsp);
        goto done;
    }
    
    json_request = evbuffer_pullup(evr, request_length);
    if (json_request == NULL) {
        ej_err = ej_error_create(0, EJ_ERROR_INVALID_REQUEST, "Empty Request");
        js_rsp = ej_err->json;
        json_incref(js_rsp);
        goto done;
    }

    if (settings->verbose) {
        fprintf(stderr, "Request(%s:%i) %s\n",
                req->remote_host,
                req->remote_port, json_request);
    }

    /* Parse Request */
    js_req = json_loads(json_request, &js_err);
    if (js_req == NULL) {
        /* Format Parse Error */
        ej_err = ej_error_create(0, EJ_ERROR_PARSE, js_err.text);
        js_rsp = ej_err->json;
        json_incref(js_rsp);
        goto done;
    }

    /* Run Handler */
    ej_err = ej_run_handle(js_req, &js_rsp);

    /* Write data */
done:
    json_response = json_dumps(js_rsp, 0);
    if (json_response) {
        evbuffer_add(evb, json_response, strlen(json_response));
    }
    free(json_response);

    evhttp_send_reply(req, HTTP_OK, "ej", evb);

    /* Free data */
    json_decref(js_rsp);
    if (ej_err) ej_error_destroy(ej_err);        
    evbuffer_free(evb);
}

void
ej_init(ej_settings_t *settings)
{
    memset(settings, 0, sizeof(*settings));
}

int 
main(int argc, const char *argv[])
{
    int ch;
    const char *listen_addr = "0.0.0.0";
    unsigned short port = 9009;
    struct event_base *base = event_base_new();
    struct evhttp *http = evhttp_new(base);
    ej_settings_t settings;

    ej_init(&settings);

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

    evhttp_set_cb(http, "/api", ej_api_cb, &settings);
    evhttp_bind_socket(http, listen_addr, port);

    event_base_dispatch(base);
    return 0;
}

