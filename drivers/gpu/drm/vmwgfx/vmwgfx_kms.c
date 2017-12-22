/**************************************************************************
 *
<<<<<<< HEAD
 * Copyright © 2009 VMware, Inc., Palo Alto, CA., USA
=======
 * Copyright © 2009-2015 VMware, Inc., Palo Alto, CA., USA
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS, AUTHORS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/

#include "vmwgfx_kms.h"


/* Might need a hrtimer here? */
#define VMWGFX_PRESENT_RATE ((HZ / 60 > 0) ? HZ / 60 : 1)

<<<<<<< HEAD

struct vmw_clip_rect {
	int x1, x2, y1, y2;
};

/**
 * Clip @num_rects number of @rects against @clip storing the
 * results in @out_rects and the number of passed rects in @out_num.
 */
static void vmw_clip_cliprects(struct drm_clip_rect *rects,
			int num_rects,
			struct vmw_clip_rect clip,
			SVGASignedRect *out_rects,
			int *out_num)
{
	int i, k;

	for (i = 0, k = 0; i < num_rects; i++) {
		int x1 = max_t(int, clip.x1, rects[i].x1);
		int y1 = max_t(int, clip.y1, rects[i].y1);
		int x2 = min_t(int, clip.x2, rects[i].x2);
		int y2 = min_t(int, clip.y2, rects[i].y2);

		if (x1 >= x2)
			continue;
		if (y1 >= y2)
			continue;

		out_rects[k].left   = x1;
		out_rects[k].top    = y1;
		out_rects[k].right  = x2;
		out_rects[k].bottom = y2;
		k++;
	}

	*out_num = k;
}

void vmw_display_unit_cleanup(struct vmw_display_unit *du)
=======
void vmw_du_cleanup(struct vmw_display_unit *du)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
{
	if (du->cursor_surface)
		vmw_surface_unreference(&du->cursor_surface);
	if (du->cursor_dmabuf)
		vmw_dmabuf_unreference(&du->cursor_dmabuf);
	drm_connector_unregister(&du->connector);
	drm_crtc_cleanup(&du->crtc);
	drm_encoder_cleanup(&du->encoder);
	drm_connector_cleanup(&du->connector);
}

/*
 * Display Unit Cursor functions
 */

int vmw_cursor_update_image(struct vmw_private *dev_priv,
			    u32 *image, u32 width, u32 height,
			    u32 hotspotX, u32 hotspotY)
{
	struct {
		u32 cmd;
		SVGAFifoCmdDefineAlphaCursor cursor;
	} *cmd;
	u32 image_size = width * height * 4;
	u32 cmd_size = sizeof(*cmd) + image_size;

	if (!image)
		return -EINVAL;

	cmd = vmw_fifo_reserve(dev_priv, cmd_size);
	if (unlikely(cmd == NULL)) {
		DRM_ERROR("Fifo reserve failed.\n");
		return -ENOMEM;
	}

	memset(cmd, 0, sizeof(*cmd));

	memcpy(&cmd[1], image, image_size);

<<<<<<< HEAD
	cmd->cmd = cpu_to_le32(SVGA_CMD_DEFINE_ALPHA_CURSOR);
	cmd->cursor.id = cpu_to_le32(0);
	cmd->cursor.width = cpu_to_le32(width);
	cmd->cursor.height = cpu_to_le32(height);
	cmd->cursor.hotspotX = cpu_to_le32(hotspotX);
	cmd->cursor.hotspotY = cpu_to_le32(hotspotY);

	vmw_fifo_commit(dev_priv, cmd_size);
=======
	cmd->cmd = SVGA_CMD_DEFINE_ALPHA_CURSOR;
	cmd->cursor.id = 0;
	cmd->cursor.width = width;
	cmd->cursor.height = height;
	cmd->cursor.hotspotX = hotspotX;
	cmd->cursor.hotspotY = hotspotY;

	vmw_fifo_commit_flush(dev_priv, cmd_size);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	return 0;
}

int vmw_cursor_update_dmabuf(struct vmw_private *dev_priv,
			     struct vmw_dma_buffer *dmabuf,
			     u32 width, u32 height,
			     u32 hotspotX, u32 hotspotY)
{
	struct ttm_bo_kmap_obj map;
	unsigned long kmap_offset;
	unsigned long kmap_num;
	void *virtual;
	bool dummy;
	int ret;

	kmap_offset = 0;
	kmap_num = (width*height*4 + PAGE_SIZE - 1) >> PAGE_SHIFT;

	ret = ttm_bo_reserve(&dmabuf->base, true, false, false, NULL);
	if (unlikely(ret != 0)) {
		DRM_ERROR("reserve failed\n");
		return -EINVAL;
	}

	ret = ttm_bo_kmap(&dmabuf->base, kmap_offset, kmap_num, &map);
	if (unlikely(ret != 0))
		goto err_unreserve;

	virtual = ttm_kmap_obj_virtual(&map, &dummy);
	ret = vmw_cursor_update_image(dev_priv, virtual, width, height,
				      hotspotX, hotspotY);

	ttm_bo_kunmap(&map);
err_unreserve:
	ttm_bo_unreserve(&dmabuf->base);

	return ret;
}


void vmw_cursor_update_position(struct vmw_private *dev_priv,
				bool show, int x, int y)
{
<<<<<<< HEAD
	__le32 __iomem *fifo_mem = dev_priv->mmio_virt;
	uint32_t count;

	iowrite32(show ? 1 : 0, fifo_mem + SVGA_FIFO_CURSOR_ON);
	iowrite32(x, fifo_mem + SVGA_FIFO_CURSOR_X);
	iowrite32(y, fifo_mem + SVGA_FIFO_CURSOR_Y);
	count = ioread32(fifo_mem + SVGA_FIFO_CURSOR_COUNT);
	iowrite32(++count, fifo_mem + SVGA_FIFO_CURSOR_COUNT);
}

int vmw_du_crtc_cursor_set(struct drm_crtc *crtc, struct drm_file *file_priv,
			   uint32_t handle, uint32_t width, uint32_t height)
=======
	u32 *fifo_mem = dev_priv->mmio_virt;
	uint32_t count;

	vmw_mmio_write(show ? 1 : 0, fifo_mem + SVGA_FIFO_CURSOR_ON);
	vmw_mmio_write(x, fifo_mem + SVGA_FIFO_CURSOR_X);
	vmw_mmio_write(y, fifo_mem + SVGA_FIFO_CURSOR_Y);
	count = vmw_mmio_read(fifo_mem + SVGA_FIFO_CURSOR_COUNT);
	vmw_mmio_write(++count, fifo_mem + SVGA_FIFO_CURSOR_COUNT);
}


/*
 * vmw_du_crtc_cursor_set2 - Driver cursor_set2 callback.
 */
int vmw_du_crtc_cursor_set2(struct drm_crtc *crtc, struct drm_file *file_priv,
			    uint32_t handle, uint32_t width, uint32_t height,
			    int32_t hot_x, int32_t hot_y)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
{
	struct vmw_private *dev_priv = vmw_priv(crtc->dev);
	struct vmw_display_unit *du = vmw_crtc_to_du(crtc);
	struct vmw_surface *surface = NULL;
	struct vmw_dma_buffer *dmabuf = NULL;
<<<<<<< HEAD
=======
	s32 hotspot_x, hotspot_y;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	int ret;

	/*
	 * FIXME: Unclear whether there's any global state touched by the
	 * cursor_set function, especially vmw_cursor_update_position looks
	 * suspicious. For now take the easy route and reacquire all locks. We
	 * can do this since the caller in the drm core doesn't check anything
	 * which is protected by any looks.
	 */
	drm_modeset_unlock_crtc(crtc);
	drm_modeset_lock_all(dev_priv->dev);
<<<<<<< HEAD
=======
	hotspot_x = hot_x + du->hotspot_x;
	hotspot_y = hot_y + du->hotspot_y;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	/* A lot of the code assumes this */
	if (handle && (width != 64 || height != 64)) {
		ret = -EINVAL;
		goto out;
	}

	if (handle) {
		struct ttm_object_file *tfile = vmw_fpriv(file_priv)->tfile;

		ret = vmw_user_lookup_handle(dev_priv, tfile,
					     handle, &surface, &dmabuf);
		if (ret) {
			DRM_ERROR("failed to find surface or dmabuf: %i\n", ret);
			ret = -EINVAL;
			goto out;
		}
	}

	/* need to do this before taking down old image */
	if (surface && !surface->snooper.image) {
		DRM_ERROR("surface not suitable for cursor\n");
		vmw_surface_unreference(&surface);
		ret = -EINVAL;
		goto out;
	}

	/* takedown old cursor */
	if (du->cursor_surface) {
		du->cursor_surface->snooper.crtc = NULL;
		vmw_surface_unreference(&du->cursor_surface);
	}
	if (du->cursor_dmabuf)
		vmw_dmabuf_unreference(&du->cursor_dmabuf);

	/* setup new image */
<<<<<<< HEAD
=======
	ret = 0;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	if (surface) {
		/* vmw_user_surface_lookup takes one reference */
		du->cursor_surface = surface;

		du->cursor_surface->snooper.crtc = crtc;
		du->cursor_age = du->cursor_surface->snooper.age;
<<<<<<< HEAD
		vmw_cursor_update_image(dev_priv, surface->snooper.image,
					64, 64, du->hotspot_x, du->hotspot_y);
=======
		ret = vmw_cursor_update_image(dev_priv, surface->snooper.image,
					      64, 64, hotspot_x, hotspot_y);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	} else if (dmabuf) {
		/* vmw_user_surface_lookup takes one reference */
		du->cursor_dmabuf = dmabuf;

		ret = vmw_cursor_update_dmabuf(dev_priv, dmabuf, width, height,
<<<<<<< HEAD
					       du->hotspot_x, du->hotspot_y);
	} else {
		vmw_cursor_update_position(dev_priv, false, 0, 0);
		ret = 0;
		goto out;
	}

	vmw_cursor_update_position(dev_priv, true,
				   du->cursor_x + du->hotspot_x,
				   du->cursor_y + du->hotspot_y);

	ret = 0;
out:
	drm_modeset_unlock_all(dev_priv->dev);
	drm_modeset_lock_crtc(crtc);
=======
					       hotspot_x, hotspot_y);
	} else {
		vmw_cursor_update_position(dev_priv, false, 0, 0);
		goto out;
	}

	if (!ret) {
		vmw_cursor_update_position(dev_priv, true,
					   du->cursor_x + hotspot_x,
					   du->cursor_y + hotspot_y);
		du->core_hotspot_x = hot_x;
		du->core_hotspot_y = hot_y;
	}

out:
	drm_modeset_unlock_all(dev_priv->dev);
	drm_modeset_lock_crtc(crtc, crtc->cursor);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	return ret;
}

int vmw_du_crtc_cursor_move(struct drm_crtc *crtc, int x, int y)
{
	struct vmw_private *dev_priv = vmw_priv(crtc->dev);
	struct vmw_display_unit *du = vmw_crtc_to_du(crtc);
	bool shown = du->cursor_surface || du->cursor_dmabuf ? true : false;

	du->cursor_x = x + crtc->x;
	du->cursor_y = y + crtc->y;

	/*
	 * FIXME: Unclear whether there's any global state touched by the
	 * cursor_set function, especially vmw_cursor_update_position looks
	 * suspicious. For now take the easy route and reacquire all locks. We
	 * can do this since the caller in the drm core doesn't check anything
	 * which is protected by any looks.
	 */
	drm_modeset_unlock_crtc(crtc);
	drm_modeset_lock_all(dev_priv->dev);

	vmw_cursor_update_position(dev_priv, shown,
<<<<<<< HEAD
				   du->cursor_x + du->hotspot_x,
				   du->cursor_y + du->hotspot_y);

	drm_modeset_unlock_all(dev_priv->dev);
	drm_modeset_lock_crtc(crtc);
=======
				   du->cursor_x + du->hotspot_x +
				   du->core_hotspot_x,
				   du->cursor_y + du->hotspot_y +
				   du->core_hotspot_y);

	drm_modeset_unlock_all(dev_priv->dev);
	drm_modeset_lock_crtc(crtc, crtc->cursor);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	return 0;
}

void vmw_kms_cursor_snoop(struct vmw_surface *srf,
			  struct ttm_object_file *tfile,
			  struct ttm_buffer_object *bo,
			  SVGA3dCmdHeader *header)
{
	struct ttm_bo_kmap_obj map;
	unsigned long kmap_offset;
	unsigned long kmap_num;
	SVGA3dCopyBox *box;
	unsigned box_count;
	void *virtual;
	bool dummy;
	struct vmw_dma_cmd {
		SVGA3dCmdHeader header;
		SVGA3dCmdSurfaceDMA dma;
	} *cmd;
	int i, ret;

	cmd = container_of(header, struct vmw_dma_cmd, header);

	/* No snooper installed */
	if (!srf->snooper.image)
		return;

	if (cmd->dma.host.face != 0 || cmd->dma.host.mipmap != 0) {
		DRM_ERROR("face and mipmap for cursors should never != 0\n");
		return;
	}

	if (cmd->header.size < 64) {
		DRM_ERROR("at least one full copy box must be given\n");
		return;
	}

	box = (SVGA3dCopyBox *)&cmd[1];
	box_count = (cmd->header.size - sizeof(SVGA3dCmdSurfaceDMA)) /
			sizeof(SVGA3dCopyBox);

	if (cmd->dma.guest.ptr.offset % PAGE_SIZE ||
	    box->x != 0    || box->y != 0    || box->z != 0    ||
	    box->srcx != 0 || box->srcy != 0 || box->srcz != 0 ||
	    box->d != 1    || box_count != 1) {
		/* TODO handle none page aligned offsets */
		/* TODO handle more dst & src != 0 */
		/* TODO handle more then one copy */
		DRM_ERROR("Cant snoop dma request for cursor!\n");
		DRM_ERROR("(%u, %u, %u) (%u, %u, %u) (%ux%ux%u) %u %u\n",
			  box->srcx, box->srcy, box->srcz,
			  box->x, box->y, box->z,
			  box->w, box->h, box->d, box_count,
			  cmd->dma.guest.ptr.offset);
		return;
	}

	kmap_offset = cmd->dma.guest.ptr.offset >> PAGE_SHIFT;
	kmap_num = (64*64*4) >> PAGE_SHIFT;

	ret = ttm_bo_reserve(bo, true, false, false, NULL);
	if (unlikely(ret != 0)) {
		DRM_ERROR("reserve failed\n");
		return;
	}

	ret = ttm_bo_kmap(bo, kmap_offset, kmap_num, &map);
	if (unlikely(ret != 0))
		goto err_unreserve;

	virtual = ttm_kmap_obj_virtual(&map, &dummy);

	if (box->w == 64 && cmd->dma.guest.pitch == 64*4) {
		memcpy(srf->snooper.image, virtual, 64*64*4);
	} else {
		/* Image is unsigned pointer. */
		for (i = 0; i < box->h; i++)
			memcpy(srf->snooper.image + i * 64,
			       virtual + i * cmd->dma.guest.pitch,
			       box->w * 4);
	}

	srf->snooper.age++;

<<<<<<< HEAD
	/* we can't call this function from this function since execbuf has
	 * reserved fifo space.
	 *
	 * if (srf->snooper.crtc)
	 *	vmw_ldu_crtc_cursor_update_image(dev_priv,
	 *					 srf->snooper.image, 64, 64,
	 *					 du->hotspot_x, du->hotspot_y);
	 */

=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	ttm_bo_kunmap(&map);
err_unreserve:
	ttm_bo_unreserve(bo);
}

<<<<<<< HEAD
=======
/**
 * vmw_kms_legacy_hotspot_clear - Clear legacy hotspots
 *
 * @dev_priv: Pointer to the device private struct.
 *
 * Clears all legacy hotspots.
 */
void vmw_kms_legacy_hotspot_clear(struct vmw_private *dev_priv)
{
	struct drm_device *dev = dev_priv->dev;
	struct vmw_display_unit *du;
	struct drm_crtc *crtc;

	drm_modeset_lock_all(dev);
	drm_for_each_crtc(crtc, dev) {
		du = vmw_crtc_to_du(crtc);

		du->hotspot_x = 0;
		du->hotspot_y = 0;
	}
	drm_modeset_unlock_all(dev);
}

>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
void vmw_kms_cursor_post_execbuf(struct vmw_private *dev_priv)
{
	struct drm_device *dev = dev_priv->dev;
	struct vmw_display_unit *du;
	struct drm_crtc *crtc;

	mutex_lock(&dev->mode_config.mutex);

	list_for_each_entry(crtc, &dev->mode_config.crtc_list, head) {
		du = vmw_crtc_to_du(crtc);
		if (!du->cursor_surface ||
		    du->cursor_age == du->cursor_surface->snooper.age)
			continue;

		du->cursor_age = du->cursor_surface->snooper.age;
		vmw_cursor_update_image(dev_priv,
					du->cursor_surface->snooper.image,
<<<<<<< HEAD
					64, 64, du->hotspot_x, du->hotspot_y);
=======
					64, 64,
					du->hotspot_x + du->core_hotspot_x,
					du->hotspot_y + du->core_hotspot_y);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	}

	mutex_unlock(&dev->mode_config.mutex);
}

/*
 * Generic framebuffer code
 */

/*
 * Surface framebuffer code
 */

<<<<<<< HEAD
#define vmw_framebuffer_to_vfbs(x) \
	container_of(x, struct vmw_framebuffer_surface, base.base)

struct vmw_framebuffer_surface {
	struct vmw_framebuffer base;
	struct vmw_surface *surface;
	struct vmw_dma_buffer *buffer;
	struct list_head head;
	struct drm_master *master;
};

=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
static void vmw_framebuffer_surface_destroy(struct drm_framebuffer *framebuffer)
{
	struct vmw_framebuffer_surface *vfbs =
		vmw_framebuffer_to_vfbs(framebuffer);
<<<<<<< HEAD
	struct vmw_master *vmaster = vmw_master(vfbs->master);


	mutex_lock(&vmaster->fb_surf_mutex);
	list_del(&vfbs->head);
	mutex_unlock(&vmaster->fb_surf_mutex);

	drm_master_put(&vfbs->master);
	drm_framebuffer_cleanup(framebuffer);
	vmw_surface_unreference(&vfbs->surface);
	ttm_base_object_unref(&vfbs->base.user_obj);
=======

	drm_framebuffer_cleanup(framebuffer);
	vmw_surface_unreference(&vfbs->surface);
	if (vfbs->base.user_obj)
		ttm_base_object_unref(&vfbs->base.user_obj);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	kfree(vfbs);
}

<<<<<<< HEAD
static int do_surface_dirty_sou(struct vmw_private *dev_priv,
				struct drm_file *file_priv,
				struct vmw_framebuffer *framebuffer,
				unsigned flags, unsigned color,
				struct drm_clip_rect *clips,
				unsigned num_clips, int inc,
				struct vmw_fence_obj **out_fence)
{
	struct vmw_display_unit *units[VMWGFX_NUM_DISPLAY_UNITS];
	struct drm_clip_rect *clips_ptr;
	struct drm_clip_rect *tmp;
	struct drm_crtc *crtc;
	size_t fifo_size;
	int i, num_units;
	int ret = 0; /* silence warning */
	int left, right, top, bottom;

	struct {
		SVGA3dCmdHeader header;
		SVGA3dCmdBlitSurfaceToScreen body;
	} *cmd;
	SVGASignedRect *blits;

	num_units = 0;
	list_for_each_entry(crtc, &dev_priv->dev->mode_config.crtc_list,
			    head) {
		if (crtc->primary->fb != &framebuffer->base)
			continue;
		units[num_units++] = vmw_crtc_to_du(crtc);
	}

	BUG_ON(!clips || !num_clips);

	tmp = kzalloc(sizeof(*tmp) * num_clips, GFP_KERNEL);
	if (unlikely(tmp == NULL)) {
		DRM_ERROR("Temporary cliprect memory alloc failed.\n");
		return -ENOMEM;
	}

	fifo_size = sizeof(*cmd) + sizeof(SVGASignedRect) * num_clips;
	cmd = kzalloc(fifo_size, GFP_KERNEL);
	if (unlikely(cmd == NULL)) {
		DRM_ERROR("Temporary fifo memory alloc failed.\n");
		ret = -ENOMEM;
		goto out_free_tmp;
	}

	/* setup blits pointer */
	blits = (SVGASignedRect *)&cmd[1];

	/* initial clip region */
	left = clips->x1;
	right = clips->x2;
	top = clips->y1;
	bottom = clips->y2;

	/* skip the first clip rect */
	for (i = 1, clips_ptr = clips + inc;
	     i < num_clips; i++, clips_ptr += inc) {
		left = min_t(int, left, (int)clips_ptr->x1);
		right = max_t(int, right, (int)clips_ptr->x2);
		top = min_t(int, top, (int)clips_ptr->y1);
		bottom = max_t(int, bottom, (int)clips_ptr->y2);
	}

	/* only need to do this once */
	cmd->header.id = cpu_to_le32(SVGA_3D_CMD_BLIT_SURFACE_TO_SCREEN);
	cmd->header.size = cpu_to_le32(fifo_size - sizeof(cmd->header));

	cmd->body.srcRect.left = left;
	cmd->body.srcRect.right = right;
	cmd->body.srcRect.top = top;
	cmd->body.srcRect.bottom = bottom;

	clips_ptr = clips;
	for (i = 0; i < num_clips; i++, clips_ptr += inc) {
		tmp[i].x1 = clips_ptr->x1 - left;
		tmp[i].x2 = clips_ptr->x2 - left;
		tmp[i].y1 = clips_ptr->y1 - top;
		tmp[i].y2 = clips_ptr->y2 - top;
	}

	/* do per unit writing, reuse fifo for each */
	for (i = 0; i < num_units; i++) {
		struct vmw_display_unit *unit = units[i];
		struct vmw_clip_rect clip;
		int num;

		clip.x1 = left - unit->crtc.x;
		clip.y1 = top - unit->crtc.y;
		clip.x2 = right - unit->crtc.x;
		clip.y2 = bottom - unit->crtc.y;

		/* skip any crtcs that misses the clip region */
		if (clip.x1 >= unit->crtc.mode.hdisplay ||
		    clip.y1 >= unit->crtc.mode.vdisplay ||
		    clip.x2 <= 0 || clip.y2 <= 0)
			continue;

		/*
		 * In order for the clip rects to be correctly scaled
		 * the src and dest rects needs to be the same size.
		 */
		cmd->body.destRect.left = clip.x1;
		cmd->body.destRect.right = clip.x2;
		cmd->body.destRect.top = clip.y1;
		cmd->body.destRect.bottom = clip.y2;

		/* create a clip rect of the crtc in dest coords */
		clip.x2 = unit->crtc.mode.hdisplay - clip.x1;
		clip.y2 = unit->crtc.mode.vdisplay - clip.y1;
		clip.x1 = 0 - clip.x1;
		clip.y1 = 0 - clip.y1;

		/* need to reset sid as it is changed by execbuf */
		cmd->body.srcImage.sid = cpu_to_le32(framebuffer->user_handle);
		cmd->body.destScreenId = unit->unit;

		/* clip and write blits to cmd stream */
		vmw_clip_cliprects(tmp, num_clips, clip, blits, &num);

		/* if no cliprects hit skip this */
		if (num == 0)
			continue;

		/* only return the last fence */
		if (out_fence && *out_fence)
			vmw_fence_obj_unreference(out_fence);

		/* recalculate package length */
		fifo_size = sizeof(*cmd) + sizeof(SVGASignedRect) * num;
		cmd->header.size = cpu_to_le32(fifo_size - sizeof(cmd->header));
		ret = vmw_execbuf_process(file_priv, dev_priv, NULL, cmd,
					  fifo_size, 0, NULL, out_fence);

		if (unlikely(ret != 0))
			break;
	}


	kfree(cmd);
out_free_tmp:
	kfree(tmp);

	return ret;
}

=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
static int vmw_framebuffer_surface_dirty(struct drm_framebuffer *framebuffer,
				  struct drm_file *file_priv,
				  unsigned flags, unsigned color,
				  struct drm_clip_rect *clips,
				  unsigned num_clips)
{
	struct vmw_private *dev_priv = vmw_priv(framebuffer->dev);
	struct vmw_framebuffer_surface *vfbs =
		vmw_framebuffer_to_vfbs(framebuffer);
	struct drm_clip_rect norect;
	int ret, inc = 1;

<<<<<<< HEAD
	if (unlikely(vfbs->master != file_priv->master))
		return -EINVAL;

	/* Require ScreenObject support for 3D */
	if (!dev_priv->sou_priv)
=======
	/* Legacy Display Unit does not support 3D */
	if (dev_priv->active_display_unit == vmw_du_legacy)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		return -EINVAL;

	drm_modeset_lock_all(dev_priv->dev);

	ret = ttm_read_lock(&dev_priv->reservation_sem, true);
	if (unlikely(ret != 0)) {
		drm_modeset_unlock_all(dev_priv->dev);
		return ret;
	}

	if (!num_clips) {
		num_clips = 1;
		clips = &norect;
		norect.x1 = norect.y1 = 0;
		norect.x2 = framebuffer->width;
		norect.y2 = framebuffer->height;
	} else if (flags & DRM_MODE_FB_DIRTY_ANNOTATE_COPY) {
		num_clips /= 2;
		inc = 2; /* skip source rects */
	}

<<<<<<< HEAD
	ret = do_surface_dirty_sou(dev_priv, file_priv, &vfbs->base,
				   flags, color,
				   clips, num_clips, inc, NULL);

=======
	if (dev_priv->active_display_unit == vmw_du_screen_object)
		ret = vmw_kms_sou_do_surface_dirty(dev_priv, &vfbs->base,
						   clips, NULL, NULL, 0, 0,
						   num_clips, inc, NULL);
	else
		ret = vmw_kms_stdu_surface_dirty(dev_priv, &vfbs->base,
						 clips, NULL, NULL, 0, 0,
						 num_clips, inc, NULL);

	vmw_fifo_flush(dev_priv, false);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	ttm_read_unlock(&dev_priv->reservation_sem);

	drm_modeset_unlock_all(dev_priv->dev);

	return 0;
}

<<<<<<< HEAD
=======
/**
 * vmw_kms_readback - Perform a readback from the screen system to
 * a dma-buffer backed framebuffer.
 *
 * @dev_priv: Pointer to the device private structure.
 * @file_priv: Pointer to a struct drm_file identifying the caller.
 * Must be set to NULL if @user_fence_rep is NULL.
 * @vfb: Pointer to the dma-buffer backed framebuffer.
 * @user_fence_rep: User-space provided structure for fence information.
 * Must be set to non-NULL if @file_priv is non-NULL.
 * @vclips: Array of clip rects.
 * @num_clips: Number of clip rects in @vclips.
 *
 * Returns 0 on success, negative error code on failure. -ERESTARTSYS if
 * interrupted.
 */
int vmw_kms_readback(struct vmw_private *dev_priv,
		     struct drm_file *file_priv,
		     struct vmw_framebuffer *vfb,
		     struct drm_vmw_fence_rep __user *user_fence_rep,
		     struct drm_vmw_rect *vclips,
		     uint32_t num_clips)
{
	switch (dev_priv->active_display_unit) {
	case vmw_du_screen_object:
		return vmw_kms_sou_readback(dev_priv, file_priv, vfb,
					    user_fence_rep, vclips, num_clips);
	case vmw_du_screen_target:
		return vmw_kms_stdu_dma(dev_priv, file_priv, vfb,
					user_fence_rep, NULL, vclips, num_clips,
					1, false, true);
	default:
		WARN_ONCE(true,
			  "Readback called with invalid display system.\n");
}

	return -ENOSYS;
}


>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
static struct drm_framebuffer_funcs vmw_framebuffer_surface_funcs = {
	.destroy = vmw_framebuffer_surface_destroy,
	.dirty = vmw_framebuffer_surface_dirty,
};

static int vmw_kms_new_framebuffer_surface(struct vmw_private *dev_priv,
<<<<<<< HEAD
					   struct drm_file *file_priv,
					   struct vmw_surface *surface,
					   struct vmw_framebuffer **out,
					   const struct drm_mode_fb_cmd
					   *mode_cmd)
=======
					   struct vmw_surface *surface,
					   struct vmw_framebuffer **out,
					   const struct drm_mode_fb_cmd
					   *mode_cmd,
					   bool is_dmabuf_proxy)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

{
	struct drm_device *dev = dev_priv->dev;
	struct vmw_framebuffer_surface *vfbs;
	enum SVGA3dSurfaceFormat format;
<<<<<<< HEAD
	struct vmw_master *vmaster = vmw_master(file_priv->master);
	int ret;

	/* 3D is only supported on HWv8 hosts which supports screen objects */
	if (!dev_priv->sou_priv)
=======
	int ret;

	/* 3D is only supported on HWv8 and newer hosts */
	if (dev_priv->active_display_unit == vmw_du_legacy)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		return -ENOSYS;

	/*
	 * Sanity checks.
	 */

	/* Surface must be marked as a scanout. */
	if (unlikely(!surface->scanout))
		return -EINVAL;

	if (unlikely(surface->mip_levels[0] != 1 ||
		     surface->num_sizes != 1 ||
		     surface->base_size.width < mode_cmd->width ||
		     surface->base_size.height < mode_cmd->height ||
		     surface->base_size.depth != 1)) {
		DRM_ERROR("Incompatible surface dimensions "
			  "for requested mode.\n");
		return -EINVAL;
	}

	switch (mode_cmd->depth) {
	case 32:
		format = SVGA3D_A8R8G8B8;
		break;
	case 24:
		format = SVGA3D_X8R8G8B8;
		break;
	case 16:
		format = SVGA3D_R5G6B5;
		break;
	case 15:
		format = SVGA3D_A1R5G5B5;
		break;
<<<<<<< HEAD
	case 8:
		format = SVGA3D_LUMINANCE8;
		break;
=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	default:
		DRM_ERROR("Invalid color depth: %d\n", mode_cmd->depth);
		return -EINVAL;
	}

<<<<<<< HEAD
	if (unlikely(format != surface->format)) {
=======
	/*
	 * For DX, surface format validation is done when surface->scanout
	 * is set.
	 */
	if (!dev_priv->has_dx && format != surface->format) {
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		DRM_ERROR("Invalid surface format for requested mode.\n");
		return -EINVAL;
	}

	vfbs = kzalloc(sizeof(*vfbs), GFP_KERNEL);
	if (!vfbs) {
		ret = -ENOMEM;
		goto out_err1;
	}

<<<<<<< HEAD
	if (!vmw_surface_reference(surface)) {
		DRM_ERROR("failed to reference surface %p\n", surface);
		ret = -EINVAL;
		goto out_err2;
	}

=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	/* XXX get the first 3 from the surface info */
	vfbs->base.base.bits_per_pixel = mode_cmd->bpp;
	vfbs->base.base.pitches[0] = mode_cmd->pitch;
	vfbs->base.base.depth = mode_cmd->depth;
	vfbs->base.base.width = mode_cmd->width;
	vfbs->base.base.height = mode_cmd->height;
<<<<<<< HEAD
	vfbs->surface = surface;
	vfbs->base.user_handle = mode_cmd->handle;
	vfbs->master = drm_master_get(file_priv->master);

	mutex_lock(&vmaster->fb_surf_mutex);
	list_add_tail(&vfbs->head, &vmaster->fb_surf);
	mutex_unlock(&vmaster->fb_surf_mutex);
=======
	vfbs->surface = vmw_surface_reference(surface);
	vfbs->base.user_handle = mode_cmd->handle;
	vfbs->is_dmabuf_proxy = is_dmabuf_proxy;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	*out = &vfbs->base;

	ret = drm_framebuffer_init(dev, &vfbs->base.base,
				   &vmw_framebuffer_surface_funcs);
	if (ret)
<<<<<<< HEAD
		goto out_err3;

	return 0;

out_err3:
	vmw_surface_unreference(&surface);
out_err2:
=======
		goto out_err2;

	return 0;

out_err2:
	vmw_surface_unreference(&surface);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	kfree(vfbs);
out_err1:
	return ret;
}

/*
 * Dmabuf framebuffer code
 */

<<<<<<< HEAD
#define vmw_framebuffer_to_vfbd(x) \
	container_of(x, struct vmw_framebuffer_dmabuf, base.base)

struct vmw_framebuffer_dmabuf {
	struct vmw_framebuffer base;
	struct vmw_dma_buffer *buffer;
};

=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
static void vmw_framebuffer_dmabuf_destroy(struct drm_framebuffer *framebuffer)
{
	struct vmw_framebuffer_dmabuf *vfbd =
		vmw_framebuffer_to_vfbd(framebuffer);

	drm_framebuffer_cleanup(framebuffer);
	vmw_dmabuf_unreference(&vfbd->buffer);
<<<<<<< HEAD
	ttm_base_object_unref(&vfbd->base.user_obj);
=======
	if (vfbd->base.user_obj)
		ttm_base_object_unref(&vfbd->base.user_obj);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	kfree(vfbd);
}

<<<<<<< HEAD
static int do_dmabuf_dirty_ldu(struct vmw_private *dev_priv,
			       struct vmw_framebuffer *framebuffer,
			       unsigned flags, unsigned color,
			       struct drm_clip_rect *clips,
			       unsigned num_clips, int increment)
{
	size_t fifo_size;
	int i;

	struct {
		uint32_t header;
		SVGAFifoCmdUpdate body;
	} *cmd;

	fifo_size = sizeof(*cmd) * num_clips;
	cmd = vmw_fifo_reserve(dev_priv, fifo_size);
	if (unlikely(cmd == NULL)) {
		DRM_ERROR("Fifo reserve failed.\n");
		return -ENOMEM;
	}

	memset(cmd, 0, fifo_size);
	for (i = 0; i < num_clips; i++, clips += increment) {
		cmd[i].header = cpu_to_le32(SVGA_CMD_UPDATE);
		cmd[i].body.x = cpu_to_le32(clips->x1);
		cmd[i].body.y = cpu_to_le32(clips->y1);
		cmd[i].body.width = cpu_to_le32(clips->x2 - clips->x1);
		cmd[i].body.height = cpu_to_le32(clips->y2 - clips->y1);
	}

	vmw_fifo_commit(dev_priv, fifo_size);
	return 0;
}

static int do_dmabuf_define_gmrfb(struct drm_file *file_priv,
				  struct vmw_private *dev_priv,
				  struct vmw_framebuffer *framebuffer)
{
	int depth = framebuffer->base.depth;
	size_t fifo_size;
	int ret;

	struct {
		uint32_t header;
		SVGAFifoCmdDefineGMRFB body;
	} *cmd;

	/* Emulate RGBA support, contrary to svga_reg.h this is not
	 * supported by hosts. This is only a problem if we are reading
	 * this value later and expecting what we uploaded back.
	 */
	if (depth == 32)
		depth = 24;

	fifo_size = sizeof(*cmd);
	cmd = kmalloc(fifo_size, GFP_KERNEL);
	if (unlikely(cmd == NULL)) {
		DRM_ERROR("Failed to allocate temporary cmd buffer.\n");
		return -ENOMEM;
	}

	memset(cmd, 0, fifo_size);
	cmd->header = SVGA_CMD_DEFINE_GMRFB;
	cmd->body.format.bitsPerPixel = framebuffer->base.bits_per_pixel;
	cmd->body.format.colorDepth = depth;
	cmd->body.format.reserved = 0;
	cmd->body.bytesPerLine = framebuffer->base.pitches[0];
	cmd->body.ptr.gmrId = framebuffer->user_handle;
	cmd->body.ptr.offset = 0;

	ret = vmw_execbuf_process(file_priv, dev_priv, NULL, cmd,
				  fifo_size, 0, NULL, NULL);

	kfree(cmd);

	return ret;
}

static int do_dmabuf_dirty_sou(struct drm_file *file_priv,
			       struct vmw_private *dev_priv,
			       struct vmw_framebuffer *framebuffer,
			       unsigned flags, unsigned color,
			       struct drm_clip_rect *clips,
			       unsigned num_clips, int increment,
			       struct vmw_fence_obj **out_fence)
{
	struct vmw_display_unit *units[VMWGFX_NUM_DISPLAY_UNITS];
	struct drm_clip_rect *clips_ptr;
	int i, k, num_units, ret;
	struct drm_crtc *crtc;
	size_t fifo_size;

	struct {
		uint32_t header;
		SVGAFifoCmdBlitGMRFBToScreen body;
	} *blits;

	ret = do_dmabuf_define_gmrfb(file_priv, dev_priv, framebuffer);
	if (unlikely(ret != 0))
		return ret; /* define_gmrfb prints warnings */

	fifo_size = sizeof(*blits) * num_clips;
	blits = kmalloc(fifo_size, GFP_KERNEL);
	if (unlikely(blits == NULL)) {
		DRM_ERROR("Failed to allocate temporary cmd buffer.\n");
		return -ENOMEM;
	}

	num_units = 0;
	list_for_each_entry(crtc, &dev_priv->dev->mode_config.crtc_list, head) {
		if (crtc->primary->fb != &framebuffer->base)
			continue;
		units[num_units++] = vmw_crtc_to_du(crtc);
	}

	for (k = 0; k < num_units; k++) {
		struct vmw_display_unit *unit = units[k];
		int hit_num = 0;

		clips_ptr = clips;
		for (i = 0; i < num_clips; i++, clips_ptr += increment) {
			int clip_x1 = clips_ptr->x1 - unit->crtc.x;
			int clip_y1 = clips_ptr->y1 - unit->crtc.y;
			int clip_x2 = clips_ptr->x2 - unit->crtc.x;
			int clip_y2 = clips_ptr->y2 - unit->crtc.y;
			int move_x, move_y;

			/* skip any crtcs that misses the clip region */
			if (clip_x1 >= unit->crtc.mode.hdisplay ||
			    clip_y1 >= unit->crtc.mode.vdisplay ||
			    clip_x2 <= 0 || clip_y2 <= 0)
				continue;

			/* clip size to crtc size */
			clip_x2 = min_t(int, clip_x2, unit->crtc.mode.hdisplay);
			clip_y2 = min_t(int, clip_y2, unit->crtc.mode.vdisplay);

			/* translate both src and dest to bring clip into screen */
			move_x = min_t(int, clip_x1, 0);
			move_y = min_t(int, clip_y1, 0);

			/* actual translate done here */
			blits[hit_num].header = SVGA_CMD_BLIT_GMRFB_TO_SCREEN;
			blits[hit_num].body.destScreenId = unit->unit;
			blits[hit_num].body.srcOrigin.x = clips_ptr->x1 - move_x;
			blits[hit_num].body.srcOrigin.y = clips_ptr->y1 - move_y;
			blits[hit_num].body.destRect.left = clip_x1 - move_x;
			blits[hit_num].body.destRect.top = clip_y1 - move_y;
			blits[hit_num].body.destRect.right = clip_x2;
			blits[hit_num].body.destRect.bottom = clip_y2;
			hit_num++;
		}

		/* no clips hit the crtc */
		if (hit_num == 0)
			continue;

		/* only return the last fence */
		if (out_fence && *out_fence)
			vmw_fence_obj_unreference(out_fence);

		fifo_size = sizeof(*blits) * hit_num;
		ret = vmw_execbuf_process(file_priv, dev_priv, NULL, blits,
					  fifo_size, 0, NULL, out_fence);

		if (unlikely(ret != 0))
			break;
	}

	kfree(blits);

	return ret;
}

=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
static int vmw_framebuffer_dmabuf_dirty(struct drm_framebuffer *framebuffer,
				 struct drm_file *file_priv,
				 unsigned flags, unsigned color,
				 struct drm_clip_rect *clips,
				 unsigned num_clips)
{
	struct vmw_private *dev_priv = vmw_priv(framebuffer->dev);
	struct vmw_framebuffer_dmabuf *vfbd =
		vmw_framebuffer_to_vfbd(framebuffer);
	struct drm_clip_rect norect;
	int ret, increment = 1;

	drm_modeset_lock_all(dev_priv->dev);

	ret = ttm_read_lock(&dev_priv->reservation_sem, true);
	if (unlikely(ret != 0)) {
		drm_modeset_unlock_all(dev_priv->dev);
		return ret;
	}

	if (!num_clips) {
		num_clips = 1;
		clips = &norect;
		norect.x1 = norect.y1 = 0;
		norect.x2 = framebuffer->width;
		norect.y2 = framebuffer->height;
	} else if (flags & DRM_MODE_FB_DIRTY_ANNOTATE_COPY) {
		num_clips /= 2;
		increment = 2;
	}

<<<<<<< HEAD
	if (dev_priv->ldu_priv) {
		ret = do_dmabuf_dirty_ldu(dev_priv, &vfbd->base,
					  flags, color,
					  clips, num_clips, increment);
	} else {
		ret = do_dmabuf_dirty_sou(file_priv, dev_priv, &vfbd->base,
					  flags, color,
					  clips, num_clips, increment, NULL);
	}

=======
	switch (dev_priv->active_display_unit) {
	case vmw_du_screen_target:
		ret = vmw_kms_stdu_dma(dev_priv, NULL, &vfbd->base, NULL,
				       clips, NULL, num_clips, increment,
				       true, true);
		break;
	case vmw_du_screen_object:
		ret = vmw_kms_sou_do_dmabuf_dirty(dev_priv, &vfbd->base,
						  clips, num_clips, increment,
						  true,
						  NULL);
		break;
	case vmw_du_legacy:
		ret = vmw_kms_ldu_do_dmabuf_dirty(dev_priv, &vfbd->base, 0, 0,
						  clips, num_clips, increment);
		break;
	default:
		ret = -EINVAL;
		WARN_ONCE(true, "Dirty called with invalid display system.\n");
		break;
	}

	vmw_fifo_flush(dev_priv, false);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	ttm_read_unlock(&dev_priv->reservation_sem);

	drm_modeset_unlock_all(dev_priv->dev);

	return ret;
}

static struct drm_framebuffer_funcs vmw_framebuffer_dmabuf_funcs = {
	.destroy = vmw_framebuffer_dmabuf_destroy,
	.dirty = vmw_framebuffer_dmabuf_dirty,
};

/**
 * Pin the dmabuffer to the start of vram.
 */
<<<<<<< HEAD
static int vmw_framebuffer_dmabuf_pin(struct vmw_framebuffer *vfb)
{
	struct vmw_private *dev_priv = vmw_priv(vfb->base.dev);
	struct vmw_framebuffer_dmabuf *vfbd =
		vmw_framebuffer_to_vfbd(&vfb->base);
	int ret;

	/* This code should not be used with screen objects */
	BUG_ON(dev_priv->sou_priv);

	vmw_overlay_pause_all(dev_priv);

	ret = vmw_dmabuf_to_start_of_vram(dev_priv, vfbd->buffer, true, false);

	vmw_overlay_resume_all(dev_priv);

	WARN_ON(ret != 0);

	return 0;
}

static int vmw_framebuffer_dmabuf_unpin(struct vmw_framebuffer *vfb)
{
	struct vmw_private *dev_priv = vmw_priv(vfb->base.dev);
	struct vmw_framebuffer_dmabuf *vfbd =
		vmw_framebuffer_to_vfbd(&vfb->base);

	if (!vfbd->buffer) {
		WARN_ON(!vfbd->buffer);
		return 0;
	}

	return vmw_dmabuf_unpin(dev_priv, vfbd->buffer, false);
}

=======
static int vmw_framebuffer_pin(struct vmw_framebuffer *vfb)
{
	struct vmw_private *dev_priv = vmw_priv(vfb->base.dev);
	struct vmw_dma_buffer *buf;
	int ret;

	buf = vfb->dmabuf ?  vmw_framebuffer_to_vfbd(&vfb->base)->buffer :
		vmw_framebuffer_to_vfbs(&vfb->base)->surface->res.backup;

	if (!buf)
		return 0;

	switch (dev_priv->active_display_unit) {
	case vmw_du_legacy:
		vmw_overlay_pause_all(dev_priv);
		ret = vmw_dmabuf_pin_in_start_of_vram(dev_priv, buf, false);
		vmw_overlay_resume_all(dev_priv);
		break;
	case vmw_du_screen_object:
	case vmw_du_screen_target:
		if (vfb->dmabuf)
			return vmw_dmabuf_pin_in_vram_or_gmr(dev_priv, buf,
							     false);

		return vmw_dmabuf_pin_in_placement(dev_priv, buf,
						   &vmw_mob_placement, false);
	default:
		return -EINVAL;
	}

	return ret;
}

static int vmw_framebuffer_unpin(struct vmw_framebuffer *vfb)
{
	struct vmw_private *dev_priv = vmw_priv(vfb->base.dev);
	struct vmw_dma_buffer *buf;

	buf = vfb->dmabuf ?  vmw_framebuffer_to_vfbd(&vfb->base)->buffer :
		vmw_framebuffer_to_vfbs(&vfb->base)->surface->res.backup;

	if (WARN_ON(!buf))
		return 0;

	return vmw_dmabuf_unpin(dev_priv, buf, false);
}

/**
 * vmw_create_dmabuf_proxy - create a proxy surface for the DMA buf
 *
 * @dev: DRM device
 * @mode_cmd: parameters for the new surface
 * @dmabuf_mob: MOB backing the DMA buf
 * @srf_out: newly created surface
 *
 * When the content FB is a DMA buf, we create a surface as a proxy to the
 * same buffer.  This way we can do a surface copy rather than a surface DMA.
 * This is a more efficient approach
 *
 * RETURNS:
 * 0 on success, error code otherwise
 */
static int vmw_create_dmabuf_proxy(struct drm_device *dev,
				   const struct drm_mode_fb_cmd *mode_cmd,
				   struct vmw_dma_buffer *dmabuf_mob,
				   struct vmw_surface **srf_out)
{
	uint32_t format;
	struct drm_vmw_size content_base_size;
	struct vmw_resource *res;
	unsigned int bytes_pp;
	int ret;

	switch (mode_cmd->depth) {
	case 32:
	case 24:
		format = SVGA3D_X8R8G8B8;
		bytes_pp = 4;
		break;

	case 16:
	case 15:
		format = SVGA3D_R5G6B5;
		bytes_pp = 2;
		break;

	case 8:
		format = SVGA3D_P8;
		bytes_pp = 1;
		break;

	default:
		DRM_ERROR("Invalid framebuffer format %d\n", mode_cmd->depth);
		return -EINVAL;
	}

	content_base_size.width  = mode_cmd->pitch / bytes_pp;
	content_base_size.height = mode_cmd->height;
	content_base_size.depth  = 1;

	ret = vmw_surface_gb_priv_define(dev,
			0, /* kernel visible only */
			0, /* flags */
			format,
			true, /* can be a scanout buffer */
			1, /* num of mip levels */
			0,
			0,
			content_base_size,
			srf_out);
	if (ret) {
		DRM_ERROR("Failed to allocate proxy content buffer\n");
		return ret;
	}

	res = &(*srf_out)->res;

	/* Reserve and switch the backing mob. */
	mutex_lock(&res->dev_priv->cmdbuf_mutex);
	(void) vmw_resource_reserve(res, false, true);
	vmw_dmabuf_unreference(&res->backup);
	res->backup = vmw_dmabuf_reference(dmabuf_mob);
	res->backup_offset = 0;
	vmw_resource_unreserve(res, false, NULL, 0);
	mutex_unlock(&res->dev_priv->cmdbuf_mutex);

	return 0;
}



>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
static int vmw_kms_new_framebuffer_dmabuf(struct vmw_private *dev_priv,
					  struct vmw_dma_buffer *dmabuf,
					  struct vmw_framebuffer **out,
					  const struct drm_mode_fb_cmd
					  *mode_cmd)

{
	struct drm_device *dev = dev_priv->dev;
	struct vmw_framebuffer_dmabuf *vfbd;
	unsigned int requested_size;
	int ret;

	requested_size = mode_cmd->height * mode_cmd->pitch;
	if (unlikely(requested_size > dmabuf->base.num_pages * PAGE_SIZE)) {
		DRM_ERROR("Screen buffer object size is too small "
			  "for requested mode.\n");
		return -EINVAL;
	}

	/* Limited framebuffer color depth support for screen objects */
<<<<<<< HEAD
	if (dev_priv->sou_priv) {
=======
	if (dev_priv->active_display_unit == vmw_du_screen_object) {
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		switch (mode_cmd->depth) {
		case 32:
		case 24:
			/* Only support 32 bpp for 32 and 24 depth fbs */
			if (mode_cmd->bpp == 32)
				break;

			DRM_ERROR("Invalid color depth/bbp: %d %d\n",
				  mode_cmd->depth, mode_cmd->bpp);
			return -EINVAL;
		case 16:
		case 15:
			/* Only support 16 bpp for 16 and 15 depth fbs */
			if (mode_cmd->bpp == 16)
				break;

			DRM_ERROR("Invalid color depth/bbp: %d %d\n",
				  mode_cmd->depth, mode_cmd->bpp);
			return -EINVAL;
		default:
			DRM_ERROR("Invalid color depth: %d\n", mode_cmd->depth);
			return -EINVAL;
		}
	}

	vfbd = kzalloc(sizeof(*vfbd), GFP_KERNEL);
	if (!vfbd) {
		ret = -ENOMEM;
		goto out_err1;
	}

<<<<<<< HEAD
	if (!vmw_dmabuf_reference(dmabuf)) {
		DRM_ERROR("failed to reference dmabuf %p\n", dmabuf);
		ret = -EINVAL;
		goto out_err2;
	}

=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	vfbd->base.base.bits_per_pixel = mode_cmd->bpp;
	vfbd->base.base.pitches[0] = mode_cmd->pitch;
	vfbd->base.base.depth = mode_cmd->depth;
	vfbd->base.base.width = mode_cmd->width;
	vfbd->base.base.height = mode_cmd->height;
<<<<<<< HEAD
	if (!dev_priv->sou_priv) {
		vfbd->base.pin = vmw_framebuffer_dmabuf_pin;
		vfbd->base.unpin = vmw_framebuffer_dmabuf_unpin;
	}
	vfbd->base.dmabuf = true;
	vfbd->buffer = dmabuf;
=======
	vfbd->base.dmabuf = true;
	vfbd->buffer = vmw_dmabuf_reference(dmabuf);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	vfbd->base.user_handle = mode_cmd->handle;
	*out = &vfbd->base;

	ret = drm_framebuffer_init(dev, &vfbd->base.base,
				   &vmw_framebuffer_dmabuf_funcs);
	if (ret)
<<<<<<< HEAD
		goto out_err3;

	return 0;

out_err3:
	vmw_dmabuf_unreference(&dmabuf);
out_err2:
=======
		goto out_err2;

	return 0;

out_err2:
	vmw_dmabuf_unreference(&dmabuf);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	kfree(vfbd);
out_err1:
	return ret;
}

<<<<<<< HEAD
/*
 * Generic Kernel modesetting functions
 */

static struct drm_framebuffer *vmw_kms_fb_create(struct drm_device *dev,
=======
/**
 * vmw_kms_new_framebuffer - Create a new framebuffer.
 *
 * @dev_priv: Pointer to device private struct.
 * @dmabuf: Pointer to dma buffer to wrap the kms framebuffer around.
 * Either @dmabuf or @surface must be NULL.
 * @surface: Pointer to a surface to wrap the kms framebuffer around.
 * Either @dmabuf or @surface must be NULL.
 * @only_2d: No presents will occur to this dma buffer based framebuffer. This
 * Helps the code to do some important optimizations.
 * @mode_cmd: Frame-buffer metadata.
 */
struct vmw_framebuffer *
vmw_kms_new_framebuffer(struct vmw_private *dev_priv,
			struct vmw_dma_buffer *dmabuf,
			struct vmw_surface *surface,
			bool only_2d,
			const struct drm_mode_fb_cmd *mode_cmd)
{
	struct vmw_framebuffer *vfb = NULL;
	bool is_dmabuf_proxy = false;
	int ret;

	/*
	 * We cannot use the SurfaceDMA command in an non-accelerated VM,
	 * therefore, wrap the DMA buf in a surface so we can use the
	 * SurfaceCopy command.
	 */
	if (dmabuf && only_2d &&
	    dev_priv->active_display_unit == vmw_du_screen_target) {
		ret = vmw_create_dmabuf_proxy(dev_priv->dev, mode_cmd,
					      dmabuf, &surface);
		if (ret)
			return ERR_PTR(ret);

		is_dmabuf_proxy = true;
	}

	/* Create the new framebuffer depending one what we have */
	if (surface) {
		ret = vmw_kms_new_framebuffer_surface(dev_priv, surface, &vfb,
						      mode_cmd,
						      is_dmabuf_proxy);

		/*
		 * vmw_create_dmabuf_proxy() adds a reference that is no longer
		 * needed
		 */
		if (is_dmabuf_proxy)
			vmw_surface_unreference(&surface);
	} else if (dmabuf) {
		ret = vmw_kms_new_framebuffer_dmabuf(dev_priv, dmabuf, &vfb,
						     mode_cmd);
	} else {
		BUG();
	}

	if (ret)
		return ERR_PTR(ret);

	vfb->pin = vmw_framebuffer_pin;
	vfb->unpin = vmw_framebuffer_unpin;

	return vfb;
}

/*
 * Generic Kernel modesetting functions
 */

static struct drm_framebuffer *vmw_kms_fb_create(struct drm_device *dev,
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
						 struct drm_file *file_priv,
						 struct drm_mode_fb_cmd2 *mode_cmd2)
{
	struct vmw_private *dev_priv = vmw_priv(dev);
	struct ttm_object_file *tfile = vmw_fpriv(file_priv)->tfile;
	struct vmw_framebuffer *vfb = NULL;
	struct vmw_surface *surface = NULL;
	struct vmw_dma_buffer *bo = NULL;
	struct ttm_base_object *user_obj;
	struct drm_mode_fb_cmd mode_cmd;
	int ret;

	mode_cmd.width = mode_cmd2->width;
	mode_cmd.height = mode_cmd2->height;
	mode_cmd.pitch = mode_cmd2->pitches[0];
	mode_cmd.handle = mode_cmd2->handles[0];
	drm_fb_get_bpp_depth(mode_cmd2->pixel_format, &mode_cmd.depth,
				    &mode_cmd.bpp);

	/**
	 * This code should be conditioned on Screen Objects not being used.
	 * If screen objects are used, we can allocate a GMR to hold the
	 * requested framebuffer.
	 */

	if (!vmw_kms_validate_mode_vram(dev_priv,
					mode_cmd.pitch,
					mode_cmd.height)) {
<<<<<<< HEAD
		DRM_ERROR("VRAM size is too small for requested mode.\n");
=======
		DRM_ERROR("Requested mode exceed bounding box limit.\n");
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		return ERR_PTR(-ENOMEM);
	}

	/*
	 * Take a reference on the user object of the resource
	 * backing the kms fb. This ensures that user-space handle
	 * lookups on that resource will always work as long as
	 * it's registered with a kms framebuffer. This is important,
	 * since vmw_execbuf_process identifies resources in the
	 * command stream using user-space handles.
	 */

	user_obj = ttm_base_object_lookup(tfile, mode_cmd.handle);
	if (unlikely(user_obj == NULL)) {
		DRM_ERROR("Could not locate requested kms frame buffer.\n");
		return ERR_PTR(-ENOENT);
	}

	/**
	 * End conditioned code.
	 */

	/* returns either a dmabuf or surface */
	ret = vmw_user_lookup_handle(dev_priv, tfile,
				     mode_cmd.handle,
				     &surface, &bo);
	if (ret)
		goto err_out;

<<<<<<< HEAD
	/* Create the new framebuffer depending one what we got back */
	if (bo)
		ret = vmw_kms_new_framebuffer_dmabuf(dev_priv, bo, &vfb,
						     &mode_cmd);
	else if (surface)
		ret = vmw_kms_new_framebuffer_surface(dev_priv, file_priv,
						      surface, &vfb, &mode_cmd);
	else
		BUG();
=======
	vfb = vmw_kms_new_framebuffer(dev_priv, bo, surface,
				      !(dev_priv->capabilities & SVGA_CAP_3D),
				      &mode_cmd);
	if (IS_ERR(vfb)) {
		ret = PTR_ERR(vfb);
		goto err_out;
 	}
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

err_out:
	/* vmw_user_lookup_handle takes one ref so does new_fb */
	if (bo)
		vmw_dmabuf_unreference(&bo);
	if (surface)
		vmw_surface_unreference(&surface);

	if (ret) {
		DRM_ERROR("failed to create vmw_framebuffer: %i\n", ret);
		ttm_base_object_unref(&user_obj);
		return ERR_PTR(ret);
	} else
		vfb->user_obj = user_obj;

	return &vfb->base;
}

static const struct drm_mode_config_funcs vmw_kms_funcs = {
	.fb_create = vmw_kms_fb_create,
};

<<<<<<< HEAD
=======
static int vmw_kms_generic_present(struct vmw_private *dev_priv,
				   struct drm_file *file_priv,
				   struct vmw_framebuffer *vfb,
				   struct vmw_surface *surface,
				   uint32_t sid,
				   int32_t destX, int32_t destY,
				   struct drm_vmw_rect *clips,
				   uint32_t num_clips)
{
	return vmw_kms_sou_do_surface_dirty(dev_priv, vfb, NULL, clips,
					    &surface->res, destX, destY,
					    num_clips, 1, NULL);
}


>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
int vmw_kms_present(struct vmw_private *dev_priv,
		    struct drm_file *file_priv,
		    struct vmw_framebuffer *vfb,
		    struct vmw_surface *surface,
		    uint32_t sid,
		    int32_t destX, int32_t destY,
		    struct drm_vmw_rect *clips,
		    uint32_t num_clips)
{
<<<<<<< HEAD
	struct vmw_display_unit *units[VMWGFX_NUM_DISPLAY_UNITS];
	struct drm_clip_rect *tmp;
	struct drm_crtc *crtc;
	size_t fifo_size;
	int i, k, num_units;
	int ret = 0; /* silence warning */
	int left, right, top, bottom;

	struct {
		SVGA3dCmdHeader header;
		SVGA3dCmdBlitSurfaceToScreen body;
	} *cmd;
	SVGASignedRect *blits;

	num_units = 0;
	list_for_each_entry(crtc, &dev_priv->dev->mode_config.crtc_list, head) {
		if (crtc->primary->fb != &vfb->base)
			continue;
		units[num_units++] = vmw_crtc_to_du(crtc);
	}

	BUG_ON(surface == NULL);
	BUG_ON(!clips || !num_clips);

	tmp = kzalloc(sizeof(*tmp) * num_clips, GFP_KERNEL);
	if (unlikely(tmp == NULL)) {
		DRM_ERROR("Temporary cliprect memory alloc failed.\n");
		return -ENOMEM;
	}

	fifo_size = sizeof(*cmd) + sizeof(SVGASignedRect) * num_clips;
	cmd = kmalloc(fifo_size, GFP_KERNEL);
	if (unlikely(cmd == NULL)) {
		DRM_ERROR("Failed to allocate temporary fifo memory.\n");
		ret = -ENOMEM;
		goto out_free_tmp;
	}

	left = clips->x;
	right = clips->x + clips->w;
	top = clips->y;
	bottom = clips->y + clips->h;

	for (i = 1; i < num_clips; i++) {
		left = min_t(int, left, (int)clips[i].x);
		right = max_t(int, right, (int)clips[i].x + clips[i].w);
		top = min_t(int, top, (int)clips[i].y);
		bottom = max_t(int, bottom, (int)clips[i].y + clips[i].h);
	}

	/* only need to do this once */
	memset(cmd, 0, fifo_size);
	cmd->header.id = cpu_to_le32(SVGA_3D_CMD_BLIT_SURFACE_TO_SCREEN);

	blits = (SVGASignedRect *)&cmd[1];

	cmd->body.srcRect.left = left;
	cmd->body.srcRect.right = right;
	cmd->body.srcRect.top = top;
	cmd->body.srcRect.bottom = bottom;

	for (i = 0; i < num_clips; i++) {
		tmp[i].x1 = clips[i].x - left;
		tmp[i].x2 = clips[i].x + clips[i].w - left;
		tmp[i].y1 = clips[i].y - top;
		tmp[i].y2 = clips[i].y + clips[i].h - top;
	}

	for (k = 0; k < num_units; k++) {
		struct vmw_display_unit *unit = units[k];
		struct vmw_clip_rect clip;
		int num;

		clip.x1 = left + destX - unit->crtc.x;
		clip.y1 = top + destY - unit->crtc.y;
		clip.x2 = right + destX - unit->crtc.x;
		clip.y2 = bottom + destY - unit->crtc.y;

		/* skip any crtcs that misses the clip region */
		if (clip.x1 >= unit->crtc.mode.hdisplay ||
		    clip.y1 >= unit->crtc.mode.vdisplay ||
		    clip.x2 <= 0 || clip.y2 <= 0)
			continue;

		/*
		 * In order for the clip rects to be correctly scaled
		 * the src and dest rects needs to be the same size.
		 */
		cmd->body.destRect.left = clip.x1;
		cmd->body.destRect.right = clip.x2;
		cmd->body.destRect.top = clip.y1;
		cmd->body.destRect.bottom = clip.y2;

		/* create a clip rect of the crtc in dest coords */
		clip.x2 = unit->crtc.mode.hdisplay - clip.x1;
		clip.y2 = unit->crtc.mode.vdisplay - clip.y1;
		clip.x1 = 0 - clip.x1;
		clip.y1 = 0 - clip.y1;

		/* need to reset sid as it is changed by execbuf */
		cmd->body.srcImage.sid = sid;
		cmd->body.destScreenId = unit->unit;

		/* clip and write blits to cmd stream */
		vmw_clip_cliprects(tmp, num_clips, clip, blits, &num);

		/* if no cliprects hit skip this */
		if (num == 0)
			continue;

		/* recalculate package length */
		fifo_size = sizeof(*cmd) + sizeof(SVGASignedRect) * num;
		cmd->header.size = cpu_to_le32(fifo_size - sizeof(cmd->header));
		ret = vmw_execbuf_process(file_priv, dev_priv, NULL, cmd,
					  fifo_size, 0, NULL, NULL);

		if (unlikely(ret != 0))
			break;
	}

	kfree(cmd);
out_free_tmp:
	kfree(tmp);

	return ret;
}

int vmw_kms_readback(struct vmw_private *dev_priv,
		     struct drm_file *file_priv,
		     struct vmw_framebuffer *vfb,
		     struct drm_vmw_fence_rep __user *user_fence_rep,
		     struct drm_vmw_rect *clips,
		     uint32_t num_clips)
{
	struct vmw_framebuffer_dmabuf *vfbd =
		vmw_framebuffer_to_vfbd(&vfb->base);
	struct vmw_dma_buffer *dmabuf = vfbd->buffer;
	struct vmw_display_unit *units[VMWGFX_NUM_DISPLAY_UNITS];
	struct drm_crtc *crtc;
	size_t fifo_size;
	int i, k, ret, num_units, blits_pos;

	struct {
		uint32_t header;
		SVGAFifoCmdDefineGMRFB body;
	} *cmd;
	struct {
		uint32_t header;
		SVGAFifoCmdBlitScreenToGMRFB body;
	} *blits;

	num_units = 0;
	list_for_each_entry(crtc, &dev_priv->dev->mode_config.crtc_list, head) {
		if (crtc->primary->fb != &vfb->base)
			continue;
		units[num_units++] = vmw_crtc_to_du(crtc);
	}

	BUG_ON(dmabuf == NULL);
	BUG_ON(!clips || !num_clips);

	/* take a safe guess at fifo size */
	fifo_size = sizeof(*cmd) + sizeof(*blits) * num_clips * num_units;
	cmd = kmalloc(fifo_size, GFP_KERNEL);
	if (unlikely(cmd == NULL)) {
		DRM_ERROR("Failed to allocate temporary fifo memory.\n");
		return -ENOMEM;
	}

	memset(cmd, 0, fifo_size);
	cmd->header = SVGA_CMD_DEFINE_GMRFB;
	cmd->body.format.bitsPerPixel = vfb->base.bits_per_pixel;
	cmd->body.format.colorDepth = vfb->base.depth;
	cmd->body.format.reserved = 0;
	cmd->body.bytesPerLine = vfb->base.pitches[0];
	cmd->body.ptr.gmrId = vfb->user_handle;
	cmd->body.ptr.offset = 0;

	blits = (void *)&cmd[1];
	blits_pos = 0;
	for (i = 0; i < num_units; i++) {
		struct drm_vmw_rect *c = clips;
		for (k = 0; k < num_clips; k++, c++) {
			/* transform clip coords to crtc origin based coords */
			int clip_x1 = c->x - units[i]->crtc.x;
			int clip_x2 = c->x - units[i]->crtc.x + c->w;
			int clip_y1 = c->y - units[i]->crtc.y;
			int clip_y2 = c->y - units[i]->crtc.y + c->h;
			int dest_x = c->x;
			int dest_y = c->y;

			/* compensate for clipping, we negate
			 * a negative number and add that.
			 */
			if (clip_x1 < 0)
				dest_x += -clip_x1;
			if (clip_y1 < 0)
				dest_y += -clip_y1;

			/* clip */
			clip_x1 = max(clip_x1, 0);
			clip_y1 = max(clip_y1, 0);
			clip_x2 = min(clip_x2, units[i]->crtc.mode.hdisplay);
			clip_y2 = min(clip_y2, units[i]->crtc.mode.vdisplay);

			/* and cull any rects that misses the crtc */
			if (clip_x1 >= units[i]->crtc.mode.hdisplay ||
			    clip_y1 >= units[i]->crtc.mode.vdisplay ||
			    clip_x2 <= 0 || clip_y2 <= 0)
				continue;

			blits[blits_pos].header = SVGA_CMD_BLIT_SCREEN_TO_GMRFB;
			blits[blits_pos].body.srcScreenId = units[i]->unit;
			blits[blits_pos].body.destOrigin.x = dest_x;
			blits[blits_pos].body.destOrigin.y = dest_y;

			blits[blits_pos].body.srcRect.left = clip_x1;
			blits[blits_pos].body.srcRect.top = clip_y1;
			blits[blits_pos].body.srcRect.right = clip_x2;
			blits[blits_pos].body.srcRect.bottom = clip_y2;
			blits_pos++;
		}
	}
	/* reset size here and use calculated exact size from loops */
	fifo_size = sizeof(*cmd) + sizeof(*blits) * blits_pos;

	ret = vmw_execbuf_process(file_priv, dev_priv, NULL, cmd, fifo_size,
				  0, user_fence_rep, NULL);

	kfree(cmd);

	return ret;
=======
	int ret;

	switch (dev_priv->active_display_unit) {
	case vmw_du_screen_target:
		ret = vmw_kms_stdu_surface_dirty(dev_priv, vfb, NULL, clips,
						 &surface->res, destX, destY,
						 num_clips, 1, NULL);
		break;
	case vmw_du_screen_object:
		ret = vmw_kms_generic_present(dev_priv, file_priv, vfb, surface,
					      sid, destX, destY, clips,
					      num_clips);
		break;
	default:
		WARN_ONCE(true,
			  "Present called with invalid display system.\n");
		ret = -ENOSYS;
		break;
	}
	if (ret)
		return ret;

	vmw_fifo_flush(dev_priv, false);

	return 0;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

int vmw_kms_init(struct vmw_private *dev_priv)
{
	struct drm_device *dev = dev_priv->dev;
	int ret;

	drm_mode_config_init(dev);
	dev->mode_config.funcs = &vmw_kms_funcs;
	dev->mode_config.min_width = 1;
	dev->mode_config.min_height = 1;
<<<<<<< HEAD
	/* assumed largest fb size */
	dev->mode_config.max_width = 8192;
	dev->mode_config.max_height = 8192;

	ret = vmw_kms_init_screen_object_display(dev_priv);
	if (ret) /* Fallback */
		(void)vmw_kms_init_legacy_display_system(dev_priv);

	return 0;
=======
	dev->mode_config.max_width = dev_priv->texture_max_width;
	dev->mode_config.max_height = dev_priv->texture_max_height;

	ret = vmw_kms_stdu_init_display(dev_priv);
	if (ret) {
		ret = vmw_kms_sou_init_display(dev_priv);
		if (ret) /* Fallback */
			ret = vmw_kms_ldu_init_display(dev_priv);
	}

	return ret;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

int vmw_kms_close(struct vmw_private *dev_priv)
{
<<<<<<< HEAD
=======
	int ret;

>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	/*
	 * Docs says we should take the lock before calling this function
	 * but since it destroys encoders and our destructor calls
	 * drm_encoder_cleanup which takes the lock we deadlock.
	 */
	drm_mode_config_cleanup(dev_priv->dev);
<<<<<<< HEAD
	if (dev_priv->sou_priv)
		vmw_kms_close_screen_object_display(dev_priv);
	else
		vmw_kms_close_legacy_display_system(dev_priv);
	return 0;
=======
	if (dev_priv->active_display_unit == vmw_du_screen_object)
		ret = vmw_kms_sou_close_display(dev_priv);
	else if (dev_priv->active_display_unit == vmw_du_screen_target)
		ret = vmw_kms_stdu_close_display(dev_priv);
	else
		ret = vmw_kms_ldu_close_display(dev_priv);

	return ret;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

int vmw_kms_cursor_bypass_ioctl(struct drm_device *dev, void *data,
				struct drm_file *file_priv)
{
	struct drm_vmw_cursor_bypass_arg *arg = data;
	struct vmw_display_unit *du;
	struct drm_crtc *crtc;
	int ret = 0;


	mutex_lock(&dev->mode_config.mutex);
	if (arg->flags & DRM_VMW_CURSOR_BYPASS_ALL) {

		list_for_each_entry(crtc, &dev->mode_config.crtc_list, head) {
			du = vmw_crtc_to_du(crtc);
			du->hotspot_x = arg->xhot;
			du->hotspot_y = arg->yhot;
		}

		mutex_unlock(&dev->mode_config.mutex);
		return 0;
	}

	crtc = drm_crtc_find(dev, arg->crtc_id);
	if (!crtc) {
		ret = -ENOENT;
		goto out;
	}

	du = vmw_crtc_to_du(crtc);

	du->hotspot_x = arg->xhot;
	du->hotspot_y = arg->yhot;

out:
	mutex_unlock(&dev->mode_config.mutex);

	return ret;
}

int vmw_kms_write_svga(struct vmw_private *vmw_priv,
			unsigned width, unsigned height, unsigned pitch,
			unsigned bpp, unsigned depth)
{
	if (vmw_priv->capabilities & SVGA_CAP_PITCHLOCK)
		vmw_write(vmw_priv, SVGA_REG_PITCHLOCK, pitch);
	else if (vmw_fifo_have_pitchlock(vmw_priv))
<<<<<<< HEAD
		iowrite32(pitch, vmw_priv->mmio_virt + SVGA_FIFO_PITCHLOCK);
=======
		vmw_mmio_write(pitch, vmw_priv->mmio_virt +
			       SVGA_FIFO_PITCHLOCK);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	vmw_write(vmw_priv, SVGA_REG_WIDTH, width);
	vmw_write(vmw_priv, SVGA_REG_HEIGHT, height);
	vmw_write(vmw_priv, SVGA_REG_BITS_PER_PIXEL, bpp);

	if (vmw_read(vmw_priv, SVGA_REG_DEPTH) != depth) {
		DRM_ERROR("Invalid depth %u for %u bpp, host expects %u\n",
			  depth, bpp, vmw_read(vmw_priv, SVGA_REG_DEPTH));
		return -EINVAL;
	}

	return 0;
}

int vmw_kms_save_vga(struct vmw_private *vmw_priv)
{
	struct vmw_vga_topology_state *save;
	uint32_t i;

	vmw_priv->vga_width = vmw_read(vmw_priv, SVGA_REG_WIDTH);
	vmw_priv->vga_height = vmw_read(vmw_priv, SVGA_REG_HEIGHT);
	vmw_priv->vga_bpp = vmw_read(vmw_priv, SVGA_REG_BITS_PER_PIXEL);
	if (vmw_priv->capabilities & SVGA_CAP_PITCHLOCK)
		vmw_priv->vga_pitchlock =
		  vmw_read(vmw_priv, SVGA_REG_PITCHLOCK);
	else if (vmw_fifo_have_pitchlock(vmw_priv))
<<<<<<< HEAD
		vmw_priv->vga_pitchlock = ioread32(vmw_priv->mmio_virt +
						       SVGA_FIFO_PITCHLOCK);
=======
		vmw_priv->vga_pitchlock = vmw_mmio_read(vmw_priv->mmio_virt +
							SVGA_FIFO_PITCHLOCK);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	if (!(vmw_priv->capabilities & SVGA_CAP_DISPLAY_TOPOLOGY))
		return 0;

	vmw_priv->num_displays = vmw_read(vmw_priv,
					  SVGA_REG_NUM_GUEST_DISPLAYS);

	if (vmw_priv->num_displays == 0)
		vmw_priv->num_displays = 1;

	for (i = 0; i < vmw_priv->num_displays; ++i) {
		save = &vmw_priv->vga_save[i];
		vmw_write(vmw_priv, SVGA_REG_DISPLAY_ID, i);
		save->primary = vmw_read(vmw_priv, SVGA_REG_DISPLAY_IS_PRIMARY);
		save->pos_x = vmw_read(vmw_priv, SVGA_REG_DISPLAY_POSITION_X);
		save->pos_y = vmw_read(vmw_priv, SVGA_REG_DISPLAY_POSITION_Y);
		save->width = vmw_read(vmw_priv, SVGA_REG_DISPLAY_WIDTH);
		save->height = vmw_read(vmw_priv, SVGA_REG_DISPLAY_HEIGHT);
		vmw_write(vmw_priv, SVGA_REG_DISPLAY_ID, SVGA_ID_INVALID);
		if (i == 0 && vmw_priv->num_displays == 1 &&
		    save->width == 0 && save->height == 0) {

			/*
			 * It should be fairly safe to assume that these
			 * values are uninitialized.
			 */

			save->width = vmw_priv->vga_width - save->pos_x;
			save->height = vmw_priv->vga_height - save->pos_y;
		}
	}

	return 0;
}

int vmw_kms_restore_vga(struct vmw_private *vmw_priv)
{
	struct vmw_vga_topology_state *save;
	uint32_t i;

	vmw_write(vmw_priv, SVGA_REG_WIDTH, vmw_priv->vga_width);
	vmw_write(vmw_priv, SVGA_REG_HEIGHT, vmw_priv->vga_height);
	vmw_write(vmw_priv, SVGA_REG_BITS_PER_PIXEL, vmw_priv->vga_bpp);
	if (vmw_priv->capabilities & SVGA_CAP_PITCHLOCK)
		vmw_write(vmw_priv, SVGA_REG_PITCHLOCK,
			  vmw_priv->vga_pitchlock);
	else if (vmw_fifo_have_pitchlock(vmw_priv))
<<<<<<< HEAD
		iowrite32(vmw_priv->vga_pitchlock,
			  vmw_priv->mmio_virt + SVGA_FIFO_PITCHLOCK);
=======
		vmw_mmio_write(vmw_priv->vga_pitchlock,
			       vmw_priv->mmio_virt + SVGA_FIFO_PITCHLOCK);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	if (!(vmw_priv->capabilities & SVGA_CAP_DISPLAY_TOPOLOGY))
		return 0;

	for (i = 0; i < vmw_priv->num_displays; ++i) {
		save = &vmw_priv->vga_save[i];
		vmw_write(vmw_priv, SVGA_REG_DISPLAY_ID, i);
		vmw_write(vmw_priv, SVGA_REG_DISPLAY_IS_PRIMARY, save->primary);
		vmw_write(vmw_priv, SVGA_REG_DISPLAY_POSITION_X, save->pos_x);
		vmw_write(vmw_priv, SVGA_REG_DISPLAY_POSITION_Y, save->pos_y);
		vmw_write(vmw_priv, SVGA_REG_DISPLAY_WIDTH, save->width);
		vmw_write(vmw_priv, SVGA_REG_DISPLAY_HEIGHT, save->height);
		vmw_write(vmw_priv, SVGA_REG_DISPLAY_ID, SVGA_ID_INVALID);
	}

	return 0;
}

bool vmw_kms_validate_mode_vram(struct vmw_private *dev_priv,
				uint32_t pitch,
				uint32_t height)
{
<<<<<<< HEAD
	return ((u64) pitch * (u64) height) < (u64) dev_priv->prim_bb_mem;
=======
	return ((u64) pitch * (u64) height) < (u64)
		((dev_priv->active_display_unit == vmw_du_screen_target) ?
		 dev_priv->prim_bb_mem : dev_priv->vram_size);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}


/**
 * Function called by DRM code called with vbl_lock held.
 */
<<<<<<< HEAD
u32 vmw_get_vblank_counter(struct drm_device *dev, int crtc)
=======
u32 vmw_get_vblank_counter(struct drm_device *dev, unsigned int pipe)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
{
	return 0;
}

/**
 * Function called by DRM code called with vbl_lock held.
 */
<<<<<<< HEAD
int vmw_enable_vblank(struct drm_device *dev, int crtc)
=======
int vmw_enable_vblank(struct drm_device *dev, unsigned int pipe)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
{
	return -ENOSYS;
}

/**
 * Function called by DRM code called with vbl_lock held.
 */
<<<<<<< HEAD
void vmw_disable_vblank(struct drm_device *dev, int crtc)
=======
void vmw_disable_vblank(struct drm_device *dev, unsigned int pipe)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
{
}


/*
 * Small shared kms functions.
 */

static int vmw_du_update_layout(struct vmw_private *dev_priv, unsigned num,
			 struct drm_vmw_rect *rects)
{
	struct drm_device *dev = dev_priv->dev;
	struct vmw_display_unit *du;
	struct drm_connector *con;

	mutex_lock(&dev->mode_config.mutex);

#if 0
	{
		unsigned int i;

		DRM_INFO("%s: new layout ", __func__);
		for (i = 0; i < num; i++)
			DRM_INFO("(%i, %i %ux%u) ", rects[i].x, rects[i].y,
				 rects[i].w, rects[i].h);
		DRM_INFO("\n");
	}
#endif

	list_for_each_entry(con, &dev->mode_config.connector_list, head) {
		du = vmw_connector_to_du(con);
		if (num > du->unit) {
			du->pref_width = rects[du->unit].w;
			du->pref_height = rects[du->unit].h;
			du->pref_active = true;
			du->gui_x = rects[du->unit].x;
			du->gui_y = rects[du->unit].y;
		} else {
			du->pref_width = 800;
			du->pref_height = 600;
			du->pref_active = false;
		}
		con->status = vmw_du_connector_detect(con, true);
	}

	mutex_unlock(&dev->mode_config.mutex);

	return 0;
}

<<<<<<< HEAD
int vmw_du_page_flip(struct drm_crtc *crtc,
		     struct drm_framebuffer *fb,
		     struct drm_pending_vblank_event *event,
		     uint32_t page_flip_flags)
{
	struct vmw_private *dev_priv = vmw_priv(crtc->dev);
	struct drm_framebuffer *old_fb = crtc->primary->fb;
	struct vmw_framebuffer *vfb = vmw_framebuffer_to_vfb(fb);
	struct drm_file *file_priv ;
	struct vmw_fence_obj *fence = NULL;
	struct drm_clip_rect clips;
	int ret;

	if (event == NULL)
		return -EINVAL;

	/* require ScreenObject support for page flipping */
	if (!dev_priv->sou_priv)
		return -ENOSYS;

	file_priv = event->base.file_priv;
	if (!vmw_kms_screen_object_flippable(dev_priv, crtc))
		return -EINVAL;

	crtc->primary->fb = fb;

	/* do a full screen dirty update */
	clips.x1 = clips.y1 = 0;
	clips.x2 = fb->width;
	clips.y2 = fb->height;

	if (vfb->dmabuf)
		ret = do_dmabuf_dirty_sou(file_priv, dev_priv, vfb,
					  0, 0, &clips, 1, 1, &fence);
	else
		ret = do_surface_dirty_sou(dev_priv, file_priv, vfb,
					   0, 0, &clips, 1, 1, &fence);


	if (ret != 0)
		goto out_no_fence;
	if (!fence) {
		ret = -EINVAL;
		goto out_no_fence;
	}

	ret = vmw_event_fence_action_queue(file_priv, fence,
					   &event->base,
					   &event->event.tv_sec,
					   &event->event.tv_usec,
					   true);

	/*
	 * No need to hold on to this now. The only cleanup
	 * we need to do if we fail is unref the fence.
	 */
	vmw_fence_obj_unreference(&fence);

	if (vmw_crtc_to_du(crtc)->is_implicit)
		vmw_kms_screen_object_update_implicit_fb(dev_priv, crtc);

	return ret;

out_no_fence:
	crtc->primary->fb = old_fb;
	return ret;
}


=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
void vmw_du_crtc_save(struct drm_crtc *crtc)
{
}

void vmw_du_crtc_restore(struct drm_crtc *crtc)
{
}

void vmw_du_crtc_gamma_set(struct drm_crtc *crtc,
			   u16 *r, u16 *g, u16 *b,
			   uint32_t start, uint32_t size)
{
	struct vmw_private *dev_priv = vmw_priv(crtc->dev);
	int i;

	for (i = 0; i < size; i++) {
		DRM_DEBUG("%d r/g/b = 0x%04x / 0x%04x / 0x%04x\n", i,
			  r[i], g[i], b[i]);
		vmw_write(dev_priv, SVGA_PALETTE_BASE + i * 3 + 0, r[i] >> 8);
		vmw_write(dev_priv, SVGA_PALETTE_BASE + i * 3 + 1, g[i] >> 8);
		vmw_write(dev_priv, SVGA_PALETTE_BASE + i * 3 + 2, b[i] >> 8);
	}
}

<<<<<<< HEAD
void vmw_du_connector_dpms(struct drm_connector *connector, int mode)
{
=======
int vmw_du_connector_dpms(struct drm_connector *connector, int mode)
{
	return 0;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

void vmw_du_connector_save(struct drm_connector *connector)
{
}

void vmw_du_connector_restore(struct drm_connector *connector)
{
}

enum drm_connector_status
vmw_du_connector_detect(struct drm_connector *connector, bool force)
{
	uint32_t num_displays;
	struct drm_device *dev = connector->dev;
	struct vmw_private *dev_priv = vmw_priv(dev);
	struct vmw_display_unit *du = vmw_connector_to_du(connector);

	num_displays = vmw_read(dev_priv, SVGA_REG_NUM_DISPLAYS);

	return ((vmw_connector_to_du(connector)->unit < num_displays &&
		 du->pref_active) ?
		connector_status_connected : connector_status_disconnected);
}

static struct drm_display_mode vmw_kms_connector_builtin[] = {
	/* 640x480@60Hz */
	{ DRM_MODE("640x480", DRM_MODE_TYPE_DRIVER, 25175, 640, 656,
		   752, 800, 0, 480, 489, 492, 525, 0,
		   DRM_MODE_FLAG_NHSYNC | DRM_MODE_FLAG_NVSYNC) },
	/* 800x600@60Hz */
	{ DRM_MODE("800x600", DRM_MODE_TYPE_DRIVER, 40000, 800, 840,
		   968, 1056, 0, 600, 601, 605, 628, 0,
		   DRM_MODE_FLAG_PHSYNC | DRM_MODE_FLAG_PVSYNC) },
	/* 1024x768@60Hz */
	{ DRM_MODE("1024x768", DRM_MODE_TYPE_DRIVER, 65000, 1024, 1048,
		   1184, 1344, 0, 768, 771, 777, 806, 0,
		   DRM_MODE_FLAG_NHSYNC | DRM_MODE_FLAG_NVSYNC) },
	/* 1152x864@75Hz */
	{ DRM_MODE("1152x864", DRM_MODE_TYPE_DRIVER, 108000, 1152, 1216,
		   1344, 1600, 0, 864, 865, 868, 900, 0,
		   DRM_MODE_FLAG_PHSYNC | DRM_MODE_FLAG_PVSYNC) },
	/* 1280x768@60Hz */
	{ DRM_MODE("1280x768", DRM_MODE_TYPE_DRIVER, 79500, 1280, 1344,
		   1472, 1664, 0, 768, 771, 778, 798, 0,
		   DRM_MODE_FLAG_NHSYNC | DRM_MODE_FLAG_PVSYNC) },
	/* 1280x800@60Hz */
	{ DRM_MODE("1280x800", DRM_MODE_TYPE_DRIVER, 83500, 1280, 1352,
		   1480, 1680, 0, 800, 803, 809, 831, 0,
		   DRM_MODE_FLAG_PHSYNC | DRM_MODE_FLAG_NVSYNC) },
	/* 1280x960@60Hz */
	{ DRM_MODE("1280x960", DRM_MODE_TYPE_DRIVER, 108000, 1280, 1376,
		   1488, 1800, 0, 960, 961, 964, 1000, 0,
		   DRM_MODE_FLAG_PHSYNC | DRM_MODE_FLAG_PVSYNC) },
	/* 1280x1024@60Hz */
	{ DRM_MODE("1280x1024", DRM_MODE_TYPE_DRIVER, 108000, 1280, 1328,
		   1440, 1688, 0, 1024, 1025, 1028, 1066, 0,
		   DRM_MODE_FLAG_PHSYNC | DRM_MODE_FLAG_PVSYNC) },
	/* 1360x768@60Hz */
	{ DRM_MODE("1360x768", DRM_MODE_TYPE_DRIVER, 85500, 1360, 1424,
		   1536, 1792, 0, 768, 771, 777, 795, 0,
		   DRM_MODE_FLAG_PHSYNC | DRM_MODE_FLAG_PVSYNC) },
	/* 1440x1050@60Hz */
	{ DRM_MODE("1400x1050", DRM_MODE_TYPE_DRIVER, 121750, 1400, 1488,
		   1632, 1864, 0, 1050, 1053, 1057, 1089, 0,
		   DRM_MODE_FLAG_NHSYNC | DRM_MODE_FLAG_PVSYNC) },
	/* 1440x900@60Hz */
	{ DRM_MODE("1440x900", DRM_MODE_TYPE_DRIVER, 106500, 1440, 1520,
		   1672, 1904, 0, 900, 903, 909, 934, 0,
		   DRM_MODE_FLAG_NHSYNC | DRM_MODE_FLAG_PVSYNC) },
	/* 1600x1200@60Hz */
	{ DRM_MODE("1600x1200", DRM_MODE_TYPE_DRIVER, 162000, 1600, 1664,
		   1856, 2160, 0, 1200, 1201, 1204, 1250, 0,
		   DRM_MODE_FLAG_PHSYNC | DRM_MODE_FLAG_PVSYNC) },
	/* 1680x1050@60Hz */
	{ DRM_MODE("1680x1050", DRM_MODE_TYPE_DRIVER, 146250, 1680, 1784,
		   1960, 2240, 0, 1050, 1053, 1059, 1089, 0,
		   DRM_MODE_FLAG_NHSYNC | DRM_MODE_FLAG_PVSYNC) },
	/* 1792x1344@60Hz */
	{ DRM_MODE("1792x1344", DRM_MODE_TYPE_DRIVER, 204750, 1792, 1920,
		   2120, 2448, 0, 1344, 1345, 1348, 1394, 0,
		   DRM_MODE_FLAG_NHSYNC | DRM_MODE_FLAG_PVSYNC) },
	/* 1853x1392@60Hz */
	{ DRM_MODE("1856x1392", DRM_MODE_TYPE_DRIVER, 218250, 1856, 1952,
		   2176, 2528, 0, 1392, 1393, 1396, 1439, 0,
		   DRM_MODE_FLAG_NHSYNC | DRM_MODE_FLAG_PVSYNC) },
	/* 1920x1200@60Hz */
	{ DRM_MODE("1920x1200", DRM_MODE_TYPE_DRIVER, 193250, 1920, 2056,
		   2256, 2592, 0, 1200, 1203, 1209, 1245, 0,
		   DRM_MODE_FLAG_NHSYNC | DRM_MODE_FLAG_PVSYNC) },
	/* 1920x1440@60Hz */
	{ DRM_MODE("1920x1440", DRM_MODE_TYPE_DRIVER, 234000, 1920, 2048,
		   2256, 2600, 0, 1440, 1441, 1444, 1500, 0,
		   DRM_MODE_FLAG_NHSYNC | DRM_MODE_FLAG_PVSYNC) },
	/* 2560x1600@60Hz */
	{ DRM_MODE("2560x1600", DRM_MODE_TYPE_DRIVER, 348500, 2560, 2752,
		   3032, 3504, 0, 1600, 1603, 1609, 1658, 0,
		   DRM_MODE_FLAG_NHSYNC | DRM_MODE_FLAG_PVSYNC) },
	/* Terminate */
	{ DRM_MODE("", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) },
};

/**
 * vmw_guess_mode_timing - Provide fake timings for a
 * 60Hz vrefresh mode.
 *
 * @mode - Pointer to a struct drm_display_mode with hdisplay and vdisplay
 * members filled in.
 */
<<<<<<< HEAD
static void vmw_guess_mode_timing(struct drm_display_mode *mode)
=======
void vmw_guess_mode_timing(struct drm_display_mode *mode)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
{
	mode->hsync_start = mode->hdisplay + 50;
	mode->hsync_end = mode->hsync_start + 50;
	mode->htotal = mode->hsync_end + 50;

	mode->vsync_start = mode->vdisplay + 50;
	mode->vsync_end = mode->vsync_start + 50;
	mode->vtotal = mode->vsync_end + 50;

	mode->clock = (u32)mode->htotal * (u32)mode->vtotal / 100 * 6;
	mode->vrefresh = drm_mode_vrefresh(mode);
}


int vmw_du_connector_fill_modes(struct drm_connector *connector,
				uint32_t max_width, uint32_t max_height)
{
	struct vmw_display_unit *du = vmw_connector_to_du(connector);
	struct drm_device *dev = connector->dev;
	struct vmw_private *dev_priv = vmw_priv(dev);
	struct drm_display_mode *mode = NULL;
	struct drm_display_mode *bmode;
	struct drm_display_mode prefmode = { DRM_MODE("preferred",
		DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		DRM_MODE_FLAG_NHSYNC | DRM_MODE_FLAG_PVSYNC)
	};
	int i;
<<<<<<< HEAD
	u32 assumed_bpp = 2;

	/*
	 * If using screen objects, then assume 32-bpp because that's what the
	 * SVGA device is assuming
	 */
	if (dev_priv->sou_priv)
		assumed_bpp = 4;

	/* Add preferred mode */
	{
		mode = drm_mode_duplicate(dev, &prefmode);
		if (!mode)
			return 0;
		mode->hdisplay = du->pref_width;
		mode->vdisplay = du->pref_height;
		vmw_guess_mode_timing(mode);

		if (vmw_kms_validate_mode_vram(dev_priv,
						mode->hdisplay * assumed_bpp,
						mode->vdisplay)) {
			drm_mode_probed_add(connector, mode);
		} else {
			drm_mode_destroy(dev, mode);
			mode = NULL;
		}

		if (du->pref_mode) {
			list_del_init(&du->pref_mode->head);
			drm_mode_destroy(dev, du->pref_mode);
		}

		/* mode might be null here, this is intended */
		du->pref_mode = mode;
	}

=======
	u32 assumed_bpp = 4;

	if (dev_priv->assume_16bpp)
		assumed_bpp = 2;

	if (dev_priv->active_display_unit == vmw_du_screen_target) {
		max_width  = min(max_width,  dev_priv->stdu_max_width);
		max_height = min(max_height, dev_priv->stdu_max_height);
	}

	/* Add preferred mode */
	mode = drm_mode_duplicate(dev, &prefmode);
	if (!mode)
		return 0;
	mode->hdisplay = du->pref_width;
	mode->vdisplay = du->pref_height;
	vmw_guess_mode_timing(mode);

	if (vmw_kms_validate_mode_vram(dev_priv,
					mode->hdisplay * assumed_bpp,
					mode->vdisplay)) {
		drm_mode_probed_add(connector, mode);
	} else {
		drm_mode_destroy(dev, mode);
		mode = NULL;
	}

	if (du->pref_mode) {
		list_del_init(&du->pref_mode->head);
		drm_mode_destroy(dev, du->pref_mode);
	}

	/* mode might be null here, this is intended */
	du->pref_mode = mode;

>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	for (i = 0; vmw_kms_connector_builtin[i].type != 0; i++) {
		bmode = &vmw_kms_connector_builtin[i];
		if (bmode->hdisplay > max_width ||
		    bmode->vdisplay > max_height)
			continue;

		if (!vmw_kms_validate_mode_vram(dev_priv,
						bmode->hdisplay * assumed_bpp,
						bmode->vdisplay))
			continue;

		mode = drm_mode_duplicate(dev, bmode);
		if (!mode)
			return 0;
		mode->vrefresh = drm_mode_vrefresh(mode);

		drm_mode_probed_add(connector, mode);
	}

<<<<<<< HEAD
	/* Move the prefered mode first, help apps pick the right mode. */
	if (du->pref_mode)
		list_move(&du->pref_mode->head, &connector->probed_modes);

	drm_mode_connector_list_update(connector, true);
=======
	drm_mode_connector_list_update(connector, true);
	/* Move the prefered mode first, help apps pick the right mode. */
	drm_mode_sort(&connector->modes);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	return 1;
}

int vmw_du_connector_set_property(struct drm_connector *connector,
				  struct drm_property *property,
				  uint64_t val)
{
	return 0;
}


int vmw_kms_update_layout_ioctl(struct drm_device *dev, void *data,
				struct drm_file *file_priv)
{
	struct vmw_private *dev_priv = vmw_priv(dev);
	struct drm_vmw_update_layout_arg *arg =
		(struct drm_vmw_update_layout_arg *)data;
	void __user *user_rects;
	struct drm_vmw_rect *rects;
	unsigned rects_size;
	int ret;
	int i;
<<<<<<< HEAD
	struct drm_mode_config *mode_config = &dev->mode_config;
=======
	u64 total_pixels = 0;
	struct drm_mode_config *mode_config = &dev->mode_config;
	struct drm_vmw_rect bounding_box = {0};
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	if (!arg->num_outputs) {
		struct drm_vmw_rect def_rect = {0, 0, 800, 600};
		vmw_du_update_layout(dev_priv, 1, &def_rect);
		return 0;
	}

	rects_size = arg->num_outputs * sizeof(struct drm_vmw_rect);
	rects = kcalloc(arg->num_outputs, sizeof(struct drm_vmw_rect),
			GFP_KERNEL);
	if (unlikely(!rects))
		return -ENOMEM;

	user_rects = (void __user *)(unsigned long)arg->rects;
	ret = copy_from_user(rects, user_rects, rects_size);
	if (unlikely(ret != 0)) {
		DRM_ERROR("Failed to get rects.\n");
		ret = -EFAULT;
		goto out_free;
	}

	for (i = 0; i < arg->num_outputs; ++i) {
		if (rects[i].x < 0 ||
		    rects[i].y < 0 ||
		    rects[i].x + rects[i].w > mode_config->max_width ||
		    rects[i].y + rects[i].h > mode_config->max_height) {
			DRM_ERROR("Invalid GUI layout.\n");
			ret = -EINVAL;
			goto out_free;
		}
<<<<<<< HEAD
=======

		/*
		 * bounding_box.w and bunding_box.h are used as
		 * lower-right coordinates
		 */
		if (rects[i].x + rects[i].w > bounding_box.w)
			bounding_box.w = rects[i].x + rects[i].w;

		if (rects[i].y + rects[i].h > bounding_box.h)
			bounding_box.h = rects[i].y + rects[i].h;

		total_pixels += (u64) rects[i].w * (u64) rects[i].h;
	}

	if (dev_priv->active_display_unit == vmw_du_screen_target) {
		/*
		 * For Screen Targets, the limits for a toplogy are:
		 *	1. Bounding box (assuming 32bpp) must be < prim_bb_mem
		 *      2. Total pixels (assuming 32bpp) must be < prim_bb_mem
		 */
		u64 bb_mem    = bounding_box.w * bounding_box.h * 4;
		u64 pixel_mem = total_pixels * 4;

		if (bb_mem > dev_priv->prim_bb_mem) {
			DRM_ERROR("Topology is beyond supported limits.\n");
			ret = -EINVAL;
			goto out_free;
		}

		if (pixel_mem > dev_priv->prim_bb_mem) {
			DRM_ERROR("Combined output size too large\n");
			ret = -EINVAL;
			goto out_free;
		}
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	}

	vmw_du_update_layout(dev_priv, arg->num_outputs, rects);

out_free:
	kfree(rects);
	return ret;
}
<<<<<<< HEAD
=======

/**
 * vmw_kms_helper_dirty - Helper to build commands and perform actions based
 * on a set of cliprects and a set of display units.
 *
 * @dev_priv: Pointer to a device private structure.
 * @framebuffer: Pointer to the framebuffer on which to perform the actions.
 * @clips: A set of struct drm_clip_rect. Either this os @vclips must be NULL.
 * Cliprects are given in framebuffer coordinates.
 * @vclips: A set of struct drm_vmw_rect cliprects. Either this or @clips must
 * be NULL. Cliprects are given in source coordinates.
 * @dest_x: X coordinate offset for the crtc / destination clip rects.
 * @dest_y: Y coordinate offset for the crtc / destination clip rects.
 * @num_clips: Number of cliprects in the @clips or @vclips array.
 * @increment: Integer with which to increment the clip counter when looping.
 * Used to skip a predetermined number of clip rects.
 * @dirty: Closure structure. See the description of struct vmw_kms_dirty.
 */
int vmw_kms_helper_dirty(struct vmw_private *dev_priv,
			 struct vmw_framebuffer *framebuffer,
			 const struct drm_clip_rect *clips,
			 const struct drm_vmw_rect *vclips,
			 s32 dest_x, s32 dest_y,
			 int num_clips,
			 int increment,
			 struct vmw_kms_dirty *dirty)
{
	struct vmw_display_unit *units[VMWGFX_NUM_DISPLAY_UNITS];
	struct drm_crtc *crtc;
	u32 num_units = 0;
	u32 i, k;

	dirty->dev_priv = dev_priv;

	list_for_each_entry(crtc, &dev_priv->dev->mode_config.crtc_list, head) {
		if (crtc->primary->fb != &framebuffer->base)
			continue;
		units[num_units++] = vmw_crtc_to_du(crtc);
	}

	for (k = 0; k < num_units; k++) {
		struct vmw_display_unit *unit = units[k];
		s32 crtc_x = unit->crtc.x;
		s32 crtc_y = unit->crtc.y;
		s32 crtc_width = unit->crtc.mode.hdisplay;
		s32 crtc_height = unit->crtc.mode.vdisplay;
		const struct drm_clip_rect *clips_ptr = clips;
		const struct drm_vmw_rect *vclips_ptr = vclips;

		dirty->unit = unit;
		if (dirty->fifo_reserve_size > 0) {
			dirty->cmd = vmw_fifo_reserve(dev_priv,
						      dirty->fifo_reserve_size);
			if (!dirty->cmd) {
				DRM_ERROR("Couldn't reserve fifo space "
					  "for dirty blits.\n");
				return -ENOMEM;
			}
			memset(dirty->cmd, 0, dirty->fifo_reserve_size);
		}
		dirty->num_hits = 0;
		for (i = 0; i < num_clips; i++, clips_ptr += increment,
		       vclips_ptr += increment) {
			s32 clip_left;
			s32 clip_top;

			/*
			 * Select clip array type. Note that integer type
			 * in @clips is unsigned short, whereas in @vclips
			 * it's 32-bit.
			 */
			if (clips) {
				dirty->fb_x = (s32) clips_ptr->x1;
				dirty->fb_y = (s32) clips_ptr->y1;
				dirty->unit_x2 = (s32) clips_ptr->x2 + dest_x -
					crtc_x;
				dirty->unit_y2 = (s32) clips_ptr->y2 + dest_y -
					crtc_y;
			} else {
				dirty->fb_x = vclips_ptr->x;
				dirty->fb_y = vclips_ptr->y;
				dirty->unit_x2 = dirty->fb_x + vclips_ptr->w +
					dest_x - crtc_x;
				dirty->unit_y2 = dirty->fb_y + vclips_ptr->h +
					dest_y - crtc_y;
			}

			dirty->unit_x1 = dirty->fb_x + dest_x - crtc_x;
			dirty->unit_y1 = dirty->fb_y + dest_y - crtc_y;

			/* Skip this clip if it's outside the crtc region */
			if (dirty->unit_x1 >= crtc_width ||
			    dirty->unit_y1 >= crtc_height ||
			    dirty->unit_x2 <= 0 || dirty->unit_y2 <= 0)
				continue;

			/* Clip right and bottom to crtc limits */
			dirty->unit_x2 = min_t(s32, dirty->unit_x2,
					       crtc_width);
			dirty->unit_y2 = min_t(s32, dirty->unit_y2,
					       crtc_height);

			/* Clip left and top to crtc limits */
			clip_left = min_t(s32, dirty->unit_x1, 0);
			clip_top = min_t(s32, dirty->unit_y1, 0);
			dirty->unit_x1 -= clip_left;
			dirty->unit_y1 -= clip_top;
			dirty->fb_x -= clip_left;
			dirty->fb_y -= clip_top;

			dirty->clip(dirty);
		}

		dirty->fifo_commit(dirty);
	}

	return 0;
}

/**
 * vmw_kms_helper_buffer_prepare - Reserve and validate a buffer object before
 * command submission.
 *
 * @dev_priv. Pointer to a device private structure.
 * @buf: The buffer object
 * @interruptible: Whether to perform waits as interruptible.
 * @validate_as_mob: Whether the buffer should be validated as a MOB. If false,
 * The buffer will be validated as a GMR. Already pinned buffers will not be
 * validated.
 *
 * Returns 0 on success, negative error code on failure, -ERESTARTSYS if
 * interrupted by a signal.
 */
int vmw_kms_helper_buffer_prepare(struct vmw_private *dev_priv,
				  struct vmw_dma_buffer *buf,
				  bool interruptible,
				  bool validate_as_mob)
{
	struct ttm_buffer_object *bo = &buf->base;
	int ret;

	ttm_bo_reserve(bo, false, false, interruptible, NULL);
	ret = vmw_validate_single_buffer(dev_priv, bo, interruptible,
					 validate_as_mob);
	if (ret)
		ttm_bo_unreserve(bo);

	return ret;
}

/**
 * vmw_kms_helper_buffer_revert - Undo the actions of
 * vmw_kms_helper_buffer_prepare.
 *
 * @res: Pointer to the buffer object.
 *
 * Helper to be used if an error forces the caller to undo the actions of
 * vmw_kms_helper_buffer_prepare.
 */
void vmw_kms_helper_buffer_revert(struct vmw_dma_buffer *buf)
{
	if (buf)
		ttm_bo_unreserve(&buf->base);
}

/**
 * vmw_kms_helper_buffer_finish - Unreserve and fence a buffer object after
 * kms command submission.
 *
 * @dev_priv: Pointer to a device private structure.
 * @file_priv: Pointer to a struct drm_file representing the caller's
 * connection. Must be set to NULL if @user_fence_rep is NULL, and conversely
 * if non-NULL, @user_fence_rep must be non-NULL.
 * @buf: The buffer object.
 * @out_fence:  Optional pointer to a fence pointer. If non-NULL, a
 * ref-counted fence pointer is returned here.
 * @user_fence_rep: Optional pointer to a user-space provided struct
 * drm_vmw_fence_rep. If provided, @file_priv must also be provided and the
 * function copies fence data to user-space in a fail-safe manner.
 */
void vmw_kms_helper_buffer_finish(struct vmw_private *dev_priv,
				  struct drm_file *file_priv,
				  struct vmw_dma_buffer *buf,
				  struct vmw_fence_obj **out_fence,
				  struct drm_vmw_fence_rep __user *
				  user_fence_rep)
{
	struct vmw_fence_obj *fence;
	uint32_t handle;
	int ret;

	ret = vmw_execbuf_fence_commands(file_priv, dev_priv, &fence,
					 file_priv ? &handle : NULL);
	if (buf)
		vmw_fence_single_bo(&buf->base, fence);
	if (file_priv)
		vmw_execbuf_copy_fence_user(dev_priv, vmw_fpriv(file_priv),
					    ret, user_fence_rep, fence,
					    handle);
	if (out_fence)
		*out_fence = fence;
	else
		vmw_fence_obj_unreference(&fence);

	vmw_kms_helper_buffer_revert(buf);
}


/**
 * vmw_kms_helper_resource_revert - Undo the actions of
 * vmw_kms_helper_resource_prepare.
 *
 * @res: Pointer to the resource. Typically a surface.
 *
 * Helper to be used if an error forces the caller to undo the actions of
 * vmw_kms_helper_resource_prepare.
 */
void vmw_kms_helper_resource_revert(struct vmw_resource *res)
{
	vmw_kms_helper_buffer_revert(res->backup);
	vmw_resource_unreserve(res, false, NULL, 0);
	mutex_unlock(&res->dev_priv->cmdbuf_mutex);
}

/**
 * vmw_kms_helper_resource_prepare - Reserve and validate a resource before
 * command submission.
 *
 * @res: Pointer to the resource. Typically a surface.
 * @interruptible: Whether to perform waits as interruptible.
 *
 * Reserves and validates also the backup buffer if a guest-backed resource.
 * Returns 0 on success, negative error code on failure. -ERESTARTSYS if
 * interrupted by a signal.
 */
int vmw_kms_helper_resource_prepare(struct vmw_resource *res,
				    bool interruptible)
{
	int ret = 0;

	if (interruptible)
		ret = mutex_lock_interruptible(&res->dev_priv->cmdbuf_mutex);
	else
		mutex_lock(&res->dev_priv->cmdbuf_mutex);

	if (unlikely(ret != 0))
		return -ERESTARTSYS;

	ret = vmw_resource_reserve(res, interruptible, false);
	if (ret)
		goto out_unlock;

	if (res->backup) {
		ret = vmw_kms_helper_buffer_prepare(res->dev_priv, res->backup,
						    interruptible,
						    res->dev_priv->has_mob);
		if (ret)
			goto out_unreserve;
	}
	ret = vmw_resource_validate(res);
	if (ret)
		goto out_revert;
	return 0;

out_revert:
	vmw_kms_helper_buffer_revert(res->backup);
out_unreserve:
	vmw_resource_unreserve(res, false, NULL, 0);
out_unlock:
	mutex_unlock(&res->dev_priv->cmdbuf_mutex);
	return ret;
}

/**
 * vmw_kms_helper_resource_finish - Unreserve and fence a resource after
 * kms command submission.
 *
 * @res: Pointer to the resource. Typically a surface.
 * @out_fence: Optional pointer to a fence pointer. If non-NULL, a
 * ref-counted fence pointer is returned here.
 */
void vmw_kms_helper_resource_finish(struct vmw_resource *res,
			     struct vmw_fence_obj **out_fence)
{
	if (res->backup || out_fence)
		vmw_kms_helper_buffer_finish(res->dev_priv, NULL, res->backup,
					     out_fence, NULL);

	vmw_resource_unreserve(res, false, NULL, 0);
	mutex_unlock(&res->dev_priv->cmdbuf_mutex);
}

/**
 * vmw_kms_update_proxy - Helper function to update a proxy surface from
 * its backing MOB.
 *
 * @res: Pointer to the surface resource
 * @clips: Clip rects in framebuffer (surface) space.
 * @num_clips: Number of clips in @clips.
 * @increment: Integer with which to increment the clip counter when looping.
 * Used to skip a predetermined number of clip rects.
 *
 * This function makes sure the proxy surface is updated from its backing MOB
 * using the region given by @clips. The surface resource @res and its backing
 * MOB needs to be reserved and validated on call.
 */
int vmw_kms_update_proxy(struct vmw_resource *res,
			 const struct drm_clip_rect *clips,
			 unsigned num_clips,
			 int increment)
{
	struct vmw_private *dev_priv = res->dev_priv;
	struct drm_vmw_size *size = &vmw_res_to_srf(res)->base_size;
	struct {
		SVGA3dCmdHeader header;
		SVGA3dCmdUpdateGBImage body;
	} *cmd;
	SVGA3dBox *box;
	size_t copy_size = 0;
	int i;

	if (!clips)
		return 0;

	cmd = vmw_fifo_reserve(dev_priv, sizeof(*cmd) * num_clips);
	if (!cmd) {
		DRM_ERROR("Couldn't reserve fifo space for proxy surface "
			  "update.\n");
		return -ENOMEM;
	}

	for (i = 0; i < num_clips; ++i, clips += increment, ++cmd) {
		box = &cmd->body.box;

		cmd->header.id = SVGA_3D_CMD_UPDATE_GB_IMAGE;
		cmd->header.size = sizeof(cmd->body);
		cmd->body.image.sid = res->id;
		cmd->body.image.face = 0;
		cmd->body.image.mipmap = 0;

		if (clips->x1 > size->width || clips->x2 > size->width ||
		    clips->y1 > size->height || clips->y2 > size->height) {
			DRM_ERROR("Invalid clips outsize of framebuffer.\n");
			return -EINVAL;
		}

		box->x = clips->x1;
		box->y = clips->y1;
		box->z = 0;
		box->w = clips->x2 - clips->x1;
		box->h = clips->y2 - clips->y1;
		box->d = 1;

		copy_size += sizeof(*cmd);
	}

	vmw_fifo_commit(dev_priv, copy_size);

	return 0;
}

int vmw_kms_fbdev_init_data(struct vmw_private *dev_priv,
			    unsigned unit,
			    u32 max_width,
			    u32 max_height,
			    struct drm_connector **p_con,
			    struct drm_crtc **p_crtc,
			    struct drm_display_mode **p_mode)
{
	struct drm_connector *con;
	struct vmw_display_unit *du;
	struct drm_display_mode *mode;
	int i = 0;

	list_for_each_entry(con, &dev_priv->dev->mode_config.connector_list,
			    head) {
		if (i == unit)
			break;

		++i;
	}

	if (i != unit) {
		DRM_ERROR("Could not find initial display unit.\n");
		return -EINVAL;
	}

	if (list_empty(&con->modes))
		(void) vmw_du_connector_fill_modes(con, max_width, max_height);

	if (list_empty(&con->modes)) {
		DRM_ERROR("Could not find initial display mode.\n");
		return -EINVAL;
	}

	du = vmw_connector_to_du(con);
	*p_con = con;
	*p_crtc = &du->crtc;

	list_for_each_entry(mode, &con->modes, head) {
		if (mode->type & DRM_MODE_TYPE_PREFERRED)
			break;
	}

	if (mode->type & DRM_MODE_TYPE_PREFERRED)
		*p_mode = mode;
	else {
		WARN_ONCE(true, "Could not find initial preferred mode.\n");
		*p_mode = list_first_entry(&con->modes,
					   struct drm_display_mode,
					   head);
	}

	return 0;
}
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
