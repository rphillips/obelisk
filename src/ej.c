#include <event2/event.h>
#include <event2/http.h>
#include <jansson.h> /* json */
#include <stdlib.h>
#include <string.h>

static json_t*
error_object(const char *str)
{
    json_t *json = json_object();
    {
        json_t *json_str = json_string(str);
        json_object_set(json, "error", json_str);
        json_decref(json_str);
    }
    return json;
}

static void
http_api_cb(struct evhttp_request *req, void *arg)
{
    json_error_t js_err;
    json_t *js_rsp;
    json_t *js_req;
    unsigned char *json_request;
    unsigned char *json_response;
    struct evbuffer *evb = evbuffer_new();
    struct evbuffer *evr = evhttp_request_get_input_buffer(req);
    ev_ssize_t request_length = evbuffer_get_length(evr);

    /* Check for an empty request */
    if (request_length == 0) {
        /* Format Empty Request */
        js_rsp = error_object("Empty Request");
        json_response = json_dumps(js_rsp, 0);
        evbuffer_add(evb, json_response, strlen(json_response));
        free(json_response);
        json_decref(js_rsp);
        goto err;
    }

    /* Parse Request */
    json_request = evbuffer_pullup(evr, request_length);
    js_req = json_loads(json_request, &js_err);
    if (!js_req) {
        /* Format Parse Error */
        js_rsp = error_object(js_err.text);
        json_response = json_dumps(js_rsp, 0);
        evbuffer_add(evb, json_response, strlen(json_response));
        free(json_response);
        json_decref(js_rsp);
        goto err;
    }

    /* Create Response */
    js_rsp = json_object();
    {
        json_t *json_str = json_string("Hello World\n");
        json_object_set(js_rsp, "universe", json_str);
        json_decref(json_str);
    }

    /* Output Json */
    json_response = json_dumps(js_rsp, 0);
    if (json_response) {
        evbuffer_add(evb, json_response, strlen(json_response));
    }
    free(json_response);
    json_decref(js_rsp);

    /* Write data */
err:
    evhttp_send_reply(req, HTTP_OK, "", evb);
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

