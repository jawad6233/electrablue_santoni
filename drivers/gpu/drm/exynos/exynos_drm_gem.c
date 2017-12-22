/* exynos_drm_gem.c
 *
 * Copyright (c) 2011 Samsung Electronics Co., Ltd.
 * Author: Inki Dae <inki.dae@samsung.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <drm/drmP.h>
#include <drm/drm_vma_manager.h>

#include <linux/shmem_fs.h>
<<<<<<< HEAD
=======
#include <linux/dma-buf.h>
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
#include <drm/exynos_drm.h>

#include "exynos_drm_drv.h"
#include "exynos_drm_gem.h"
<<<<<<< HEAD
#include "exynos_drm_buf.h"
#include "exynos_drm_iommu.h"

static unsigned int convert_to_vm_err_msg(int msg)
{
	unsigned int out_msg;

	switch (msg) {
	case 0:
	case -ERESTARTSYS:
	case -EINTR:
		out_msg = VM_FAULT_NOPAGE;
		break;

	case -ENOMEM:
		out_msg = VM_FAULT_OOM;
		break;

	default:
		out_msg = VM_FAULT_SIGBUS;
		break;
	}

	return out_msg;
}

static int check_gem_flags(unsigned int flags)
{
	if (flags & ~(EXYNOS_BO_MASK)) {
		DRM_ERROR("invalid flags.\n");
		return -EINVAL;
	}

	return 0;
}

static void update_vm_cache_attr(struct exynos_drm_gem_obj *obj,
					struct vm_area_struct *vma)
{
	DRM_DEBUG_KMS("flags = 0x%x\n", obj->flags);

	/* non-cachable as default. */
	if (obj->flags & EXYNOS_BO_CACHABLE)
		vma->vm_page_prot = vm_get_page_prot(vma->vm_flags);
	else if (obj->flags & EXYNOS_BO_WC)
		vma->vm_page_prot =
			pgprot_writecombine(vm_get_page_prot(vma->vm_flags));
	else
		vma->vm_page_prot =
			pgprot_noncached(vm_get_page_prot(vma->vm_flags));
}

static unsigned long roundup_gem_size(unsigned long size, unsigned int flags)
{
	/* TODO */

	return roundup(size, PAGE_SIZE);
}

static int exynos_drm_gem_map_buf(struct drm_gem_object *obj,
					struct vm_area_struct *vma,
					unsigned long f_vaddr,
					pgoff_t page_offset)
{
	struct exynos_drm_gem_obj *exynos_gem_obj = to_exynos_gem_obj(obj);
	struct exynos_drm_gem_buf *buf = exynos_gem_obj->buffer;
	struct scatterlist *sgl;
	unsigned long pfn;
	int i;

	if (!buf->sgt)
		return -EINTR;

	if (page_offset >= (buf->size >> PAGE_SHIFT)) {
		DRM_ERROR("invalid page offset\n");
		return -EINVAL;
	}

	sgl = buf->sgt->sgl;
	for_each_sg(buf->sgt->sgl, sgl, buf->sgt->nents, i) {
		if (page_offset < (sgl->length >> PAGE_SHIFT))
			break;
		page_offset -=	(sgl->length >> PAGE_SHIFT);
	}

	pfn = __phys_to_pfn(sg_phys(sgl)) + page_offset;

	return vm_insert_mixed(vma, f_vaddr, pfn);
=======
#include "exynos_drm_iommu.h"

static int exynos_drm_alloc_buf(struct exynos_drm_gem *exynos_gem)
{
	struct drm_device *dev = exynos_gem->base.dev;
	enum dma_attr attr;
	unsigned int nr_pages;
	struct sg_table sgt;
	int ret = -ENOMEM;

	if (exynos_gem->dma_addr) {
		DRM_DEBUG_KMS("already allocated.\n");
		return 0;
	}

	init_dma_attrs(&exynos_gem->dma_attrs);

	/*
	 * if EXYNOS_BO_CONTIG, fully physically contiguous memory
	 * region will be allocated else physically contiguous
	 * as possible.
	 */
	if (!(exynos_gem->flags & EXYNOS_BO_NONCONTIG))
		dma_set_attr(DMA_ATTR_FORCE_CONTIGUOUS, &exynos_gem->dma_attrs);

	/*
	 * if EXYNOS_BO_WC or EXYNOS_BO_NONCACHABLE, writecombine mapping
	 * else cachable mapping.
	 */
	if (exynos_gem->flags & EXYNOS_BO_WC ||
			!(exynos_gem->flags & EXYNOS_BO_CACHABLE))
		attr = DMA_ATTR_WRITE_COMBINE;
	else
		attr = DMA_ATTR_NON_CONSISTENT;

	dma_set_attr(attr, &exynos_gem->dma_attrs);
	dma_set_attr(DMA_ATTR_NO_KERNEL_MAPPING, &exynos_gem->dma_attrs);

	nr_pages = exynos_gem->size >> PAGE_SHIFT;

	exynos_gem->pages = drm_calloc_large(nr_pages, sizeof(struct page *));
	if (!exynos_gem->pages) {
		DRM_ERROR("failed to allocate pages.\n");
		return -ENOMEM;
	}

	exynos_gem->cookie = dma_alloc_attrs(dev->dev, exynos_gem->size,
					     &exynos_gem->dma_addr, GFP_KERNEL,
					     &exynos_gem->dma_attrs);
	if (!exynos_gem->cookie) {
		DRM_ERROR("failed to allocate buffer.\n");
		goto err_free;
	}

	ret = dma_get_sgtable_attrs(dev->dev, &sgt, exynos_gem->cookie,
				    exynos_gem->dma_addr, exynos_gem->size,
				    &exynos_gem->dma_attrs);
	if (ret < 0) {
		DRM_ERROR("failed to get sgtable.\n");
		goto err_dma_free;
	}

	if (drm_prime_sg_to_page_addr_arrays(&sgt, exynos_gem->pages, NULL,
					     nr_pages)) {
		DRM_ERROR("invalid sgtable.\n");
		ret = -EINVAL;
		goto err_sgt_free;
	}

	sg_free_table(&sgt);

	DRM_DEBUG_KMS("dma_addr(0x%lx), size(0x%lx)\n",
			(unsigned long)exynos_gem->dma_addr, exynos_gem->size);

	return 0;

err_sgt_free:
	sg_free_table(&sgt);
err_dma_free:
	dma_free_attrs(dev->dev, exynos_gem->size, exynos_gem->cookie,
		       exynos_gem->dma_addr, &exynos_gem->dma_attrs);
err_free:
	drm_free_large(exynos_gem->pages);

	return ret;
}

static void exynos_drm_free_buf(struct exynos_drm_gem *exynos_gem)
{
	struct drm_device *dev = exynos_gem->base.dev;

	if (!exynos_gem->dma_addr) {
		DRM_DEBUG_KMS("dma_addr is invalid.\n");
		return;
	}

	DRM_DEBUG_KMS("dma_addr(0x%lx), size(0x%lx)\n",
			(unsigned long)exynos_gem->dma_addr, exynos_gem->size);

	dma_free_attrs(dev->dev, exynos_gem->size, exynos_gem->cookie,
			(dma_addr_t)exynos_gem->dma_addr,
			&exynos_gem->dma_attrs);

	drm_free_large(exynos_gem->pages);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

static int exynos_drm_gem_handle_create(struct drm_gem_object *obj,
					struct drm_file *file_priv,
					unsigned int *handle)
{
	int ret;

	/*
	 * allocate a id of idr table where the obj is registered
	 * and handle has the id what user can see.
	 */
	ret = drm_gem_handle_create(file_priv, obj, handle);
	if (ret)
		return ret;

	DRM_DEBUG_KMS("gem handle = 0x%x\n", *handle);

	/* drop reference from allocate - handle holds it now. */
	drm_gem_object_unreference_unlocked(obj);

	return 0;
}

<<<<<<< HEAD
void exynos_drm_gem_destroy(struct exynos_drm_gem_obj *exynos_gem_obj)
{
	struct drm_gem_object *obj;
	struct exynos_drm_gem_buf *buf;

	obj = &exynos_gem_obj->base;
	buf = exynos_gem_obj->buffer;
=======
void exynos_drm_gem_destroy(struct exynos_drm_gem *exynos_gem)
{
	struct drm_gem_object *obj = &exynos_gem->base;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	DRM_DEBUG_KMS("handle count = %d\n", obj->handle_count);

	/*
	 * do not release memory region from exporter.
	 *
	 * the region will be released by exporter
	 * once dmabuf's refcount becomes 0.
	 */
	if (obj->import_attach)
<<<<<<< HEAD
		goto out;

	exynos_drm_free_buf(obj->dev, exynos_gem_obj->flags, buf);

out:
	exynos_drm_fini_buf(obj->dev, buf);
	exynos_gem_obj->buffer = NULL;

	drm_gem_free_mmap_offset(obj);
=======
		drm_prime_gem_destroy(obj, exynos_gem->sgt);
	else
		exynos_drm_free_buf(exynos_gem);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	/* release file pointer to gem object. */
	drm_gem_object_release(obj);

<<<<<<< HEAD
	kfree(exynos_gem_obj);
	exynos_gem_obj = NULL;
=======
	kfree(exynos_gem);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

unsigned long exynos_drm_gem_get_size(struct drm_device *dev,
						unsigned int gem_handle,
						struct drm_file *file_priv)
{
<<<<<<< HEAD
	struct exynos_drm_gem_obj *exynos_gem_obj;
=======
	struct exynos_drm_gem *exynos_gem;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	struct drm_gem_object *obj;

	obj = drm_gem_object_lookup(dev, file_priv, gem_handle);
	if (!obj) {
		DRM_ERROR("failed to lookup gem object.\n");
		return 0;
	}

<<<<<<< HEAD
	exynos_gem_obj = to_exynos_gem_obj(obj);

	drm_gem_object_unreference_unlocked(obj);

	return exynos_gem_obj->buffer->size;
}


struct exynos_drm_gem_obj *exynos_drm_gem_init(struct drm_device *dev,
						      unsigned long size)
{
	struct exynos_drm_gem_obj *exynos_gem_obj;
	struct drm_gem_object *obj;
	int ret;

	exynos_gem_obj = kzalloc(sizeof(*exynos_gem_obj), GFP_KERNEL);
	if (!exynos_gem_obj)
		return NULL;

	exynos_gem_obj->size = size;
	obj = &exynos_gem_obj->base;
=======
	exynos_gem = to_exynos_gem(obj);

	drm_gem_object_unreference_unlocked(obj);

	return exynos_gem->size;
}

static struct exynos_drm_gem *exynos_drm_gem_init(struct drm_device *dev,
						  unsigned long size)
{
	struct exynos_drm_gem *exynos_gem;
	struct drm_gem_object *obj;
	int ret;

	exynos_gem = kzalloc(sizeof(*exynos_gem), GFP_KERNEL);
	if (!exynos_gem)
		return ERR_PTR(-ENOMEM);

	exynos_gem->size = size;
	obj = &exynos_gem->base;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	ret = drm_gem_object_init(dev, obj, size);
	if (ret < 0) {
		DRM_ERROR("failed to initialize gem object\n");
<<<<<<< HEAD
		kfree(exynos_gem_obj);
		return NULL;
=======
		kfree(exynos_gem);
		return ERR_PTR(ret);
	}

	ret = drm_gem_create_mmap_offset(obj);
	if (ret < 0) {
		drm_gem_object_release(obj);
		kfree(exynos_gem);
		return ERR_PTR(ret);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	}

	DRM_DEBUG_KMS("created file object = 0x%x\n", (unsigned int)obj->filp);

<<<<<<< HEAD
	return exynos_gem_obj;
}

struct exynos_drm_gem_obj *exynos_drm_gem_create(struct drm_device *dev,
						unsigned int flags,
						unsigned long size)
{
	struct exynos_drm_gem_obj *exynos_gem_obj;
	struct exynos_drm_gem_buf *buf;
	int ret;

=======
	return exynos_gem;
}

struct exynos_drm_gem *exynos_drm_gem_create(struct drm_device *dev,
					     unsigned int flags,
					     unsigned long size)
{
	struct exynos_drm_gem *exynos_gem;
	int ret;

	if (flags & ~(EXYNOS_BO_MASK)) {
		DRM_ERROR("invalid flags.\n");
		return ERR_PTR(-EINVAL);
	}

>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	if (!size) {
		DRM_ERROR("invalid size.\n");
		return ERR_PTR(-EINVAL);
	}

<<<<<<< HEAD
	size = roundup_gem_size(size, flags);

	ret = check_gem_flags(flags);
	if (ret)
		return ERR_PTR(ret);

	buf = exynos_drm_init_buf(dev, size);
	if (!buf)
		return ERR_PTR(-ENOMEM);

	exynos_gem_obj = exynos_drm_gem_init(dev, size);
	if (!exynos_gem_obj) {
		ret = -ENOMEM;
		goto err_fini_buf;
	}

	exynos_gem_obj->buffer = buf;

	/* set memory type and cache attribute from user side. */
	exynos_gem_obj->flags = flags;

	ret = exynos_drm_alloc_buf(dev, buf, flags);
	if (ret < 0)
		goto err_gem_fini;

	return exynos_gem_obj;

err_gem_fini:
	drm_gem_object_release(&exynos_gem_obj->base);
	kfree(exynos_gem_obj);
err_fini_buf:
	exynos_drm_fini_buf(dev, buf);
	return ERR_PTR(ret);
=======
	size = roundup(size, PAGE_SIZE);

	exynos_gem = exynos_drm_gem_init(dev, size);
	if (IS_ERR(exynos_gem))
		return exynos_gem;

	/* set memory type and cache attribute from user side. */
	exynos_gem->flags = flags;

	ret = exynos_drm_alloc_buf(exynos_gem);
	if (ret < 0) {
		drm_gem_object_release(&exynos_gem->base);
		kfree(exynos_gem);
		return ERR_PTR(ret);
	}

	return exynos_gem;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

int exynos_drm_gem_create_ioctl(struct drm_device *dev, void *data,
				struct drm_file *file_priv)
{
	struct drm_exynos_gem_create *args = data;
<<<<<<< HEAD
	struct exynos_drm_gem_obj *exynos_gem_obj;
	int ret;

	exynos_gem_obj = exynos_drm_gem_create(dev, args->flags, args->size);
	if (IS_ERR(exynos_gem_obj))
		return PTR_ERR(exynos_gem_obj);

	ret = exynos_drm_gem_handle_create(&exynos_gem_obj->base, file_priv,
			&args->handle);
	if (ret) {
		exynos_drm_gem_destroy(exynos_gem_obj);
=======
	struct exynos_drm_gem *exynos_gem;
	int ret;

	exynos_gem = exynos_drm_gem_create(dev, args->flags, args->size);
	if (IS_ERR(exynos_gem))
		return PTR_ERR(exynos_gem);

	ret = exynos_drm_gem_handle_create(&exynos_gem->base, file_priv,
					   &args->handle);
	if (ret) {
		exynos_drm_gem_destroy(exynos_gem);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		return ret;
	}

	return 0;
}

dma_addr_t *exynos_drm_gem_get_dma_addr(struct drm_device *dev,
					unsigned int gem_handle,
					struct drm_file *filp)
{
<<<<<<< HEAD
	struct exynos_drm_gem_obj *exynos_gem_obj;
=======
	struct exynos_drm_gem *exynos_gem;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	struct drm_gem_object *obj;

	obj = drm_gem_object_lookup(dev, filp, gem_handle);
	if (!obj) {
		DRM_ERROR("failed to lookup gem object.\n");
		return ERR_PTR(-EINVAL);
	}

<<<<<<< HEAD
	exynos_gem_obj = to_exynos_gem_obj(obj);

	return &exynos_gem_obj->buffer->dma_addr;
=======
	exynos_gem = to_exynos_gem(obj);

	return &exynos_gem->dma_addr;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

void exynos_drm_gem_put_dma_addr(struct drm_device *dev,
					unsigned int gem_handle,
					struct drm_file *filp)
{
	struct drm_gem_object *obj;

	obj = drm_gem_object_lookup(dev, filp, gem_handle);
	if (!obj) {
		DRM_ERROR("failed to lookup gem object.\n");
		return;
	}

	drm_gem_object_unreference_unlocked(obj);

	/*
	 * decrease obj->refcount one more time because we has already
	 * increased it at exynos_drm_gem_get_dma_addr().
	 */
	drm_gem_object_unreference_unlocked(obj);
}

<<<<<<< HEAD
int exynos_drm_gem_mmap_buffer(struct exynos_drm_gem_obj *exynos_gem_obj,
				      struct vm_area_struct *vma)
{
	struct drm_device *drm_dev = exynos_gem_obj->base.dev;
	struct exynos_drm_gem_buf *buffer;
=======
static int exynos_drm_gem_mmap_buffer(struct exynos_drm_gem *exynos_gem,
				      struct vm_area_struct *vma)
{
	struct drm_device *drm_dev = exynos_gem->base.dev;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	unsigned long vm_size;
	int ret;

	vma->vm_flags &= ~VM_PFNMAP;
	vma->vm_pgoff = 0;

	vm_size = vma->vm_end - vma->vm_start;

<<<<<<< HEAD
	/*
	 * a buffer contains information to physically continuous memory
	 * allocated by user request or at framebuffer creation.
	 */
	buffer = exynos_gem_obj->buffer;

	/* check if user-requested size is valid. */
	if (vm_size > buffer->size)
		return -EINVAL;

	ret = dma_mmap_attrs(drm_dev->dev, vma, buffer->pages,
				buffer->dma_addr, buffer->size,
				&buffer->dma_attrs);
=======
	/* check if user-requested size is valid. */
	if (vm_size > exynos_gem->size)
		return -EINVAL;

	ret = dma_mmap_attrs(drm_dev->dev, vma, exynos_gem->pages,
			     exynos_gem->dma_addr, exynos_gem->size,
			     &exynos_gem->dma_attrs);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	if (ret < 0) {
		DRM_ERROR("failed to mmap.\n");
		return ret;
	}

	return 0;
}

int exynos_drm_gem_get_ioctl(struct drm_device *dev, void *data,
				      struct drm_file *file_priv)
<<<<<<< HEAD
{	struct exynos_drm_gem_obj *exynos_gem_obj;
=======
{
	struct exynos_drm_gem *exynos_gem;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	struct drm_exynos_gem_info *args = data;
	struct drm_gem_object *obj;

	mutex_lock(&dev->struct_mutex);

	obj = drm_gem_object_lookup(dev, file_priv, args->handle);
	if (!obj) {
		DRM_ERROR("failed to lookup gem object.\n");
		mutex_unlock(&dev->struct_mutex);
		return -EINVAL;
	}

<<<<<<< HEAD
	exynos_gem_obj = to_exynos_gem_obj(obj);

	args->flags = exynos_gem_obj->flags;
	args->size = exynos_gem_obj->size;
=======
	exynos_gem = to_exynos_gem(obj);

	args->flags = exynos_gem->flags;
	args->size = exynos_gem->size;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	drm_gem_object_unreference(obj);
	mutex_unlock(&dev->struct_mutex);

	return 0;
}

<<<<<<< HEAD
struct vm_area_struct *exynos_gem_get_vma(struct vm_area_struct *vma)
{
	struct vm_area_struct *vma_copy;

	vma_copy = kmalloc(sizeof(*vma_copy), GFP_KERNEL);
	if (!vma_copy)
		return NULL;

	if (vma->vm_ops && vma->vm_ops->open)
		vma->vm_ops->open(vma);

	if (vma->vm_file)
		get_file(vma->vm_file);

	memcpy(vma_copy, vma, sizeof(*vma));

	vma_copy->vm_mm = NULL;
	vma_copy->vm_next = NULL;
	vma_copy->vm_prev = NULL;

	return vma_copy;
}

void exynos_gem_put_vma(struct vm_area_struct *vma)
{
	if (!vma)
		return;

	if (vma->vm_ops && vma->vm_ops->close)
		vma->vm_ops->close(vma);

	if (vma->vm_file)
		fput(vma->vm_file);

	kfree(vma);
}

int exynos_gem_get_pages_from_userptr(unsigned long start,
						unsigned int npages,
						struct page **pages,
						struct vm_area_struct *vma)
{
	int get_npages;

	/* the memory region mmaped with VM_PFNMAP. */
	if (vma_is_io(vma)) {
		unsigned int i;

		for (i = 0; i < npages; ++i, start += PAGE_SIZE) {
			unsigned long pfn;
			int ret = follow_pfn(vma, start, &pfn);
			if (ret)
				return ret;

			pages[i] = pfn_to_page(pfn);
		}

		if (i != npages) {
			DRM_ERROR("failed to get user_pages.\n");
			return -EINVAL;
		}

		return 0;
	}

	get_npages = get_user_pages(current, current->mm, start,
					npages, 1, 1, pages, NULL);
	get_npages = max(get_npages, 0);
	if (get_npages != npages) {
		DRM_ERROR("failed to get user_pages.\n");
		while (get_npages)
			put_page(pages[--get_npages]);
		return -EFAULT;
	}

	return 0;
}

void exynos_gem_put_pages_to_userptr(struct page **pages,
					unsigned int npages,
					struct vm_area_struct *vma)
{
	if (!vma_is_io(vma)) {
		unsigned int i;

		for (i = 0; i < npages; i++) {
			set_page_dirty_lock(pages[i]);

			/*
			 * undo the reference we took when populating
			 * the table.
			 */
			put_page(pages[i]);
		}
	}
}

=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
int exynos_gem_map_sgt_with_dma(struct drm_device *drm_dev,
				struct sg_table *sgt,
				enum dma_data_direction dir)
{
	int nents;

	mutex_lock(&drm_dev->struct_mutex);

	nents = dma_map_sg(drm_dev->dev, sgt->sgl, sgt->nents, dir);
	if (!nents) {
		DRM_ERROR("failed to map sgl with dma.\n");
		mutex_unlock(&drm_dev->struct_mutex);
		return nents;
	}

	mutex_unlock(&drm_dev->struct_mutex);
	return 0;
}

void exynos_gem_unmap_sgt_from_dma(struct drm_device *drm_dev,
				struct sg_table *sgt,
				enum dma_data_direction dir)
{
	dma_unmap_sg(drm_dev->dev, sgt->sgl, sgt->nents, dir);
}

void exynos_drm_gem_free_object(struct drm_gem_object *obj)
{
<<<<<<< HEAD
	struct exynos_drm_gem_obj *exynos_gem_obj;
	struct exynos_drm_gem_buf *buf;

	exynos_gem_obj = to_exynos_gem_obj(obj);
	buf = exynos_gem_obj->buffer;

	if (obj->import_attach)
		drm_prime_gem_destroy(obj, buf->sgt);

	exynos_drm_gem_destroy(to_exynos_gem_obj(obj));
=======
	exynos_drm_gem_destroy(to_exynos_gem(obj));
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

int exynos_drm_gem_dumb_create(struct drm_file *file_priv,
			       struct drm_device *dev,
			       struct drm_mode_create_dumb *args)
{
<<<<<<< HEAD
	struct exynos_drm_gem_obj *exynos_gem_obj;
=======
	struct exynos_drm_gem *exynos_gem;
	unsigned int flags;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	int ret;

	/*
	 * allocate memory to be used for framebuffer.
	 * - this callback would be called by user application
	 *	with DRM_IOCTL_MODE_CREATE_DUMB command.
	 */

	args->pitch = args->width * ((args->bpp + 7) / 8);
	args->size = args->pitch * args->height;

<<<<<<< HEAD
	if (is_drm_iommu_supported(dev)) {
		exynos_gem_obj = exynos_drm_gem_create(dev,
			EXYNOS_BO_NONCONTIG | EXYNOS_BO_WC,
			args->size);
	} else {
		exynos_gem_obj = exynos_drm_gem_create(dev,
			EXYNOS_BO_CONTIG | EXYNOS_BO_WC,
			args->size);
	}

	if (IS_ERR(exynos_gem_obj)) {
		dev_warn(dev->dev, "FB allocation failed.\n");
		return PTR_ERR(exynos_gem_obj);
	}

	ret = exynos_drm_gem_handle_create(&exynos_gem_obj->base, file_priv,
			&args->handle);
	if (ret) {
		exynos_drm_gem_destroy(exynos_gem_obj);
=======
	if (is_drm_iommu_supported(dev))
		flags = EXYNOS_BO_NONCONTIG | EXYNOS_BO_WC;
	else
		flags = EXYNOS_BO_CONTIG | EXYNOS_BO_WC;

	exynos_gem = exynos_drm_gem_create(dev, flags, args->size);
	if (IS_ERR(exynos_gem)) {
		dev_warn(dev->dev, "FB allocation failed.\n");
		return PTR_ERR(exynos_gem);
	}

	ret = exynos_drm_gem_handle_create(&exynos_gem->base, file_priv,
					   &args->handle);
	if (ret) {
		exynos_drm_gem_destroy(exynos_gem);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		return ret;
	}

	return 0;
}

int exynos_drm_gem_dumb_map_offset(struct drm_file *file_priv,
				   struct drm_device *dev, uint32_t handle,
				   uint64_t *offset)
{
	struct drm_gem_object *obj;
	int ret = 0;

	mutex_lock(&dev->struct_mutex);

	/*
	 * get offset of memory allocated for drm framebuffer.
	 * - this callback would be called by user application
	 *	with DRM_IOCTL_MODE_MAP_DUMB command.
	 */

	obj = drm_gem_object_lookup(dev, file_priv, handle);
	if (!obj) {
		DRM_ERROR("failed to lookup gem object.\n");
		ret = -EINVAL;
		goto unlock;
	}

<<<<<<< HEAD
	ret = drm_gem_create_mmap_offset(obj);
	if (ret)
		goto out;

	*offset = drm_vma_node_offset_addr(&obj->vma_node);
	DRM_DEBUG_KMS("offset = 0x%lx\n", (unsigned long)*offset);

out:
=======
	*offset = drm_vma_node_offset_addr(&obj->vma_node);
	DRM_DEBUG_KMS("offset = 0x%lx\n", (unsigned long)*offset);

>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	drm_gem_object_unreference(obj);
unlock:
	mutex_unlock(&dev->struct_mutex);
	return ret;
}

int exynos_drm_gem_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
	struct drm_gem_object *obj = vma->vm_private_data;
<<<<<<< HEAD
	struct drm_device *dev = obj->dev;
	unsigned long f_vaddr;
=======
	struct exynos_drm_gem *exynos_gem = to_exynos_gem(obj);
	unsigned long pfn;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	pgoff_t page_offset;
	int ret;

	page_offset = ((unsigned long)vmf->virtual_address -
			vma->vm_start) >> PAGE_SHIFT;
<<<<<<< HEAD
	f_vaddr = (unsigned long)vmf->virtual_address;

	mutex_lock(&dev->struct_mutex);

	ret = exynos_drm_gem_map_buf(obj, vma, f_vaddr, page_offset);
	if (ret < 0)
		DRM_ERROR("failed to map a buffer with user.\n");

	mutex_unlock(&dev->struct_mutex);

	return convert_to_vm_err_msg(ret);
=======

	if (page_offset >= (exynos_gem->size >> PAGE_SHIFT)) {
		DRM_ERROR("invalid page offset\n");
		ret = -EINVAL;
		goto out;
	}

	pfn = page_to_pfn(exynos_gem->pages[page_offset]);
	ret = vm_insert_mixed(vma, (unsigned long)vmf->virtual_address, pfn);

out:
	switch (ret) {
	case 0:
	case -ERESTARTSYS:
	case -EINTR:
		return VM_FAULT_NOPAGE;
	case -ENOMEM:
		return VM_FAULT_OOM;
	default:
		return VM_FAULT_SIGBUS;
	}
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

int exynos_drm_gem_mmap(struct file *filp, struct vm_area_struct *vma)
{
<<<<<<< HEAD
	struct exynos_drm_gem_obj *exynos_gem_obj;
=======
	struct exynos_drm_gem *exynos_gem;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	struct drm_gem_object *obj;
	int ret;

	/* set vm_area_struct. */
	ret = drm_gem_mmap(filp, vma);
	if (ret < 0) {
		DRM_ERROR("failed to mmap.\n");
		return ret;
	}

	obj = vma->vm_private_data;
<<<<<<< HEAD
	exynos_gem_obj = to_exynos_gem_obj(obj);

	ret = check_gem_flags(exynos_gem_obj->flags);
	if (ret)
		goto err_close_vm;

	update_vm_cache_attr(exynos_gem_obj, vma);

	ret = exynos_drm_gem_mmap_buffer(exynos_gem_obj, vma);
=======
	exynos_gem = to_exynos_gem(obj);

	DRM_DEBUG_KMS("flags = 0x%x\n", exynos_gem->flags);

	/* non-cachable as default. */
	if (exynos_gem->flags & EXYNOS_BO_CACHABLE)
		vma->vm_page_prot = vm_get_page_prot(vma->vm_flags);
	else if (exynos_gem->flags & EXYNOS_BO_WC)
		vma->vm_page_prot =
			pgprot_writecombine(vm_get_page_prot(vma->vm_flags));
	else
		vma->vm_page_prot =
			pgprot_noncached(vm_get_page_prot(vma->vm_flags));

	ret = exynos_drm_gem_mmap_buffer(exynos_gem, vma);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	if (ret)
		goto err_close_vm;

	return ret;

err_close_vm:
	drm_gem_vm_close(vma);
<<<<<<< HEAD
	drm_gem_free_mmap_offset(obj);

	return ret;
}
=======

	return ret;
}

/* low-level interface prime helpers */
struct sg_table *exynos_drm_gem_prime_get_sg_table(struct drm_gem_object *obj)
{
	struct exynos_drm_gem *exynos_gem = to_exynos_gem(obj);
	int npages;

	npages = exynos_gem->size >> PAGE_SHIFT;

	return drm_prime_pages_to_sg(exynos_gem->pages, npages);
}

struct drm_gem_object *
exynos_drm_gem_prime_import_sg_table(struct drm_device *dev,
				     struct dma_buf_attachment *attach,
				     struct sg_table *sgt)
{
	struct exynos_drm_gem *exynos_gem;
	int npages;
	int ret;

	exynos_gem = exynos_drm_gem_init(dev, attach->dmabuf->size);
	if (IS_ERR(exynos_gem)) {
		ret = PTR_ERR(exynos_gem);
		return ERR_PTR(ret);
	}

	exynos_gem->dma_addr = sg_dma_address(sgt->sgl);

	npages = exynos_gem->size >> PAGE_SHIFT;
	exynos_gem->pages = drm_malloc_ab(npages, sizeof(struct page *));
	if (!exynos_gem->pages) {
		ret = -ENOMEM;
		goto err;
	}

	ret = drm_prime_sg_to_page_addr_arrays(sgt, exynos_gem->pages, NULL,
					       npages);
	if (ret < 0)
		goto err_free_large;

	exynos_gem->sgt = sgt;

	if (sgt->nents == 1) {
		/* always physically continuous memory if sgt->nents is 1. */
		exynos_gem->flags |= EXYNOS_BO_CONTIG;
	} else {
		/*
		 * this case could be CONTIG or NONCONTIG type but for now
		 * sets NONCONTIG.
		 * TODO. we have to find a way that exporter can notify
		 * the type of its own buffer to importer.
		 */
		exynos_gem->flags |= EXYNOS_BO_NONCONTIG;
	}

	return &exynos_gem->base;

err_free_large:
	drm_free_large(exynos_gem->pages);
err:
	drm_gem_object_release(&exynos_gem->base);
	kfree(exynos_gem);
	return ERR_PTR(ret);
}

void *exynos_drm_gem_prime_vmap(struct drm_gem_object *obj)
{
	return NULL;
}

void exynos_drm_gem_prime_vunmap(struct drm_gem_object *obj, void *vaddr)
{
	/* Nothing to do */
}
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
