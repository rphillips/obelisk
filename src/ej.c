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
#include <jansson.h> /* json */
#include <stdlib.h>
#include <string.h>
#include "ej_error.h"

static void
http_api_cb(struct evhttp_request *req, void *arg)
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

#if 0
    /* TODO: Not in libevent2 yet */
    if (evhttp_request_get_type(req) != EVHTTP_REQ_POST) {
        /* Format Empty Request */
        ej_err = ej_error_create(EJ_ERROR_GENERAL, "Request not a HTTP POST");
        js_rsp = ej_err->json;
        goto done;
    }
#endif

    /* Check for an empty request */
    if (request_length == 0) {
        /* Format Empty Request */
        ej_err = ej_error_create(EJ_ERROR_GENERAL, "Empty Request");
        js_rsp = ej_err->json;
        json_incref(js_rsp);
        goto done;
    }

    /* Read Request */
    json_request = evbuffer_pullup(evr, request_length);
    if (json_request == NULL) {
        ej_err = ej_error_create(EJ_ERROR_GENERAL, "Empty Request");
        js_rsp = ej_err->json;
        json_incref(js_rsp);
        goto done;
    }

    /* Parse Request */
    js_req = json_loads(json_request, &js_err);
    if (js_req == NULL) {
        /* Format Parse Error */
        ej_err = ej_error_create(EJ_ERROR_GENERAL, js_err.text);
        js_rsp = ej_err->json;
        json_incref(js_rsp);
        goto done;
    }

    /* Create Response */
    js_rsp = json_object();
    {
        json_t *json_str = json_string("Hello World");
        json_object_set(js_rsp, "universe", json_str);
        json_decref(json_str);
    }

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

    if (ej_err) {
        ej_error_destroy(ej_err);        
    }
    evbuffer_free(evb);
}

int 
main(int argc, const char *argv[])
{
    int port = 9009;
    struct event_base *base = event_base_new();
    struct evhttp *http = evhttp_new(base);

    evhttp_set_cb(http, "/api", http_api_cb, NULL);

    evhttp_bind_socket(http, "127.0.0.1", port);

    event_base_dispatch(base);
    return 0;
}

