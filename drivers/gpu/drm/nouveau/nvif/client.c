/*
 * Copyright 2013 Red Hat Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors: Ben Skeggs <bskeggs@redhat.com>
 */

<<<<<<< HEAD
#include "client.h"
#include "driver.h"
#include "ioctl.h"
=======
#include <nvif/client.h>
#include <nvif/driver.h>
#include <nvif/ioctl.h>
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

int
nvif_client_ioctl(struct nvif_client *client, void *data, u32 size)
{
<<<<<<< HEAD
	return client->driver->ioctl(client->base.priv, client->super, data, size, NULL);
=======
	return client->driver->ioctl(client->object.priv, client->super, data, size, NULL);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

int
nvif_client_suspend(struct nvif_client *client)
{
<<<<<<< HEAD
	return client->driver->suspend(client->base.priv);
=======
	return client->driver->suspend(client->object.priv);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

int
nvif_client_resume(struct nvif_client *client)
{
<<<<<<< HEAD
	return client->driver->resume(client->base.priv);
=======
	return client->driver->resume(client->object.priv);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

void
nvif_client_fini(struct nvif_client *client)
{
	if (client->driver) {
<<<<<<< HEAD
		client->driver->fini(client->base.priv);
		client->driver = NULL;
		client->base.parent = NULL;
		nvif_object_fini(&client->base);
=======
		client->driver->fini(client->object.priv);
		client->driver = NULL;
		client->object.client = NULL;
		nvif_object_fini(&client->object);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	}
}

const struct nvif_driver *
nvif_drivers[] = {
#ifdef __KERNEL__
	&nvif_driver_nvkm,
#else
	&nvif_driver_drm,
	&nvif_driver_lib,
<<<<<<< HEAD
=======
	&nvif_driver_null,
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
#endif
	NULL
};

int
<<<<<<< HEAD
nvif_client_init(void (*dtor)(struct nvif_client *), const char *driver,
		 const char *name, u64 device, const char *cfg, const char *dbg,
		 struct nvif_client *client)
{
	int ret, i;

	ret = nvif_object_init(NULL, (void*)dtor, 0, 0, NULL, 0, &client->base);
	if (ret)
		return ret;

	client->base.parent = &client->base;
	client->base.handle = ~0;
	client->object = &client->base;
=======
nvif_client_init(const char *driver, const char *name, u64 device,
		 const char *cfg, const char *dbg, struct nvif_client *client)
{
	struct {
		struct nvif_ioctl_v0 ioctl;
		struct nvif_ioctl_nop_v0 nop;
	} args = {};
	int ret, i;

	ret = nvif_object_init(NULL, 0, 0, NULL, 0, &client->object);
	if (ret)
		return ret;

	client->object.client = client;
	client->object.handle = ~0;
	client->route = NVIF_IOCTL_V0_ROUTE_NVIF;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	client->super = true;

	for (i = 0, ret = -EINVAL; (client->driver = nvif_drivers[i]); i++) {
		if (!driver || !strcmp(client->driver->name, driver)) {
			ret = client->driver->init(name, device, cfg, dbg,
<<<<<<< HEAD
						  &client->base.priv);
=======
						  &client->object.priv);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
			if (!ret || driver)
				break;
		}
	}

<<<<<<< HEAD
=======
	if (ret == 0) {
		ret = nvif_client_ioctl(client, &args, sizeof(args));
		client->version = args.nop.version;
	}

>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	if (ret)
		nvif_client_fini(client);
	return ret;
}
<<<<<<< HEAD

static void
nvif_client_del(struct nvif_client *client)
{
	nvif_client_fini(client);
	kfree(client);
}

int
nvif_client_new(const char *driver, const char *name, u64 device,
		const char *cfg, const char *dbg,
		struct nvif_client **pclient)
{
	struct nvif_client *client = kzalloc(sizeof(*client), GFP_KERNEL);
	if (client) {
		int ret = nvif_client_init(nvif_client_del, driver, name,
					   device, cfg, dbg, client);
		if (ret) {
			kfree(client);
			client = NULL;
		}
		*pclient = client;
		return ret;
	}
	return -ENOMEM;
}

void
nvif_client_ref(struct nvif_client *client, struct nvif_client **pclient)
{
	nvif_object_ref(&client->base, (struct nvif_object **)pclient);
}
=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
