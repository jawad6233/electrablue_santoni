/*
<<<<<<< HEAD
=======
 * Copyright (c) 2014-2015 The Linux Foundation. All rights reserved.
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
 * Copyright (C) 2013 Red Hat
 * Author: Rob Clark <robdclark@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mdp5_kms.h"

<<<<<<< HEAD
=======
#include <linux/sort.h>
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
#include <drm/drm_mode.h>
#include "drm_crtc.h"
#include "drm_crtc_helper.h"
#include "drm_flip_work.h"
<<<<<<< HEAD
=======
#include "mdp5_plane.h"

#define CURSOR_WIDTH	64
#define CURSOR_HEIGHT	64

#define SSPP_MAX	(SSPP_RGB3 + 1) /* TODO: Add SSPP_MAX in mdp5.xml.h */
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

struct mdp5_crtc {
	struct drm_crtc base;
	char name[8];
<<<<<<< HEAD
	struct drm_plane *plane;
	struct drm_plane *planes[8];
	int id;
	bool enabled;

	/* which mixer/encoder we route output to: */
	int mixer;

	/* if there is a pending flip, these will be non-null: */
	struct drm_pending_vblank_event *event;
	struct msm_fence_cb pageflip_cb;
=======
	int id;
	bool enabled;

	/* layer mixer used for this CRTC (+ its lock): */
#define GET_LM_ID(crtc_id)	((crtc_id == 3) ? 5 : crtc_id)
	int lm;
	spinlock_t lm_lock;	/* protect REG_MDP5_LM_* registers */

	/* CTL used for this CRTC: */
	struct mdp5_ctl *ctl;

	/* if there is a pending flip, these will be non-null: */
	struct drm_pending_vblank_event *event;

	/* Bits have been flushed at the last commit,
	 * used to decide if a vsync has happened since last commit.
	 */
	u32 flushed_mask;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

#define PENDING_CURSOR 0x1
#define PENDING_FLIP   0x2
	atomic_t pending;

<<<<<<< HEAD
	/* the fb that we logically (from PoV of KMS API) hold a ref
	 * to.  Which we may not yet be scanning out (we may still
	 * be scanning out previous in case of page_flip while waiting
	 * for gpu rendering to complete:
	 */
	struct drm_framebuffer *fb;

	/* the fb that we currently hold a scanout ref to: */
	struct drm_framebuffer *scanout_fb;

	/* for unref'ing framebuffers after scanout completes: */
	struct drm_flip_work unref_fb_work;

	struct mdp_irq vblank;
	struct mdp_irq err;
=======
	/* for unref'ing cursor bo's after scanout completes: */
	struct drm_flip_work unref_cursor_work;

	struct mdp_irq vblank;
	struct mdp_irq err;
	struct mdp_irq pp_done;

	struct completion pp_completion;

	bool cmd_mode;

	struct {
		/* protect REG_MDP5_LM_CURSOR* registers and cursor scanout_bo*/
		spinlock_t lock;

		/* current cursor being scanned out: */
		struct drm_gem_object *scanout_bo;
		uint32_t width, height;
		uint32_t x, y;
	} cursor;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
};
#define to_mdp5_crtc(x) container_of(x, struct mdp5_crtc, base)

static struct mdp5_kms *get_kms(struct drm_crtc *crtc)
{
	struct msm_drm_private *priv = crtc->dev->dev_private;
	return to_mdp5_kms(to_mdp_kms(priv->kms));
}

static void request_pending(struct drm_crtc *crtc, uint32_t pending)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);

	atomic_or(pending, &mdp5_crtc->pending);
	mdp_irq_register(&get_kms(crtc)->base, &mdp5_crtc->vblank);
}

<<<<<<< HEAD
static void crtc_flush(struct drm_crtc *crtc)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	struct mdp5_kms *mdp5_kms = get_kms(crtc);
	int id = mdp5_crtc->id;
	uint32_t i, flush = 0;

	for (i = 0; i < ARRAY_SIZE(mdp5_crtc->planes); i++) {
		struct drm_plane *plane = mdp5_crtc->planes[i];
		if (plane) {
			enum mdp5_pipe pipe = mdp5_plane_pipe(plane);
			flush |= pipe2flush(pipe);
		}
	}
	flush |= mixer2flush(mdp5_crtc->id);
	flush |= MDP5_CTL_FLUSH_CTL;

	DBG("%s: flush=%08x", mdp5_crtc->name, flush);

	mdp5_write(mdp5_kms, REG_MDP5_CTL_FLUSH(id), flush);
}

static void update_fb(struct drm_crtc *crtc, struct drm_framebuffer *new_fb)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	struct drm_framebuffer *old_fb = mdp5_crtc->fb;

	/* grab reference to incoming scanout fb: */
	drm_framebuffer_reference(new_fb);
	mdp5_crtc->base.primary->fb = new_fb;
	mdp5_crtc->fb = new_fb;

	if (old_fb)
		drm_flip_work_queue(&mdp5_crtc->unref_fb_work, old_fb);
}

/* unlike update_fb(), take a ref to the new scanout fb *before* updating
 * plane, then call this.  Needed to ensure we don't unref the buffer that
 * is actually still being scanned out.
 *
 * Note that this whole thing goes away with atomic.. since we can defer
 * calling into driver until rendering is done.
 */
static void update_scanout(struct drm_crtc *crtc, struct drm_framebuffer *fb)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);

	/* flush updates, to make sure hw is updated to new scanout fb,
	 * so that we can safely queue unref to current fb (ie. next
	 * vblank we know hw is done w/ previous scanout_fb).
	 */
	crtc_flush(crtc);

	if (mdp5_crtc->scanout_fb)
		drm_flip_work_queue(&mdp5_crtc->unref_fb_work,
				mdp5_crtc->scanout_fb);

	mdp5_crtc->scanout_fb = fb;

	/* enable vblank to complete flip: */
	request_pending(crtc, PENDING_FLIP);
=======
static void request_pp_done_pending(struct drm_crtc *crtc)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	reinit_completion(&mdp5_crtc->pp_completion);
}

static u32 crtc_flush(struct drm_crtc *crtc, u32 flush_mask)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);

	DBG("%s: flush=%08x", mdp5_crtc->name, flush_mask);
	return mdp5_ctl_commit(mdp5_crtc->ctl, flush_mask);
}

/*
 * flush updates, to make sure hw is updated to new scanout fb,
 * so that we can safely queue unref to current fb (ie. next
 * vblank we know hw is done w/ previous scanout_fb).
 */
static u32 crtc_flush_all(struct drm_crtc *crtc)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	struct drm_plane *plane;
	uint32_t flush_mask = 0;

	/* this should not happen: */
	if (WARN_ON(!mdp5_crtc->ctl))
		return 0;

	drm_atomic_crtc_for_each_plane(plane, crtc) {
		flush_mask |= mdp5_plane_get_flush(plane);
	}

	flush_mask |= mdp_ctl_flush_mask_lm(mdp5_crtc->lm);

	return crtc_flush(crtc, flush_mask);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

/* if file!=NULL, this is preclose potential cancel-flip path */
static void complete_flip(struct drm_crtc *crtc, struct drm_file *file)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	struct drm_device *dev = crtc->dev;
	struct drm_pending_vblank_event *event;
<<<<<<< HEAD
	unsigned long flags, i;
=======
	struct drm_plane *plane;
	unsigned long flags;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	spin_lock_irqsave(&dev->event_lock, flags);
	event = mdp5_crtc->event;
	if (event) {
		/* if regular vblank case (!file) or if cancel-flip from
		 * preclose on file that requested flip, then send the
		 * event:
		 */
		if (!file || (event->base.file_priv == file)) {
			mdp5_crtc->event = NULL;
<<<<<<< HEAD
=======
			DBG("%s: send event: %p", mdp5_crtc->name, event);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
			drm_send_vblank_event(dev, mdp5_crtc->id, event);
		}
	}
	spin_unlock_irqrestore(&dev->event_lock, flags);

<<<<<<< HEAD
	for (i = 0; i < ARRAY_SIZE(mdp5_crtc->planes); i++) {
		struct drm_plane *plane = mdp5_crtc->planes[i];
		if (plane)
			mdp5_plane_complete_flip(plane);
	}
}

static void pageflip_cb(struct msm_fence_cb *cb)
{
	struct mdp5_crtc *mdp5_crtc =
		container_of(cb, struct mdp5_crtc, pageflip_cb);
	struct drm_crtc *crtc = &mdp5_crtc->base;
	struct drm_framebuffer *fb = mdp5_crtc->fb;

	if (!fb)
		return;

	drm_framebuffer_reference(fb);
	mdp5_plane_set_scanout(mdp5_crtc->plane, fb);
	update_scanout(crtc, fb);
}

static void unref_fb_worker(struct drm_flip_work *work, void *val)
{
	struct mdp5_crtc *mdp5_crtc =
		container_of(work, struct mdp5_crtc, unref_fb_work);
	struct drm_device *dev = mdp5_crtc->base.dev;

	mutex_lock(&dev->mode_config.mutex);
	drm_framebuffer_unreference(val);
	mutex_unlock(&dev->mode_config.mutex);
=======
	drm_atomic_crtc_for_each_plane(plane, crtc) {
		mdp5_plane_complete_flip(plane);
	}

	if (mdp5_crtc->ctl && !crtc->state->enable) {
		/* set STAGE_UNUSED for all layers */
		mdp5_ctl_blend(mdp5_crtc->ctl, NULL, 0, 0);
		mdp5_crtc->ctl = NULL;
	}
}

static void unref_cursor_worker(struct drm_flip_work *work, void *val)
{
	struct mdp5_crtc *mdp5_crtc =
		container_of(work, struct mdp5_crtc, unref_cursor_work);
	struct mdp5_kms *mdp5_kms = get_kms(&mdp5_crtc->base);

	msm_gem_put_iova(val, mdp5_kms->id);
	drm_gem_object_unreference_unlocked(val);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

static void mdp5_crtc_destroy(struct drm_crtc *crtc)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);

	drm_crtc_cleanup(crtc);
<<<<<<< HEAD
	drm_flip_work_cleanup(&mdp5_crtc->unref_fb_work);
=======
	drm_flip_work_cleanup(&mdp5_crtc->unref_cursor_work);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	kfree(mdp5_crtc);
}

<<<<<<< HEAD
static void mdp5_crtc_dpms(struct drm_crtc *crtc, int mode)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	struct mdp5_kms *mdp5_kms = get_kms(crtc);
	bool enabled = (mode == DRM_MODE_DPMS_ON);

	DBG("%s: mode=%d", mdp5_crtc->name, mode);

	if (enabled != mdp5_crtc->enabled) {
		if (enabled) {
			mdp5_enable(mdp5_kms);
			mdp_irq_register(&mdp5_kms->base, &mdp5_crtc->err);
		} else {
			mdp_irq_unregister(&mdp5_kms->base, &mdp5_crtc->err);
			mdp5_disable(mdp5_kms);
		}
		mdp5_crtc->enabled = enabled;
	}
}

=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
static bool mdp5_crtc_mode_fixup(struct drm_crtc *crtc,
		const struct drm_display_mode *mode,
		struct drm_display_mode *adjusted_mode)
{
	return true;
}

<<<<<<< HEAD
=======
/*
 * blend_setup() - blend all the planes of a CRTC
 *
 * If no base layer is available, border will be enabled as the base layer.
 * Otherwise all layers will be blended based on their stage calculated
 * in mdp5_crtc_atomic_check.
 */
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
static void blend_setup(struct drm_crtc *crtc)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	struct mdp5_kms *mdp5_kms = get_kms(crtc);
<<<<<<< HEAD
	int id = mdp5_crtc->id;

	/*
	 * Hard-coded setup for now until I figure out how the
	 * layer-mixer works
	 */

	/* LM[id]: */
	mdp5_write(mdp5_kms, REG_MDP5_LM_BLEND_COLOR_OUT(id),
			MDP5_LM_BLEND_COLOR_OUT_STAGE0_FG_ALPHA);
	mdp5_write(mdp5_kms, REG_MDP5_LM_BLEND_OP_MODE(id, 0),
			MDP5_LM_BLEND_OP_MODE_FG_ALPHA(FG_CONST) |
			MDP5_LM_BLEND_OP_MODE_BG_ALPHA(FG_PIXEL) |
			MDP5_LM_BLEND_OP_MODE_BG_INV_ALPHA);
	mdp5_write(mdp5_kms, REG_MDP5_LM_BLEND_FG_ALPHA(id, 0), 0xff);
	mdp5_write(mdp5_kms, REG_MDP5_LM_BLEND_BG_ALPHA(id, 0), 0x00);

	/* NOTE: seems that LM[n] and CTL[m], we do not need n==m.. but
	 * we want to be setting CTL[m].LAYER[n].  Not sure what the
	 * point of having CTL[m].LAYER[o] (for o!=n).. maybe that is
	 * used when chaining up mixers for high resolution displays?
	 */

	/* CTL[id]: */
	mdp5_write(mdp5_kms, REG_MDP5_CTL_LAYER_REG(id, 0),
			MDP5_CTL_LAYER_REG_RGB0(STAGE0) |
			MDP5_CTL_LAYER_REG_BORDER_COLOR);
	mdp5_write(mdp5_kms, REG_MDP5_CTL_LAYER_REG(id, 1), 0);
	mdp5_write(mdp5_kms, REG_MDP5_CTL_LAYER_REG(id, 2), 0);
	mdp5_write(mdp5_kms, REG_MDP5_CTL_LAYER_REG(id, 3), 0);
	mdp5_write(mdp5_kms, REG_MDP5_CTL_LAYER_REG(id, 4), 0);
}

static int mdp5_crtc_mode_set(struct drm_crtc *crtc,
		struct drm_display_mode *mode,
		struct drm_display_mode *adjusted_mode,
		int x, int y,
		struct drm_framebuffer *old_fb)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	struct mdp5_kms *mdp5_kms = get_kms(crtc);
	int ret;

	mode = adjusted_mode;
=======
	struct drm_plane *plane;
	const struct mdp5_cfg_hw *hw_cfg;
	struct mdp5_plane_state *pstate, *pstates[STAGE_MAX + 1] = {NULL};
	const struct mdp_format *format;
	uint32_t lm = mdp5_crtc->lm;
	uint32_t blend_op, fg_alpha, bg_alpha, ctl_blend_flags = 0;
	unsigned long flags;
	uint8_t stage[STAGE_MAX + 1];
	int i, plane_cnt = 0;
#define blender(stage)	((stage) - STAGE0)

	hw_cfg = mdp5_cfg_get_hw_config(mdp5_kms->cfg);

	spin_lock_irqsave(&mdp5_crtc->lm_lock, flags);

	/* ctl could be released already when we are shutting down: */
	if (!mdp5_crtc->ctl)
		goto out;

	/* Collect all plane information */
	drm_atomic_crtc_for_each_plane(plane, crtc) {
		pstate = to_mdp5_plane_state(plane->state);
		pstates[pstate->stage] = pstate;
		stage[pstate->stage] = mdp5_plane_pipe(plane);
		plane_cnt++;
	}

	/*
	* If there is no base layer, enable border color.
	* Although it's not possbile in current blend logic,
	* put it here as a reminder.
	*/
	if (!pstates[STAGE_BASE] && plane_cnt) {
		ctl_blend_flags |= MDP5_CTL_BLEND_OP_FLAG_BORDER_OUT;
		DBG("Border Color is enabled");
	}

	/* The reset for blending */
	for (i = STAGE0; i <= STAGE_MAX; i++) {
		if (!pstates[i])
			continue;

		format = to_mdp_format(
			msm_framebuffer_format(pstates[i]->base.fb));
		plane = pstates[i]->base.plane;
		blend_op = MDP5_LM_BLEND_OP_MODE_FG_ALPHA(FG_CONST) |
			MDP5_LM_BLEND_OP_MODE_BG_ALPHA(BG_CONST);
		fg_alpha = pstates[i]->alpha;
		bg_alpha = 0xFF - pstates[i]->alpha;
		DBG("Stage %d fg_alpha %x bg_alpha %x", i, fg_alpha, bg_alpha);

		if (format->alpha_enable && pstates[i]->premultiplied) {
			blend_op = MDP5_LM_BLEND_OP_MODE_FG_ALPHA(FG_CONST) |
				MDP5_LM_BLEND_OP_MODE_BG_ALPHA(FG_PIXEL);
			if (fg_alpha != 0xff) {
				bg_alpha = fg_alpha;
				blend_op |=
					MDP5_LM_BLEND_OP_MODE_BG_MOD_ALPHA |
					MDP5_LM_BLEND_OP_MODE_BG_INV_MOD_ALPHA;
			} else {
				blend_op |= MDP5_LM_BLEND_OP_MODE_BG_INV_ALPHA;
			}
		} else if (format->alpha_enable) {
			blend_op = MDP5_LM_BLEND_OP_MODE_FG_ALPHA(FG_PIXEL) |
				MDP5_LM_BLEND_OP_MODE_BG_ALPHA(FG_PIXEL);
			if (fg_alpha != 0xff) {
				bg_alpha = fg_alpha;
				blend_op |=
				       MDP5_LM_BLEND_OP_MODE_FG_MOD_ALPHA |
				       MDP5_LM_BLEND_OP_MODE_FG_INV_MOD_ALPHA |
				       MDP5_LM_BLEND_OP_MODE_BG_MOD_ALPHA |
				       MDP5_LM_BLEND_OP_MODE_BG_INV_MOD_ALPHA;
			} else {
				blend_op |= MDP5_LM_BLEND_OP_MODE_BG_INV_ALPHA;
			}
		}

		mdp5_write(mdp5_kms, REG_MDP5_LM_BLEND_OP_MODE(lm,
				blender(i)), blend_op);
		mdp5_write(mdp5_kms, REG_MDP5_LM_BLEND_FG_ALPHA(lm,
				blender(i)), fg_alpha);
		mdp5_write(mdp5_kms, REG_MDP5_LM_BLEND_BG_ALPHA(lm,
				blender(i)), bg_alpha);
	}

	mdp5_ctl_blend(mdp5_crtc->ctl, stage, plane_cnt, ctl_blend_flags);

out:
	spin_unlock_irqrestore(&mdp5_crtc->lm_lock, flags);
}

static void mdp5_crtc_mode_set_nofb(struct drm_crtc *crtc)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	struct mdp5_kms *mdp5_kms = get_kms(crtc);
	unsigned long flags;
	struct drm_display_mode *mode;

	if (WARN_ON(!crtc->state))
		return;

	mode = &crtc->state->adjusted_mode;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	DBG("%s: set mode: %d:\"%s\" %d %d %d %d %d %d %d %d %d %d 0x%x 0x%x",
			mdp5_crtc->name, mode->base.id, mode->name,
			mode->vrefresh, mode->clock,
			mode->hdisplay, mode->hsync_start,
			mode->hsync_end, mode->htotal,
			mode->vdisplay, mode->vsync_start,
			mode->vsync_end, mode->vtotal,
			mode->type, mode->flags);

<<<<<<< HEAD
	/* grab extra ref for update_scanout() */
	drm_framebuffer_reference(crtc->primary->fb);

	ret = mdp5_plane_mode_set(mdp5_crtc->plane, crtc, crtc->primary->fb,
			0, 0, mode->hdisplay, mode->vdisplay,
			x << 16, y << 16,
			mode->hdisplay << 16, mode->vdisplay << 16);
	if (ret) {
		drm_framebuffer_unreference(crtc->primary->fb);
		dev_err(crtc->dev->dev, "%s: failed to set mode on plane: %d\n",
				mdp5_crtc->name, ret);
		return ret;
	}

	mdp5_write(mdp5_kms, REG_MDP5_LM_OUT_SIZE(mdp5_crtc->id),
			MDP5_LM_OUT_SIZE_WIDTH(mode->hdisplay) |
			MDP5_LM_OUT_SIZE_HEIGHT(mode->vdisplay));

	update_fb(crtc, crtc->primary->fb);
	update_scanout(crtc, crtc->primary->fb);

	return 0;
}

static void mdp5_crtc_prepare(struct drm_crtc *crtc)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	DBG("%s", mdp5_crtc->name);
	/* make sure we hold a ref to mdp clks while setting up mode: */
	mdp5_enable(get_kms(crtc));
	mdp5_crtc_dpms(crtc, DRM_MODE_DPMS_OFF);
}

static void mdp5_crtc_commit(struct drm_crtc *crtc)
{
	mdp5_crtc_dpms(crtc, DRM_MODE_DPMS_ON);
	crtc_flush(crtc);
	/* drop the ref to mdp clk's that we got in prepare: */
	mdp5_disable(get_kms(crtc));
}

static int mdp5_crtc_mode_set_base(struct drm_crtc *crtc, int x, int y,
		struct drm_framebuffer *old_fb)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	struct drm_plane *plane = mdp5_crtc->plane;
	struct drm_display_mode *mode = &crtc->mode;
	int ret;

	/* grab extra ref for update_scanout() */
	drm_framebuffer_reference(crtc->primary->fb);

	ret = mdp5_plane_mode_set(plane, crtc, crtc->primary->fb,
			0, 0, mode->hdisplay, mode->vdisplay,
			x << 16, y << 16,
			mode->hdisplay << 16, mode->vdisplay << 16);
	if (ret) {
		drm_framebuffer_unreference(crtc->primary->fb);
		return ret;
	}

	update_fb(crtc, crtc->primary->fb);
	update_scanout(crtc, crtc->primary->fb);
=======
	spin_lock_irqsave(&mdp5_crtc->lm_lock, flags);
	mdp5_write(mdp5_kms, REG_MDP5_LM_OUT_SIZE(mdp5_crtc->lm),
			MDP5_LM_OUT_SIZE_WIDTH(mode->hdisplay) |
			MDP5_LM_OUT_SIZE_HEIGHT(mode->vdisplay));
	spin_unlock_irqrestore(&mdp5_crtc->lm_lock, flags);
}

static void mdp5_crtc_disable(struct drm_crtc *crtc)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	struct mdp5_kms *mdp5_kms = get_kms(crtc);

	DBG("%s", mdp5_crtc->name);

	if (WARN_ON(!mdp5_crtc->enabled))
		return;

	if (mdp5_crtc->cmd_mode)
		mdp_irq_unregister(&mdp5_kms->base, &mdp5_crtc->pp_done);

	mdp_irq_unregister(&mdp5_kms->base, &mdp5_crtc->err);
	mdp5_disable(mdp5_kms);

	mdp5_crtc->enabled = false;
}

static void mdp5_crtc_enable(struct drm_crtc *crtc)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	struct mdp5_kms *mdp5_kms = get_kms(crtc);

	DBG("%s", mdp5_crtc->name);

	if (WARN_ON(mdp5_crtc->enabled))
		return;

	mdp5_enable(mdp5_kms);
	mdp_irq_register(&mdp5_kms->base, &mdp5_crtc->err);

	if (mdp5_crtc->cmd_mode)
		mdp_irq_register(&mdp5_kms->base, &mdp5_crtc->pp_done);

	mdp5_crtc->enabled = true;
}

struct plane_state {
	struct drm_plane *plane;
	struct mdp5_plane_state *state;
};

static int pstate_cmp(const void *a, const void *b)
{
	struct plane_state *pa = (struct plane_state *)a;
	struct plane_state *pb = (struct plane_state *)b;
	return pa->state->zpos - pb->state->zpos;
}

static int mdp5_crtc_atomic_check(struct drm_crtc *crtc,
		struct drm_crtc_state *state)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	struct mdp5_kms *mdp5_kms = get_kms(crtc);
	struct drm_plane *plane;
	struct drm_device *dev = crtc->dev;
	struct plane_state pstates[STAGE_MAX + 1];
	const struct mdp5_cfg_hw *hw_cfg;
	int cnt = 0, i;

	DBG("%s: check", mdp5_crtc->name);

	/* verify that there are not too many planes attached to crtc
	 * and that we don't have conflicting mixer stages:
	 */
	hw_cfg = mdp5_cfg_get_hw_config(mdp5_kms->cfg);
	drm_atomic_crtc_state_for_each_plane(plane, state) {
		struct drm_plane_state *pstate;
		if (cnt >= (hw_cfg->lm.nb_stages)) {
			dev_err(dev->dev, "too many planes!\n");
			return -EINVAL;
		}

		pstate = state->state->plane_states[drm_plane_index(plane)];

		/* plane might not have changed, in which case take
		 * current state:
		 */
		if (!pstate)
			pstate = plane->state;
		pstates[cnt].plane = plane;
		pstates[cnt].state = to_mdp5_plane_state(pstate);

		cnt++;
	}

	/* assign a stage based on sorted zpos property */
	sort(pstates, cnt, sizeof(pstates[0]), pstate_cmp, NULL);

	for (i = 0; i < cnt; i++) {
		pstates[i].state->stage = STAGE_BASE + i;
		DBG("%s: assign pipe %s on stage=%d", mdp5_crtc->name,
				pipe2name(mdp5_plane_pipe(pstates[i].plane)),
				pstates[i].state->stage);
	}
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	return 0;
}

<<<<<<< HEAD
static void mdp5_crtc_load_lut(struct drm_crtc *crtc)
{
}

static int mdp5_crtc_page_flip(struct drm_crtc *crtc,
		struct drm_framebuffer *new_fb,
		struct drm_pending_vblank_event *event,
		uint32_t page_flip_flags)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	struct drm_device *dev = crtc->dev;
	struct drm_gem_object *obj;
	unsigned long flags;

	if (mdp5_crtc->event) {
		dev_err(dev->dev, "already pending flip!\n");
		return -EBUSY;
	}

	obj = msm_framebuffer_bo(new_fb, 0);

	spin_lock_irqsave(&dev->event_lock, flags);
	mdp5_crtc->event = event;
	spin_unlock_irqrestore(&dev->event_lock, flags);

	update_fb(crtc, new_fb);

	return msm_gem_queue_inactive_cb(obj, &mdp5_crtc->pageflip_cb);
=======
static void mdp5_crtc_atomic_begin(struct drm_crtc *crtc,
				   struct drm_crtc_state *old_crtc_state)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	DBG("%s: begin", mdp5_crtc->name);
}

static void mdp5_crtc_atomic_flush(struct drm_crtc *crtc,
				   struct drm_crtc_state *old_crtc_state)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	struct drm_device *dev = crtc->dev;
	unsigned long flags;

	DBG("%s: event: %p", mdp5_crtc->name, crtc->state->event);

	WARN_ON(mdp5_crtc->event);

	spin_lock_irqsave(&dev->event_lock, flags);
	mdp5_crtc->event = crtc->state->event;
	spin_unlock_irqrestore(&dev->event_lock, flags);

	/*
	 * If no CTL has been allocated in mdp5_crtc_atomic_check(),
	 * it means we are trying to flush a CRTC whose state is disabled:
	 * nothing else needs to be done.
	 */
	if (unlikely(!mdp5_crtc->ctl))
		return;

	blend_setup(crtc);

	/* PP_DONE irq is only used by command mode for now.
	 * It is better to request pending before FLUSH and START trigger
	 * to make sure no pp_done irq missed.
	 * This is safe because no pp_done will happen before SW trigger
	 * in command mode.
	 */
	if (mdp5_crtc->cmd_mode)
		request_pp_done_pending(crtc);

	mdp5_crtc->flushed_mask = crtc_flush_all(crtc);

	request_pending(crtc, PENDING_FLIP);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

static int mdp5_crtc_set_property(struct drm_crtc *crtc,
		struct drm_property *property, uint64_t val)
{
	// XXX
	return -EINVAL;
}

<<<<<<< HEAD
static const struct drm_crtc_funcs mdp5_crtc_funcs = {
	.set_config = drm_crtc_helper_set_config,
	.destroy = mdp5_crtc_destroy,
	.page_flip = mdp5_crtc_page_flip,
	.set_property = mdp5_crtc_set_property,
};

static const struct drm_crtc_helper_funcs mdp5_crtc_helper_funcs = {
	.dpms = mdp5_crtc_dpms,
	.mode_fixup = mdp5_crtc_mode_fixup,
	.mode_set = mdp5_crtc_mode_set,
	.prepare = mdp5_crtc_prepare,
	.commit = mdp5_crtc_commit,
	.mode_set_base = mdp5_crtc_mode_set_base,
	.load_lut = mdp5_crtc_load_lut,
=======
static void get_roi(struct drm_crtc *crtc, uint32_t *roi_w, uint32_t *roi_h)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	uint32_t xres = crtc->mode.hdisplay;
	uint32_t yres = crtc->mode.vdisplay;

	/*
	 * Cursor Region Of Interest (ROI) is a plane read from cursor
	 * buffer to render. The ROI region is determined by the visibility of
	 * the cursor point. In the default Cursor image the cursor point will
	 * be at the top left of the cursor image, unless it is specified
	 * otherwise using hotspot feature.
	 *
	 * If the cursor point reaches the right (xres - x < cursor.width) or
	 * bottom (yres - y < cursor.height) boundary of the screen, then ROI
	 * width and ROI height need to be evaluated to crop the cursor image
	 * accordingly.
	 * (xres-x) will be new cursor width when x > (xres - cursor.width)
	 * (yres-y) will be new cursor height when y > (yres - cursor.height)
	 */
	*roi_w = min(mdp5_crtc->cursor.width, xres -
			mdp5_crtc->cursor.x);
	*roi_h = min(mdp5_crtc->cursor.height, yres -
			mdp5_crtc->cursor.y);
}

static enum mdp5_pipe get_cursor_pipe_id(u32 cursor_id)
{
	switch (cursor_id) {
	case 0: return SSPP_CURSOR0;
	case 1: return SSPP_CURSOR1;
	default: return SSPP_CURSOR0;
	}
}

static int mdp5_crtc_cursor_set(struct drm_crtc *crtc,
		struct drm_file *file, uint32_t handle,
		uint32_t width, uint32_t height)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	struct drm_device *dev = crtc->dev;
	struct mdp5_kms *mdp5_kms = get_kms(crtc);
	struct drm_gem_object *cursor_bo, *old_bo = NULL;
	uint32_t cursor_addr, stride;
	int ret, bpp, lm;
	unsigned int depth;
	uint32_t flush_mask;
	uint32_t roi_w, roi_h;
	bool cursor_enable = true;
	unsigned long flags;
	enum mdp5_pipe cursor_pipe;
	u32 cursor_id;
	const struct mdp_format *format;
	const struct msm_format *msm_format;
	uint32_t pixel_fmt, blend_op;
	int pe_left[COMP_MAX], pe_right[COMP_MAX];
	int pe_top[COMP_MAX], pe_bottom[COMP_MAX];
	uint32_t phasex_step[COMP_MAX] = {0,}, phasey_step[COMP_MAX] = {0,};

	/*
	 * Since cursor is not 1:1 mapping to crtc any more, need to implement
	 * a way to track all cursor usage. For now, only enables cursor for
	 * first two CRTCs.
	 */

	if (mdp5_crtc->id >= 2) {
		DBG("HW cursor is only enabled for first two CRTSs, id=%d\n",
			mdp5_crtc->id);
		return 0;
	}
	if ((width > CURSOR_WIDTH) || (height > CURSOR_HEIGHT)) {
		dev_err(dev->dev, "bad cursor size: %dx%d\n", width, height);
		return -EINVAL;
	}

	if (NULL == mdp5_crtc->ctl)
		return -EINVAL;

	if (!handle) {
		DBG("Cursor off, handle is NULL\n");
		cursor_enable = false;
		goto set_cursor;
	}

	cursor_bo = drm_gem_object_lookup(dev, file, handle);
	if (!cursor_bo) {
		dev_err(dev->dev, "cursor_bo is NULL\n");
		return -ENOENT;
	}

	ret = msm_gem_get_iova(cursor_bo, mdp5_kms->id, &cursor_addr);
	if (ret) {
		dev_err(dev->dev, "cursor msm_gem_get_iova ret=%d\n", ret);
		return -EINVAL;
	}

	cursor_id = mdp5_crtc->id;
	cursor_pipe = get_cursor_pipe_id(cursor_id);

	pixel_fmt = DRM_FORMAT_ARGB8888;
	msm_format = mdp_get_format(&(mdp5_kms->base.base), pixel_fmt, NULL, 0);
	format = to_mdp_format(msm_format);
	lm = mdp5_crtc->lm;
	drm_fb_get_bpp_depth(pixel_fmt, &depth, &bpp);
	stride = width * (bpp >> 3);

	spin_lock_irqsave(&mdp5_crtc->cursor.lock, flags);
	old_bo = mdp5_crtc->cursor.scanout_bo;

	mdp5_crtc->cursor.scanout_bo = cursor_bo;
	mdp5_crtc->cursor.width = width;
	mdp5_crtc->cursor.height = height;

	get_roi(crtc, &roi_w, &roi_h);

	if (roi_w == 0 || roi_h == 0) {
		/* Disable cursor if roi is 0*/
		DBG("Cursor off, roi_w=%d or roi_h=%d is 0\n", roi_w, roi_h);
		cursor_enable = false;
		spin_unlock_irqrestore(&mdp5_crtc->cursor.lock, flags);
		goto set_cursor;
	}

	ret = mdp5_plane_calc_scalex_steps(mdp5_kms, pixel_fmt, roi_w, roi_w,
		phasex_step);
	if (ret) {
		dev_err(dev->dev, "%s cursor calc_scalex_steps fails ret=%d\n",
			__func__, ret);
		spin_unlock_irqrestore(&mdp5_crtc->cursor.lock, flags);
		mdp5_disable(mdp5_kms);
		return ret;
	}

	ret = mdp5_plane_calc_scaley_steps(mdp5_kms, pixel_fmt, roi_h, roi_h,
		phasey_step);
	if (ret) {
		dev_err(dev->dev, "%s cursor calc_scaley_steps fails ret=%d\n",
			__func__, ret);
		spin_unlock_irqrestore(&mdp5_crtc->cursor.lock, flags);
		mdp5_disable(mdp5_kms);
		return ret;
	}

	mdp5_plane_calc_pixel_ext(format, roi_w, roi_w, phasex_step,
				 pe_left, pe_right, true);
	mdp5_plane_calc_pixel_ext(format, roi_h, roi_h, phasey_step,
				pe_top, pe_bottom, false);

	blend_op = MDP5_LM_BLEND_OP_MODE_FG_ALPHA(FG_PIXEL) |
		MDP5_LM_BLEND_OP_MODE_BG_ALPHA(FG_PIXEL) |
		MDP5_LM_BLEND_OP_MODE_BG_INV_ALPHA;

	mdp5_write(mdp5_kms, REG_MDP5_PIPE_SRC_STRIDE_A(cursor_pipe), stride);
	mdp5_write(mdp5_kms, REG_MDP5_PIPE_SRC_FORMAT(cursor_pipe),
		MDP5_PIPE_SRC_FORMAT_A_BPC(format->bpc_a) |
		MDP5_PIPE_SRC_FORMAT_R_BPC(format->bpc_r) |
		MDP5_PIPE_SRC_FORMAT_G_BPC(format->bpc_g) |
		MDP5_PIPE_SRC_FORMAT_B_BPC(format->bpc_b) |
		COND(format->alpha_enable, MDP5_PIPE_SRC_FORMAT_ALPHA_ENABLE) |
		MDP5_PIPE_SRC_FORMAT_CPP(format->cpp - 1) |
		MDP5_PIPE_SRC_FORMAT_UNPACK_COUNT(format->unpack_count - 1) |
		COND(format->unpack_tight, MDP5_PIPE_SRC_FORMAT_UNPACK_TIGHT) |
		MDP5_PIPE_SRC_FORMAT_FETCH_TYPE(format->fetch_type) |
		MDP5_PIPE_SRC_FORMAT_CHROMA_SAMP(format->chroma_sample));
	mdp5_write(mdp5_kms, REG_MDP5_PIPE_SRC_UNPACK(cursor_pipe),
			MDP5_PIPE_SRC_UNPACK_ELEM0(format->unpack[0]) |
			MDP5_PIPE_SRC_UNPACK_ELEM1(format->unpack[1]) |
			MDP5_PIPE_SRC_UNPACK_ELEM2(format->unpack[2]) |
			MDP5_PIPE_SRC_UNPACK_ELEM3(format->unpack[3]));
	mdp5_write(mdp5_kms, REG_MDP5_PIPE_SRC_IMG_SIZE(cursor_pipe),
			MDP5_PIPE_SRC_IMG_SIZE_WIDTH(width) |
			MDP5_PIPE_SRC_IMG_SIZE_HEIGHT(height));
	mdp5_write(mdp5_kms, REG_MDP5_PIPE_SRC_SIZE(cursor_pipe),
			MDP5_PIPE_SRC_SIZE_WIDTH(roi_w) |
			MDP5_PIPE_SRC_SIZE_HEIGHT(roi_h));
	mdp5_write(mdp5_kms, REG_MDP5_PIPE_OUT_SIZE(cursor_pipe),
			MDP5_PIPE_OUT_SIZE_WIDTH(roi_w) |
			MDP5_PIPE_OUT_SIZE_HEIGHT(roi_h));
	mdp5_write(mdp5_kms, REG_MDP5_PIPE_SRC0_ADDR(cursor_pipe), cursor_addr);
	mdp5_pipe_write_pixel_ext(mdp5_kms, cursor_pipe, format,
			roi_w, pe_left, pe_right,
			roi_h, pe_top, pe_bottom);
	mdp5_write(mdp5_kms, REG_MDP5_LM_BLEND_OP_MODE(lm,
			(STAGE6 - STAGE0)), blend_op);
	mdp5_write(mdp5_kms, REG_MDP5_LM_BLEND_FG_ALPHA(lm,
			(STAGE6 - STAGE0)), 0);
	mdp5_write(mdp5_kms, REG_MDP5_LM_BLEND_BG_ALPHA(lm,
			(STAGE6 - STAGE0)), 0xFF);

	spin_unlock_irqrestore(&mdp5_crtc->cursor.lock, flags);

set_cursor:
	ret = mdp5_ctl_set_cursor(mdp5_crtc->ctl, cursor_id, cursor_enable);
	if (ret) {
		dev_err(dev->dev, "failed to %sable cursor[%d]: %d\n",
				cursor_enable ? "en" : "dis", cursor_id, ret);
		goto end;
	}
	flush_mask = (mdp_ctl_flush_mask_cursor(mdp5_crtc->id) |
				mdp_ctl_flush_mask_lm(lm));

	crtc_flush(crtc, flush_mask);

end:
	if (old_bo) {
		drm_flip_work_queue(&mdp5_crtc->unref_cursor_work, old_bo);
		/* enable vblank to complete cursor work: */
		request_pending(crtc, PENDING_CURSOR);
	}
	return ret;
}

static int mdp5_crtc_cursor_move(struct drm_crtc *crtc, int x, int y)
{
	struct mdp5_kms *mdp5_kms = get_kms(crtc);
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	struct drm_device *dev = crtc->dev;
	int lm = mdp5_crtc->lm;
	uint32_t flush_mask = (mdp_ctl_flush_mask_cursor(mdp5_crtc->id) |
				mdp_ctl_flush_mask_lm(lm));
	uint32_t roi_w;
	uint32_t roi_h;
	unsigned long flags;
	enum mdp5_pipe cursor_pipe;
	u32 cursor_id;
	bool cursor_enable = true;
	int ret;
	int pe_left[COMP_MAX], pe_right[COMP_MAX];
	int pe_top[COMP_MAX], pe_bottom[COMP_MAX];
	uint32_t phasex_step[COMP_MAX] = {0,}, phasey_step[COMP_MAX] = {0,};
	uint32_t pixel_fmt;
	const struct mdp_format *format;
	const struct msm_format *msm_format;

	/* In case the CRTC is disabled, just drop the cursor update */
	if (unlikely(!crtc->state->enable))
		return 0;

	mdp5_crtc->cursor.x = x = max(x, 0);
	mdp5_crtc->cursor.y = y = max(y, 0);

	get_roi(crtc, &roi_w, &roi_h);
	cursor_id = mdp5_crtc->id;

	mdp5_enable(mdp5_kms);
	if (roi_w == 0 || roi_h == 0) {
		/* Disable cursor if roi is 0*/
		DBG("Cursor off in move, roi_w=%d or roi_h=%d is 0\n",
			roi_w, roi_h);
		cursor_enable = false;
		goto set_cursor;
	}

	pixel_fmt = DRM_FORMAT_ARGB8888;
	msm_format = mdp_get_format(&(mdp5_kms->base.base), pixel_fmt, NULL, 0);
	format = to_mdp_format(msm_format);
	ret = mdp5_plane_calc_scalex_steps(mdp5_kms, pixel_fmt, roi_w, roi_w,
		phasex_step);
	if (ret) {
		dev_err(dev->dev, "%s cursor calc_scalex_steps fails ret=%d\n",
			__func__, ret);
		mdp5_disable(mdp5_kms);
		return ret;
	}

	ret = mdp5_plane_calc_scaley_steps(mdp5_kms, pixel_fmt, roi_h, roi_h,
		phasey_step);
	if (ret) {
		dev_err(dev->dev, "%s cursor calc_scaley_steps fails ret=%d\n",
			__func__, ret);
		mdp5_disable(mdp5_kms);
		return ret;
	}

	mdp5_plane_calc_pixel_ext(format, roi_w, roi_w, phasex_step,
				 pe_left, pe_right, true);
	mdp5_plane_calc_pixel_ext(format, roi_h, roi_h, phasey_step,
				pe_top, pe_bottom, false);

	spin_lock_irqsave(&mdp5_crtc->cursor.lock, flags);
	cursor_pipe = get_cursor_pipe_id(cursor_id);
	mdp5_write(mdp5_kms, REG_MDP5_PIPE_SRC_SIZE(cursor_pipe),
			MDP5_PIPE_SRC_SIZE_WIDTH(roi_w) |
			MDP5_PIPE_SRC_SIZE_HEIGHT(roi_h));
	mdp5_write(mdp5_kms, REG_MDP5_PIPE_OUT_SIZE(cursor_pipe),
			MDP5_PIPE_OUT_SIZE_WIDTH(roi_w) |
			MDP5_PIPE_OUT_SIZE_HEIGHT(roi_h));
	mdp5_write(mdp5_kms, REG_MDP5_PIPE_OUT_XY(cursor_pipe),
			MDP5_PIPE_OUT_XY_X(x) |
			MDP5_PIPE_OUT_XY_Y(y));
	mdp5_pipe_write_pixel_ext(mdp5_kms, cursor_pipe, format,
			roi_w, pe_left, pe_right,
			roi_h, pe_top, pe_bottom);
	spin_unlock_irqrestore(&mdp5_crtc->cursor.lock, flags);

set_cursor:
	ret = mdp5_ctl_set_cursor(mdp5_crtc->ctl, cursor_id, cursor_enable);
	if (ret) {
		dev_err(dev->dev, "%s failed to %sable cursor[%d]: %d\n",
				__func__, cursor_enable ? "en" : "dis",
				cursor_id, ret);
		goto end;
	}
	crtc_flush(crtc, flush_mask);

end:
	mdp5_disable(mdp5_kms);
	return ret;
}

static const struct drm_crtc_funcs mdp5_crtc_funcs = {
	.set_config = drm_atomic_helper_set_config,
	.destroy = mdp5_crtc_destroy,
	.page_flip = drm_atomic_helper_page_flip,
	.set_property = mdp5_crtc_set_property,
	.reset = drm_atomic_helper_crtc_reset,
	.atomic_duplicate_state = drm_atomic_helper_crtc_duplicate_state,
	.atomic_destroy_state = drm_atomic_helper_crtc_destroy_state,
	.cursor_set = mdp5_crtc_cursor_set,
	.cursor_move = mdp5_crtc_cursor_move,
};

static const struct drm_crtc_helper_funcs mdp5_crtc_helper_funcs = {
	.mode_fixup = mdp5_crtc_mode_fixup,
	.mode_set_nofb = mdp5_crtc_mode_set_nofb,
	.disable = mdp5_crtc_disable,
	.enable = mdp5_crtc_enable,
	.atomic_check = mdp5_crtc_atomic_check,
	.atomic_begin = mdp5_crtc_atomic_begin,
	.atomic_flush = mdp5_crtc_atomic_flush,
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
};

static void mdp5_crtc_vblank_irq(struct mdp_irq *irq, uint32_t irqstatus)
{
	struct mdp5_crtc *mdp5_crtc = container_of(irq, struct mdp5_crtc, vblank);
	struct drm_crtc *crtc = &mdp5_crtc->base;
	struct msm_drm_private *priv = crtc->dev->dev_private;
	unsigned pending;

	mdp_irq_unregister(&get_kms(crtc)->base, &mdp5_crtc->vblank);

	pending = atomic_xchg(&mdp5_crtc->pending, 0);

	if (pending & PENDING_FLIP) {
		complete_flip(crtc, NULL);
<<<<<<< HEAD
		drm_flip_work_commit(&mdp5_crtc->unref_fb_work, priv->wq);
	}
=======
	}

	if (pending & PENDING_CURSOR)
		drm_flip_work_commit(&mdp5_crtc->unref_cursor_work, priv->wq);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

static void mdp5_crtc_err_irq(struct mdp_irq *irq, uint32_t irqstatus)
{
	struct mdp5_crtc *mdp5_crtc = container_of(irq, struct mdp5_crtc, err);
<<<<<<< HEAD
	struct drm_crtc *crtc = &mdp5_crtc->base;
	DBG("%s: error: %08x", mdp5_crtc->name, irqstatus);
	crtc_flush(crtc);
}

uint32_t mdp5_crtc_vblank(struct drm_crtc *crtc)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	return mdp5_crtc->vblank.irqmask;
}

void mdp5_crtc_cancel_pending_flip(struct drm_crtc *crtc, struct drm_file *file)
{
	DBG("cancel: %p", file);
	complete_flip(crtc, file);
}

/* set interface for routing crtc->encoder: */
void mdp5_crtc_set_intf(struct drm_crtc *crtc, int intf,
		enum mdp5_intf intf_id)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	struct mdp5_kms *mdp5_kms = get_kms(crtc);
	static const enum mdp5_intfnum intfnum[] = {
			INTF0, INTF1, INTF2, INTF3,
	};
	uint32_t intf_sel;

	/* now that we know what irq's we want: */
	mdp5_crtc->err.irqmask = intf2err(intf);
	mdp5_crtc->vblank.irqmask = intf2vblank(intf);

	/* when called from modeset_init(), skip the rest until later: */
	if (!mdp5_kms)
		return;

	intf_sel = mdp5_read(mdp5_kms, REG_MDP5_DISP_INTF_SEL);

	switch (intf) {
	case 0:
		intf_sel &= ~MDP5_DISP_INTF_SEL_INTF0__MASK;
		intf_sel |= MDP5_DISP_INTF_SEL_INTF0(intf_id);
		break;
	case 1:
		intf_sel &= ~MDP5_DISP_INTF_SEL_INTF1__MASK;
		intf_sel |= MDP5_DISP_INTF_SEL_INTF1(intf_id);
		break;
	case 2:
		intf_sel &= ~MDP5_DISP_INTF_SEL_INTF2__MASK;
		intf_sel |= MDP5_DISP_INTF_SEL_INTF2(intf_id);
		break;
	case 3:
		intf_sel &= ~MDP5_DISP_INTF_SEL_INTF3__MASK;
		intf_sel |= MDP5_DISP_INTF_SEL_INTF3(intf_id);
		break;
	default:
		BUG();
		break;
	}

	blend_setup(crtc);

	DBG("%s: intf_sel=%08x", mdp5_crtc->name, intf_sel);

	mdp5_write(mdp5_kms, REG_MDP5_DISP_INTF_SEL, intf_sel);
	mdp5_write(mdp5_kms, REG_MDP5_CTL_OP(mdp5_crtc->id),
			MDP5_CTL_OP_MODE(MODE_NONE) |
			MDP5_CTL_OP_INTF_NUM(intfnum[intf]));

	crtc_flush(crtc);
}

static void set_attach(struct drm_crtc *crtc, enum mdp5_pipe pipe_id,
		struct drm_plane *plane)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);

	BUG_ON(pipe_id >= ARRAY_SIZE(mdp5_crtc->planes));

	if (mdp5_crtc->planes[pipe_id] == plane)
		return;

	mdp5_crtc->planes[pipe_id] = plane;
	blend_setup(crtc);
	if (mdp5_crtc->enabled && (plane != mdp5_crtc->plane))
		crtc_flush(crtc);
}

void mdp5_crtc_attach(struct drm_crtc *crtc, struct drm_plane *plane)
{
	set_attach(crtc, mdp5_plane_pipe(plane), plane);
}

void mdp5_crtc_detach(struct drm_crtc *crtc, struct drm_plane *plane)
{
	/* don't actually detatch our primary plane: */
	if (to_mdp5_crtc(crtc)->plane == plane)
		return;
	set_attach(crtc, mdp5_plane_pipe(plane), NULL);
=======

	DBG("%s: error: %08x", mdp5_crtc->name, irqstatus);
}

static void mdp5_crtc_pp_done_irq(struct mdp_irq *irq, uint32_t irqstatus)
{
	struct mdp5_crtc *mdp5_crtc = container_of(irq, struct mdp5_crtc,
								pp_done);

	complete(&mdp5_crtc->pp_completion);
}

static void mdp5_crtc_wait_for_pp_done(struct drm_crtc *crtc)
{
	struct drm_device *dev = crtc->dev;
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	int ret;

	ret = wait_for_completion_timeout(&mdp5_crtc->pp_completion,
						msecs_to_jiffies(50));
	if (ret == 0)
		dev_warn(dev->dev, "pp done time out, lm=%d\n", mdp5_crtc->lm);
}

static void mdp5_crtc_wait_for_flush_done(struct drm_crtc *crtc)
{
	struct drm_device *dev = crtc->dev;
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	int ret;

	/* Should not call this function if crtc is disabled. */
	if (!mdp5_crtc->ctl)
		return;

	ret = drm_crtc_vblank_get(crtc);
	if (ret)
		return;

	ret = wait_event_timeout(dev->vblank[drm_crtc_index(crtc)].queue,
		((mdp5_ctl_get_commit_status(mdp5_crtc->ctl) &
		mdp5_crtc->flushed_mask) == 0),
		msecs_to_jiffies(50));
	if (ret <= 0)
		dev_warn(dev->dev, "vblank time out, crtc=%d\n", mdp5_crtc->id);

	mdp5_crtc->flushed_mask = 0;

	drm_crtc_vblank_put(crtc);
}

uint32_t mdp5_crtc_vblank(struct drm_crtc *crtc)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	return mdp5_crtc->vblank.irqmask;
}

void mdp5_crtc_cancel_pending_flip(struct drm_crtc *crtc, struct drm_file *file)
{
	DBG("cancel: %p", file);
	complete_flip(crtc, file);
}

void mdp5_crtc_set_pipeline(struct drm_crtc *crtc,
		struct mdp5_interface *intf, struct mdp5_ctl *ctl)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	struct mdp5_kms *mdp5_kms = get_kms(crtc);
	int lm = mdp5_crtc_get_lm(crtc);

	/* now that we know what irq's we want: */
	mdp5_crtc->err.irqmask = intf2err(intf->num);
	mdp5_crtc->vblank.irqmask = intf2vblank(lm, intf);

	if ((intf->type == INTF_DSI) &&
		(intf->mode == MDP5_INTF_DSI_MODE_COMMAND)) {
		mdp5_crtc->pp_done.irqmask = lm2ppdone(lm);
		mdp5_crtc->pp_done.irq = mdp5_crtc_pp_done_irq;
		mdp5_crtc->cmd_mode = true;
	} else {
		mdp5_crtc->pp_done.irqmask = 0;
		mdp5_crtc->pp_done.irq = NULL;
		mdp5_crtc->cmd_mode = false;
	}

	mdp_irq_update(&mdp5_kms->base);

	mdp5_crtc->ctl = ctl;
	mdp5_ctl_set_pipeline(ctl, intf, lm);
}

int mdp5_crtc_get_lm(struct drm_crtc *crtc)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);
	return WARN_ON(!crtc) ? -EINVAL : mdp5_crtc->lm;
}

void mdp5_crtc_wait_for_commit_done(struct drm_crtc *crtc)
{
	struct mdp5_crtc *mdp5_crtc = to_mdp5_crtc(crtc);

	if (mdp5_crtc->cmd_mode)
		mdp5_crtc_wait_for_pp_done(crtc);
	else
		mdp5_crtc_wait_for_flush_done(crtc);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

/* initialize crtc */
struct drm_crtc *mdp5_crtc_init(struct drm_device *dev,
		struct drm_plane *plane, int id)
{
	struct drm_crtc *crtc = NULL;
	struct mdp5_crtc *mdp5_crtc;
<<<<<<< HEAD
	int ret;

	mdp5_crtc = kzalloc(sizeof(*mdp5_crtc), GFP_KERNEL);
	if (!mdp5_crtc) {
		ret = -ENOMEM;
		goto fail;
	}

	crtc = &mdp5_crtc->base;

	mdp5_crtc->plane = plane;
	mdp5_crtc->id = id;
=======

	mdp5_crtc = kzalloc(sizeof(*mdp5_crtc), GFP_KERNEL);
	if (!mdp5_crtc)
		return ERR_PTR(-ENOMEM);

	crtc = &mdp5_crtc->base;

	mdp5_crtc->id = id;
	mdp5_crtc->lm = GET_LM_ID(id);

	spin_lock_init(&mdp5_crtc->lm_lock);
	spin_lock_init(&mdp5_crtc->cursor.lock);
	init_completion(&mdp5_crtc->pp_completion);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	mdp5_crtc->vblank.irq = mdp5_crtc_vblank_irq;
	mdp5_crtc->err.irq = mdp5_crtc_err_irq;

	snprintf(mdp5_crtc->name, sizeof(mdp5_crtc->name), "%s:%d",
			pipe2name(mdp5_plane_pipe(plane)), id);

<<<<<<< HEAD
	ret = drm_flip_work_init(&mdp5_crtc->unref_fb_work, 16,
			"unref fb", unref_fb_worker);
	if (ret)
		goto fail;

	INIT_FENCE_CB(&mdp5_crtc->pageflip_cb, pageflip_cb);

	drm_crtc_init_with_planes(dev, crtc, plane, NULL, &mdp5_crtc_funcs);
	drm_crtc_helper_add(crtc, &mdp5_crtc_helper_funcs);

	mdp5_plane_install_properties(mdp5_crtc->plane, &crtc->base);

	return crtc;

fail:
	if (crtc)
		mdp5_crtc_destroy(crtc);

	return ERR_PTR(ret);
=======
	drm_crtc_init_with_planes(dev, crtc, plane, NULL, &mdp5_crtc_funcs);

	drm_flip_work_init(&mdp5_crtc->unref_cursor_work,
			"unref cursor", unref_cursor_worker);

	drm_crtc_helper_add(crtc, &mdp5_crtc_helper_funcs);
	plane->crtc = crtc;

	return crtc;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}
