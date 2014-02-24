/*
 * Copyright © 2014 NVIDIA Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "drm-test-tegra.h"
#include "tegra.h"

int main(int argc, char *argv[])
{
	struct drm_tegra_channel *channel;
	struct drm_tegra_pushbuf *pushbuf;
	struct drm_tegra_fence *fence;
	struct drm_tegra_job *job;
	struct drm_tegra *drm;
	int fd, err;

	fd = drm_open(argv[1]);
	if (fd < 0) {
		fprintf(stderr, "failed to open DRM device %s: %s\n", argv[1],
			strerror(errno));
		return 1;
	}

	err = drm_tegra_new(&drm, fd);
	if (err < 0) {
		fprintf(stderr, "failed to create Tegra DRM context: %s\n",
			strerror(-err));
		return 1;
	}

	err = drm_tegra_channel_open(&channel, drm, DRM_TEGRA_VIC);
	if (err < 0) {
		fprintf(stderr, "failed to open channel to VIC: %s\n",
			strerror(-err));
		return 1;
	}

	err = drm_tegra_job_new(&job, channel);
	if (err < 0)
		return 1;

	err = drm_tegra_pushbuf_new(&pushbuf, job);
	if (err < 0)
		return 1;

	err = drm_tegra_pushbuf_prepare(pushbuf, 4);
	if (err < 0)
		return 1;

	*pushbuf->ptr++ = HOST1X_OPCODE_SETCL(0, HOST1X_CLASS_GR2D, 0);

	err = drm_tegra_pushbuf_sync(pushbuf, DRM_TEGRA_SYNCPT_COND_OP_DONE);
	if (err < 0)
		return 1;

	err = drm_tegra_job_submit(job, &fence);
	if (err < 0) {
		return 1;
	}

	err = drm_tegra_fence_wait(fence);
	if (err < 0) {
		return 1;
	}

	drm_tegra_fence_free(fence);
	drm_tegra_pushbuf_free(pushbuf);
	drm_tegra_job_free(job);

	drm_tegra_channel_close(channel);
	drm_tegra_close(drm);
	drm_close(fd);

	return 0;
}
