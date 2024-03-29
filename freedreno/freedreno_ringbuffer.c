/* -*- mode: C; c-file-style: "k&r"; tab-width 4; indent-tabs-mode: t; -*- */

/*
 * Copyright (C) 2012 Rob Clark <robclark@freedesktop.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Authors:
 *    Rob Clark <robclark@freedesktop.org>
 */

#include <assert.h>

#include "freedreno_drmif.h"
#include "freedreno_priv.h"
#include "freedreno_ringbuffer.h"

drm_public struct fd_ringbuffer *
fd_ringbuffer_new(struct fd_pipe *pipe, uint32_t size)
{
	struct fd_ringbuffer *ring;

	ring = pipe->funcs->ringbuffer_new(pipe, size);
	if (!ring)
		return NULL;

	ring->size = size;
	ring->pipe = pipe;
	ring->start = ring->funcs->hostptr(ring);
	ring->end = &(ring->start[size/4]);

	ring->cur = ring->last_start = ring->start;

	return ring;
}

drm_public void fd_ringbuffer_del(struct fd_ringbuffer *ring)
{
	ring->funcs->destroy(ring);
}

/* ringbuffers which are IB targets should set the toplevel rb (ie.
 * the IB source) as it's parent before emitting reloc's, to ensure
 * the bookkeeping works out properly.
 */
drm_public void fd_ringbuffer_set_parent(struct fd_ringbuffer *ring,
					 struct fd_ringbuffer *parent)
{
	ring->parent = parent;
}

drm_public void fd_ringbuffer_reset(struct fd_ringbuffer *ring)
{
	uint32_t *start = ring->start;
	if (ring->pipe->id == FD_PIPE_2D)
		start = &ring->start[0x140];
	ring->cur = ring->last_start = start;
	if (ring->funcs->reset)
		ring->funcs->reset(ring);
}

/* maybe get rid of this and use fd_ringmarker_flush() from DDX too? */
drm_public int fd_ringbuffer_flush(struct fd_ringbuffer *ring)
{
	return ring->funcs->flush(ring, ring->last_start);
}

drm_public uint32_t fd_ringbuffer_timestamp(struct fd_ringbuffer *ring)
{
	return ring->last_timestamp;
}

drm_public void fd_ringbuffer_reloc(struct fd_ringbuffer *ring,
				    const struct fd_reloc *reloc)
{
	ring->funcs->emit_reloc(ring, reloc);
}

drm_public void
fd_ringbuffer_emit_reloc_ring(struct fd_ringbuffer *ring,
			      struct fd_ringmarker *target,
			      struct fd_ringmarker *end)
{
	assert(target->ring == end->ring);
	ring->funcs->emit_reloc_ring(ring, target, end);
}

drm_public struct fd_ringmarker * fd_ringmarker_new(struct fd_ringbuffer *ring)
{
	struct fd_ringmarker *marker = NULL;

	marker = calloc(1, sizeof(*marker));
	if (!marker) {
		ERROR_MSG("allocation failed");
		return NULL;
	}

	marker->ring = ring;

	fd_ringmarker_mark(marker);

	return marker;
}

drm_public void fd_ringmarker_del(struct fd_ringmarker *marker)
{
	free(marker);
}

drm_public void fd_ringmarker_mark(struct fd_ringmarker *marker)
{
	marker->cur = marker->ring->cur;
}

drm_public uint32_t fd_ringmarker_dwords(struct fd_ringmarker *start,
					 struct fd_ringmarker *end)
{
	return end->cur - start->cur;
}

drm_public int fd_ringmarker_flush(struct fd_ringmarker *marker)
{
	struct fd_ringbuffer *ring = marker->ring;
	return ring->funcs->flush(ring, marker->cur);
}
