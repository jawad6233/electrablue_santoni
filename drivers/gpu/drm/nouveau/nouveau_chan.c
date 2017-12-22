/*
 * Copyright 2012 Red Hat Inc.
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
 * Authors: Ben Skeggs
 */

#include <nvif/os.h>
#include <nvif/class.h>
<<<<<<< HEAD
=======
#include <nvif/ioctl.h>
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

/*XXX*/
#include <core/client.h>

#include "nouveau_drm.h"
#include "nouveau_dma.h"
#include "nouveau_bo.h"
#include "nouveau_chan.h"
#include "nouveau_fence.h"
#include "nouveau_abi16.h"

MODULE_PARM_DESC(vram_pushbuf, "Create DMA push buffers in VRAM");
int nouveau_vram_pushbuf;
module_param_named(vram_pushbuf, nouveau_vram_pushbuf, int, 0400);

int
nouveau_channel_idle(struct nouveau_channel *chan)
{
<<<<<<< HEAD
	struct nouveau_cli *cli = (void *)nvif_client(chan->object);
	struct nouveau_fence *fence = NULL;
	int ret;

	ret = nouveau_fence_new(chan, false, &fence);
	if (!ret) {
		ret = nouveau_fence_wait(fence, false, false);
		nouveau_fence_unref(&fence);
	}

	if (ret)
		NV_PRINTK(error, cli, "failed to idle channel 0x%08x [%s]\n",
			  chan->object->handle, nvkm_client(&cli->base)->name);
	return ret;
=======
	if (likely(chan && chan->fence)) {
		struct nouveau_cli *cli = (void *)chan->user.client;
		struct nouveau_fence *fence = NULL;
		int ret;

		ret = nouveau_fence_new(chan, false, &fence);
		if (!ret) {
			ret = nouveau_fence_wait(fence, false, false);
			nouveau_fence_unref(&fence);
		}

		if (ret) {
			NV_PRINTK(err, cli, "failed to idle channel %d [%s]\n",
				  chan->chid, nvxx_client(&cli->base)->name);
			return ret;
		}
	}
	return 0;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

void
nouveau_channel_del(struct nouveau_channel **pchan)
{
	struct nouveau_channel *chan = *pchan;
	if (chan) {
<<<<<<< HEAD
		if (chan->fence) {
			nouveau_channel_idle(chan);
			nouveau_fence(chan->drm)->context_del(chan);
		}
		nvif_object_fini(&chan->nvsw);
		nvif_object_fini(&chan->gart);
		nvif_object_fini(&chan->vram);
		nvif_object_ref(NULL, &chan->object);
=======
		if (chan->fence)
			nouveau_fence(chan->drm)->context_del(chan);
		nvif_object_fini(&chan->nvsw);
		nvif_object_fini(&chan->gart);
		nvif_object_fini(&chan->vram);
		nvif_object_fini(&chan->user);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		nvif_object_fini(&chan->push.ctxdma);
		nouveau_bo_vma_del(chan->push.buffer, &chan->push.vma);
		nouveau_bo_unmap(chan->push.buffer);
		if (chan->push.buffer && chan->push.buffer->pin_refcnt)
			nouveau_bo_unpin(chan->push.buffer);
		nouveau_bo_ref(NULL, &chan->push.buffer);
<<<<<<< HEAD
		nvif_device_ref(NULL, &chan->device);
=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		kfree(chan);
	}
	*pchan = NULL;
}

static int
nouveau_channel_prep(struct nouveau_drm *drm, struct nvif_device *device,
<<<<<<< HEAD
		     u32 handle, u32 size, struct nouveau_channel **pchan)
{
	struct nouveau_cli *cli = (void *)nvif_client(&device->base);
	struct nouveau_vmmgr *vmm = nvkm_vmmgr(device);
=======
		     u32 size, struct nouveau_channel **pchan)
{
	struct nouveau_cli *cli = (void *)device->object.client;
	struct nvkm_mmu *mmu = nvxx_mmu(device);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	struct nv_dma_v0 args = {};
	struct nouveau_channel *chan;
	u32 target;
	int ret;

	chan = *pchan = kzalloc(sizeof(*chan), GFP_KERNEL);
	if (!chan)
		return -ENOMEM;

<<<<<<< HEAD
	nvif_device_ref(device, &chan->device);
	chan->drm = drm;

	/* allocate memory for dma push buffer */
	target = TTM_PL_FLAG_TT;
=======
	chan->device = device;
	chan->drm = drm;

	/* allocate memory for dma push buffer */
	target = TTM_PL_FLAG_TT | TTM_PL_FLAG_UNCACHED;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	if (nouveau_vram_pushbuf)
		target = TTM_PL_FLAG_VRAM;

	ret = nouveau_bo_new(drm->dev, size, 0, target, 0, 0, NULL, NULL,
			    &chan->push.buffer);
	if (ret == 0) {
<<<<<<< HEAD
		ret = nouveau_bo_pin(chan->push.buffer, target);
=======
		ret = nouveau_bo_pin(chan->push.buffer, target, false);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		if (ret == 0)
			ret = nouveau_bo_map(chan->push.buffer);
	}

	if (ret) {
		nouveau_channel_del(pchan);
		return ret;
	}

	/* create dma object covering the *entire* memory space that the
	 * pushbuf lives in, this is because the GEM code requires that
	 * we be able to call out to other (indirect) push buffers
	 */
	chan->push.vma.offset = chan->push.buffer->bo.offset;

	if (device->info.family >= NV_DEVICE_INFO_V0_TESLA) {
		ret = nouveau_bo_vma_add(chan->push.buffer, cli->vm,
					&chan->push.vma);
		if (ret) {
			nouveau_channel_del(pchan);
			return ret;
		}

		args.target = NV_DMA_V0_TARGET_VM;
		args.access = NV_DMA_V0_ACCESS_VM;
		args.start = 0;
<<<<<<< HEAD
		args.limit = cli->vm->vmm->limit - 1;
=======
		args.limit = cli->vm->mmu->limit - 1;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	} else
	if (chan->push.buffer->bo.mem.mem_type == TTM_PL_VRAM) {
		if (device->info.family == NV_DEVICE_INFO_V0_TNT) {
			/* nv04 vram pushbuf hack, retarget to its location in
			 * the framebuffer bar rather than direct vram access..
			 * nfi why this exists, it came from the -nv ddx.
			 */
			args.target = NV_DMA_V0_TARGET_PCI;
			args.access = NV_DMA_V0_ACCESS_RDWR;
<<<<<<< HEAD
			args.start = nv_device_resource_start(nvkm_device(device), 1);
=======
			args.start = nvxx_device(device)->func->
				resource_addr(nvxx_device(device), 1);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
			args.limit = args.start + device->info.ram_user - 1;
		} else {
			args.target = NV_DMA_V0_TARGET_VRAM;
			args.access = NV_DMA_V0_ACCESS_RDWR;
			args.start = 0;
			args.limit = device->info.ram_user - 1;
		}
	} else {
<<<<<<< HEAD
		if (chan->drm->agp.stat == ENABLED) {
=======
		if (chan->drm->agp.bridge) {
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
			args.target = NV_DMA_V0_TARGET_AGP;
			args.access = NV_DMA_V0_ACCESS_RDWR;
			args.start = chan->drm->agp.base;
			args.limit = chan->drm->agp.base +
				     chan->drm->agp.size - 1;
		} else {
			args.target = NV_DMA_V0_TARGET_VM;
			args.access = NV_DMA_V0_ACCESS_RDWR;
			args.start = 0;
<<<<<<< HEAD
			args.limit = vmm->limit - 1;
		}
	}

	ret = nvif_object_init(nvif_object(device), NULL, NVDRM_PUSH |
			       (handle & 0xffff), NV_DMA_FROM_MEMORY,
=======
			args.limit = mmu->limit - 1;
		}
	}

	ret = nvif_object_init(&device->object, 0, NV_DMA_FROM_MEMORY,
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
			       &args, sizeof(args), &chan->push.ctxdma);
	if (ret) {
		nouveau_channel_del(pchan);
		return ret;
	}

	return 0;
}

static int
nouveau_channel_ind(struct nouveau_drm *drm, struct nvif_device *device,
<<<<<<< HEAD
		    u32 handle, u32 engine, struct nouveau_channel **pchan)
{
	static const u16 oclasses[] = { KEPLER_CHANNEL_GPFIFO_A,
=======
		    u32 engine, struct nouveau_channel **pchan)
{
	static const u16 oclasses[] = { MAXWELL_CHANNEL_GPFIFO_A,
					KEPLER_CHANNEL_GPFIFO_A,
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
					FERMI_CHANNEL_GPFIFO,
					G82_CHANNEL_GPFIFO,
					NV50_CHANNEL_GPFIFO,
					0 };
	const u16 *oclass = oclasses;
	union {
		struct nv50_channel_gpfifo_v0 nv50;
<<<<<<< HEAD
		struct kepler_channel_gpfifo_a_v0 kepler;
	} args, *retn;
=======
		struct fermi_channel_gpfifo_v0 fermi;
		struct kepler_channel_gpfifo_a_v0 kepler;
	} args;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	struct nouveau_channel *chan;
	u32 size;
	int ret;

	/* allocate dma push buffer */
<<<<<<< HEAD
	ret = nouveau_channel_prep(drm, device, handle, 0x12000, &chan);
=======
	ret = nouveau_channel_prep(drm, device, 0x12000, &chan);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	*pchan = chan;
	if (ret)
		return ret;

	/* create channel object */
	do {
		if (oclass[0] >= KEPLER_CHANNEL_GPFIFO_A) {
			args.kepler.version = 0;
			args.kepler.engine  = engine;
<<<<<<< HEAD
			args.kepler.pushbuf = chan->push.ctxdma.handle;
			args.kepler.ilength = 0x02000;
			args.kepler.ioffset = 0x10000 + chan->push.vma.offset;
			size = sizeof(args.kepler);
		} else {
			args.nv50.version = 0;
			args.nv50.pushbuf = chan->push.ctxdma.handle;
			args.nv50.ilength = 0x02000;
			args.nv50.ioffset = 0x10000 + chan->push.vma.offset;
			size = sizeof(args.nv50);
		}

		ret = nvif_object_new(nvif_object(device), handle, *oclass++,
				      &args, size, &chan->object);
		if (ret == 0) {
			retn = chan->object->data;
			if (chan->object->oclass >= KEPLER_CHANNEL_GPFIFO_A)
				chan->chid = retn->kepler.chid;
			else
				chan->chid = retn->nv50.chid;
=======
			args.kepler.ilength = 0x02000;
			args.kepler.ioffset = 0x10000 + chan->push.vma.offset;
			args.kepler.vm = 0;
			size = sizeof(args.kepler);
		} else
		if (oclass[0] >= FERMI_CHANNEL_GPFIFO) {
			args.fermi.version = 0;
			args.fermi.ilength = 0x02000;
			args.fermi.ioffset = 0x10000 + chan->push.vma.offset;
			args.fermi.vm = 0;
			size = sizeof(args.fermi);
		} else {
			args.nv50.version = 0;
			args.nv50.ilength = 0x02000;
			args.nv50.ioffset = 0x10000 + chan->push.vma.offset;
			args.nv50.pushbuf = nvif_handle(&chan->push.ctxdma);
			args.nv50.vm = 0;
			size = sizeof(args.nv50);
		}

		ret = nvif_object_init(&device->object, 0, *oclass++,
				       &args, size, &chan->user);
		if (ret == 0) {
			if (chan->user.oclass >= KEPLER_CHANNEL_GPFIFO_A)
				chan->chid = args.kepler.chid;
			else
			if (chan->user.oclass >= FERMI_CHANNEL_GPFIFO)
				chan->chid = args.fermi.chid;
			else
				chan->chid = args.nv50.chid;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
			return ret;
		}
	} while (*oclass);

	nouveau_channel_del(pchan);
	return ret;
}

static int
nouveau_channel_dma(struct nouveau_drm *drm, struct nvif_device *device,
<<<<<<< HEAD
		    u32 handle, struct nouveau_channel **pchan)
=======
		    struct nouveau_channel **pchan)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
{
	static const u16 oclasses[] = { NV40_CHANNEL_DMA,
					NV17_CHANNEL_DMA,
					NV10_CHANNEL_DMA,
					NV03_CHANNEL_DMA,
					0 };
	const u16 *oclass = oclasses;
<<<<<<< HEAD
	struct nv03_channel_dma_v0 args, *retn;
=======
	struct nv03_channel_dma_v0 args;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	struct nouveau_channel *chan;
	int ret;

	/* allocate dma push buffer */
<<<<<<< HEAD
	ret = nouveau_channel_prep(drm, device, handle, 0x10000, &chan);
=======
	ret = nouveau_channel_prep(drm, device, 0x10000, &chan);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	*pchan = chan;
	if (ret)
		return ret;

	/* create channel object */
	args.version = 0;
<<<<<<< HEAD
	args.pushbuf = chan->push.ctxdma.handle;
	args.offset = chan->push.vma.offset;

	do {
		ret = nvif_object_new(nvif_object(device), handle, *oclass++,
				      &args, sizeof(args), &chan->object);
		if (ret == 0) {
			retn = chan->object->data;
			chan->chid = retn->chid;
=======
	args.pushbuf = nvif_handle(&chan->push.ctxdma);
	args.offset = chan->push.vma.offset;

	do {
		ret = nvif_object_init(&device->object, 0, *oclass++,
				       &args, sizeof(args), &chan->user);
		if (ret == 0) {
			chan->chid = args.chid;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
			return ret;
		}
	} while (ret && *oclass);

	nouveau_channel_del(pchan);
	return ret;
}

static int
nouveau_channel_init(struct nouveau_channel *chan, u32 vram, u32 gart)
{
	struct nvif_device *device = chan->device;
<<<<<<< HEAD
	struct nouveau_cli *cli = (void *)nvif_client(&device->base);
	struct nouveau_vmmgr *vmm = nvkm_vmmgr(device);
	struct nouveau_software_chan *swch;
	struct nv_dma_v0 args = {};
	int ret, i;
	bool save;

	nvif_object_map(chan->object);
=======
	struct nouveau_cli *cli = (void *)chan->user.client;
	struct nvkm_mmu *mmu = nvxx_mmu(device);
	struct nv_dma_v0 args = {};
	int ret, i;

	nvif_object_map(&chan->user);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	/* allocate dma objects to cover all allowed vram, and gart */
	if (device->info.family < NV_DEVICE_INFO_V0_FERMI) {
		if (device->info.family >= NV_DEVICE_INFO_V0_TESLA) {
			args.target = NV_DMA_V0_TARGET_VM;
			args.access = NV_DMA_V0_ACCESS_VM;
			args.start = 0;
<<<<<<< HEAD
			args.limit = cli->vm->vmm->limit - 1;
=======
			args.limit = cli->vm->mmu->limit - 1;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		} else {
			args.target = NV_DMA_V0_TARGET_VRAM;
			args.access = NV_DMA_V0_ACCESS_RDWR;
			args.start = 0;
			args.limit = device->info.ram_user - 1;
		}

<<<<<<< HEAD
		ret = nvif_object_init(chan->object, NULL, vram,
				       NV_DMA_IN_MEMORY, &args,
				       sizeof(args), &chan->vram);
=======
		ret = nvif_object_init(&chan->user, vram, NV_DMA_IN_MEMORY,
				       &args, sizeof(args), &chan->vram);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		if (ret)
			return ret;

		if (device->info.family >= NV_DEVICE_INFO_V0_TESLA) {
			args.target = NV_DMA_V0_TARGET_VM;
			args.access = NV_DMA_V0_ACCESS_VM;
			args.start = 0;
<<<<<<< HEAD
			args.limit = cli->vm->vmm->limit - 1;
		} else
		if (chan->drm->agp.stat == ENABLED) {
=======
			args.limit = cli->vm->mmu->limit - 1;
		} else
		if (chan->drm->agp.bridge) {
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
			args.target = NV_DMA_V0_TARGET_AGP;
			args.access = NV_DMA_V0_ACCESS_RDWR;
			args.start = chan->drm->agp.base;
			args.limit = chan->drm->agp.base +
				     chan->drm->agp.size - 1;
		} else {
			args.target = NV_DMA_V0_TARGET_VM;
			args.access = NV_DMA_V0_ACCESS_RDWR;
			args.start = 0;
<<<<<<< HEAD
			args.limit = vmm->limit - 1;
		}

		ret = nvif_object_init(chan->object, NULL, gart,
				       NV_DMA_IN_MEMORY, &args,
				       sizeof(args), &chan->gart);
=======
			args.limit = mmu->limit - 1;
		}

		ret = nvif_object_init(&chan->user, gart, NV_DMA_IN_MEMORY,
				       &args, sizeof(args), &chan->gart);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		if (ret)
			return ret;
	}

	/* initialise dma tracking parameters */
<<<<<<< HEAD
	switch (chan->object->oclass & 0x00ff) {
=======
	switch (chan->user.oclass & 0x00ff) {
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	case 0x006b:
	case 0x006e:
		chan->user_put = 0x40;
		chan->user_get = 0x44;
		chan->dma.max = (0x10000 / 4) - 2;
		break;
	default:
		chan->user_put = 0x40;
		chan->user_get = 0x44;
		chan->user_get_hi = 0x60;
		chan->dma.ib_base =  0x10000 / 4;
		chan->dma.ib_max  = (0x02000 / 8) - 1;
		chan->dma.ib_put  = 0;
		chan->dma.ib_free = chan->dma.ib_max - chan->dma.ib_put;
		chan->dma.max = chan->dma.ib_base;
		break;
	}

	chan->dma.put = 0;
	chan->dma.cur = chan->dma.put;
	chan->dma.free = chan->dma.max - chan->dma.cur;

	ret = RING_SPACE(chan, NOUVEAU_DMA_SKIPS);
	if (ret)
		return ret;

	for (i = 0; i < NOUVEAU_DMA_SKIPS; i++)
		OUT_RING(chan, 0x00000000);

	/* allocate software object class (used for fences on <= nv05) */
	if (device->info.family < NV_DEVICE_INFO_V0_CELSIUS) {
<<<<<<< HEAD
		ret = nvif_object_init(chan->object, NULL, 0x006e, 0x006e,
=======
		ret = nvif_object_init(&chan->user, 0x006e,
				       NVIF_IOCTL_NEW_V0_SW_NV04,
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
				       NULL, 0, &chan->nvsw);
		if (ret)
			return ret;

<<<<<<< HEAD
		swch = (void *)nvkm_object(&chan->nvsw)->parent;
		swch->flip = nouveau_flip_complete;
		swch->flip_data = chan;

=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		ret = RING_SPACE(chan, 2);
		if (ret)
			return ret;

		BEGIN_NV04(chan, NvSubSw, 0x0000, 1);
		OUT_RING  (chan, chan->nvsw.handle);
		FIRE_RING (chan);
	}

	/* initialise synchronisation */
<<<<<<< HEAD
	save = cli->base.super;
	cli->base.super = true; /* hack until fencenv50 fixed */
	ret = nouveau_fence(chan->drm)->context_new(chan);
	cli->base.super = save;
	return ret;
=======
	return nouveau_fence(chan->drm)->context_new(chan);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

int
nouveau_channel_new(struct nouveau_drm *drm, struct nvif_device *device,
<<<<<<< HEAD
		    u32 handle, u32 arg0, u32 arg1,
		    struct nouveau_channel **pchan)
{
	struct nouveau_cli *cli = (void *)nvif_client(&device->base);
=======
		    u32 arg0, u32 arg1, struct nouveau_channel **pchan)
{
	struct nouveau_cli *cli = (void *)device->object.client;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	bool super;
	int ret;

	/* hack until fencenv50 is fixed, and agp access relaxed */
	super = cli->base.super;
	cli->base.super = true;

<<<<<<< HEAD
	ret = nouveau_channel_ind(drm, device, handle, arg0, pchan);
	if (ret) {
		NV_PRINTK(debug, cli, "ib channel create, %d\n", ret);
		ret = nouveau_channel_dma(drm, device, handle, pchan);
		if (ret) {
			NV_PRINTK(debug, cli, "dma channel create, %d\n", ret);
=======
	ret = nouveau_channel_ind(drm, device, arg0, pchan);
	if (ret) {
		NV_PRINTK(dbg, cli, "ib channel create, %d\n", ret);
		ret = nouveau_channel_dma(drm, device, pchan);
		if (ret) {
			NV_PRINTK(dbg, cli, "dma channel create, %d\n", ret);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
			goto done;
		}
	}

	ret = nouveau_channel_init(*pchan, arg0, arg1);
	if (ret) {
<<<<<<< HEAD
		NV_PRINTK(error, cli, "channel failed to initialise, %d\n", ret);
=======
		NV_PRINTK(err, cli, "channel failed to initialise, %d\n", ret);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		nouveau_channel_del(pchan);
	}

done:
	cli->base.super = super;
	return ret;
}
