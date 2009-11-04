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
#include <errno.h>
#include "obelisk.h"
#include "obelisk_error.h"

static int
compare_methods(const void * va, const void * vb)
{
    return strcmp(va, ((obelisk_rpc_t*)vb)->method);
}

static obelisk_error_t*
obelisk_execute_rpc(json_t *request, json_t **response, obelisk_baton_t *baton)
{
    obelisk_error_t *obelisk_err = OBELISK_SUCCESS;
    json_t *method; 
    json_t *params;
    json_t *id;
    const char *method_string;
    
    /* fetch the ID first and make sure we have it, because we need it later */
    if ((id = json_object_get(request, "id")) == NULL) {
        obelisk_err = obelisk_error_create(id, OBELISK_ERROR_INVALID_REQUEST, 
                                 "id missing");
    }

    /* Check the method and params parameters */
    if ((method = json_object_get(request, "method")) == NULL ||
        (method_string = json_string_value(method)) == NULL) {
        obelisk_err = obelisk_error_create(id, OBELISK_ERROR_INVALID_REQUEST, 
                                 "method missing");
    }
    else if ((params = json_object_get(request, "params")) == NULL) {
        obelisk_err = obelisk_error_create(id, OBELISK_ERROR_INVALID_REQUEST, 
                                 "params missing");
    }
    else {
        json_t *result;
        json_t *rpc_version;
        obelisk_rpc_t *rpc = bsearch(method_string, 
                                baton->rpc,
                                baton->rpc_size / sizeof(obelisk_rpc_t),
                                sizeof(obelisk_rpc_t),
                                compare_methods);
        if (!rpc) {
            obelisk_err = obelisk_error_create(id, OBELISK_ERROR_METHOD_NOT_FOUND, "");
            goto done;
        }

        obelisk_err = (*rpc->cb)(params, &result);
        if (obelisk_err) {
            goto done;
        }

        rpc_version = json_string("2.0");

        *response = json_object();
        json_object_set(*response, "jsonrpc", rpc_version);
        json_object_set(*response, "result", result);
        json_object_set(*response, "id", id);

        json_decref(result);
        json_decref(rpc_version);
    }

done:
    return obelisk_err;
}

/** 
 * @brief Run the JSON-RPC handler
 * @param request The entire request object
 * @param response response object, freed by caller
 * @return obelisk_SUCCESS on success
 */
static obelisk_error_t*
obelisk_run_handle(json_t *request, json_t **response, obelisk_baton_t *baton)
{
    obelisk_error_t *obelisk_err = OBELISK_SUCCESS;
    int array_size = json_array_size(request);
    if (array_size == 0) { /* Single RPC Call */
        obelisk_err = obelisk_execute_rpc(request, response, baton);
    }
    else { /* Multi-RPC Call */
        int i;
        json_t *rsp_element;
        json_t *req_element;
        obelisk_error_t *rsp_err;

        /* Create response array */
        *response = json_array();

        for (i=0; i<array_size; i++) {
            req_element = json_array_get(request, i);
            rsp_err = obelisk_execute_rpc(req_element, &rsp_element, baton);
            if (rsp_err) {
                /* RPC call errored out */
                json_array_append(*response, rsp_err->json);
                obelisk_error_destroy(rsp_err);
            }
            else {
                json_array_append(*response, rsp_element);
                json_decref(rsp_element);
            }
        }
    }

    return obelisk_err;
}

static void
obelisk_api_cb(struct evhttp_request *req, void *arg)
{
    obelisk_error_t *obelisk_err = NULL;
    json_error_t js_err;
    json_t *js_rsp;
    json_t *js_req;
    unsigned char *json_request;
    unsigned char *json_response;
    struct evbuffer *evb = evbuffer_new();
    struct evbuffer *evr = evhttp_request_get_input_buffer(req);
    ev_ssize_t request_length = evbuffer_get_length(evr);
    obelisk_baton_t *baton = (obelisk_baton_t*) arg;

    /* Check for POST */
    if (req->type != EVHTTP_REQ_POST) {
        json_incref(js_rsp);
        goto done;
    }

    /* Check for an empty request */
    if (request_length == 0) {
        obelisk_err = obelisk_error_create(0, OBELISK_ERROR_INVALID_REQUEST, "Empty Request");
        js_rsp = obelisk_err->json;
        json_incref(js_rsp);
        goto done;
    }
    
    json_request = evbuffer_pullup(evr, request_length);
    if (json_request == NULL) {
        obelisk_err = obelisk_error_create(0, OBELISK_ERROR_INVALID_REQUEST, "Empty Request");
        js_rsp = obelisk_err->json;
        json_incref(js_rsp);
        goto done;
    }

    json_request[request_length] = 0;

    if (baton->settings->verbose > 1) {
        fprintf(stderr, "Request(%s:%i) %s\n",
                req->remote_host,
                req->remote_port, json_request);
    }

    /* Parse Request */
    js_req = json_loads(json_request, &js_err);
    if (js_req == NULL) {
        /* Format Parse Error */
        obelisk_err = obelisk_error_create(0, OBELISK_ERROR_PARSE, js_err.text);
        js_rsp = obelisk_err->json;
        json_incref(js_rsp);
        goto done;
    }

    /* Run Handler */
    obelisk_err = obelisk_run_handle(js_req, &js_rsp, baton);
    if (obelisk_err) {
        obelisk_err = obelisk_error_create(0, OBELISK_ERROR_INVALID_REQUEST, obelisk_err->msg);
        js_rsp = obelisk_err->json;
        json_incref(js_rsp);
        goto done;
    }

    /* Write data */
done:
    json_response = json_dumps(js_rsp, 0);
    if (json_response) {
        if (baton->settings->verbose > 1) {
            fprintf(stderr, "Response(%s:%i): %s",
                    req->remote_host,
                    req->remote_port, 
                    json_response);
        }
        evbuffer_add(evb, json_response, strlen(json_response));
    }
    free(json_response);

    /* Send the reply */
    evhttp_send_reply(req, HTTP_OK, "ej", evb);

    /* Free data */
    json_decref(js_rsp);
    if (obelisk_err) obelisk_error_destroy(obelisk_err);        
    evbuffer_free(evb);
}

void
obelisk_init(obelisk_settings_t *settings)
{
    memset(settings, 0, sizeof(*settings));
    settings->port = OBELISK_DEFAULT_PORT;
}

void 
obelisk_run(obelisk_settings_t *settings)
{
    if (settings->daemonize) {
        pid_t pid, sid;

        if (settings->verbose) {
            fprintf(stderr, "becoming a daemon\n");
        }

        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "fork error %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (pid > 0) {
            exit(0);
        }

        umask(0);

        sid = setsid();
        if (sid < 0) {
            exit(EXIT_FAILURE);
        }

        if (chdir("/") < 0) {
            exit(EXIT_FAILURE);
        }

        freopen( "/dev/null", "r", stdin);
        freopen( "/dev/null", "w", stdout);
        freopen( "/dev/null", "w", stderr);
    }

    {
        struct event_base *base = event_base_new();
        struct evhttp *http = evhttp_new(base);

        evhttp_set_cb(http, "/api", obelisk_api_cb, settings);
        evhttp_bind_socket(http, settings->bindaddr, settings->port);

        event_base_dispatch(base);
    }
}
