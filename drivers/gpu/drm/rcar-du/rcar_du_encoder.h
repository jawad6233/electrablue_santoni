/*
 * rcar_du_encoder.h  --  R-Car Display Unit Encoder
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

#ifndef __RCAR_DU_ENCODER_H__
#define __RCAR_DU_ENCODER_H__

<<<<<<< HEAD
#include <linux/platform_data/rcar-du.h>

#include <drm/drm_crtc.h>

struct rcar_du_device;
struct rcar_du_lvdsenc;

struct rcar_du_encoder {
	struct drm_encoder encoder;
	enum rcar_du_output output;
=======
#include <drm/drm_crtc.h>
#include <drm/drm_encoder_slave.h>

struct rcar_du_device;
struct rcar_du_hdmienc;
struct rcar_du_lvdsenc;

enum rcar_du_encoder_type {
	RCAR_DU_ENCODER_UNUSED = 0,
	RCAR_DU_ENCODER_NONE,
	RCAR_DU_ENCODER_VGA,
	RCAR_DU_ENCODER_LVDS,
	RCAR_DU_ENCODER_HDMI,
};

struct rcar_du_encoder {
	struct drm_encoder_slave slave;
	enum rcar_du_output output;
	struct rcar_du_hdmienc *hdmi;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	struct rcar_du_lvdsenc *lvds;
};

#define to_rcar_encoder(e) \
<<<<<<< HEAD
	container_of(e, struct rcar_du_encoder, encoder)
=======
	container_of(e, struct rcar_du_encoder, slave.base)

#define rcar_encoder_to_drm_encoder(e)	(&(e)->slave.base)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

struct rcar_du_connector {
	struct drm_connector connector;
	struct rcar_du_encoder *encoder;
};

#define to_rcar_connector(c) \
	container_of(c, struct rcar_du_connector, connector)

struct drm_encoder *
rcar_du_connector_best_encoder(struct drm_connector *connector);

int rcar_du_encoder_init(struct rcar_du_device *rcdu,
			 enum rcar_du_encoder_type type,
			 enum rcar_du_output output,
<<<<<<< HEAD
			 const struct rcar_du_encoder_data *data,
			 struct device_node *np);
=======
			 struct device_node *enc_node,
			 struct device_node *con_node);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

#endif /* __RCAR_DU_ENCODER_H__ */
