bin_PROGRAMS = ej
ej_LDADD = \
	$(top_srcdir)/deps/libevent/libevent/libevent.la \
	$(top_srcdir)/deps/jansson/src/libjansson.la
ej_CFLAGS = \
	-I$(top_srcdir)/deps/jansson/src \
	-I$(top_srcdir)/deps/libevent/libevent/include \
	-I$(top_srcdir)/deps/libevent/libevent
ej_SOURCES = \
	ej.c \
	ej_error.c \
	main.c 

