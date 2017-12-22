/*
 * Copyright (C) STMicroelectronics SA 2014
 * Author: Fabien Dessenne <fabien.dessenne@st.com> for STMicroelectronics.
 * License terms:  GNU General Public License (GPL), version 2
 */

#include <drm/drmP.h>

<<<<<<< HEAD
#include "sti_layer.h"
=======
#include "sti_plane.h"
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
#include "sti_vid.h"
#include "sti_vtg.h"

/* Registers */
#define VID_CTL                 0x00
#define VID_ALP                 0x04
#define VID_CLF                 0x08
#define VID_VPO                 0x0C
#define VID_VPS                 0x10
#define VID_KEY1                0x28
#define VID_KEY2                0x2C
#define VID_MPR0                0x30
#define VID_MPR1                0x34
#define VID_MPR2                0x38
#define VID_MPR3                0x3C
#define VID_MST                 0x68
#define VID_BC                  0x70
#define VID_TINT                0x74
#define VID_CSAT                0x78

/* Registers values */
#define VID_CTL_IGNORE          (BIT(31) | BIT(30))
#define VID_CTL_PSI_ENABLE      (BIT(2) | BIT(1) | BIT(0))
#define VID_ALP_OPAQUE          0x00000080
#define VID_BC_DFLT             0x00008000
#define VID_TINT_DFLT           0x00000000
#define VID_CSAT_DFLT           0x00000080
/* YCbCr to RGB BT709:
 * R = Y+1.5391Cr
 * G = Y-0.4590Cr-0.1826Cb
 * B = Y+1.8125Cb */
#define VID_MPR0_BT709          0x0A800000
#define VID_MPR1_BT709          0x0AC50000
#define VID_MPR2_BT709          0x07150545
#define VID_MPR3_BT709          0x00000AE8

<<<<<<< HEAD
static int sti_vid_prepare_layer(struct sti_layer *vid, bool first_prepare)
{
	u32 val;
=======
void sti_vid_commit(struct sti_vid *vid,
		    struct drm_plane_state *state)
{
	struct drm_crtc *crtc = state->crtc;
	struct drm_display_mode *mode = &crtc->mode;
	int dst_x = state->crtc_x;
	int dst_y = state->crtc_y;
	int dst_w = clamp_val(state->crtc_w, 0, mode->crtc_hdisplay - dst_x);
	int dst_h = clamp_val(state->crtc_h, 0, mode->crtc_vdisplay - dst_y);
	u32 val, ydo, xdo, yds, xds;

	/* Input / output size
	 * Align to upper even value */
	dst_w = ALIGN(dst_w, 2);
	dst_h = ALIGN(dst_h, 2);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	/* Unmask */
	val = readl(vid->regs + VID_CTL);
	val &= ~VID_CTL_IGNORE;
	writel(val, vid->regs + VID_CTL);

<<<<<<< HEAD
	return 0;
}

static int sti_vid_commit_layer(struct sti_layer *vid)
{
	struct drm_display_mode *mode = vid->mode;
	u32 ydo, xdo, yds, xds;

	ydo = sti_vtg_get_line_number(*mode, vid->dst_y);
	yds = sti_vtg_get_line_number(*mode, vid->dst_y + vid->dst_h - 1);
	xdo = sti_vtg_get_pixel_number(*mode, vid->dst_x);
	xds = sti_vtg_get_pixel_number(*mode, vid->dst_x + vid->dst_w - 1);

	writel((ydo << 16) | xdo, vid->regs + VID_VPO);
	writel((yds << 16) | xds, vid->regs + VID_VPS);

	return 0;
}

static int sti_vid_disable_layer(struct sti_layer *vid)
=======
	ydo = sti_vtg_get_line_number(*mode, dst_y);
	yds = sti_vtg_get_line_number(*mode, dst_y + dst_h - 1);
	xdo = sti_vtg_get_pixel_number(*mode, dst_x);
	xds = sti_vtg_get_pixel_number(*mode, dst_x + dst_w - 1);

	writel((ydo << 16) | xdo, vid->regs + VID_VPO);
	writel((yds << 16) | xds, vid->regs + VID_VPS);
}

void sti_vid_disable(struct sti_vid *vid)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
{
	u32 val;

	/* Mask */
	val = readl(vid->regs + VID_CTL);
	val |= VID_CTL_IGNORE;
	writel(val, vid->regs + VID_CTL);
<<<<<<< HEAD

	return 0;
}

static const uint32_t *sti_vid_get_formats(struct sti_layer *layer)
{
	return NULL;
}

static unsigned int sti_vid_get_nb_formats(struct sti_layer *layer)
{
	return 0;
}

static void sti_vid_init(struct sti_layer *vid)
=======
}

static void sti_vid_init(struct sti_vid *vid)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
{
	/* Enable PSI, Mask layer */
	writel(VID_CTL_PSI_ENABLE | VID_CTL_IGNORE, vid->regs + VID_CTL);

	/* Opaque */
	writel(VID_ALP_OPAQUE, vid->regs + VID_ALP);

	/* Color conversion parameters */
	writel(VID_MPR0_BT709, vid->regs + VID_MPR0);
	writel(VID_MPR1_BT709, vid->regs + VID_MPR1);
	writel(VID_MPR2_BT709, vid->regs + VID_MPR2);
	writel(VID_MPR3_BT709, vid->regs + VID_MPR3);

	/* Brightness, contrast, tint, saturation */
	writel(VID_BC_DFLT, vid->regs + VID_BC);
	writel(VID_TINT_DFLT, vid->regs + VID_TINT);
	writel(VID_CSAT_DFLT, vid->regs + VID_CSAT);
}

<<<<<<< HEAD
static const struct sti_layer_funcs vid_ops = {
	.get_formats = sti_vid_get_formats,
	.get_nb_formats = sti_vid_get_nb_formats,
	.init = sti_vid_init,
	.prepare = sti_vid_prepare_layer,
	.commit = sti_vid_commit_layer,
	.disable = sti_vid_disable_layer,
};

struct sti_layer *sti_vid_create(struct device *dev)
{
	struct sti_layer *vid;
=======
struct sti_vid *sti_vid_create(struct device *dev, int id,
			       void __iomem *baseaddr)
{
	struct sti_vid *vid;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	vid = devm_kzalloc(dev, sizeof(*vid), GFP_KERNEL);
	if (!vid) {
		DRM_ERROR("Failed to allocate memory for VID\n");
		return NULL;
	}

<<<<<<< HEAD
	vid->ops = &vid_ops;
=======
	vid->dev = dev;
	vid->regs = baseaddr;
	vid->id = id;

	sti_vid_init(vid);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	return vid;
}
