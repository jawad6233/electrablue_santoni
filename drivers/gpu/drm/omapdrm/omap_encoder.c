/*
 * drivers/gpu/drm/omapdrm/omap_encoder.c
 *
 * Copyright (C) 2011 Texas Instruments
 * Author: Rob Clark <rob@ti.com>
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

<<<<<<< HEAD
=======
#include <linux/list.h>

#include <drm/drm_crtc.h>
#include <drm/drm_crtc_helper.h>
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
#include <drm/drm_edid.h>

#include "omap_drv.h"

<<<<<<< HEAD
#include "drm_crtc.h"
#include "drm_crtc_helper.h"

#include <linux/list.h>


=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
/*
 * encoder funcs
 */

#define to_omap_encoder(x) container_of(x, struct omap_encoder, base)

/* The encoder and connector both map to same dssdev.. the encoder
 * handles the 'active' parts, ie. anything the modifies the state
 * of the hw, and the connector handles the 'read-only' parts, like
 * detecting connection and reading edid.
 */
struct omap_encoder {
	struct drm_encoder base;
	struct omap_dss_device *dssdev;
};

struct omap_dss_device *omap_encoder_get_dssdev(struct drm_encoder *encoder)
{
	struct omap_encoder *omap_encoder = to_omap_encoder(encoder);

	return omap_encoder->dssdev;
}

static void omap_encoder_destroy(struct drm_encoder *encoder)
{
	struct omap_encoder *omap_encoder = to_omap_encoder(encoder);

<<<<<<< HEAD
	omap_encoder_set_enabled(encoder, false);

=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	drm_encoder_cleanup(encoder);
	kfree(omap_encoder);
}

static const struct drm_encoder_funcs omap_encoder_funcs = {
	.destroy = omap_encoder_destroy,
};

<<<<<<< HEAD
/*
 * The CRTC drm_crtc_helper_set_mode() doesn't really give us the right
 * order.. the easiest way to work around this for now is to make all
 * the encoder-helper's no-op's and have the omap_crtc code take care
 * of the sequencing and call us in the right points.
 *
 * Eventually to handle connecting CRTCs to different encoders properly,
 * either the CRTC helpers need to change or we need to replace
 * drm_crtc_helper_set_mode(), but lets wait until atomic-modeset for
 * that.
 */

static void omap_encoder_dpms(struct drm_encoder *encoder, int mode)
{
}

static bool omap_encoder_mode_fixup(struct drm_encoder *encoder,
				  const struct drm_display_mode *mode,
				  struct drm_display_mode *adjusted_mode)
{
	return true;
}

=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
static void omap_encoder_mode_set(struct drm_encoder *encoder,
				struct drm_display_mode *mode,
				struct drm_display_mode *adjusted_mode)
{
	struct drm_device *dev = encoder->dev;
	struct omap_encoder *omap_encoder = to_omap_encoder(encoder);
	struct omap_dss_device *dssdev = omap_encoder->dssdev;
	struct drm_connector *connector;
	bool hdmi_mode;
	int r;

	hdmi_mode = false;
	list_for_each_entry(connector, &dev->mode_config.connector_list, head) {
		if (connector->encoder == encoder) {
			hdmi_mode = omap_connector_get_hdmi_mode(connector);
			break;
		}
	}

	if (dssdev->driver->set_hdmi_mode)
		dssdev->driver->set_hdmi_mode(dssdev, hdmi_mode);

	if (hdmi_mode && dssdev->driver->set_hdmi_infoframe) {
		struct hdmi_avi_infoframe avi;

		r = drm_hdmi_avi_infoframe_from_display_mode(&avi, adjusted_mode);
		if (r == 0)
			dssdev->driver->set_hdmi_infoframe(dssdev, &avi);
	}
}

<<<<<<< HEAD
static void omap_encoder_prepare(struct drm_encoder *encoder)
{
}

static void omap_encoder_commit(struct drm_encoder *encoder)
{
}

static const struct drm_encoder_helper_funcs omap_encoder_helper_funcs = {
	.dpms = omap_encoder_dpms,
	.mode_fixup = omap_encoder_mode_fixup,
	.mode_set = omap_encoder_mode_set,
	.prepare = omap_encoder_prepare,
	.commit = omap_encoder_commit,
};

/*
 * Instead of relying on the helpers for modeset, the omap_crtc code
 * calls these functions in the proper sequence.
 */

int omap_encoder_set_enabled(struct drm_encoder *encoder, bool enabled)
=======
static void omap_encoder_disable(struct drm_encoder *encoder)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
{
	struct omap_encoder *omap_encoder = to_omap_encoder(encoder);
	struct omap_dss_device *dssdev = omap_encoder->dssdev;
	struct omap_dss_driver *dssdrv = dssdev->driver;

<<<<<<< HEAD
	if (enabled) {
		return dssdrv->enable(dssdev);
	} else {
		dssdrv->disable(dssdev);
		return 0;
	}
}

int omap_encoder_update(struct drm_encoder *encoder,
		struct omap_overlay_manager *mgr,
		struct omap_video_timings *timings)
=======
	dssdrv->disable(dssdev);
}

static int omap_encoder_update(struct drm_encoder *encoder,
			       enum omap_channel channel,
			       struct omap_video_timings *timings)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
{
	struct drm_device *dev = encoder->dev;
	struct omap_encoder *omap_encoder = to_omap_encoder(encoder);
	struct omap_dss_device *dssdev = omap_encoder->dssdev;
	struct omap_dss_driver *dssdrv = dssdev->driver;
	int ret;

<<<<<<< HEAD
	dssdev->src->manager = mgr;
=======
	dssdev->src->manager = omap_dss_get_overlay_manager(channel);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	if (dssdrv->check_timings) {
		ret = dssdrv->check_timings(dssdev, timings);
	} else {
		struct omap_video_timings t = {0};

		dssdrv->get_timings(dssdev, &t);

		if (memcmp(timings, &t, sizeof(struct omap_video_timings)))
			ret = -EINVAL;
		else
			ret = 0;
	}

	if (ret) {
		dev_err(dev->dev, "could not set timings: %d\n", ret);
		return ret;
	}

	if (dssdrv->set_timings)
		dssdrv->set_timings(dssdev, timings);

	return 0;
}

<<<<<<< HEAD
=======
static void omap_encoder_enable(struct drm_encoder *encoder)
{
	struct omap_encoder *omap_encoder = to_omap_encoder(encoder);
	struct omap_dss_device *dssdev = omap_encoder->dssdev;
	struct omap_dss_driver *dssdrv = dssdev->driver;

	omap_encoder_update(encoder, omap_crtc_channel(encoder->crtc),
			    omap_crtc_timings(encoder->crtc));

	dssdrv->enable(dssdev);
}

static int omap_encoder_atomic_check(struct drm_encoder *encoder,
				     struct drm_crtc_state *crtc_state,
				     struct drm_connector_state *conn_state)
{
	return 0;
}

static const struct drm_encoder_helper_funcs omap_encoder_helper_funcs = {
	.mode_set = omap_encoder_mode_set,
	.disable = omap_encoder_disable,
	.enable = omap_encoder_enable,
	.atomic_check = omap_encoder_atomic_check,
};

>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
/* initialize encoder */
struct drm_encoder *omap_encoder_init(struct drm_device *dev,
		struct omap_dss_device *dssdev)
{
	struct drm_encoder *encoder = NULL;
	struct omap_encoder *omap_encoder;

	omap_encoder = kzalloc(sizeof(*omap_encoder), GFP_KERNEL);
	if (!omap_encoder)
		goto fail;

	omap_encoder->dssdev = dssdev;

	encoder = &omap_encoder->base;

	drm_encoder_init(dev, encoder, &omap_encoder_funcs,
			 DRM_MODE_ENCODER_TMDS);
	drm_encoder_helper_add(encoder, &omap_encoder_helper_funcs);

	return encoder;

fail:
	if (encoder)
		omap_encoder_destroy(encoder);

	return NULL;
}
