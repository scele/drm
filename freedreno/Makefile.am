AUTOMAKE_OPTIONS=subdir-objects

AM_CFLAGS = \
	$(WARN_CFLAGS) \
	$(VISIBILITY_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/freedreno \
	$(PTHREADSTUBS_CFLAGS) \
	-I$(top_srcdir)/include/drm

libdrm_freedreno_la_LTLIBRARIES = libdrm_freedreno.la
libdrm_freedreno_ladir = $(libdir)
libdrm_freedreno_la_LDFLAGS = -version-number 1:0:0 -no-undefined
libdrm_freedreno_la_LIBADD = ../libdrm.la @PTHREADSTUBS_LIBS@

libdrm_freedreno_la_SOURCES = \
	freedreno_device.c \
	freedreno_pipe.c \
	freedreno_priv.h \
	freedreno_ringbuffer.c \
	freedreno_bo.c \
	kgsl/kgsl_bo.c \
	kgsl/kgsl_device.c \
	kgsl/kgsl_drm.h \
	kgsl/kgsl_pipe.c \
	kgsl/kgsl_priv.h \
	kgsl/kgsl_ringbuffer.c \
	kgsl/msm_kgsl.h \
	msm/msm_bo.c \
	msm/msm_device.c \
	msm/msm_drm.h \
	msm/msm_pipe.c \
	msm/msm_priv.h \
	msm/msm_ringbuffer.c \
	list.h

libdrm_freedrenocommonincludedir = ${includedir}/freedreno
libdrm_freedrenocommoninclude_HEADERS = freedreno_drmif.h freedreno_ringbuffer.h

pkgconfigdir = @pkgconfigdir@
pkgconfig_DATA = libdrm_freedreno.pc
