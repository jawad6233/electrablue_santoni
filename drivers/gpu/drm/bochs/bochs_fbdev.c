/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "bochs.h"

/* ---------------------------------------------------------------------- */

<<<<<<< HEAD
=======
static int bochsfb_mmap(struct fb_info *info,
			struct vm_area_struct *vma)
{
	struct drm_fb_helper *fb_helper = info->par;
	struct bochs_device *bochs =
		container_of(fb_helper, struct bochs_device, fb.helper);
	struct bochs_bo *bo = gem_to_bochs_bo(bochs->fb.gfb.obj);

	return ttm_fbdev_mmap(vma, &bo->bo);
}

>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
static struct fb_ops bochsfb_ops = {
	.owner = THIS_MODULE,
	.fb_check_var = drm_fb_helper_check_var,
	.fb_set_par = drm_fb_helper_set_par,
<<<<<<< HEAD
	.fb_fillrect = sys_fillrect,
	.fb_copyarea = sys_copyarea,
	.fb_imageblit = sys_imageblit,
	.fb_pan_display = drm_fb_helper_pan_display,
	.fb_blank = drm_fb_helper_blank,
	.fb_setcmap = drm_fb_helper_setcmap,
=======
	.fb_fillrect = drm_fb_helper_sys_fillrect,
	.fb_copyarea = drm_fb_helper_sys_copyarea,
	.fb_imageblit = drm_fb_helper_sys_imageblit,
	.fb_pan_display = drm_fb_helper_pan_display,
	.fb_blank = drm_fb_helper_blank,
	.fb_setcmap = drm_fb_helper_setcmap,
	.fb_mmap = bochsfb_mmap,
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
};

static int bochsfb_create_object(struct bochs_device *bochs,
				 struct drm_mode_fb_cmd2 *mode_cmd,
				 struct drm_gem_object **gobj_p)
{
	struct drm_device *dev = bochs->dev;
	struct drm_gem_object *gobj;
	u32 size;
	int ret = 0;

	size = mode_cmd->pitches[0] * mode_cmd->height;
	ret = bochs_gem_create(dev, size, true, &gobj);
	if (ret)
		return ret;

	*gobj_p = gobj;
	return ret;
}

static int bochsfb_create(struct drm_fb_helper *helper,
			  struct drm_fb_helper_surface_size *sizes)
{
	struct bochs_device *bochs =
		container_of(helper, struct bochs_device, fb.helper);
<<<<<<< HEAD
	struct drm_device *dev = bochs->dev;
	struct fb_info *info;
	struct drm_framebuffer *fb;
	struct drm_mode_fb_cmd2 mode_cmd;
	struct device *device = &dev->pdev->dev;
=======
	struct fb_info *info;
	struct drm_framebuffer *fb;
	struct drm_mode_fb_cmd2 mode_cmd;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	struct drm_gem_object *gobj = NULL;
	struct bochs_bo *bo = NULL;
	int size, ret;

	if (sizes->surface_bpp != 32)
		return -EINVAL;

	mode_cmd.width = sizes->surface_width;
	mode_cmd.height = sizes->surface_height;
	mode_cmd.pitches[0] = mode_cmd.width * ((sizes->surface_bpp + 7) / 8);
	mode_cmd.pixel_format = drm_mode_legacy_fb_format(sizes->surface_bpp,
							  sizes->surface_depth);
	size = mode_cmd.pitches[0] * mode_cmd.height;

	/* alloc, pin & map bo */
	ret = bochsfb_create_object(bochs, &mode_cmd, &gobj);
	if (ret) {
		DRM_ERROR("failed to create fbcon backing object %d\n", ret);
		return ret;
	}

	bo = gem_to_bochs_bo(gobj);

	ret = ttm_bo_reserve(&bo->bo, true, false, false, NULL);
	if (ret)
		return ret;

	ret = bochs_bo_pin(bo, TTM_PL_FLAG_VRAM, NULL);
	if (ret) {
		DRM_ERROR("failed to pin fbcon\n");
		ttm_bo_unreserve(&bo->bo);
		return ret;
	}

	ret = ttm_bo_kmap(&bo->bo, 0, bo->bo.num_pages,
			  &bo->kmap);
	if (ret) {
		DRM_ERROR("failed to kmap fbcon\n");
		ttm_bo_unreserve(&bo->bo);
		return ret;
	}

	ttm_bo_unreserve(&bo->bo);

	/* init fb device */
<<<<<<< HEAD
	info = framebuffer_alloc(0, device);
	if (info == NULL)
		return -ENOMEM;
=======
	info = drm_fb_helper_alloc_fbi(helper);
	if (IS_ERR(info))
		return PTR_ERR(info);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	info->par = &bochs->fb.helper;

	ret = bochs_framebuffer_init(bochs->dev, &bochs->fb.gfb, &mode_cmd, gobj);
<<<<<<< HEAD
	if (ret)
		return ret;
=======
	if (ret) {
		drm_fb_helper_release_fbi(helper);
		return ret;
	}
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	bochs->fb.size = size;

	/* setup helper */
	fb = &bochs->fb.gfb.base;
	bochs->fb.helper.fb = fb;
<<<<<<< HEAD
	bochs->fb.helper.fbdev = info;
=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	strcpy(info->fix.id, "bochsdrmfb");

	info->flags = FBINFO_DEFAULT;
	info->fbops = &bochsfb_ops;

	drm_fb_helper_fill_fix(info, fb->pitches[0], fb->depth);
	drm_fb_helper_fill_var(info, &bochs->fb.helper, sizes->fb_width,
			       sizes->fb_height);

	info->screen_base = bo->kmap.virtual;
	info->screen_size = size;

<<<<<<< HEAD
#if 0
	/* FIXME: get this right for mmap(/dev/fb0) */
	info->fix.smem_start = bochs_bo_mmap_offset(bo);
	info->fix.smem_len = size;
#endif

	ret = fb_alloc_cmap(&info->cmap, 256, 0);
	if (ret) {
		DRM_ERROR("%s: can't allocate color map\n", info->fix.id);
		return -ENOMEM;
	}
=======
	drm_vma_offset_remove(&bo->bo.bdev->vma_manager, &bo->bo.vma_node);
	info->fix.smem_start = 0;
	info->fix.smem_len = size;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	return 0;
}

static int bochs_fbdev_destroy(struct bochs_device *bochs)
{
	struct bochs_framebuffer *gfb = &bochs->fb.gfb;
<<<<<<< HEAD
	struct fb_info *info;

	DRM_DEBUG_DRIVER("\n");

	if (bochs->fb.helper.fbdev) {
		info = bochs->fb.helper.fbdev;

		unregister_framebuffer(info);
		if (info->cmap.len)
			fb_dealloc_cmap(&info->cmap);
		framebuffer_release(info);
	}
=======

	DRM_DEBUG_DRIVER("\n");

	drm_fb_helper_unregister_fbi(&bochs->fb.helper);
	drm_fb_helper_release_fbi(&bochs->fb.helper);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	if (gfb->obj) {
		drm_gem_object_unreference_unlocked(gfb->obj);
		gfb->obj = NULL;
	}

	drm_fb_helper_fini(&bochs->fb.helper);
	drm_framebuffer_unregister_private(&gfb->base);
	drm_framebuffer_cleanup(&gfb->base);

	return 0;
}

void bochs_fb_gamma_set(struct drm_crtc *crtc, u16 red, u16 green,
			u16 blue, int regno)
{
}

void bochs_fb_gamma_get(struct drm_crtc *crtc, u16 *red, u16 *green,
			u16 *blue, int regno)
{
	*red   = regno;
	*green = regno;
	*blue  = regno;
}

static const struct drm_fb_helper_funcs bochs_fb_helper_funcs = {
	.gamma_set = bochs_fb_gamma_set,
	.gamma_get = bochs_fb_gamma_get,
	.fb_probe = bochsfb_create,
};

int bochs_fbdev_init(struct bochs_device *bochs)
{
	int ret;

	drm_fb_helper_prepare(bochs->dev, &bochs->fb.helper,
			      &bochs_fb_helper_funcs);

	ret = drm_fb_helper_init(bochs->dev, &bochs->fb.helper,
				 1, 1);
	if (ret)
		return ret;

	ret = drm_fb_helper_single_add_all_connectors(&bochs->fb.helper);
	if (ret)
		goto fini;

	drm_helper_disable_unused_functions(bochs->dev);

	ret = drm_fb_helper_initial_config(&bochs->fb.helper, 32);
	if (ret)
		goto fini;

	bochs->fb.initialized = true;
	return 0;

fini:
	drm_fb_helper_fini(&bochs->fb.helper);
	return ret;
}

void bochs_fbdev_fini(struct bochs_device *bochs)
{
	if (!bochs->fb.initialized)
		return;

	bochs_fbdev_destroy(bochs);
	bochs->fb.initialized = false;
}
