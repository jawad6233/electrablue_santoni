/*
 * fs/f2fs/checkpoint.c
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *             http://www.samsung.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/fs.h>
#include <linux/bio.h>
#include <linux/mpage.h>
#include <linux/writeback.h>
#include <linux/blkdev.h>
#include <linux/f2fs_fs.h>
#include <linux/pagevec.h>
#include <linux/swap.h>

#include "f2fs.h"
#include "node.h"
#include "segment.h"
<<<<<<< HEAD
#include <trace/events/f2fs.h>

static struct kmem_cache *ino_entry_slab;
static struct kmem_cache *inode_entry_slab;
=======
#include "trace.h"
#include <trace/events/f2fs.h>

static struct kmem_cache *ino_entry_slab;
struct kmem_cache *inode_entry_slab;

void f2fs_stop_checkpoint(struct f2fs_sb_info *sbi, bool end_io)
{
	set_ckpt_flags(sbi, CP_ERROR_FLAG);
	sbi->sb->s_flags |= MS_RDONLY;
	if (!end_io)
		f2fs_flush_merged_writes(sbi);
}
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

/*
 * We guarantee no failure on the returned page.
 */
struct page *grab_meta_page(struct f2fs_sb_info *sbi, pgoff_t index)
{
	struct address_space *mapping = META_MAPPING(sbi);
	struct page *page = NULL;
repeat:
<<<<<<< HEAD
	page = grab_cache_page(mapping, index);
=======
	page = f2fs_grab_cache_page(mapping, index, false);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	if (!page) {
		cond_resched();
		goto repeat;
	}
<<<<<<< HEAD
	f2fs_wait_on_page_writeback(page, META);
	SetPageUptodate(page);
=======
	f2fs_wait_on_page_writeback(page, META, true);
	if (!PageUptodate(page))
		SetPageUptodate(page);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	return page;
}

/*
 * We guarantee no failure on the returned page.
 */
<<<<<<< HEAD
struct page *get_meta_page(struct f2fs_sb_info *sbi, pgoff_t index)
{
	struct address_space *mapping = META_MAPPING(sbi);
	struct page *page;
repeat:
	page = grab_cache_page(mapping, index);
=======
static struct page *__get_meta_page(struct f2fs_sb_info *sbi, pgoff_t index,
							bool is_meta)
{
	struct address_space *mapping = META_MAPPING(sbi);
	struct page *page;
	struct f2fs_io_info fio = {
		.sbi = sbi,
		.type = META,
		.op = REQ_OP_READ,
		.op_flags = REQ_META | REQ_PRIO,
		.old_blkaddr = index,
		.new_blkaddr = index,
		.encrypted_page = NULL,
	};

	if (unlikely(!is_meta))
		fio.op_flags &= ~REQ_META;
repeat:
	page = f2fs_grab_cache_page(mapping, index, false);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	if (!page) {
		cond_resched();
		goto repeat;
	}
	if (PageUptodate(page))
		goto out;

<<<<<<< HEAD
	if (f2fs_submit_page_bio(sbi, page, index,
				READ_SYNC | REQ_META | REQ_PRIO))
		goto repeat;
=======
	fio.page = page;

	if (f2fs_submit_page_bio(&fio)) {
		f2fs_put_page(page, 1);
		goto repeat;
	}
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	lock_page(page);
	if (unlikely(page->mapping != mapping)) {
		f2fs_put_page(page, 1);
		goto repeat;
	}
<<<<<<< HEAD
out:
	return page;
}

struct page *get_meta_page_ra(struct f2fs_sb_info *sbi, pgoff_t index)
{
	bool readahead = false;
	struct page *page;

	page = find_get_page(META_MAPPING(sbi), index);
	if (!page || (page && !PageUptodate(page)))
		readahead = true;
	f2fs_put_page(page, 0);

	if (readahead)
		ra_meta_pages(sbi, index, MAX_BIO_BLOCKS(sbi), META_POR);
	return get_meta_page(sbi, index);
}

static inline block_t get_max_meta_blks(struct f2fs_sb_info *sbi, int type)
{
	switch (type) {
	case META_NAT:
		return NM_I(sbi)->max_nid / NAT_ENTRY_PER_BLOCK;
	case META_SIT:
		return SIT_BLK_CNT(sbi);
	case META_SSA:
	case META_CP:
		return 0;
	case META_POR:
		return MAX_BLKADDR(sbi);
	default:
		BUG();
	}
=======

	/*
	 * if there is any IO error when accessing device, make our filesystem
	 * readonly and make sure do not write checkpoint with non-uptodate
	 * meta page.
	 */
	if (unlikely(!PageUptodate(page)))
		f2fs_stop_checkpoint(sbi, false);
out:
	mark_page_accessed(page);
	return page;
}

struct page *get_meta_page(struct f2fs_sb_info *sbi, pgoff_t index)
{
	return __get_meta_page(sbi, index, true);
}

/* for POR only */
struct page *get_tmp_page(struct f2fs_sb_info *sbi, pgoff_t index)
{
	return __get_meta_page(sbi, index, false);
}

bool is_valid_blkaddr(struct f2fs_sb_info *sbi, block_t blkaddr, int type)
{
	switch (type) {
	case META_NAT:
		break;
	case META_SIT:
		if (unlikely(blkaddr >= SIT_BLK_CNT(sbi)))
			return false;
		break;
	case META_SSA:
		if (unlikely(blkaddr >= MAIN_BLKADDR(sbi) ||
			blkaddr < SM_I(sbi)->ssa_blkaddr))
			return false;
		break;
	case META_CP:
		if (unlikely(blkaddr >= SIT_I(sbi)->sit_base_addr ||
			blkaddr < __start_cp_addr(sbi)))
			return false;
		break;
	case META_POR:
		if (unlikely(blkaddr >= MAX_BLKADDR(sbi) ||
			blkaddr < MAIN_BLKADDR(sbi)))
			return false;
		break;
	default:
		BUG();
	}

	return true;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

/*
 * Readahead CP/NAT/SIT/SSA pages
 */
<<<<<<< HEAD
int ra_meta_pages(struct f2fs_sb_info *sbi, block_t start, int nrpages, int type)
{
	block_t prev_blk_addr = 0;
	struct page *page;
	block_t blkno = start;
	block_t max_blks = get_max_meta_blks(sbi, type);

	struct f2fs_io_info fio = {
		.type = META,
		.rw = READ_SYNC | REQ_META | REQ_PRIO
	};

	for (; nrpages-- > 0; blkno++) {
		block_t blk_addr;

		switch (type) {
		case META_NAT:
			/* get nat block addr */
			if (unlikely(blkno >= max_blks))
				blkno = 0;
			blk_addr = current_nat_addr(sbi,
=======
int ra_meta_pages(struct f2fs_sb_info *sbi, block_t start, int nrpages,
							int type, bool sync)
{
	struct page *page;
	block_t blkno = start;
	struct f2fs_io_info fio = {
		.sbi = sbi,
		.type = META,
		.op = REQ_OP_READ,
		.op_flags = sync ? (REQ_META | REQ_PRIO) : REQ_RAHEAD,
		.encrypted_page = NULL,
		.in_list = false,
	};
	struct blk_plug plug;

	if (unlikely(type == META_POR))
		fio.op_flags &= ~REQ_META;

	blk_start_plug(&plug);
	for (; nrpages-- > 0; blkno++) {

		if (!is_valid_blkaddr(sbi, blkno, type))
			goto out;

		switch (type) {
		case META_NAT:
			if (unlikely(blkno >=
					NAT_BLOCK_OFFSET(NM_I(sbi)->max_nid)))
				blkno = 0;
			/* get nat block addr */
			fio.new_blkaddr = current_nat_addr(sbi,
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
					blkno * NAT_ENTRY_PER_BLOCK);
			break;
		case META_SIT:
			/* get sit block addr */
<<<<<<< HEAD
			if (unlikely(blkno >= max_blks))
				goto out;
			blk_addr = current_sit_addr(sbi,
					blkno * SIT_ENTRY_PER_BLOCK);
			if (blkno != start && prev_blk_addr + 1 != blk_addr)
				goto out;
			prev_blk_addr = blk_addr;
=======
			fio.new_blkaddr = current_sit_addr(sbi,
					blkno * SIT_ENTRY_PER_BLOCK);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
			break;
		case META_SSA:
		case META_CP:
		case META_POR:
<<<<<<< HEAD
			if (unlikely(blkno >= max_blks))
				goto out;
			if (unlikely(blkno < SEG0_BLKADDR(sbi)))
				goto out;
			blk_addr = blkno;
=======
			fio.new_blkaddr = blkno;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
			break;
		default:
			BUG();
		}

<<<<<<< HEAD
		page = grab_cache_page(META_MAPPING(sbi), blk_addr);
=======
		page = f2fs_grab_cache_page(META_MAPPING(sbi),
						fio.new_blkaddr, false);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		if (!page)
			continue;
		if (PageUptodate(page)) {
			f2fs_put_page(page, 1);
			continue;
		}

<<<<<<< HEAD
		f2fs_submit_page_mbio(sbi, page, blk_addr, &fio);
		f2fs_put_page(page, 0);
	}
out:
	f2fs_submit_merged_bio(sbi, META, READ);
	return blkno - start;
}

=======
		fio.page = page;
		f2fs_submit_page_bio(&fio);
		f2fs_put_page(page, 0);
	}
out:
	blk_finish_plug(&plug);
	return blkno - start;
}

void ra_meta_pages_cond(struct f2fs_sb_info *sbi, pgoff_t index)
{
	struct page *page;
	bool readahead = false;

	page = find_get_page(META_MAPPING(sbi), index);
	if (!page || !PageUptodate(page))
		readahead = true;
	f2fs_put_page(page, 0);

	if (readahead)
		ra_meta_pages(sbi, index, BIO_MAX_PAGES, META_POR, true);
}

>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
static int f2fs_write_meta_page(struct page *page,
				struct writeback_control *wbc)
{
	struct f2fs_sb_info *sbi = F2FS_P_SB(page);

	trace_f2fs_writepage(page, META);

<<<<<<< HEAD
	if (unlikely(sbi->por_doing))
		goto redirty_out;
	if (wbc->for_reclaim)
=======
	if (unlikely(is_sbi_flag_set(sbi, SBI_POR_DOING)))
		goto redirty_out;
	if (wbc->for_reclaim && page->index < GET_SUM_BLOCK(sbi, 0))
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		goto redirty_out;
	if (unlikely(f2fs_cp_error(sbi)))
		goto redirty_out;

<<<<<<< HEAD
	f2fs_wait_on_page_writeback(page, META);
	write_meta_page(sbi, page);
	dec_page_count(sbi, F2FS_DIRTY_META);
	unlock_page(page);
=======
	write_meta_page(sbi, page);
	dec_page_count(sbi, F2FS_DIRTY_META);

	if (wbc->for_reclaim)
		f2fs_submit_merged_write_cond(sbi, page->mapping->host,
						0, page->index, META);

	unlock_page(page);

	if (unlikely(f2fs_cp_error(sbi)))
		f2fs_submit_merged_write(sbi, META);

>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	return 0;

redirty_out:
	redirty_page_for_writepage(wbc, page);
	return AOP_WRITEPAGE_ACTIVATE;
}

static int f2fs_write_meta_pages(struct address_space *mapping,
				struct writeback_control *wbc)
{
	struct f2fs_sb_info *sbi = F2FS_M_SB(mapping);
	long diff, written;

<<<<<<< HEAD
	trace_f2fs_writepages(mapping->host, wbc, META);
=======
	if (unlikely(is_sbi_flag_set(sbi, SBI_POR_DOING)))
		goto skip_write;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	/* collect a number of dirty meta pages and write together */
	if (wbc->for_kupdate ||
		get_pages(sbi, F2FS_DIRTY_META) < nr_pages_to_skip(sbi, META))
		goto skip_write;

<<<<<<< HEAD
	/* if mounting is failed, skip writing node pages */
	mutex_lock(&sbi->cp_mutex);
=======
	/* if locked failed, cp will flush dirty pages instead */
	if (!mutex_trylock(&sbi->cp_mutex))
		goto skip_write;

	trace_f2fs_writepages(mapping->host, wbc, META);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	diff = nr_pages_to_write(sbi, META, wbc);
	written = sync_meta_pages(sbi, META, wbc->nr_to_write);
	mutex_unlock(&sbi->cp_mutex);
	wbc->nr_to_write = max((long)0, wbc->nr_to_write - written - diff);
	return 0;

skip_write:
	wbc->pages_skipped += get_pages(sbi, F2FS_DIRTY_META);
<<<<<<< HEAD
=======
	trace_f2fs_writepages(mapping->host, wbc, META);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	return 0;
}

long sync_meta_pages(struct f2fs_sb_info *sbi, enum page_type type,
						long nr_to_write)
{
	struct address_space *mapping = META_MAPPING(sbi);
<<<<<<< HEAD
	pgoff_t index = 0, end = LONG_MAX;
=======
	pgoff_t index = 0, end = ULONG_MAX, prev = ULONG_MAX;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	struct pagevec pvec;
	long nwritten = 0;
	struct writeback_control wbc = {
		.for_reclaim = 0,
	};
<<<<<<< HEAD

	pagevec_init(&pvec, 0);

=======
	struct blk_plug plug;

	pagevec_init(&pvec, 0);

	blk_start_plug(&plug);

>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	while (index <= end) {
		int i, nr_pages;
		nr_pages = pagevec_lookup_tag(&pvec, mapping, &index,
				PAGECACHE_TAG_DIRTY,
				min(end - index, (pgoff_t)PAGEVEC_SIZE-1) + 1);
		if (unlikely(nr_pages == 0))
			break;

		for (i = 0; i < nr_pages; i++) {
			struct page *page = pvec.pages[i];

<<<<<<< HEAD
=======
			if (prev == ULONG_MAX)
				prev = page->index - 1;
			if (nr_to_write != LONG_MAX && page->index != prev + 1) {
				pagevec_release(&pvec);
				goto stop;
			}

>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
			lock_page(page);

			if (unlikely(page->mapping != mapping)) {
continue_unlock:
				unlock_page(page);
				continue;
			}
			if (!PageDirty(page)) {
				/* someone wrote it for us */
				goto continue_unlock;
			}

<<<<<<< HEAD
			if (!clear_page_dirty_for_io(page))
				goto continue_unlock;

			if (f2fs_write_meta_page(page, &wbc)) {
=======
			f2fs_wait_on_page_writeback(page, META, true);

			BUG_ON(PageWriteback(page));
			if (!clear_page_dirty_for_io(page))
				goto continue_unlock;

			if (mapping->a_ops->writepage(page, &wbc)) {
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
				unlock_page(page);
				break;
			}
			nwritten++;
<<<<<<< HEAD
=======
			prev = page->index;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
			if (unlikely(nwritten >= nr_to_write))
				break;
		}
		pagevec_release(&pvec);
		cond_resched();
	}
<<<<<<< HEAD

	if (nwritten)
		f2fs_submit_merged_bio(sbi, type, WRITE);
=======
stop:
	if (nwritten)
		f2fs_submit_merged_write(sbi, type);

	blk_finish_plug(&plug);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	return nwritten;
}

static int f2fs_set_meta_page_dirty(struct page *page)
{
	trace_f2fs_set_page_dirty(page, META);

<<<<<<< HEAD
	SetPageUptodate(page);
	if (!PageDirty(page)) {
		__set_page_dirty_nobuffers(page);
		inc_page_count(F2FS_P_SB(page), F2FS_DIRTY_META);
=======
	if (!PageUptodate(page))
		SetPageUptodate(page);
	if (!PageDirty(page)) {
		f2fs_set_page_dirty_nobuffers(page);
		inc_page_count(F2FS_P_SB(page), F2FS_DIRTY_META);
		SetPagePrivate(page);
		f2fs_trace_pid(page);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		return 1;
	}
	return 0;
}

const struct address_space_operations f2fs_meta_aops = {
	.writepage	= f2fs_write_meta_page,
	.writepages	= f2fs_write_meta_pages,
	.set_page_dirty	= f2fs_set_meta_page_dirty,
<<<<<<< HEAD
=======
	.invalidatepage = f2fs_invalidate_page,
	.releasepage	= f2fs_release_page,
#ifdef CONFIG_MIGRATION
	.migratepage    = f2fs_migrate_page,
#endif
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
};

static void __add_ino_entry(struct f2fs_sb_info *sbi, nid_t ino, int type)
{
<<<<<<< HEAD
	struct ino_entry *e;
retry:
	spin_lock(&sbi->ino_lock[type]);

	e = radix_tree_lookup(&sbi->ino_root[type], ino);
	if (!e) {
		e = kmem_cache_alloc(ino_entry_slab, GFP_ATOMIC);
		if (!e) {
			spin_unlock(&sbi->ino_lock[type]);
			goto retry;
		}
		if (radix_tree_insert(&sbi->ino_root[type], ino, e)) {
			spin_unlock(&sbi->ino_lock[type]);
			kmem_cache_free(ino_entry_slab, e);
=======
	struct inode_management *im = &sbi->im[type];
	struct ino_entry *e, *tmp;

	tmp = f2fs_kmem_cache_alloc(ino_entry_slab, GFP_NOFS);
retry:
	radix_tree_preload(GFP_NOFS | __GFP_NOFAIL);

	spin_lock(&im->ino_lock);
	e = radix_tree_lookup(&im->ino_root, ino);
	if (!e) {
		e = tmp;
		if (radix_tree_insert(&im->ino_root, ino, e)) {
			spin_unlock(&im->ino_lock);
			radix_tree_preload_end();
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
			goto retry;
		}
		memset(e, 0, sizeof(struct ino_entry));
		e->ino = ino;

<<<<<<< HEAD
		list_add_tail(&e->list, &sbi->ino_list[type]);
	}
	spin_unlock(&sbi->ino_lock[type]);
=======
		list_add_tail(&e->list, &im->ino_list);
		if (type != ORPHAN_INO)
			im->ino_num++;
	}
	spin_unlock(&im->ino_lock);
	radix_tree_preload_end();

	if (e != tmp)
		kmem_cache_free(ino_entry_slab, tmp);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

static void __remove_ino_entry(struct f2fs_sb_info *sbi, nid_t ino, int type)
{
<<<<<<< HEAD
	struct ino_entry *e;

	spin_lock(&sbi->ino_lock[type]);
	e = radix_tree_lookup(&sbi->ino_root[type], ino);
	if (e) {
		list_del(&e->list);
		radix_tree_delete(&sbi->ino_root[type], ino);
		if (type == ORPHAN_INO)
			sbi->n_orphans--;
		spin_unlock(&sbi->ino_lock[type]);
		kmem_cache_free(ino_entry_slab, e);
		return;
	}
	spin_unlock(&sbi->ino_lock[type]);
}

void add_dirty_inode(struct f2fs_sb_info *sbi, nid_t ino, int type)
=======
	struct inode_management *im = &sbi->im[type];
	struct ino_entry *e;

	spin_lock(&im->ino_lock);
	e = radix_tree_lookup(&im->ino_root, ino);
	if (e) {
		list_del(&e->list);
		radix_tree_delete(&im->ino_root, ino);
		im->ino_num--;
		spin_unlock(&im->ino_lock);
		kmem_cache_free(ino_entry_slab, e);
		return;
	}
	spin_unlock(&im->ino_lock);
}

void add_ino_entry(struct f2fs_sb_info *sbi, nid_t ino, int type)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
{
	/* add new dirty ino entry into list */
	__add_ino_entry(sbi, ino, type);
}

<<<<<<< HEAD
void remove_dirty_inode(struct f2fs_sb_info *sbi, nid_t ino, int type)
=======
void remove_ino_entry(struct f2fs_sb_info *sbi, nid_t ino, int type)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
{
	/* remove dirty ino entry from list */
	__remove_ino_entry(sbi, ino, type);
}

/* mode should be APPEND_INO or UPDATE_INO */
bool exist_written_data(struct f2fs_sb_info *sbi, nid_t ino, int mode)
{
<<<<<<< HEAD
	struct ino_entry *e;
	spin_lock(&sbi->ino_lock[mode]);
	e = radix_tree_lookup(&sbi->ino_root[mode], ino);
	spin_unlock(&sbi->ino_lock[mode]);
	return e ? true : false;
}

void release_dirty_inode(struct f2fs_sb_info *sbi)
=======
	struct inode_management *im = &sbi->im[mode];
	struct ino_entry *e;

	spin_lock(&im->ino_lock);
	e = radix_tree_lookup(&im->ino_root, ino);
	spin_unlock(&im->ino_lock);
	return e ? true : false;
}

void release_ino_entry(struct f2fs_sb_info *sbi, bool all)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
{
	struct ino_entry *e, *tmp;
	int i;

<<<<<<< HEAD
	for (i = APPEND_INO; i <= UPDATE_INO; i++) {
		spin_lock(&sbi->ino_lock[i]);
		list_for_each_entry_safe(e, tmp, &sbi->ino_list[i], list) {
			list_del(&e->list);
			radix_tree_delete(&sbi->ino_root[i], e->ino);
			kmem_cache_free(ino_entry_slab, e);
		}
		spin_unlock(&sbi->ino_lock[i]);
=======
	for (i = all ? ORPHAN_INO: APPEND_INO; i <= UPDATE_INO; i++) {
		struct inode_management *im = &sbi->im[i];

		spin_lock(&im->ino_lock);
		list_for_each_entry_safe(e, tmp, &im->ino_list, list) {
			list_del(&e->list);
			radix_tree_delete(&im->ino_root, e->ino);
			kmem_cache_free(ino_entry_slab, e);
			im->ino_num--;
		}
		spin_unlock(&im->ino_lock);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	}
}

int acquire_orphan_inode(struct f2fs_sb_info *sbi)
{
<<<<<<< HEAD
	int err = 0;

	spin_lock(&sbi->ino_lock[ORPHAN_INO]);
	if (unlikely(sbi->n_orphans >= sbi->max_orphans))
		err = -ENOSPC;
	else
		sbi->n_orphans++;
	spin_unlock(&sbi->ino_lock[ORPHAN_INO]);
=======
	struct inode_management *im = &sbi->im[ORPHAN_INO];
	int err = 0;

	spin_lock(&im->ino_lock);

#ifdef CONFIG_F2FS_FAULT_INJECTION
	if (time_to_inject(sbi, FAULT_ORPHAN)) {
		spin_unlock(&im->ino_lock);
		f2fs_show_injection_info(FAULT_ORPHAN);
		return -ENOSPC;
	}
#endif
	if (unlikely(im->ino_num >= sbi->max_orphans))
		err = -ENOSPC;
	else
		im->ino_num++;
	spin_unlock(&im->ino_lock);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	return err;
}

void release_orphan_inode(struct f2fs_sb_info *sbi)
{
<<<<<<< HEAD
	spin_lock(&sbi->ino_lock[ORPHAN_INO]);
	f2fs_bug_on(sbi, sbi->n_orphans == 0);
	sbi->n_orphans--;
	spin_unlock(&sbi->ino_lock[ORPHAN_INO]);
}

void add_orphan_inode(struct f2fs_sb_info *sbi, nid_t ino)
{
	/* add new orphan ino entry into list */
	__add_ino_entry(sbi, ino, ORPHAN_INO);
=======
	struct inode_management *im = &sbi->im[ORPHAN_INO];

	spin_lock(&im->ino_lock);
	f2fs_bug_on(sbi, im->ino_num == 0);
	im->ino_num--;
	spin_unlock(&im->ino_lock);
}

void add_orphan_inode(struct inode *inode)
{
	/* add new orphan ino entry into list */
	__add_ino_entry(F2FS_I_SB(inode), inode->i_ino, ORPHAN_INO);
	update_inode_page(inode);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

void remove_orphan_inode(struct f2fs_sb_info *sbi, nid_t ino)
{
	/* remove orphan entry from orphan list */
	__remove_ino_entry(sbi, ino, ORPHAN_INO);
}

<<<<<<< HEAD
static void recover_orphan_inode(struct f2fs_sb_info *sbi, nid_t ino)
{
	struct inode *inode = f2fs_iget(sbi->sb, ino);
	f2fs_bug_on(sbi, IS_ERR(inode));
=======
static int recover_orphan_inode(struct f2fs_sb_info *sbi, nid_t ino)
{
	struct inode *inode;
	struct node_info ni;
	int err = acquire_orphan_inode(sbi);

	if (err) {
		set_sbi_flag(sbi, SBI_NEED_FSCK);
		f2fs_msg(sbi->sb, KERN_WARNING,
				"%s: orphan failed (ino=%x), run fsck to fix.",
				__func__, ino);
		return err;
	}

	__add_ino_entry(sbi, ino, ORPHAN_INO);

	inode = f2fs_iget_retry(sbi->sb, ino);
	if (IS_ERR(inode)) {
		/*
		 * there should be a bug that we can't find the entry
		 * to orphan inode.
		 */
		f2fs_bug_on(sbi, PTR_ERR(inode) == -ENOENT);
		return PTR_ERR(inode);
	}

>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	clear_nlink(inode);

	/* truncate all the data during iput */
	iput(inode);
<<<<<<< HEAD
}

void recover_orphan_inodes(struct f2fs_sb_info *sbi)
{
	block_t start_blk, orphan_blkaddr, i, j;

	if (!is_set_ckpt_flags(F2FS_CKPT(sbi), CP_ORPHAN_PRESENT_FLAG))
		return;

	sbi->por_doing = true;

	start_blk = __start_cp_addr(sbi) + 1 +
		le32_to_cpu(F2FS_RAW_SUPER(sbi)->cp_payload);
	orphan_blkaddr = __start_sum_addr(sbi) - 1;

	ra_meta_pages(sbi, start_blk, orphan_blkaddr, META_CP);

	for (i = 0; i < orphan_blkaddr; i++) {
=======

	get_node_info(sbi, ino, &ni);

	/* ENOMEM was fully retried in f2fs_evict_inode. */
	if (ni.blk_addr != NULL_ADDR) {
		set_sbi_flag(sbi, SBI_NEED_FSCK);
		f2fs_msg(sbi->sb, KERN_WARNING,
			"%s: orphan failed (ino=%x) by kernel, retry mount.",
				__func__, ino);
		return -EIO;
	}
	__remove_ino_entry(sbi, ino, ORPHAN_INO);
	return 0;
}

int recover_orphan_inodes(struct f2fs_sb_info *sbi)
{
	block_t start_blk, orphan_blocks, i, j;
	int err;

	if (!is_set_ckpt_flags(sbi, CP_ORPHAN_PRESENT_FLAG))
		return 0;

	start_blk = __start_cp_addr(sbi) + 1 + __cp_payload(sbi);
	orphan_blocks = __start_sum_addr(sbi) - 1 - __cp_payload(sbi);

	ra_meta_pages(sbi, start_blk, orphan_blocks, META_CP, true);

	for (i = 0; i < orphan_blocks; i++) {
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		struct page *page = get_meta_page(sbi, start_blk + i);
		struct f2fs_orphan_block *orphan_blk;

		orphan_blk = (struct f2fs_orphan_block *)page_address(page);
		for (j = 0; j < le32_to_cpu(orphan_blk->entry_count); j++) {
			nid_t ino = le32_to_cpu(orphan_blk->ino[j]);
<<<<<<< HEAD
			recover_orphan_inode(sbi, ino);
=======
			err = recover_orphan_inode(sbi, ino);
			if (err) {
				f2fs_put_page(page, 1);
				return err;
			}
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		}
		f2fs_put_page(page, 1);
	}
	/* clear Orphan Flag */
<<<<<<< HEAD
	clear_ckpt_flags(F2FS_CKPT(sbi), CP_ORPHAN_PRESENT_FLAG);
	sbi->por_doing = false;
	return;
=======
	clear_ckpt_flags(sbi, CP_ORPHAN_PRESENT_FLAG);
	return 0;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

static void write_orphan_inodes(struct f2fs_sb_info *sbi, block_t start_blk)
{
	struct list_head *head;
	struct f2fs_orphan_block *orphan_blk = NULL;
	unsigned int nentries = 0;
<<<<<<< HEAD
	unsigned short index;
	unsigned short orphan_blocks =
			(unsigned short)GET_ORPHAN_BLOCKS(sbi->n_orphans);
	struct page *page = NULL;
	struct ino_entry *orphan = NULL;

	for (index = 0; index < orphan_blocks; index++)
		grab_meta_page(sbi, start_blk + index);

	index = 1;
	spin_lock(&sbi->ino_lock[ORPHAN_INO]);
	head = &sbi->ino_list[ORPHAN_INO];
=======
	unsigned short index = 1;
	unsigned short orphan_blocks;
	struct page *page = NULL;
	struct ino_entry *orphan = NULL;
	struct inode_management *im = &sbi->im[ORPHAN_INO];

	orphan_blocks = GET_ORPHAN_BLOCKS(im->ino_num);

	/*
	 * we don't need to do spin_lock(&im->ino_lock) here, since all the
	 * orphan inode operations are covered under f2fs_lock_op().
	 * And, spin_lock should be avoided due to page operations below.
	 */
	head = &im->ino_list;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	/* loop for each orphan inode entry and write them in Jornal block */
	list_for_each_entry(orphan, head, list) {
		if (!page) {
<<<<<<< HEAD
			page = find_get_page(META_MAPPING(sbi), start_blk++);
			f2fs_bug_on(sbi, !page);
			orphan_blk =
				(struct f2fs_orphan_block *)page_address(page);
			memset(orphan_blk, 0, sizeof(*orphan_blk));
			f2fs_put_page(page, 0);
=======
			page = grab_meta_page(sbi, start_blk++);
			orphan_blk =
				(struct f2fs_orphan_block *)page_address(page);
			memset(orphan_blk, 0, sizeof(*orphan_blk));
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		}

		orphan_blk->ino[nentries++] = cpu_to_le32(orphan->ino);

		if (nentries == F2FS_ORPHANS_PER_BLOCK) {
			/*
			 * an orphan block is full of 1020 entries,
			 * then we need to flush current orphan blocks
			 * and bring another one in memory
			 */
			orphan_blk->blk_addr = cpu_to_le16(index);
			orphan_blk->blk_count = cpu_to_le16(orphan_blocks);
			orphan_blk->entry_count = cpu_to_le32(nentries);
			set_page_dirty(page);
			f2fs_put_page(page, 1);
			index++;
			nentries = 0;
			page = NULL;
		}
	}

	if (page) {
		orphan_blk->blk_addr = cpu_to_le16(index);
		orphan_blk->blk_count = cpu_to_le16(orphan_blocks);
		orphan_blk->entry_count = cpu_to_le32(nentries);
		set_page_dirty(page);
		f2fs_put_page(page, 1);
	}
<<<<<<< HEAD

	spin_unlock(&sbi->ino_lock[ORPHAN_INO]);
}

static struct page *validate_checkpoint(struct f2fs_sb_info *sbi,
				block_t cp_addr, unsigned long long *version)
{
	struct page *cp_page_1, *cp_page_2 = NULL;
	unsigned long blk_size = sbi->blocksize;
	struct f2fs_checkpoint *cp_block;
	unsigned long long cur_version = 0, pre_version = 0;
	size_t crc_offset;
	__u32 crc = 0;

	/* Read the 1st cp block in this CP pack */
	cp_page_1 = get_meta_page(sbi, cp_addr);

	/* get the version number */
	cp_block = (struct f2fs_checkpoint *)page_address(cp_page_1);
	crc_offset = le32_to_cpu(cp_block->checksum_offset);
	if (crc_offset >= blk_size)
		goto invalid_cp1;

	crc = le32_to_cpu(*((__u32 *)((unsigned char *)cp_block + crc_offset)));
	if (!f2fs_crc_valid(crc, cp_block, crc_offset))
		goto invalid_cp1;

	pre_version = cur_cp_version(cp_block);

	/* Read the 2nd cp block in this CP pack */
	cp_addr += le32_to_cpu(cp_block->cp_pack_total_block_count) - 1;
	cp_page_2 = get_meta_page(sbi, cp_addr);

	cp_block = (struct f2fs_checkpoint *)page_address(cp_page_2);
	crc_offset = le32_to_cpu(cp_block->checksum_offset);
	if (crc_offset >= blk_size)
		goto invalid_cp2;

	crc = le32_to_cpu(*((__u32 *)((unsigned char *)cp_block + crc_offset)));
	if (!f2fs_crc_valid(crc, cp_block, crc_offset))
		goto invalid_cp2;

	cur_version = cur_cp_version(cp_block);
=======
}

static int get_checkpoint_version(struct f2fs_sb_info *sbi, block_t cp_addr,
		struct f2fs_checkpoint **cp_block, struct page **cp_page,
		unsigned long long *version)
{
	unsigned long blk_size = sbi->blocksize;
	size_t crc_offset = 0;
	__u32 crc = 0;

	*cp_page = get_meta_page(sbi, cp_addr);
	*cp_block = (struct f2fs_checkpoint *)page_address(*cp_page);

	crc_offset = le32_to_cpu((*cp_block)->checksum_offset);
	if (crc_offset > (blk_size - sizeof(__le32))) {
		f2fs_msg(sbi->sb, KERN_WARNING,
			"invalid crc_offset: %zu", crc_offset);
		return -EINVAL;
	}

	crc = cur_cp_crc(*cp_block);
	if (!f2fs_crc_valid(sbi, crc, *cp_block, crc_offset)) {
		f2fs_msg(sbi->sb, KERN_WARNING, "invalid crc value");
		return -EINVAL;
	}

	*version = cur_cp_version(*cp_block);
	return 0;
}

static struct page *validate_checkpoint(struct f2fs_sb_info *sbi,
				block_t cp_addr, unsigned long long *version)
{
	struct page *cp_page_1 = NULL, *cp_page_2 = NULL;
	struct f2fs_checkpoint *cp_block = NULL;
	unsigned long long cur_version = 0, pre_version = 0;
	int err;

	err = get_checkpoint_version(sbi, cp_addr, &cp_block,
					&cp_page_1, version);
	if (err)
		goto invalid_cp1;
	pre_version = *version;

	cp_addr += le32_to_cpu(cp_block->cp_pack_total_block_count) - 1;
	err = get_checkpoint_version(sbi, cp_addr, &cp_block,
					&cp_page_2, version);
	if (err)
		goto invalid_cp2;
	cur_version = *version;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	if (cur_version == pre_version) {
		*version = cur_version;
		f2fs_put_page(cp_page_2, 1);
		return cp_page_1;
	}
invalid_cp2:
	f2fs_put_page(cp_page_2, 1);
invalid_cp1:
	f2fs_put_page(cp_page_1, 1);
	return NULL;
}

int get_valid_checkpoint(struct f2fs_sb_info *sbi)
{
	struct f2fs_checkpoint *cp_block;
	struct f2fs_super_block *fsb = sbi->raw_super;
	struct page *cp1, *cp2, *cur_page;
	unsigned long blk_size = sbi->blocksize;
	unsigned long long cp1_version = 0, cp2_version = 0;
	unsigned long long cp_start_blk_no;
<<<<<<< HEAD
	unsigned int cp_blks = 1 + le32_to_cpu(F2FS_RAW_SUPER(sbi)->cp_payload);
=======
	unsigned int cp_blks = 1 + __cp_payload(sbi);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	block_t cp_blk_no;
	int i;

	sbi->ckpt = kzalloc(cp_blks * blk_size, GFP_KERNEL);
	if (!sbi->ckpt)
		return -ENOMEM;
	/*
	 * Finding out valid cp block involves read both
	 * sets( cp pack1 and cp pack 2)
	 */
	cp_start_blk_no = le32_to_cpu(fsb->cp_blkaddr);
	cp1 = validate_checkpoint(sbi, cp_start_blk_no, &cp1_version);

	/* The second checkpoint pack should start at the next segment */
	cp_start_blk_no += ((unsigned long long)1) <<
				le32_to_cpu(fsb->log_blocks_per_seg);
	cp2 = validate_checkpoint(sbi, cp_start_blk_no, &cp2_version);

	if (cp1 && cp2) {
		if (ver_after(cp2_version, cp1_version))
			cur_page = cp2;
		else
			cur_page = cp1;
	} else if (cp1) {
		cur_page = cp1;
	} else if (cp2) {
		cur_page = cp2;
	} else {
		goto fail_no_cp;
	}

	cp_block = (struct f2fs_checkpoint *)page_address(cur_page);
	memcpy(sbi->ckpt, cp_block, blk_size);

<<<<<<< HEAD
=======
	/* Sanity checking of checkpoint */
	if (sanity_check_ckpt(sbi))
		goto free_fail_no_cp;

	if (cur_page == cp1)
		sbi->cur_cp_pack = 1;
	else
		sbi->cur_cp_pack = 2;

>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	if (cp_blks <= 1)
		goto done;

	cp_blk_no = le32_to_cpu(fsb->cp_blkaddr);
	if (cur_page == cp2)
		cp_blk_no += 1 << le32_to_cpu(fsb->log_blocks_per_seg);

	for (i = 1; i < cp_blks; i++) {
		void *sit_bitmap_ptr;
		unsigned char *ckpt = (unsigned char *)sbi->ckpt;

		cur_page = get_meta_page(sbi, cp_blk_no + i);
		sit_bitmap_ptr = page_address(cur_page);
		memcpy(ckpt + i * blk_size, sit_bitmap_ptr, blk_size);
		f2fs_put_page(cur_page, 1);
	}
done:
	f2fs_put_page(cp1, 1);
	f2fs_put_page(cp2, 1);
	return 0;

<<<<<<< HEAD
=======
free_fail_no_cp:
	f2fs_put_page(cp1, 1);
	f2fs_put_page(cp2, 1);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
fail_no_cp:
	kfree(sbi->ckpt);
	return -EINVAL;
}

<<<<<<< HEAD
static int __add_dirty_inode(struct inode *inode, struct dir_inode_entry *new)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);

	if (is_inode_flag_set(F2FS_I(inode), FI_DIRTY_DIR))
		return -EEXIST;

	set_inode_flag(F2FS_I(inode), FI_DIRTY_DIR);
	F2FS_I(inode)->dirty_dir = new;
	list_add_tail(&new->list, &sbi->dir_inode_list);
	stat_inc_dirty_dir(sbi);
	return 0;
}

void update_dirty_page(struct inode *inode, struct page *page)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	struct dir_inode_entry *new;
	int ret = 0;

	if (!S_ISDIR(inode->i_mode) && !S_ISREG(inode->i_mode))
		return;

	if (!S_ISDIR(inode->i_mode)) {
		inode_inc_dirty_pages(inode);
		goto out;
	}

	new = f2fs_kmem_cache_alloc(inode_entry_slab, GFP_NOFS);
	new->inode = inode;
	INIT_LIST_HEAD(&new->list);

	spin_lock(&sbi->dir_inode_lock);
	ret = __add_dirty_inode(inode, new);
	inode_inc_dirty_pages(inode);
	spin_unlock(&sbi->dir_inode_lock);

	if (ret)
		kmem_cache_free(inode_entry_slab, new);
out:
	SetPagePrivate(page);
}

void add_dirty_dir_inode(struct inode *inode)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	struct dir_inode_entry *new =
			f2fs_kmem_cache_alloc(inode_entry_slab, GFP_NOFS);
	int ret = 0;

	new->inode = inode;
	INIT_LIST_HEAD(&new->list);

	spin_lock(&sbi->dir_inode_lock);
	ret = __add_dirty_inode(inode, new);
	spin_unlock(&sbi->dir_inode_lock);

	if (ret)
		kmem_cache_free(inode_entry_slab, new);
}

void remove_dirty_dir_inode(struct inode *inode)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	struct dir_inode_entry *entry;

	if (!S_ISDIR(inode->i_mode))
		return;

	spin_lock(&sbi->dir_inode_lock);
	if (get_dirty_pages(inode) ||
			!is_inode_flag_set(F2FS_I(inode), FI_DIRTY_DIR)) {
		spin_unlock(&sbi->dir_inode_lock);
		return;
	}

	entry = F2FS_I(inode)->dirty_dir;
	list_del(&entry->list);
	F2FS_I(inode)->dirty_dir = NULL;
	clear_inode_flag(F2FS_I(inode), FI_DIRTY_DIR);
	stat_dec_dirty_dir(sbi);
	spin_unlock(&sbi->dir_inode_lock);
	kmem_cache_free(inode_entry_slab, entry);

	/* Only from the recovery routine */
	if (is_inode_flag_set(F2FS_I(inode), FI_DELAY_IPUT)) {
		clear_inode_flag(F2FS_I(inode), FI_DELAY_IPUT);
		iput(inode);
	}
}

void sync_dirty_dir_inodes(struct f2fs_sb_info *sbi)
{
	struct list_head *head;
	struct dir_inode_entry *entry;
	struct inode *inode;
retry:
	spin_lock(&sbi->dir_inode_lock);

	head = &sbi->dir_inode_list;
	if (list_empty(head)) {
		spin_unlock(&sbi->dir_inode_lock);
		return;
	}
	entry = list_entry(head->next, struct dir_inode_entry, list);
	inode = igrab(entry->inode);
	spin_unlock(&sbi->dir_inode_lock);
	if (inode) {
		filemap_fdatawrite(inode->i_mapping);
		iput(inode);
=======
static void __add_dirty_inode(struct inode *inode, enum inode_type type)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	int flag = (type == DIR_INODE) ? FI_DIRTY_DIR : FI_DIRTY_FILE;

	if (is_inode_flag_set(inode, flag))
		return;

	set_inode_flag(inode, flag);
	if (!f2fs_is_volatile_file(inode))
		list_add_tail(&F2FS_I(inode)->dirty_list,
						&sbi->inode_list[type]);
	stat_inc_dirty_inode(sbi, type);
}

static void __remove_dirty_inode(struct inode *inode, enum inode_type type)
{
	int flag = (type == DIR_INODE) ? FI_DIRTY_DIR : FI_DIRTY_FILE;

	if (get_dirty_pages(inode) || !is_inode_flag_set(inode, flag))
		return;

	list_del_init(&F2FS_I(inode)->dirty_list);
	clear_inode_flag(inode, flag);
	stat_dec_dirty_inode(F2FS_I_SB(inode), type);
}

void update_dirty_page(struct inode *inode, struct page *page)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	enum inode_type type = S_ISDIR(inode->i_mode) ? DIR_INODE : FILE_INODE;

	if (!S_ISDIR(inode->i_mode) && !S_ISREG(inode->i_mode) &&
			!S_ISLNK(inode->i_mode))
		return;

	spin_lock(&sbi->inode_lock[type]);
	if (type != FILE_INODE || test_opt(sbi, DATA_FLUSH))
		__add_dirty_inode(inode, type);
	inode_inc_dirty_pages(inode);
	spin_unlock(&sbi->inode_lock[type]);

	SetPagePrivate(page);
	f2fs_trace_pid(page);
}

void remove_dirty_inode(struct inode *inode)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	enum inode_type type = S_ISDIR(inode->i_mode) ? DIR_INODE : FILE_INODE;

	if (!S_ISDIR(inode->i_mode) && !S_ISREG(inode->i_mode) &&
			!S_ISLNK(inode->i_mode))
		return;

	if (type == FILE_INODE && !test_opt(sbi, DATA_FLUSH))
		return;

	spin_lock(&sbi->inode_lock[type]);
	__remove_dirty_inode(inode, type);
	spin_unlock(&sbi->inode_lock[type]);
}

int sync_dirty_inodes(struct f2fs_sb_info *sbi, enum inode_type type)
{
	struct list_head *head;
	struct inode *inode;
	struct f2fs_inode_info *fi;
	bool is_dir = (type == DIR_INODE);
	unsigned long ino = 0;

	trace_f2fs_sync_dirty_inodes_enter(sbi->sb, is_dir,
				get_pages(sbi, is_dir ?
				F2FS_DIRTY_DENTS : F2FS_DIRTY_DATA));
retry:
	if (unlikely(f2fs_cp_error(sbi)))
		return -EIO;

	spin_lock(&sbi->inode_lock[type]);

	head = &sbi->inode_list[type];
	if (list_empty(head)) {
		spin_unlock(&sbi->inode_lock[type]);
		trace_f2fs_sync_dirty_inodes_exit(sbi->sb, is_dir,
				get_pages(sbi, is_dir ?
				F2FS_DIRTY_DENTS : F2FS_DIRTY_DATA));
		return 0;
	}
	fi = list_first_entry(head, struct f2fs_inode_info, dirty_list);
	inode = igrab(&fi->vfs_inode);
	spin_unlock(&sbi->inode_lock[type]);
	if (inode) {
		unsigned long cur_ino = inode->i_ino;

		filemap_fdatawrite(inode->i_mapping);
		iput(inode);
		/* We need to give cpu to another writers. */
		if (ino == cur_ino) {
			congestion_wait(BLK_RW_ASYNC, HZ/50);
			cond_resched();
		} else {
			ino = cur_ino;
		}
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	} else {
		/*
		 * We should submit bio, since it exists several
		 * wribacking dentry pages in the freeing inode.
		 */
<<<<<<< HEAD
		f2fs_submit_merged_bio(sbi, DATA, WRITE);
=======
		f2fs_submit_merged_write(sbi, DATA);
		cond_resched();
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	}
	goto retry;
}

<<<<<<< HEAD
=======
int f2fs_sync_inode_meta(struct f2fs_sb_info *sbi)
{
	struct list_head *head = &sbi->inode_list[DIRTY_META];
	struct inode *inode;
	struct f2fs_inode_info *fi;
	s64 total = get_pages(sbi, F2FS_DIRTY_IMETA);

	while (total--) {
		if (unlikely(f2fs_cp_error(sbi)))
			return -EIO;

		spin_lock(&sbi->inode_lock[DIRTY_META]);
		if (list_empty(head)) {
			spin_unlock(&sbi->inode_lock[DIRTY_META]);
			return 0;
		}
		fi = list_first_entry(head, struct f2fs_inode_info,
							gdirty_list);
		inode = igrab(&fi->vfs_inode);
		spin_unlock(&sbi->inode_lock[DIRTY_META]);
		if (inode) {
			sync_inode_metadata(inode, 0);

			/* it's on eviction */
			if (is_inode_flag_set(inode, FI_DIRTY_INODE))
				update_inode_page(inode);
			iput(inode);
		}
	};
	return 0;
}

static void __prepare_cp_block(struct f2fs_sb_info *sbi)
{
	struct f2fs_checkpoint *ckpt = F2FS_CKPT(sbi);
	struct f2fs_nm_info *nm_i = NM_I(sbi);
	nid_t last_nid = nm_i->next_scan_nid;

	next_free_nid(sbi, &last_nid);
	ckpt->valid_block_count = cpu_to_le64(valid_user_blocks(sbi));
	ckpt->valid_node_count = cpu_to_le32(valid_node_count(sbi));
	ckpt->valid_inode_count = cpu_to_le32(valid_inode_count(sbi));
	ckpt->next_free_nid = cpu_to_le32(last_nid);
}

>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
/*
 * Freeze all the FS-operations for checkpoint.
 */
static int block_operations(struct f2fs_sb_info *sbi)
{
	struct writeback_control wbc = {
		.sync_mode = WB_SYNC_ALL,
		.nr_to_write = LONG_MAX,
		.for_reclaim = 0,
	};
	struct blk_plug plug;
	int err = 0;

	blk_start_plug(&plug);

retry_flush_dents:
	f2fs_lock_all(sbi);
	/* write all the dirty dentry pages */
	if (get_pages(sbi, F2FS_DIRTY_DENTS)) {
		f2fs_unlock_all(sbi);
<<<<<<< HEAD
		sync_dirty_dir_inodes(sbi);
		if (unlikely(f2fs_cp_error(sbi))) {
			err = -EIO;
			goto out;
		}
=======
		err = sync_dirty_inodes(sbi, DIR_INODE);
		if (err)
			goto out;
		cond_resched();
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		goto retry_flush_dents;
	}

	/*
	 * POR: we should ensure that there are no dirty node pages
<<<<<<< HEAD
	 * until finishing nat/sit flush.
	 */
=======
	 * until finishing nat/sit flush. inode->i_blocks can be updated.
	 */
	down_write(&sbi->node_change);

	if (get_pages(sbi, F2FS_DIRTY_IMETA)) {
		up_write(&sbi->node_change);
		f2fs_unlock_all(sbi);
		err = f2fs_sync_inode_meta(sbi);
		if (err)
			goto out;
		cond_resched();
		goto retry_flush_dents;
	}

>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
retry_flush_nodes:
	down_write(&sbi->node_write);

	if (get_pages(sbi, F2FS_DIRTY_NODES)) {
		up_write(&sbi->node_write);
<<<<<<< HEAD
		sync_node_pages(sbi, 0, &wbc);
		if (unlikely(f2fs_cp_error(sbi))) {
			f2fs_unlock_all(sbi);
			err = -EIO;
			goto out;
		}
		goto retry_flush_nodes;
	}
=======
		err = sync_node_pages(sbi, &wbc);
		if (err) {
			up_write(&sbi->node_change);
			f2fs_unlock_all(sbi);
			goto out;
		}
		cond_resched();
		goto retry_flush_nodes;
	}

	/*
	 * sbi->node_change is used only for AIO write_begin path which produces
	 * dirty node blocks and some checkpoint values by block allocation.
	 */
	__prepare_cp_block(sbi);
	up_write(&sbi->node_change);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
out:
	blk_finish_plug(&plug);
	return err;
}

static void unblock_operations(struct f2fs_sb_info *sbi)
{
	up_write(&sbi->node_write);
	f2fs_unlock_all(sbi);
}

static void wait_on_all_pages_writeback(struct f2fs_sb_info *sbi)
{
	DEFINE_WAIT(wait);

	for (;;) {
		prepare_to_wait(&sbi->cp_wait, &wait, TASK_UNINTERRUPTIBLE);

<<<<<<< HEAD
		if (!get_pages(sbi, F2FS_WRITEBACK))
			break;

		io_schedule();
=======
		if (!get_pages(sbi, F2FS_WB_CP_DATA))
			break;

		io_schedule_timeout(5*HZ);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	}
	finish_wait(&sbi->cp_wait, &wait);
}

<<<<<<< HEAD
static void do_checkpoint(struct f2fs_sb_info *sbi, struct cp_control *cpc)
{
	struct f2fs_checkpoint *ckpt = F2FS_CKPT(sbi);
	struct curseg_info *curseg = CURSEG_I(sbi, CURSEG_WARM_NODE);
	struct f2fs_nm_info *nm_i = NM_I(sbi);
	nid_t last_nid = nm_i->next_scan_nid;
	block_t start_blk;
	struct page *cp_page;
	unsigned int data_sum_blocks, orphan_blocks;
	__u32 crc32 = 0;
	void *kaddr;
	int i;
	int cp_payload_blks = le32_to_cpu(F2FS_RAW_SUPER(sbi)->cp_payload);

	/*
	 * This avoids to conduct wrong roll-forward operations and uses
	 * metapages, so should be called prior to sync_meta_pages below.
	 */
	discard_next_dnode(sbi, NEXT_FREE_BLKADDR(sbi, curseg));
=======
static void update_ckpt_flags(struct f2fs_sb_info *sbi, struct cp_control *cpc)
{
	unsigned long orphan_num = sbi->im[ORPHAN_INO].ino_num;
	struct f2fs_checkpoint *ckpt = F2FS_CKPT(sbi);
	unsigned long flags;

	spin_lock_irqsave(&sbi->cp_lock, flags);

	if ((cpc->reason & CP_UMOUNT) &&
			le32_to_cpu(ckpt->cp_pack_total_block_count) >
			sbi->blocks_per_seg - NM_I(sbi)->nat_bits_blocks)
		disable_nat_bits(sbi, false);

	if (cpc->reason & CP_TRIMMED)
		__set_ckpt_flags(ckpt, CP_TRIMMED_FLAG);

	if (cpc->reason & CP_UMOUNT)
		__set_ckpt_flags(ckpt, CP_UMOUNT_FLAG);
	else
		__clear_ckpt_flags(ckpt, CP_UMOUNT_FLAG);

	if (cpc->reason & CP_FASTBOOT)
		__set_ckpt_flags(ckpt, CP_FASTBOOT_FLAG);
	else
		__clear_ckpt_flags(ckpt, CP_FASTBOOT_FLAG);

	if (orphan_num)
		__set_ckpt_flags(ckpt, CP_ORPHAN_PRESENT_FLAG);
	else
		__clear_ckpt_flags(ckpt, CP_ORPHAN_PRESENT_FLAG);

	if (is_sbi_flag_set(sbi, SBI_NEED_FSCK))
		__set_ckpt_flags(ckpt, CP_FSCK_FLAG);

	/* set this flag to activate crc|cp_ver for recovery */
	__set_ckpt_flags(ckpt, CP_CRC_RECOVERY_FLAG);

	spin_unlock_irqrestore(&sbi->cp_lock, flags);
}

static int do_checkpoint(struct f2fs_sb_info *sbi, struct cp_control *cpc)
{
	struct f2fs_checkpoint *ckpt = F2FS_CKPT(sbi);
	struct f2fs_nm_info *nm_i = NM_I(sbi);
	unsigned long orphan_num = sbi->im[ORPHAN_INO].ino_num, flags;
	block_t start_blk;
	unsigned int data_sum_blocks, orphan_blocks;
	__u32 crc32 = 0;
	int i;
	int cp_payload_blks = __cp_payload(sbi);
	struct super_block *sb = sbi->sb;
	struct curseg_info *seg_i = CURSEG_I(sbi, CURSEG_HOT_NODE);
	u64 kbytes_written;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	/* Flush all the NAT/SIT pages */
	while (get_pages(sbi, F2FS_DIRTY_META)) {
		sync_meta_pages(sbi, META, LONG_MAX);
		if (unlikely(f2fs_cp_error(sbi)))
<<<<<<< HEAD
			return;
	}

	next_free_nid(sbi, &last_nid);

=======
			return -EIO;
	}

>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	/*
	 * modify checkpoint
	 * version number is already updated
	 */
	ckpt->elapsed_time = cpu_to_le64(get_mtime(sbi));
<<<<<<< HEAD
	ckpt->valid_block_count = cpu_to_le64(valid_user_blocks(sbi));
=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	ckpt->free_segment_count = cpu_to_le32(free_segments(sbi));
	for (i = 0; i < NR_CURSEG_NODE_TYPE; i++) {
		ckpt->cur_node_segno[i] =
			cpu_to_le32(curseg_segno(sbi, i + CURSEG_HOT_NODE));
		ckpt->cur_node_blkoff[i] =
			cpu_to_le16(curseg_blkoff(sbi, i + CURSEG_HOT_NODE));
		ckpt->alloc_type[i + CURSEG_HOT_NODE] =
				curseg_alloc_type(sbi, i + CURSEG_HOT_NODE);
	}
	for (i = 0; i < NR_CURSEG_DATA_TYPE; i++) {
		ckpt->cur_data_segno[i] =
			cpu_to_le32(curseg_segno(sbi, i + CURSEG_HOT_DATA));
		ckpt->cur_data_blkoff[i] =
			cpu_to_le16(curseg_blkoff(sbi, i + CURSEG_HOT_DATA));
		ckpt->alloc_type[i + CURSEG_HOT_DATA] =
				curseg_alloc_type(sbi, i + CURSEG_HOT_DATA);
	}

<<<<<<< HEAD
	ckpt->valid_node_count = cpu_to_le32(valid_node_count(sbi));
	ckpt->valid_inode_count = cpu_to_le32(valid_inode_count(sbi));
	ckpt->next_free_nid = cpu_to_le32(last_nid);

	/* 2 cp  + n data seg summary + orphan inode blocks */
	data_sum_blocks = npages_for_summary_flush(sbi);
	if (data_sum_blocks < NR_CURSEG_DATA_TYPE)
		set_ckpt_flags(ckpt, CP_COMPACT_SUM_FLAG);
	else
		clear_ckpt_flags(ckpt, CP_COMPACT_SUM_FLAG);

	orphan_blocks = GET_ORPHAN_BLOCKS(sbi->n_orphans);
	ckpt->cp_pack_start_sum = cpu_to_le32(1 + cp_payload_blks +
			orphan_blocks);

	if (cpc->reason == CP_UMOUNT) {
		set_ckpt_flags(ckpt, CP_UMOUNT_FLAG);
		ckpt->cp_pack_total_block_count = cpu_to_le32(F2FS_CP_PACKS+
				cp_payload_blks + data_sum_blocks +
				orphan_blocks + NR_CURSEG_NODE_TYPE);
	} else {
		clear_ckpt_flags(ckpt, CP_UMOUNT_FLAG);
		ckpt->cp_pack_total_block_count = cpu_to_le32(F2FS_CP_PACKS +
				cp_payload_blks + data_sum_blocks +
				orphan_blocks);
	}

	if (sbi->n_orphans)
		set_ckpt_flags(ckpt, CP_ORPHAN_PRESENT_FLAG);
	else
		clear_ckpt_flags(ckpt, CP_ORPHAN_PRESENT_FLAG);

	if (sbi->need_fsck)
		set_ckpt_flags(ckpt, CP_FSCK_FLAG);
=======
	/* 2 cp  + n data seg summary + orphan inode blocks */
	data_sum_blocks = npages_for_summary_flush(sbi, false);
	spin_lock_irqsave(&sbi->cp_lock, flags);
	if (data_sum_blocks < NR_CURSEG_DATA_TYPE)
		__set_ckpt_flags(ckpt, CP_COMPACT_SUM_FLAG);
	else
		__clear_ckpt_flags(ckpt, CP_COMPACT_SUM_FLAG);
	spin_unlock_irqrestore(&sbi->cp_lock, flags);

	orphan_blocks = GET_ORPHAN_BLOCKS(orphan_num);
	ckpt->cp_pack_start_sum = cpu_to_le32(1 + cp_payload_blks +
			orphan_blocks);

	if (__remain_node_summaries(cpc->reason))
		ckpt->cp_pack_total_block_count = cpu_to_le32(F2FS_CP_PACKS+
				cp_payload_blks + data_sum_blocks +
				orphan_blocks + NR_CURSEG_NODE_TYPE);
	else
		ckpt->cp_pack_total_block_count = cpu_to_le32(F2FS_CP_PACKS +
				cp_payload_blks + data_sum_blocks +
				orphan_blocks);

	/* update ckpt flag for checkpoint */
	update_ckpt_flags(sbi, cpc);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	/* update SIT/NAT bitmap */
	get_sit_bitmap(sbi, __bitmap_ptr(sbi, SIT_BITMAP));
	get_nat_bitmap(sbi, __bitmap_ptr(sbi, NAT_BITMAP));

<<<<<<< HEAD
	crc32 = f2fs_crc32(ckpt, le32_to_cpu(ckpt->checksum_offset));
=======
	crc32 = f2fs_crc32(sbi, ckpt, le32_to_cpu(ckpt->checksum_offset));
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	*((__le32 *)((unsigned char *)ckpt +
				le32_to_cpu(ckpt->checksum_offset)))
				= cpu_to_le32(crc32);

<<<<<<< HEAD
	start_blk = __start_cp_addr(sbi);

	/* write out checkpoint buffer at block 0 */
	cp_page = grab_meta_page(sbi, start_blk++);
	kaddr = page_address(cp_page);
	memcpy(kaddr, ckpt, (1 << sbi->log_blocksize));
	set_page_dirty(cp_page);
	f2fs_put_page(cp_page, 1);

	for (i = 1; i < 1 + cp_payload_blks; i++) {
		cp_page = grab_meta_page(sbi, start_blk++);
		kaddr = page_address(cp_page);
		memcpy(kaddr, (char *)ckpt + i * F2FS_BLKSIZE,
				(1 << sbi->log_blocksize));
		set_page_dirty(cp_page);
		f2fs_put_page(cp_page, 1);
	}

	if (sbi->n_orphans) {
=======
	start_blk = __start_cp_next_addr(sbi);

	/* write nat bits */
	if (enabled_nat_bits(sbi, cpc)) {
		__u64 cp_ver = cur_cp_version(ckpt);
		block_t blk;

		cp_ver |= ((__u64)crc32 << 32);
		*(__le64 *)nm_i->nat_bits = cpu_to_le64(cp_ver);

		blk = start_blk + sbi->blocks_per_seg - nm_i->nat_bits_blocks;
		for (i = 0; i < nm_i->nat_bits_blocks; i++)
			update_meta_page(sbi, nm_i->nat_bits +
					(i << F2FS_BLKSIZE_BITS), blk + i);

		/* Flush all the NAT BITS pages */
		while (get_pages(sbi, F2FS_DIRTY_META)) {
			sync_meta_pages(sbi, META, LONG_MAX);
			if (unlikely(f2fs_cp_error(sbi)))
				return -EIO;
		}
	}

	/* need to wait for end_io results */
	wait_on_all_pages_writeback(sbi);
	if (unlikely(f2fs_cp_error(sbi)))
		return -EIO;

	/* write out checkpoint buffer at block 0 */
	update_meta_page(sbi, ckpt, start_blk++);

	for (i = 1; i < 1 + cp_payload_blks; i++)
		update_meta_page(sbi, (char *)ckpt + i * F2FS_BLKSIZE,
							start_blk++);

	if (orphan_num) {
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		write_orphan_inodes(sbi, start_blk);
		start_blk += orphan_blocks;
	}

	write_data_summaries(sbi, start_blk);
	start_blk += data_sum_blocks;
<<<<<<< HEAD
	if (cpc->reason == CP_UMOUNT) {
=======

	/* Record write statistics in the hot node summary */
	kbytes_written = sbi->kbytes_written;
	if (sb->s_bdev->bd_part)
		kbytes_written += BD_PART_WRITTEN(sbi);

	seg_i->journal->info.kbytes_written = cpu_to_le64(kbytes_written);

	if (__remain_node_summaries(cpc->reason)) {
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		write_node_summaries(sbi, start_blk);
		start_blk += NR_CURSEG_NODE_TYPE;
	}

	/* writeout checkpoint block */
<<<<<<< HEAD
	cp_page = grab_meta_page(sbi, start_blk);
	kaddr = page_address(cp_page);
	memcpy(kaddr, ckpt, (1 << sbi->log_blocksize));
	set_page_dirty(cp_page);
	f2fs_put_page(cp_page, 1);
=======
	update_meta_page(sbi, ckpt, start_blk);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	/* wait for previous submitted node/meta pages writeback */
	wait_on_all_pages_writeback(sbi);

	if (unlikely(f2fs_cp_error(sbi)))
<<<<<<< HEAD
		return;

	filemap_fdatawait_range(NODE_MAPPING(sbi), 0, LONG_MAX);
	filemap_fdatawait_range(META_MAPPING(sbi), 0, LONG_MAX);

	/* update user_block_counts */
	sbi->last_valid_block_count = sbi->total_valid_block_count;
	sbi->alloc_valid_block_count = 0;
=======
		return -EIO;

	filemap_fdatawait_range(NODE_MAPPING(sbi), 0, LLONG_MAX);
	filemap_fdatawait_range(META_MAPPING(sbi), 0, LLONG_MAX);

	/* update user_block_counts */
	sbi->last_valid_block_count = sbi->total_valid_block_count;
	percpu_counter_set(&sbi->alloc_valid_block_count, 0);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	/* Here, we only have one bio having CP pack */
	sync_meta_pages(sbi, META_FLUSH, LONG_MAX);

<<<<<<< HEAD
	release_dirty_inode(sbi);

	if (unlikely(f2fs_cp_error(sbi)))
		return;

	clear_prefree_segments(sbi);
	F2FS_RESET_SB_DIRT(sbi);
=======
	/* wait for previous submitted meta pages writeback */
	wait_on_all_pages_writeback(sbi);

	release_ino_entry(sbi, false);

	if (unlikely(f2fs_cp_error(sbi)))
		return -EIO;

	clear_sbi_flag(sbi, SBI_IS_DIRTY);
	clear_sbi_flag(sbi, SBI_NEED_CP);
	__set_cp_next_pack(sbi);

	/*
	 * redirty superblock if metadata like node page or inode cache is
	 * updated during writing checkpoint.
	 */
	if (get_pages(sbi, F2FS_DIRTY_NODES) ||
			get_pages(sbi, F2FS_DIRTY_IMETA))
		set_sbi_flag(sbi, SBI_IS_DIRTY);

	f2fs_bug_on(sbi, get_pages(sbi, F2FS_DIRTY_DENTS));

	return 0;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

/*
 * We guarantee that this checkpoint procedure will not fail.
 */
<<<<<<< HEAD
void write_checkpoint(struct f2fs_sb_info *sbi, struct cp_control *cpc)
{
	struct f2fs_checkpoint *ckpt = F2FS_CKPT(sbi);
	unsigned long long ckpt_ver;

	trace_f2fs_write_checkpoint(sbi->sb, cpc->reason, "start block_ops");

	mutex_lock(&sbi->cp_mutex);

	if (!sbi->s_dirty && cpc->reason != CP_DISCARD)
		goto out;
	if (unlikely(f2fs_cp_error(sbi)))
		goto out;
	if (block_operations(sbi))
=======
int write_checkpoint(struct f2fs_sb_info *sbi, struct cp_control *cpc)
{
	struct f2fs_checkpoint *ckpt = F2FS_CKPT(sbi);
	unsigned long long ckpt_ver;
	int err = 0;

	mutex_lock(&sbi->cp_mutex);

	if (!is_sbi_flag_set(sbi, SBI_IS_DIRTY) &&
		((cpc->reason & CP_FASTBOOT) || (cpc->reason & CP_SYNC) ||
		((cpc->reason & CP_DISCARD) && !sbi->discard_blks)))
		goto out;
	if (unlikely(f2fs_cp_error(sbi))) {
		err = -EIO;
		goto out;
	}
	if (f2fs_readonly(sbi->sb)) {
		err = -EROFS;
		goto out;
	}

	trace_f2fs_write_checkpoint(sbi->sb, cpc->reason, "start block_ops");

	err = block_operations(sbi);
	if (err)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
		goto out;

	trace_f2fs_write_checkpoint(sbi->sb, cpc->reason, "finish block_ops");

<<<<<<< HEAD
	f2fs_submit_merged_bio(sbi, DATA, WRITE);
	f2fs_submit_merged_bio(sbi, NODE, WRITE);
	f2fs_submit_merged_bio(sbi, META, WRITE);
=======
	f2fs_flush_merged_writes(sbi);

	/* this is the case of multiple fstrims without any changes */
	if (cpc->reason & CP_DISCARD) {
		if (!exist_trim_candidates(sbi, cpc)) {
			unblock_operations(sbi);
			goto out;
		}

		if (NM_I(sbi)->dirty_nat_cnt == 0 &&
				SIT_I(sbi)->dirty_sentries == 0 &&
				prefree_segments(sbi) == 0) {
			flush_sit_entries(sbi, cpc);
			clear_prefree_segments(sbi, cpc);
			unblock_operations(sbi);
			goto out;
		}
	}
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	/*
	 * update checkpoint pack index
	 * Increase the version number so that
	 * SIT entries and seg summaries are written at correct place
	 */
	ckpt_ver = cur_cp_version(ckpt);
	ckpt->checkpoint_ver = cpu_to_le64(++ckpt_ver);

	/* write cached NAT/SIT entries to NAT/SIT area */
<<<<<<< HEAD
	flush_nat_entries(sbi);
	flush_sit_entries(sbi, cpc);

	/* unlock all the fs_lock[] in do_checkpoint() */
	do_checkpoint(sbi, cpc);

	unblock_operations(sbi);
	stat_inc_cp_count(sbi->stat_info);
out:
	mutex_unlock(&sbi->cp_mutex);
	trace_f2fs_write_checkpoint(sbi->sb, cpc->reason, "finish checkpoint");
=======
	flush_nat_entries(sbi, cpc);
	flush_sit_entries(sbi, cpc);

	/* unlock all the fs_lock[] in do_checkpoint() */
	err = do_checkpoint(sbi, cpc);
	if (err)
		release_discard_addrs(sbi);
	else
		clear_prefree_segments(sbi, cpc);

	unblock_operations(sbi);
	stat_inc_cp_count(sbi->stat_info);

	if (cpc->reason & CP_RECOVERY)
		f2fs_msg(sbi->sb, KERN_NOTICE,
			"checkpoint: version = %llx", ckpt_ver);

	/* do checkpoint periodically */
	f2fs_update_time(sbi, CP_TIME);
	trace_f2fs_write_checkpoint(sbi->sb, cpc->reason, "finish checkpoint");
out:
	mutex_unlock(&sbi->cp_mutex);
	return err;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

void init_ino_entry_info(struct f2fs_sb_info *sbi)
{
	int i;

	for (i = 0; i < MAX_INO_ENTRY; i++) {
<<<<<<< HEAD
		INIT_RADIX_TREE(&sbi->ino_root[i], GFP_ATOMIC);
		spin_lock_init(&sbi->ino_lock[i]);
		INIT_LIST_HEAD(&sbi->ino_list[i]);
	}

	/*
	 * considering 512 blocks in a segment 8 blocks are needed for cp
	 * and log segment summaries. Remaining blocks are used to keep
	 * orphan entries with the limitation one reserved segment
	 * for cp pack we can have max 1020*504 orphan entries
	 */
	sbi->n_orphans = 0;
	sbi->max_orphans = (sbi->blocks_per_seg - F2FS_CP_PACKS -
			NR_CURSEG_TYPE) * F2FS_ORPHANS_PER_BLOCK;
=======
		struct inode_management *im = &sbi->im[i];

		INIT_RADIX_TREE(&im->ino_root, GFP_ATOMIC);
		spin_lock_init(&im->ino_lock);
		INIT_LIST_HEAD(&im->ino_list);
		im->ino_num = 0;
	}

	sbi->max_orphans = (sbi->blocks_per_seg - F2FS_CP_PACKS -
			NR_CURSEG_TYPE - __cp_payload(sbi)) *
				F2FS_ORPHANS_PER_BLOCK;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

int __init create_checkpoint_caches(void)
{
	ino_entry_slab = f2fs_kmem_cache_create("f2fs_ino_entry",
			sizeof(struct ino_entry));
	if (!ino_entry_slab)
		return -ENOMEM;
<<<<<<< HEAD
	inode_entry_slab = f2fs_kmem_cache_create("f2fs_dirty_dir_entry",
			sizeof(struct dir_inode_entry));
=======
	inode_entry_slab = f2fs_kmem_cache_create("f2fs_inode_entry",
			sizeof(struct inode_entry));
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	if (!inode_entry_slab) {
		kmem_cache_destroy(ino_entry_slab);
		return -ENOMEM;
	}
	return 0;
}

void destroy_checkpoint_caches(void)
{
	kmem_cache_destroy(ino_entry_slab);
	kmem_cache_destroy(inode_entry_slab);
}
