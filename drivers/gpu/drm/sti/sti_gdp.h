/*
 * Copyright (C) STMicroelectronics SA 2014
 * Authors: Benjamin Gaignard <benjamin.gaignard@st.com>
 *          Fabien Dessenne <fabien.dessenne@st.com>
 *          for STMicroelectronics.
 * License terms:  GNU General Public License (GPL), version 2
 */

#ifndef _STI_GDP_H_
#define _STI_GDP_H_

#include <linux/types.h>

<<<<<<< HEAD
struct sti_layer *sti_gdp_create(struct device *dev, int id);

=======
struct drm_plane *sti_gdp_create(struct drm_device *drm_dev,
				 struct device *dev, int desc,
				 void __iomem *baseaddr,
				 unsigned int possible_crtcs,
				 enum drm_plane_type type);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
#endif
