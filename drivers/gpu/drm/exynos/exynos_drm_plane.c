/*
 * Copyright (C) 2011 Samsung Electronics Co.Ltd
 * Authors: Joonyoung Shim <jy0922.shim@samsung.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <drm/drmP.h>

#include <drm/exynos_drm.h>
<<<<<<< HEAD
=======
#include <drm/drm_plane_helper.h>
#include <drm/drm_atomic_helper.h>
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
#include "exynos_drm_drv.h"
#include "exynos_drm_crtc.h"
#include "exynos_drm_fb.h"
#include "exynos_drm_gem.h"
#include "exynos_drm_plane.h"

<<<<<<< HEAD
#define to_exynos_plane(x)	container_of(x, struct exynos_plane, base)

struct exynos_plane {
	struct drm_plane		base;
	struct exynos_drm_overlay	overlay;
	bool				enabled;
};

static const uint32_t formats[] = {
	DRM_FORMAT_XRGB8888,
	DRM_FORMAT_ARGB8888,
	DRM_FORMAT_NV12,
	DRM_FORMAT_NV12MT,
};

=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
/*
 * This function is to get X or Y size shown via screen. This needs length and
 * start position of CRTC.
 *
 *      <--- length --->
 * CRTC ----------------
 *      ^ start        ^ end
 *
 * There are six cases from a to f.
 *
 *             <----- SCREEN ----->
 *             0                 last
 *   ----------|------------------|----------
 * CRTCs
 * a -------
 *        b -------
 *        c --------------------------
 *                 d --------
 *                           e -------
 *                                  f -------
 */
static int exynos_plane_get_size(int start, unsigned length, unsigned last)
{
	int end = start + length;
	int size = 0;

	if (start <= 0) {
		if (end > 0)
			size = min_t(unsigned, end, last);
	} else if (start <= last) {
		size = min_t(unsigned, last - start, length);
	}

	return size;
}

<<<<<<< HEAD
int exynos_plane_mode_set(struct drm_plane *plane, struct drm_crtc *crtc,
			  struct drm_framebuffer *fb, int crtc_x, int crtc_y,
			  unsigned int crtc_w, unsigned int crtc_h,
			  uint32_t src_x, uint32_t src_y,
			  uint32_t src_w, uint32_t src_h)
{
	struct exynos_plane *exynos_plane = to_exynos_plane(plane);
	struct exynos_drm_overlay *overlay = &exynos_plane->overlay;
	unsigned int actual_w;
	unsigned int actual_h;
	int nr;
	int i;

	nr = exynos_drm_fb_get_buf_cnt(fb);
	for (i = 0; i < nr; i++) {
		struct exynos_drm_gem_buf *buffer = exynos_drm_fb_buffer(fb, i);

		if (!buffer) {
			DRM_DEBUG_KMS("buffer is null\n");
			return -EFAULT;
		}

		overlay->dma_addr[i] = buffer->dma_addr;

		DRM_DEBUG_KMS("buffer: %d, dma_addr = 0x%lx\n",
				i, (unsigned long)overlay->dma_addr[i]);
	}

	actual_w = exynos_plane_get_size(crtc_x, crtc_w, crtc->mode.hdisplay);
	actual_h = exynos_plane_get_size(crtc_y, crtc_h, crtc->mode.vdisplay);
=======
static void exynos_plane_mode_set(struct drm_plane *plane,
				  struct drm_crtc *crtc,
				  struct drm_framebuffer *fb,
				  int crtc_x, int crtc_y,
				  unsigned int crtc_w, unsigned int crtc_h,
				  uint32_t src_x, uint32_t src_y,
				  uint32_t src_w, uint32_t src_h)
{
	struct exynos_drm_plane *exynos_plane = to_exynos_plane(plane);
	struct drm_display_mode *mode = &crtc->state->adjusted_mode;
	unsigned int actual_w;
	unsigned int actual_h;

	actual_w = exynos_plane_get_size(crtc_x, crtc_w, mode->hdisplay);
	actual_h = exynos_plane_get_size(crtc_y, crtc_h, mode->vdisplay);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	if (crtc_x < 0) {
		if (actual_w)
			src_x -= crtc_x;
		crtc_x = 0;
	}

	if (crtc_y < 0) {
		if (actual_h)
			src_y -= crtc_y;
		crtc_y = 0;
	}

<<<<<<< HEAD
	/* set drm framebuffer data. */
	overlay->fb_x = src_x;
	overlay->fb_y = src_y;
	overlay->fb_width = fb->width;
	overlay->fb_height = fb->height;
	overlay->src_width = src_w;
	overlay->src_height = src_h;
	overlay->bpp = fb->bits_per_pixel;
	overlay->pitch = fb->pitches[0];
	overlay->pixel_format = fb->pixel_format;

	/* set overlay range to be displayed. */
	overlay->crtc_x = crtc_x;
	overlay->crtc_y = crtc_y;
	overlay->crtc_width = actual_w;
	overlay->crtc_height = actual_h;

	/* set drm mode data. */
	overlay->mode_width = crtc->mode.hdisplay;
	overlay->mode_height = crtc->mode.vdisplay;
	overlay->refresh = crtc->mode.vrefresh;
	overlay->scan_flag = crtc->mode.flags;

	DRM_DEBUG_KMS("overlay : offset_x/y(%d,%d), width/height(%d,%d)",
			overlay->crtc_x, overlay->crtc_y,
			overlay->crtc_width, overlay->crtc_height);

	plane->crtc = crtc;

	exynos_drm_crtc_plane_mode_set(crtc, overlay);

	return 0;
}

void exynos_plane_commit(struct drm_plane *plane)
{
	struct exynos_plane *exynos_plane = to_exynos_plane(plane);
	struct exynos_drm_overlay *overlay = &exynos_plane->overlay;

	exynos_drm_crtc_plane_commit(plane->crtc, overlay->zpos);
}

void exynos_plane_dpms(struct drm_plane *plane, int mode)
{
	struct exynos_plane *exynos_plane = to_exynos_plane(plane);
	struct exynos_drm_overlay *overlay = &exynos_plane->overlay;

	if (mode == DRM_MODE_DPMS_ON) {
		if (exynos_plane->enabled)
			return;

		exynos_drm_crtc_plane_enable(plane->crtc, overlay->zpos);
		exynos_plane->enabled = true;
	} else {
		if (!exynos_plane->enabled)
			return;

		exynos_drm_crtc_plane_disable(plane->crtc, overlay->zpos);
		exynos_plane->enabled = false;
	}
}

static int
exynos_update_plane(struct drm_plane *plane, struct drm_crtc *crtc,
		     struct drm_framebuffer *fb, int crtc_x, int crtc_y,
		     unsigned int crtc_w, unsigned int crtc_h,
		     uint32_t src_x, uint32_t src_y,
		     uint32_t src_w, uint32_t src_h)
{
	int ret;

	ret = exynos_plane_mode_set(plane, crtc, fb, crtc_x, crtc_y,
			crtc_w, crtc_h, src_x >> 16, src_y >> 16,
			src_w >> 16, src_h >> 16);
	if (ret < 0)
		return ret;

	exynos_plane_commit(plane);
	exynos_plane_dpms(plane, DRM_MODE_DPMS_ON);
=======
	/* set ratio */
	exynos_plane->h_ratio = (src_w << 16) / crtc_w;
	exynos_plane->v_ratio = (src_h << 16) / crtc_h;

	/* set drm framebuffer data. */
	exynos_plane->src_x = src_x;
	exynos_plane->src_y = src_y;
	exynos_plane->src_w = (actual_w * exynos_plane->h_ratio) >> 16;
	exynos_plane->src_h = (actual_h * exynos_plane->v_ratio) >> 16;

	/* set plane range to be displayed. */
	exynos_plane->crtc_x = crtc_x;
	exynos_plane->crtc_y = crtc_y;
	exynos_plane->crtc_w = actual_w;
	exynos_plane->crtc_h = actual_h;

	DRM_DEBUG_KMS("plane : offset_x/y(%d,%d), width/height(%d,%d)",
			exynos_plane->crtc_x, exynos_plane->crtc_y,
			exynos_plane->crtc_w, exynos_plane->crtc_h);

	plane->crtc = crtc;
}

static struct drm_plane_funcs exynos_plane_funcs = {
	.update_plane	= drm_atomic_helper_update_plane,
	.disable_plane	= drm_atomic_helper_disable_plane,
	.destroy	= drm_plane_cleanup,
	.reset = drm_atomic_helper_plane_reset,
	.atomic_duplicate_state = drm_atomic_helper_plane_duplicate_state,
	.atomic_destroy_state = drm_atomic_helper_plane_destroy_state,
};

static int exynos_plane_atomic_check(struct drm_plane *plane,
				     struct drm_plane_state *state)
{
	struct exynos_drm_plane *exynos_plane = to_exynos_plane(plane);
	int nr;
	int i;

	if (!state->fb)
		return 0;

	nr = drm_format_num_planes(state->fb->pixel_format);
	for (i = 0; i < nr; i++) {
		struct exynos_drm_gem *exynos_gem =
					exynos_drm_fb_gem(state->fb, i);
		if (!exynos_gem) {
			DRM_DEBUG_KMS("gem object is null\n");
			return -EFAULT;
		}

		exynos_plane->dma_addr[i] = exynos_gem->dma_addr +
					    state->fb->offsets[i];

		DRM_DEBUG_KMS("buffer: %d, dma_addr = 0x%lx\n",
				i, (unsigned long)exynos_plane->dma_addr[i]);
	}
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	return 0;
}

<<<<<<< HEAD
static int exynos_disable_plane(struct drm_plane *plane)
{
	exynos_plane_dpms(plane, DRM_MODE_DPMS_OFF);

	return 0;
}

static void exynos_plane_destroy(struct drm_plane *plane)
{
	struct exynos_plane *exynos_plane = to_exynos_plane(plane);

	exynos_disable_plane(plane);
	drm_plane_cleanup(plane);
	kfree(exynos_plane);
}

static int exynos_plane_set_property(struct drm_plane *plane,
				     struct drm_property *property,
				     uint64_t val)
{
	struct drm_device *dev = plane->dev;
	struct exynos_plane *exynos_plane = to_exynos_plane(plane);
	struct exynos_drm_private *dev_priv = dev->dev_private;

	if (property == dev_priv->plane_zpos_property) {
		exynos_plane->overlay.zpos = val;
		return 0;
	}

	return -EINVAL;
}

static struct drm_plane_funcs exynos_plane_funcs = {
	.update_plane	= exynos_update_plane,
	.disable_plane	= exynos_disable_plane,
	.destroy	= exynos_plane_destroy,
	.set_property	= exynos_plane_set_property,
};

static void exynos_plane_attach_zpos_property(struct drm_plane *plane)
=======
static void exynos_plane_atomic_update(struct drm_plane *plane,
				       struct drm_plane_state *old_state)
{
	struct drm_plane_state *state = plane->state;
	struct exynos_drm_crtc *exynos_crtc = to_exynos_crtc(state->crtc);
	struct exynos_drm_plane *exynos_plane = to_exynos_plane(plane);

	if (!state->crtc)
		return;

	exynos_plane_mode_set(plane, state->crtc, state->fb,
			      state->crtc_x, state->crtc_y,
			      state->crtc_w, state->crtc_h,
			      state->src_x >> 16, state->src_y >> 16,
			      state->src_w >> 16, state->src_h >> 16);

	exynos_plane->pending_fb = state->fb;

	if (exynos_crtc->ops->update_plane)
		exynos_crtc->ops->update_plane(exynos_crtc, exynos_plane);
}

static void exynos_plane_atomic_disable(struct drm_plane *plane,
					struct drm_plane_state *old_state)
{
	struct exynos_drm_plane *exynos_plane = to_exynos_plane(plane);
	struct exynos_drm_crtc *exynos_crtc = to_exynos_crtc(old_state->crtc);

	if (!old_state->crtc)
		return;

	if (exynos_crtc->ops->disable_plane)
		exynos_crtc->ops->disable_plane(exynos_crtc,
						exynos_plane);
}

static const struct drm_plane_helper_funcs plane_helper_funcs = {
	.atomic_check = exynos_plane_atomic_check,
	.atomic_update = exynos_plane_atomic_update,
	.atomic_disable = exynos_plane_atomic_disable,
};

static void exynos_plane_attach_zpos_property(struct drm_plane *plane,
					      unsigned int zpos)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
{
	struct drm_device *dev = plane->dev;
	struct exynos_drm_private *dev_priv = dev->dev_private;
	struct drm_property *prop;

	prop = dev_priv->plane_zpos_property;
	if (!prop) {
<<<<<<< HEAD
		prop = drm_property_create_range(dev, 0, "zpos", 0,
						 MAX_PLANE - 1);
=======
		prop = drm_property_create_range(dev, DRM_MODE_PROP_IMMUTABLE,
						 "zpos", 0, MAX_PLANE - 1);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		if (!prop)
			return;

		dev_priv->plane_zpos_property = prop;
	}

<<<<<<< HEAD
	drm_object_attach_property(&plane->base, prop, 0);
}

struct drm_plane *exynos_plane_init(struct drm_device *dev,
				    unsigned long possible_crtcs,
				    enum drm_plane_type type)
{
	struct exynos_plane *exynos_plane;
	int err;

	exynos_plane = kzalloc(sizeof(struct exynos_plane), GFP_KERNEL);
	if (!exynos_plane)
		return ERR_PTR(-ENOMEM);

	err = drm_universal_plane_init(dev, &exynos_plane->base, possible_crtcs,
				       &exynos_plane_funcs, formats,
				       ARRAY_SIZE(formats), type);
	if (err) {
		DRM_ERROR("failed to initialize plane\n");
		kfree(exynos_plane);
		return ERR_PTR(err);
	}

	if (type == DRM_PLANE_TYPE_PRIMARY)
		exynos_plane->overlay.zpos = DEFAULT_ZPOS;
	else
		exynos_plane_attach_zpos_property(&exynos_plane->base);

	return &exynos_plane->base;
=======
	drm_object_attach_property(&plane->base, prop, zpos);
}

enum drm_plane_type exynos_plane_get_type(unsigned int zpos,
					  unsigned int cursor_win)
{
		if (zpos == DEFAULT_WIN)
			return DRM_PLANE_TYPE_PRIMARY;
		else if (zpos == cursor_win)
			return DRM_PLANE_TYPE_CURSOR;
		else
			return DRM_PLANE_TYPE_OVERLAY;
}

int exynos_plane_init(struct drm_device *dev,
		      struct exynos_drm_plane *exynos_plane,
		      unsigned long possible_crtcs, enum drm_plane_type type,
		      const uint32_t *formats, unsigned int fcount,
		      unsigned int zpos)
{
	int err;

	err = drm_universal_plane_init(dev, &exynos_plane->base, possible_crtcs,
				       &exynos_plane_funcs, formats, fcount,
				       type);
	if (err) {
		DRM_ERROR("failed to initialize plane\n");
		return err;
	}

	drm_plane_helper_add(&exynos_plane->base, &plane_helper_funcs);

	exynos_plane->zpos = zpos;

	if (type == DRM_PLANE_TYPE_OVERLAY)
		exynos_plane_attach_zpos_property(&exynos_plane->base, zpos);

	return 0;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}
