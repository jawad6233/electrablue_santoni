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

<<<<<<< HEAD
int exynos_plane_mode_set(struct drm_plane *plane, struct drm_crtc *crtc,
			  struct drm_framebuffer *fb, int crtc_x, int crtc_y,
			  unsigned int crtc_w, unsigned int crtc_h,
			  uint32_t src_x, uint32_t src_y,
			  uint32_t src_w, uint32_t src_h);
void exynos_plane_commit(struct drm_plane *plane);
void exynos_plane_dpms(struct drm_plane *plane, int mode);
struct drm_plane *exynos_plane_init(struct drm_device *dev,
				    unsigned long possible_crtcs,
				    enum drm_plane_type type);
=======
enum drm_plane_type exynos_plane_get_type(unsigned int zpos,
					  unsigned int cursor_win);
int exynos_plane_init(struct drm_device *dev,
		      struct exynos_drm_plane *exynos_plane,
		      unsigned long possible_crtcs, enum drm_plane_type type,
		      const uint32_t *formats, unsigned int fcount,
		      unsigned int zpos);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
