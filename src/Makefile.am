lib_LTLIBRARIES = libobelisk.la
libobelisk_la_LIBADD = \
	$(top_srcdir)/deps/libevent/libevent/libevent.la \
	$(top_srcdir)/deps/jansson/src/libjansson.la
libobelisk_la_CFLAGS = \
	-I$(top_srcdir)/deps/jansson/src \
	-I$(top_srcdir)/deps/libevent/libevent/include \
	-I$(top_srcdir)/deps/libevent/libevent
libobelisk_la_SOURCES = \
	obelisk.c \
	obelisk_error.c \
	obelisk_json.c
