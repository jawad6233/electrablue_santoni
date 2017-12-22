/*
 * rcar_du_encoder.c  --  R-Car Display Unit Encoder
 *
 * Copyright (C) 2013-2014 Renesas Electronics Corporation
 *
 * Contact: Laurent Pinchart (laurent.pinchart@ideasonboard.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/export.h>

#include <drm/drmP.h>
#include <drm/drm_crtc.h>
#include <drm/drm_crtc_helper.h>

#include "rcar_du_drv.h"
#include "rcar_du_encoder.h"
<<<<<<< HEAD
=======
#include "rcar_du_hdmicon.h"
#include "rcar_du_hdmienc.h"
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
#include "rcar_du_kms.h"
#include "rcar_du_lvdscon.h"
#include "rcar_du_lvdsenc.h"
#include "rcar_du_vgacon.h"

/* -----------------------------------------------------------------------------
 * Common connector functions
 */

struct drm_encoder *
rcar_du_connector_best_encoder(struct drm_connector *connector)
{
	struct rcar_du_connector *rcon = to_rcar_connector(connector);

<<<<<<< HEAD
	return &rcon->encoder->encoder;
=======
	return rcar_encoder_to_drm_encoder(rcon->encoder);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

/* -----------------------------------------------------------------------------
 * Encoder
 */

<<<<<<< HEAD
static void rcar_du_encoder_dpms(struct drm_encoder *encoder, int mode)
=======
static void rcar_du_encoder_disable(struct drm_encoder *encoder)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
{
	struct rcar_du_encoder *renc = to_rcar_encoder(encoder);

	if (renc->lvds)
<<<<<<< HEAD
		rcar_du_lvdsenc_dpms(renc->lvds, encoder->crtc, mode);
}

static bool rcar_du_encoder_mode_fixup(struct drm_encoder *encoder,
				       const struct drm_display_mode *mode,
				       struct drm_display_mode *adjusted_mode)
{
	struct rcar_du_encoder *renc = to_rcar_encoder(encoder);
	const struct drm_display_mode *panel_mode;
	struct drm_device *dev = encoder->dev;
	struct drm_connector *connector;
	bool found = false;

	/* DAC encoders have currently no restriction on the mode. */
	if (encoder->encoder_type == DRM_MODE_ENCODER_DAC)
		return true;

	list_for_each_entry(connector, &dev->mode_config.connector_list, head) {
		if (connector->encoder == encoder) {
			found = true;
			break;
		}
	}

	if (!found) {
		dev_dbg(dev->dev, "mode_fixup: no connector found\n");
		return false;
	}

	if (list_empty(&connector->modes)) {
		dev_dbg(dev->dev, "mode_fixup: empty modes list\n");
		return false;
=======
		rcar_du_lvdsenc_enable(renc->lvds, encoder->crtc, false);
}

static void rcar_du_encoder_enable(struct drm_encoder *encoder)
{
	struct rcar_du_encoder *renc = to_rcar_encoder(encoder);

	if (renc->lvds)
		rcar_du_lvdsenc_enable(renc->lvds, encoder->crtc, true);
}

static int rcar_du_encoder_atomic_check(struct drm_encoder *encoder,
					struct drm_crtc_state *crtc_state,
					struct drm_connector_state *conn_state)
{
	struct rcar_du_encoder *renc = to_rcar_encoder(encoder);
	struct drm_display_mode *adjusted_mode = &crtc_state->adjusted_mode;
	const struct drm_display_mode *mode = &crtc_state->mode;
	const struct drm_display_mode *panel_mode;
	struct drm_connector *connector = conn_state->connector;
	struct drm_device *dev = encoder->dev;

	/* DAC encoders have currently no restriction on the mode. */
	if (encoder->encoder_type == DRM_MODE_ENCODER_DAC)
		return 0;

	if (list_empty(&connector->modes)) {
		dev_dbg(dev->dev, "encoder: empty modes list\n");
		return -EINVAL;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	}

	panel_mode = list_first_entry(&connector->modes,
				      struct drm_display_mode, head);

	/* We're not allowed to modify the resolution. */
	if (mode->hdisplay != panel_mode->hdisplay ||
	    mode->vdisplay != panel_mode->vdisplay)
<<<<<<< HEAD
		return false;
=======
		return -EINVAL;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	/* The flat panel mode is fixed, just copy it to the adjusted mode. */
	drm_mode_copy(adjusted_mode, panel_mode);

	/* The internal LVDS encoder has a clock frequency operating range of
	 * 30MHz to 150MHz. Clamp the clock accordingly.
	 */
	if (renc->lvds)
		adjusted_mode->clock = clamp(adjusted_mode->clock,
					     30000, 150000);

<<<<<<< HEAD
	return true;
}

static void rcar_du_encoder_mode_prepare(struct drm_encoder *encoder)
{
	struct rcar_du_encoder *renc = to_rcar_encoder(encoder);

	if (renc->lvds)
		rcar_du_lvdsenc_dpms(renc->lvds, encoder->crtc,
				     DRM_MODE_DPMS_OFF);
}

static void rcar_du_encoder_mode_commit(struct drm_encoder *encoder)
{
	struct rcar_du_encoder *renc = to_rcar_encoder(encoder);

	if (renc->lvds)
		rcar_du_lvdsenc_dpms(renc->lvds, encoder->crtc,
				     DRM_MODE_DPMS_ON);
=======
	return 0;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

static void rcar_du_encoder_mode_set(struct drm_encoder *encoder,
				     struct drm_display_mode *mode,
				     struct drm_display_mode *adjusted_mode)
{
	struct rcar_du_encoder *renc = to_rcar_encoder(encoder);

	rcar_du_crtc_route_output(encoder->crtc, renc->output);
}

static const struct drm_encoder_helper_funcs encoder_helper_funcs = {
<<<<<<< HEAD
	.dpms = rcar_du_encoder_dpms,
	.mode_fixup = rcar_du_encoder_mode_fixup,
	.prepare = rcar_du_encoder_mode_prepare,
	.commit = rcar_du_encoder_mode_commit,
	.mode_set = rcar_du_encoder_mode_set,
=======
	.mode_set = rcar_du_encoder_mode_set,
	.disable = rcar_du_encoder_disable,
	.enable = rcar_du_encoder_enable,
	.atomic_check = rcar_du_encoder_atomic_check,
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
};

static const struct drm_encoder_funcs encoder_funcs = {
	.destroy = drm_encoder_cleanup,
};

int rcar_du_encoder_init(struct rcar_du_device *rcdu,
			 enum rcar_du_encoder_type type,
			 enum rcar_du_output output,
<<<<<<< HEAD
			 const struct rcar_du_encoder_data *data,
			 struct device_node *np)
{
	struct rcar_du_encoder *renc;
=======
			 struct device_node *enc_node,
			 struct device_node *con_node)
{
	struct rcar_du_encoder *renc;
	struct drm_encoder *encoder;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	unsigned int encoder_type;
	int ret;

	renc = devm_kzalloc(rcdu->dev, sizeof(*renc), GFP_KERNEL);
	if (renc == NULL)
		return -ENOMEM;

	renc->output = output;
<<<<<<< HEAD
=======
	encoder = rcar_encoder_to_drm_encoder(renc);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	switch (output) {
	case RCAR_DU_OUTPUT_LVDS0:
		renc->lvds = rcdu->lvds[0];
		break;

	case RCAR_DU_OUTPUT_LVDS1:
		renc->lvds = rcdu->lvds[1];
		break;

	default:
		break;
	}

	switch (type) {
	case RCAR_DU_ENCODER_VGA:
		encoder_type = DRM_MODE_ENCODER_DAC;
		break;
	case RCAR_DU_ENCODER_LVDS:
		encoder_type = DRM_MODE_ENCODER_LVDS;
		break;
<<<<<<< HEAD
=======
	case RCAR_DU_ENCODER_HDMI:
		encoder_type = DRM_MODE_ENCODER_TMDS;
		break;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	case RCAR_DU_ENCODER_NONE:
	default:
		/* No external encoder, use the internal encoder type. */
		encoder_type = rcdu->info->routes[output].encoder_type;
		break;
	}

<<<<<<< HEAD
	ret = drm_encoder_init(rcdu->ddev, &renc->encoder, &encoder_funcs,
			       encoder_type);
	if (ret < 0)
		return ret;

	drm_encoder_helper_add(&renc->encoder, &encoder_helper_funcs);

	switch (encoder_type) {
	case DRM_MODE_ENCODER_LVDS: {
		const struct rcar_du_panel_data *pdata =
			data ? &data->connector.lvds.panel : NULL;
		return rcar_du_lvds_connector_init(rcdu, renc, pdata, np);
	}

	case DRM_MODE_ENCODER_DAC:
		return rcar_du_vga_connector_init(rcdu, renc);

	default:
		return -EINVAL;
	}
=======
	if (type == RCAR_DU_ENCODER_HDMI) {
		ret = rcar_du_hdmienc_init(rcdu, renc, enc_node);
		if (ret < 0)
			goto done;
	} else {
		ret = drm_encoder_init(rcdu->ddev, encoder, &encoder_funcs,
				       encoder_type);
		if (ret < 0)
			goto done;

		drm_encoder_helper_add(encoder, &encoder_helper_funcs);
	}

	switch (encoder_type) {
	case DRM_MODE_ENCODER_LVDS:
		ret = rcar_du_lvds_connector_init(rcdu, renc, con_node);
		break;

	case DRM_MODE_ENCODER_DAC:
		ret = rcar_du_vga_connector_init(rcdu, renc);
		break;

	case DRM_MODE_ENCODER_TMDS:
		ret = rcar_du_hdmi_connector_init(rcdu, renc);
		break;

	default:
		ret = -EINVAL;
		break;
	}

done:
	if (ret < 0) {
		if (encoder->name)
			encoder->funcs->destroy(encoder);
		devm_kfree(rcdu->dev, renc);
	}

	return ret;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}
