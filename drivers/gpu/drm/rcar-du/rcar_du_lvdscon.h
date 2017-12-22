/*
 * rcar_du_lvdscon.h  --  R-Car Display Unit LVDS Connector
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

#ifndef __RCAR_DU_LVDSCON_H__
#define __RCAR_DU_LVDSCON_H__

struct rcar_du_device;
struct rcar_du_encoder;
<<<<<<< HEAD
struct rcar_du_panel_data;

int rcar_du_lvds_connector_init(struct rcar_du_device *rcdu,
				struct rcar_du_encoder *renc,
				const struct rcar_du_panel_data *panel,
=======

int rcar_du_lvds_connector_init(struct rcar_du_device *rcdu,
				struct rcar_du_encoder *renc,
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
				struct device_node *np);

#endif /* __RCAR_DU_LVDSCON_H__ */
