#include <event2/event.h>
#include <event2/http.h>

static void
http_api_cb(struct evhttp_request *req, void *arg)
{
	struct evbuffer *evb = evbuffer_new();

	evbuffer_add(evb, "test", 4);

	/* allow sending of an empty reply */
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

	evhttp_bind_socket(http, "0.0.0.0", port);

	event_base_dispatch(base);
    return 0;
}

