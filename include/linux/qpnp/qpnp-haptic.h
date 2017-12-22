<<<<<<< HEAD
/* Copyright (c) 2015, The Linux Foundation. All rights reserved.
=======
/* Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef __QPNP_HAPTIC_H
<<<<<<< HEAD
=======
#define __QPNP_HAPTIC_H
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

/* interface for the other module to play different sequences */
#ifdef CONFIG_QPNP_HAPTIC
int qpnp_hap_play_byte(u8 data, bool on);
#else
<<<<<<< HEAD
int qpnp_hap_play_byte(u8 data, bool on);
=======
static inline int qpnp_hap_play_byte(u8 data, bool on)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
{
	return 0;
}
#endif
#endif
