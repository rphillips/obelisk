bin_PROGRAMS = obelisk
obelisk_LDADD = \
	$(top_srcdir)/deps/libevent/libevent/libevent.la \
	$(top_srcdir)/deps/jansson/src/libjansson.la
obelisk_CFLAGS = \
	-I$(top_srcdir)/deps/jansson/src \
	-I$(top_srcdir)/deps/libevent/libevent/include \
	-I$(top_srcdir)/deps/libevent/libevent
obelisk_SOURCES = \
	obelisk.c \
	obelisk_error.c \
	obelisk_main.c 
