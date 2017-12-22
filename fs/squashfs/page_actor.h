#ifndef PAGE_ACTOR_H
#define PAGE_ACTOR_H
/*
 * Copyright (c) 2013
 * Phillip Lougher <phillip@squashfs.org.uk>
 *
 * This work is licensed under the terms of the GNU GPL, version 2. See
 * the COPYING file in the top-level directory.
 */

<<<<<<< HEAD
#ifndef CONFIG_SQUASHFS_FILE_DIRECT
struct squashfs_page_actor {
	void	**page;
	int	pages;
	int	length;
	int	next_page;
};

static inline struct squashfs_page_actor *squashfs_page_actor_init(void **page,
	int pages, int length)
{
	struct squashfs_page_actor *actor = kmalloc(sizeof(*actor), GFP_KERNEL);

	if (actor == NULL)
		return NULL;

	actor->length = length ? : pages * PAGE_CACHE_SIZE;
	actor->page = page;
	actor->pages = pages;
	actor->next_page = 0;
	return actor;
}

static inline void *squashfs_first_page(struct squashfs_page_actor *actor)
{
	actor->next_page = 1;
	return actor->page[0];
=======
struct squashfs_page_actor {
	struct page	**page;
	void	*pageaddr;
	int	pages;
	int	length;
	int	next_page;
	void	(*release_pages)(struct page **, int, int);
};

extern struct squashfs_page_actor *squashfs_page_actor_init(struct page **,
	int, int, void (*)(struct page **, int, int));
extern void squashfs_page_actor_free(struct squashfs_page_actor *, int);

extern void squashfs_actor_to_buf(struct squashfs_page_actor *, void *, int);
extern void squashfs_buf_to_actor(void *, struct squashfs_page_actor *, int);
extern void squashfs_bh_to_actor(struct buffer_head **, int,
	struct squashfs_page_actor *, int, int, int);
extern void squashfs_bh_to_buf(struct buffer_head **, int, void *, int, int,
	int);

/*
 * Calling code should avoid sleeping between calls to squashfs_first_page()
 * and squashfs_finish_page().
 */
static inline void *squashfs_first_page(struct squashfs_page_actor *actor)
{
	actor->next_page = 1;
	return actor->pageaddr = actor->page[0] ? kmap_atomic(actor->page[0])
						: NULL;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

static inline void *squashfs_next_page(struct squashfs_page_actor *actor)
{
<<<<<<< HEAD
	return actor->next_page == actor->pages ? NULL :
		actor->page[actor->next_page++];
}

static inline void squashfs_finish_page(struct squashfs_page_actor *actor)
{
	/* empty */
}
#else
struct squashfs_page_actor {
	union {
		void		**buffer;
		struct page	**page;
	};
	void	*pageaddr;
	void    *(*squashfs_first_page)(struct squashfs_page_actor *);
	void    *(*squashfs_next_page)(struct squashfs_page_actor *);
	void    (*squashfs_finish_page)(struct squashfs_page_actor *);
	int	pages;
	int	length;
	int	next_page;
};

extern struct squashfs_page_actor *squashfs_page_actor_init(void **, int, int);
extern struct squashfs_page_actor *squashfs_page_actor_init_special(struct page
							 **, int, int);
static inline void *squashfs_first_page(struct squashfs_page_actor *actor)
{
	return actor->squashfs_first_page(actor);
}
static inline void *squashfs_next_page(struct squashfs_page_actor *actor)
{
	return actor->squashfs_next_page(actor);
}
static inline void squashfs_finish_page(struct squashfs_page_actor *actor)
{
	actor->squashfs_finish_page(actor);
}
#endif
=======
	if (!IS_ERR_OR_NULL(actor->pageaddr))
		kunmap_atomic(actor->pageaddr);

	if (actor->next_page == actor->pages)
		return actor->pageaddr = ERR_PTR(-ENODATA);

	actor->pageaddr = actor->page[actor->next_page] ?
	    kmap_atomic(actor->page[actor->next_page]) : NULL;
	++actor->next_page;
	return actor->pageaddr;
}

static inline void squashfs_finish_page(struct squashfs_page_actor *actor)
{
	if (!IS_ERR_OR_NULL(actor->pageaddr))
		kunmap_atomic(actor->pageaddr);
}

extern struct page **alloc_page_array(int, int);
extern void free_page_array(struct page **, int);

>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
#endif
