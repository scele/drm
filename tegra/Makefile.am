AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/include/drm

AM_CFLAGS = \
	@PTHREADSTUBS_CFLAGS@ \
	$(VISIBILITY_CFLAGS)

libdrm_tegra_ladir = $(libdir)
libdrm_tegra_la_LTLIBRARIES = libdrm_tegra.la
libdrm_tegra_la_LDFLAGS = -version-number 0:0:0 -no-undefined
libdrm_tegra_la_LIBADD = ../libdrm.la @PTHREADSTUBS_LIBS@

libdrm_tegra_la_SOURCES = \
	channel.c \
	fence.c \
	job.c \
	private.h \
	pushbuf.c \
	tegra.c

libdrm_tegraincludedir = ${includedir}/libdrm
libdrm_tegrainclude_HEADERS = tegra.h

pkgconfigdir = @pkgconfigdir@
pkgconfig_DATA = libdrm_tegra.pc
